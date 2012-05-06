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

Vector3f edge::asVector() const {
  edge *e0 = next;
  while (e0->next != this) {
    e0 = e0->next;
  }
  return vert->loc - e0->vert->loc;
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

void face::calculateNormal() {
  normal = e->asVector().cross(e->next->asVector());
  normal.normalize();
}

mesh::mesh() {
}

/* Deep copies with pointers are NO FUN. */
mesh::mesh(const mesh &other) {
#ifdef COPY_CHECKS
  meshCopy();
#endif
  faces = vector<face*>(other.faces.size(), NULL);
  edges = vector<edge*>(other.edges.size(), NULL);
  verteces = vector<vertex*>(other.verteces.size(), NULL);

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
    edges[e->id - 1]->pair = edges[e->pair->id - 1];
  }

  for (auto fit = other.faces.begin(); fit != other.faces.end(); fit++) {
    faces[(*fit)->id - 1]->e = edges[(*fit)->e->id - 1];
    faces[(*fit)->id - 1]->calculateNormal();
  }
}

void mesh::calculateNormals(normal_mode mode) {
  if (mode == NO_NORMALS) {
    for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
      (*vit)->normal.setZero();
    }
  } else {
    for (auto fit = faces.begin(); fit != faces.end(); fit++) {
      (*fit)->calculateNormal();
    }

    for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
      vertex *v = *vit;
      edge *e = v->e;
      v->normal.setZero();

      do {
        face *f = e->f;

        float w = 1;
        if (mode == AREA_WEIGHTS) {
          w = f->area();
        } else if (mode == ANGLE_WEIGHTS) {
          w = e->angleBetween(*e->next);
        }

        v->normal += w * f->normal;
        e = e->next->pair;
      } while (e != v->e);

      v->normal.normalize();
    }
  }
}