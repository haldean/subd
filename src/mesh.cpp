#include "mesh.h"

vertex::vertex() {
  loc.setZero();
  normal.setZero();
}

/* This is here to allow for easy breakpoints to find places where copying takes
 * place. */
inline void vertexCopy() {
  cout << "vertex copied" << endl;
}

inline void meshCopy() {
  cout << "mesh copied" << endl;
}

vertex::vertex(const vertex &other) {
#ifdef COPY_CHECKS
  vertexCopy();
#endif
  loc = other.loc;
  normal = other.normal;
  e = other.e;
  id = other.id;
}

bool vertex::operator==(const vertex &other) {
  return loc == other.loc;
}

bool vertex::onboundary() const {
  edge *eloop = e;
  do eloop = eloop->next->pair; while (eloop != e && eloop != NULL);
  return eloop == NULL;
}

edge* edge::previous() const {
  edge *prev = next;
  while (prev->next != this) prev = prev->next;
  return prev;
}

/* If a vertex is non-manifold (meaning it isn't surrounded by faces) find the
 * first edge such that repeatedly calling edge->next->pair will get you all
 * edges associated with the vertex. */
edge* edge::rewind() const {
  if (pair == NULL) return (edge*) this;
  edge *e = pair->previous();
  while (e != this && e->pair != NULL) e = e->pair->previous();
  return e;
}

Vector3f edge::asVector() const {
  return vert->loc - previous()->vert->loc;
}

Vector3f edge::midpoint() const {
  return vert->loc - 0.5 * asVector();
}

float edge::angleBetween(const edge& other) const {
  Vector3f e1 = asVector(), e2 = other.asVector();
  return e1.cross(e2).norm();
}

face::face() {
  normal.setZero();
  e = NULL;
  id = 0;
}

face::face(const face &other) {
  normal = other.normal;
  id = other.id;
  e = other.e;
}

int face::sides() const {
  int sides = 0;
  edge *e0 = e;
  do {
    sides++;
    e0 = e0->next;
  } while (e0 != e);
  return sides;
}

float face::area() const {
  float area = 0;
  edge *e0 = e;
  Vector3f v0, v1;
  do {
    v0 = e0->vert->loc;
    v1 = e0->next->vert->loc;
    area += v0.cross(v1).norm();

    e0 = e0->next;
  } while (e0 != e);
  return area / 2;
}

Vector3f face::centroid() const {
  Vector3f centroid = Vector3f::Zero();

  edge *e0 = e;
  int n = 0;
  do {
    centroid += e0->vert->loc;
    n++;
    e0 = e0->next;
  } while (e0 != e);
  centroid /= n;

  return centroid;
}

void face::calculateNormal() {
  normal = e->asVector().cross(e->next->asVector());
  normal.normalize();
}

mesh::mesh() {
  subdivision = UNKNOWN;
}

/* Deep copies with pointers are NO FUN. */
mesh::mesh(const mesh &other) {
#ifdef COPY_CHECKS
  meshCopy();
#endif
  faces = vector<face*>(other.faces.size(), NULL);
  edges = vector<edge*>(other.edges.size(), NULL);
  verteces = vector<vertex*>(other.verteces.size(), NULL);
  subdivision = other.subdivision;

  for (auto vit = other.verteces.begin(); vit != other.verteces.end(); vit++) {
    vertex *v = new vertex();
    v->id = (*vit)->id;
    v->loc = (*vit)->loc;
    v->normal = (*vit)->normal;
    v->e = NULL;
    verteces[(*vit)->id - 1] = v;
  }

  for (auto fit = other.faces.begin(); fit != other.faces.end(); fit++) {
    face *newf = new face();
    newf->id = (*fit)->id;
    newf->normal = (*fit)->normal;
    newf->e = NULL;
    faces[newf->id - 1] = newf;
  }

  for (auto eit = other.edges.begin(); eit != other.edges.end(); eit++) {
    edge *newe = new edge();
    newe->id = (*eit)->id;

    newe->vert = verteces[(*eit)->vert->id - 1];
    if (newe->vert->e == NULL) newe->vert->e = newe;

    newe->f = faces[(*eit)->f->id - 1];
    edges[newe->id - 1] = newe;
  }

  for (auto eit = other.edges.begin(); eit != other.edges.end(); eit++) {
    edge *e = *eit;
    edges[e->id - 1]->next = edges[e->next->id - 1];
    if (e->pair != NULL) {
      edges[e->id - 1]->pair = edges[e->pair->id - 1];
    }
  }

  for (auto fit = other.faces.begin(); fit != other.faces.end(); fit++) {
    faces[(*fit)->id - 1]->e = edges[(*fit)->e->id - 1];
    faces[(*fit)->id - 1]->calculateNormal();
  }
}

void mesh::guessSubdMethod() {
  for (auto fit = faces.begin(); fit != faces.end(); fit++) {
    if ((*fit)->sides() != 3) {
      subdivision = CATMULL_CLARK_SUBD;
      return;
    }
  }
  subdivision = LOOP_SUBD;
}

void mesh::calculateNormals(normal_mode mode) {
  if (mode == NO_NORMALS) {
    for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
      (*vit)->normal.setZero();
    }
  } else {
    for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
      (*vit)->normal.setZero();
    }

    for (auto fit = faces.begin(); fit != faces.end(); fit++) {
      (*fit)->calculateNormal();
      edge *e = (*fit)->e;
      do {
        e->vert->normal += (*fit)->normal;
        e = e->next;
      } while (e != (*fit)->e);
    }

    for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
      (*vit)->normal.normalize();
    }
  }
}

void mesh::scaleToUnitCube() {
  float minx = INFINITY, miny = INFINITY, minz = INFINITY,
        maxx = -INFINITY, maxy = -INFINITY, maxz = -INFINITY;

  for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
    Vector3f loc = (*vit)->loc;
    if (loc[0] < minx) minx = loc[0];
    if (loc[0] > maxx) maxx = loc[0];
    if (loc[1] < miny) miny = loc[1];
    if (loc[1] > maxy) maxy = loc[1];
    if (loc[2] < minz) minz = loc[2];
    if (loc[2] > maxz) maxz = loc[2];
  }

  Vector3f translate(
      minx - (minx - maxx) / 2,
      miny - (miny - maxy) / 2,
      minz - (minz - maxz) / 2);
  float scale = 1. / max(maxx - minx, max(maxy - miny, maxz - minz));

  for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
    (*vit)->loc -= translate;
    (*vit)->loc *= scale;
  }
}
