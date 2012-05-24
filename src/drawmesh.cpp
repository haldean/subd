#include <Eigen/Core>
#include <iostream>
#include <cstring>

#include "drawmesh.h"
#include "glinclude.h"

void drawString(string s) {
  glDisable(GL_LIGHTING);

  int matrixMode;
  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, 1.0, 0.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */

  glRasterPos3f(0.01, 0.01, 0);
  glColor4f(1., 1., 1., 1.);
  for (unsigned int i = 0; i < s.length(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
  }

  glPopAttrib();
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(matrixMode);

  glEnable(GL_LIGHTING);
}

void vector3ToGL(Vector3f v) {
  glVertex3f(v[0], v[1], v[2]);
}

void drawVertex(vertex &v, face *face, drawopts opts) {
  if (!v.normal.isZero()) {
    glNormal3f(v.normal[0], v.normal[1], v.normal[2]);
  } else {
    glNormal3f(face->normal[0], face->normal[1], face->normal[2]);
  }
  vector3ToGL(v.loc);
}

void drawFace(face* face, drawopts opts) {
  if (opts.drawFaces) {
    if (face->sides() == 3) glBegin(GL_TRIANGLES);
    //else if (face->sides() == 4) glBegin(GL_QUADS);
    else glBegin(GL_TRIANGLE_FAN);

    edge *e0 = face->e;
    edge *e = e0;
    do {
      drawVertex(*e->vert, face, opts);
      e = e->next;
    } while (e != e0);

    glEnd();
  }
}

void drawEdges(face* face, drawopts opts) {
  glBegin(GL_LINE_STRIP); {
    edge *e0 = face->e;
    edge *e = e0;
    do {
      vector3ToGL(e->vert->loc);
      e = e->next;
    } while (e != e0);
    vector3ToGL(e0->vert->loc);
  } glEnd();
}

void drawNormals(face* face, drawopts opts) {

  for (int i = 0; i < 3; i++) {
    edge *e0 = face->e;
    edge *e = e0;
    do {
      vertex *v = e->vert;
      glBegin(GL_LINE_STRIP); {
        vector3ToGL(v->loc);
        vector3ToGL(v->loc + v->normal);
      } glEnd();
      e = e->next;
    } while (e != e0);
  }
}

void drawHull(mesh &mesh, drawopts opts) {
  if (!opts.drawHull) return;

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0, 1.0);
  glLineWidth(2.0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.hullColor);

  for (auto fit = mesh.faces.begin(); fit != mesh.faces.end(); fit++) {
    drawEdges(*fit, opts);
  }

  glutWireCube(1.);
}

void drawMesh(mesh &mesh, drawopts opts) {
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0, 1.0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.meshColor);
  if (opts.drawFaces || opts.drawVerteces) {
    for (auto it = mesh.faces.begin(); it != mesh.faces.end(); it++) {
      face* face = (*it);
      drawFace(face, opts);
    }
  }

  if (opts.drawEdges) {
    glLineWidth(2.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.edgeColor);
    for (auto it = mesh.faces.begin(); it != mesh.faces.end(); it++) {
      drawEdges(*it, opts);
    }
  }

  if (opts.drawNormals) {
    glLineWidth(1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.normalColor);
    for (auto v = mesh.verteces.begin(); v != mesh.verteces.end(); v++) {
      glBegin(GL_LINE_STRIP); {
        vector3ToGL((*v)->loc);
        vector3ToGL((*v)->loc + (*v)->normal);
      } glEnd();
    }
  }

  if (opts.drawVerteces) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.meshColor);
    for (auto v = mesh.verteces.begin(); v != mesh.verteces.end(); v++) {
      glPushMatrix(); {
        glTranslatef((*v)->loc[0], (*v)->loc[1], (*v)->loc[2]);
        glutSolidSphere(0.1, 5, 5);
      } glPopMatrix();
    }
  }
}

drawopts defaultDrawOptions() {
  drawopts opts;
  opts.drawEdges = false;
  opts.drawNormals = false;
  opts.drawFaces = true;
  opts.drawVerteces = false;
  opts.drawHull = false;

  opts.useCelShader = false;
  opts.animate = true;

  opts.normalColor[0] = 1.;
  opts.normalColor[1] = .5;
  opts.normalColor[2] = .5;
  opts.normalColor[3] = 1.;

  for (int i=0; i<3; i++) opts.edgeColor[i] = 0.;
  opts.edgeColor[3] = 1.;

  opts.meshColor[0] = 1.;
  opts.meshColor[1] = .5;
  opts.meshColor[2] = 0.;
  opts.meshColor[3] = 1.;

  for (int i=0; i<3; i++) opts.hullColor[i] = 0.;
  opts.hullColor[3] = .3;

  return opts;
}
