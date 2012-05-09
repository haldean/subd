#include "subd.h"
#include <set>

void subdivideEdges(mesh &m0, mesh &m, subd_method method) {
  set<int> split_edges;
  int evenedges = m.edges.size();

  /* Divide each edge into two, adding a vertex. */
  for (int i = 0; i < evenedges; i++) {
    edge *e_split = m.edges[i];
    edge *e_previous = e_split;
    while (e_previous->next != e_split) e_previous = e_previous->next;

    vertex *v_start = e_previous->vert;
    vertex *v_end = e_split->vert;

    edge *e = new edge();
    e->id = m.edges.size() + 1;
    e->f = e_split->f;
      
    e->next = e_split;
    e_previous->next = e;

    if (split_edges.find(e_split->pair->id) == split_edges.end()) {
      vertex *midpoint = new vertex();
      midpoint->id = m.verteces.size() + 1;

      if (method == LOOP_SUBD) {
        vertex *opp1 = m0.edges[e_split->id - 1]->next->vert,
               *opp2 = m0.edges[e_split->id - 1]->pair->next->vert;
        midpoint->loc = 
          (3. / 8.) * (v_start->loc + v_end->loc) +
          (1. / 8.) * (opp1->loc + opp2->loc);
      } else {
        midpoint->loc = v_start->loc + v_end->loc 
          + m0.edges[e_split->id - 1]->f->centroid()
          + m0.edges[e_split->id - 1]->pair->f->centroid();
        midpoint->loc /= 4;
      }

      e->vert = midpoint;
      midpoint->e = e;

      m.verteces.push_back(midpoint);

    } else {
      edge *old_pair = m.edges[e_split->pair->id - 1];

      edge *pair_previous = old_pair;
      while (pair_previous->next != old_pair) pair_previous = pair_previous->next;

      e_split->pair = pair_previous;
      pair_previous->pair = e_split;

      e->vert = pair_previous->vert;

      old_pair->pair = e;
      e->pair = old_pair;
    }

    m.edges.push_back(e);
    split_edges.insert(e_split->id);
  }
}

void subdivideMesh(mesh &m0, mesh &m) {
  if (m0.subdivision == LOOP_SUBD)
    loopSubdivideMesh(m0, m);
  else
    ccSubdivideMesh(m0, m);
}
