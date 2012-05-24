#include "objparse.h"

#include <cstdio>
#include <iterator>
#include <vector>
#include <map>
#include <iostream>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

struct objface {
  unsigned int id;
  vector<unsigned int> vids;
  vector<unsigned int> nids;
};

vector<vertex*> verteces;
vector<edge*> edges;
vector<face*> faces;
vector<struct objface> objfaces;
subd_method method = UNKNOWN;

int parseVertexSpec(const string vspec) {
  unsigned int vid;
  sscanf(vspec.c_str(), "%d", &vid);
  return vid;
}

void parseLine(string line) {
  if (line[0] == '#' && line.find("loop") != string::npos) {
    method = LOOP_SUBD;
  } else if (line[0] == '#' && line.find("catmullclark") != string::npos) {
    method = CATMULL_CLARK_SUBD;
  } else if (line[0] == 'v' && line[1] == ' ') {
    vertex *v = new vertex();

    float x, y, z;
    sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
    v->loc = Vector3f(x, y, z);
    v->id = verteces.size() + 1;

    verteces.push_back(v);
  } else if (line[0] == 'f' && line[1] == ' ') {
    istringstream tokenizer(line);
    vector<string> tokens;
    copy(istream_iterator<string>(tokenizer),
        istream_iterator<string>(),
        back_inserter<vector<string> >(tokens));

    struct objface f;
    f.id = objfaces.size() + 1;
    for (unsigned int i=1; i<tokens.size(); i++) {
      string token = tokens[i];
      f.vids.push_back(parseVertexSpec(token));
    }

    objfaces.push_back(f);
  }
}

void mergeHalfEdges() {
  map<pair<unsigned int, unsigned int>, edge*> halfedges;
  for (vector<edge*>::const_iterator edge_iter = edges.begin();
       edge_iter != edges.end(); edge_iter++) {
    edge *e = *edge_iter;

    int vid1 = e->vert->id;
    edge *previous = e;
    while (previous->next != e) previous = previous->next;
    int vid2 = previous->vert->id;

    pair<unsigned int, unsigned int> vids = 
      pair<unsigned int, unsigned int>(
        vid1 < vid2 ? vid1 : vid2, vid1 < vid2 ? vid2 : vid1);

    map<pair<unsigned int, unsigned int>, edge*>::iterator
      mapval = halfedges.find(vids);

    if (mapval == halfedges.end()) {
      halfedges.insert(
          pair<pair<unsigned int, unsigned int>, edge*>(vids, e));
    } else {
      edge *opposite = mapval->second;
      e->pair = opposite;
      opposite->pair = e;
    }
  }
}

void addTriangle(vector<unsigned int> vids) {
  face *f = new face();
  f->id = faces.size() + 1;

  edge *last_edge;
  for (unsigned int i = 0; i < vids.size(); i++) {
    edge *e = new edge();
    e->vert = verteces[vids[i] - 1];
    verteces[vids[i] - 1]->e = e;
    e->f = f;
    e->id = edges.size() + 1;
    e->pair = NULL;
    if (i != 0) {
      last_edge->next = e;
    } else {
      f->e = e;
    }

    last_edge = e;
    edges.push_back(e);
  }
  last_edge->next = f->e;
  f->calculateNormal();
  faces.push_back(f);
}

void loadObjFile(istream& file, mesh &mesh) {
  string line;
  while (file.good()) {
    getline(file, line);
    parseLine(line);
  }

  for (auto objf = objfaces.begin(); objf != objfaces.end(); objf++) {
    addTriangle(objf->vids);
  }
  mergeHalfEdges();

  mesh.verteces = verteces;
  mesh.edges = edges;
  mesh.faces = faces;
  if (method != UNKNOWN) {
    mesh.subdivision = method;
  } else {
    mesh.guessSubdMethod();
  }
  mesh.scaleToUnitCube();

  cout << "Loaded mesh: " << endl
    << "  " << verteces.size() << " verteces." << endl
    << "  " << edges.size() << " edges." << endl
    << "  " << objfaces.size() << " faces in OBJ file." << endl;
}

