subd: Mesh subdivision using Catmull-Clark and Loop methods
=======
Will Brown
---

Run `make` to compile and `./display_geom [obj file]` to run the viewer.

In the viewer, use WASD or the mouse to rotate around two axes, and Q and E to
move in and out.

Keymap:
    m: Toggle normal interpolation (GL_SMOOTH)
    g: Toggle edge drawing
    f: Toggle face drawing
    n: Toggle vertex normal drawing (must be in vertex normal mode)
    v: Toggle vertex drawing (this is very expensive for large meshes)
    h: Toggle hull drawing at subdivision levels > 0
    1: Use face normals only
    2: Set vertex normals to be the average of their adjacent face normals
    <: Decrease subdivision level
    >: Increase subdivision level

Dependencies
---
* OpenGL
* GLUT
* Eigen (included in repository)
