#ifndef __MESH_H__
#define __MESH_H__

#include <Eigen/Dense>
#include <vector>
#include <iostream>

using namespace Eigen;
using namespace std;

class vertex;
class edge;
class face;

enum normal_mode {
  NO_NORMALS,
  AVERAGE,
  AREA_WEIGHTS,
  ANGLE_WEIGHTS
};

typedef pair<int, int> vertpair;
vertpair makeVertpair(int v1, int v2);

class vertex {
  public:
    vertex();
    vertex(const vertex &other);
    bool operator==(const vertex &other);

    int id;
    Vector3f loc;
    Vector3f normal;
    edge* e;
};

class edge {
  public:
    int id;
    vertex* vert;
    face* f;
    edge* next;
    edge* pair;

    Vector3f asVector() const;
    float angleBetween(const edge& other) const;
};

class face {
  public:
    face();
    face(const face &other);
    float area() const;
    int sides() const;
    void calculateNormal();

    int id;
    Vector3f normal;
    edge* e;
};

class mesh {
  public:
    mesh();
    mesh(const mesh &other);
    void calculateNormals(normal_mode mode);

    vector<vertex*> verteces;
    vector<edge*> edges;
    vector<face*> faces;
};

#endif
