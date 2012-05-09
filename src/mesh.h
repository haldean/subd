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

enum subd_method {
  LOOP_SUBD,
  CATMULL_CLARK_SUBD
};

enum normal_mode {
  NO_NORMALS,
  AVERAGE
};

typedef pair<int, int> vertpair;
vertpair makeVertpair(int v1, int v2);

class vertex {
  public:
    vertex();
    vertex(const vertex &other);
    bool operator==(const vertex &other);
    bool onboundary() const;

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

    edge* previous() const;
    edge* rewind() const;
    Vector3f asVector() const;
    Vector3f midpoint() const;
    float angleBetween(const edge& other) const;
};

class face {
  public:
    face();
    face(const face &other);
    float area() const;
    int sides() const;
    void calculateNormal();
    Vector3f centroid() const;

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
    subd_method subdivision;
};

#endif
