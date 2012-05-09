#ifndef __SUBD_H__
#define __SUBD_H__

#include "mesh.h"

void subdivideEdges(mesh &, mesh &, subd_method);
void subdivideMesh(mesh &, mesh &);
void loopSubdivideMesh(mesh &, mesh &);
void ccSubdivideMesh(mesh &, mesh &);

#endif
