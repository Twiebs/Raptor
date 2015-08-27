#ifndef CELESTIAL_MECHANICS_H
#define CELESTIAL_MECHANICS_H

#ifndef VECTOR3
struct Vector3 { double x, y, z; };
#define VECTOR3 Vector3
#endif

#define GRAV_CONST 6.67384E-11

double GravitationalForce(double m1, double m2, double dist) {
  double result = (GRAV_CONST * m1 * m2) / (dist * dist);
  return result;
}

double SurfaceGravity(double mass, double radius) {

}

#endif
