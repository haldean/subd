#include "subd.h"

void ccSubdivideMesh(mesh &m0, mesh &m) {
  m = mesh(m0);
  
  vector<vertex*> facepoints;
  vector<vertex*> edgepoints;

  subdivideEdges(m0, m, CATMULL_CLARK_SUBD);

  /* Generate new faces */
  vector<face*> faces = m.faces;
  m.faces.clear();
  for (auto fit = faces.begin(); fit != faces.end(); fit++) {
    face *f = *fit;

    vertex *vc = new vertex();
    vc->id = m.verteces.size() + 1;
    vc->loc = f->centroid();
    vc->e = NULL;

    m.verteces.push_back(vc);
    facepoints.push_back(vc);

    edge *e0 = f->e->previous(),
         *e1 = f->e,
         *e2 = f->e->next,
         *next_e;
    vector<edge*> genedges;
    do {
      next_e = e2->next;

      face *new_f = new face();
      new_f->id = m.faces.size() + 1;
      new_f->e = e1;
      m.faces.push_back(new_f);

      vertex *v0 = e0->vert;

      edge *e2c = new edge(),
           *ec0 = new edge();

      e2c->id = m.edges.size() + 1;
      m.edges.push_back(e2c);
      genedges.push_back(e2c);

      ec0->id = m.edges.size() + 1;
      m.edges.push_back(ec0);
      genedges.push_back(ec0);

      if (vc->e == NULL) vc->e = e2c;

      e2->next = e2c;
      e2c->next = ec0;
      ec0->next = e1;

      e1->f = new_f;
      e2->f = new_f;
      e2c->f = new_f;
      ec0->f = new_f;

      e2c->vert = vc;
      ec0->vert = v0;

      new_f->calculateNormal();

      e0 = e2;
      e1 = next_e;
      e2 = e1->next;
    } while (e1 != f->e);

    int N = genedges.size();
    for (int i = 0; i < N; i += 2) {
      /* black magic. */
      genedges[i]->pair = genedges[(i+3) % N];
      genedges[(i+3) % N]->pair = genedges[i];
    }
  }

  /* Move even verteces */
  for (auto vit = m0.verteces.begin(); vit != m0.verteces.end(); vit++) {
    vertex *v0 = *vit, *v = m.verteces[v0->id - 1];

    if (v->onboundary()) {
      edge *e0 = m0.edges[v0->e->id - 1]->rewind();
      edge *e1 = e0->next;
      while (e1->pair != NULL) e1 = e1->pair->next;

      v->loc = e0->midpoint() + e1->midpoint() + 6 * v->loc;
      v->loc /= 8;

      continue;
    }

    Vector3f F = Vector3f::Zero();
    Vector3f R = Vector3f::Zero();
    Vector3f P = v0->loc;
    int n = 0;

    edge *e0 = m0.edges[v0->e->id - 1];
    e0 = e0->rewind();
    edge *e = e0;
    do {
      F += e->f->centroid();
      R += e->midpoint();
      n++;

      e = e->next->pair;
    } while (e != e0 && e != NULL);

    F /= n;
    R /= n;
    v->loc = (F + 2 * R + (n - 3) * P) / n;
  }
}
