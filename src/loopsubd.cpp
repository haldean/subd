#include "subd.h"
#include <set>

void loopSubdivideMesh(mesh &m0, mesh &m) {
  m = mesh(m0);
  for (auto fit = m.faces.begin(); fit != m.faces.end(); fit++) {
    if ((*fit)->sides() != 3) {
      cout << "Loop subdivision only works on triangular meshes." << endl;
      return;
    }
  }

  int evenverts = m.verteces.size();

  subdivideEdges(m0, m, LOOP_SUBD);

  /* Split the faces */
  vector<face*> faces(m.faces);
  m.faces.clear();
  for (auto fit = faces.begin(); fit != faces.end(); fit++) {
    edge *e0 = (*fit)->e;
    edge *e_previous = e0;
    while (e_previous->next != e0) e_previous = e_previous->next;
    edge *e, *e0_next, *newe;
    edge *outer1 = NULL, *outer2 = NULL, *outer3 = NULL;

    /* make the halfedges to complete each outer face */
    do {
      e = e0->next;
      e0_next = e->next;

      face *newf = new face();
      newf->id = m.faces.size() + 1;
      newf->e = e0;
      m.faces.push_back(newf);
      
      newe = new edge();
      newe->id = m.edges.size() + 1;
      newe->f = newf;
      newe->vert = e_previous->vert;

      newe->next = e0;
      e->next = newe;

      if (outer1 == NULL) outer1 = newe;
      else if (outer2 == NULL) outer2 = newe;
      else outer3 = newe;

      e0 = e0_next;
      e_previous = e;
      m.edges.push_back(newe);
    } while (e0 != (*fit)->e);

    /* this awful thing creates the inner face */
    edge *e1 = new edge(), *e2 = new edge(), *e3 = new edge();

    face *inner = new face();
    inner->id = m.faces.size() + 1;
    inner->e = e3;
    m.faces.push_back(inner);

    e1->id = m.edges.size() + 1;
    m.edges.push_back(e1);
    e2->id = m.edges.size() + 1;
    m.edges.push_back(e2);
    e3->id = m.edges.size() + 1;
    m.edges.push_back(e3);

    e1->f = inner;
    e1->vert = outer1->next->next->vert;
    e1->pair = outer1;
    outer1->pair = e1;
    e1->next = e2;

    e2->f = inner;
    e2->vert = outer2->next->next->vert;
    e2->pair = outer2;
    outer2->pair = e2;
    e2->next = e3;

    e3->f = inner;
    e3->vert = outer3->next->next->vert;
    e3->pair = outer3;
    outer3->pair = e3;
    e3->next = e1;
  }

  /* Apply smoothing operator to even verteces */
  for (int i = 0; i < evenverts; i++) {
    vertex *v = m.verteces[i];
    vector<Vector3f> neighbors;

    edge *e0 = m0.edges[v->e->id - 1],
         *e = e0;
    do {
      neighbors.push_back(e->pair->vert->loc);
      e = e->next->pair;
    } while (e != e0);

    int n = neighbors.size();
    float beta = n == 3 ? 3. / 16. : 3. / (8. * n);

    v->loc *= 1. - (float) n * beta;
    for (auto nbr = neighbors.begin(); nbr != neighbors.end(); nbr++) {
      v->loc += beta * *nbr;
    }
  }

  for (auto fit = m.faces.begin(); fit != m.faces.end(); fit++) {
    (*fit)->calculateNormal();
  }
}
