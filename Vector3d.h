#ifndef _VECTOR3D_H
#define _VECTOR3D_H
#include <cmath>
#include <cstdio>
#include "Point3d.h"
class Vector3d
{
public:
        float d[3];			//x,y,z coordinates
#define foreachcoord_i for(int i=0;i<3;i++)

        Vector3d () {
                foreachcoord_i d[i] = 0;
        }

        Vector3d (Point3d * p) {
                d[0] = p->x;
                d[1] = p->y;
                d[2] = p->z;
        }

        Vector3d (float x, float y, float z) {
                d[0] = x;
                d[1] = y;
                d[2] = z;
        }

        //wektor ab
        Vector3d (Point3d * a, Point3d * b) {
                d[0] = b->x - a->x;
                d[1] = b->y - a->y;
                d[2] = b->z - a->z;
        }

        void add (Vector3d * v) {
                foreachcoord_i d[i] += v->d[i];
        }
        void sub (Vector3d * v) {
                foreachcoord_i d[i] -= v->d[i];
        }
        void mul (float s) {
                foreachcoord_i d[i] *= s;
        }
        float length () {
                return sqrt (d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        }
        void normalize () {
                float old_length = length();
                foreachcoord_i d[i] /= old_length;
        }
        void print () {
                printf ("v %f %f %f\n", d[0], d[1], d[2]);
        }
        
        Vector3d * crossProduct(Vector3d * a)
        {
                return new Vector3d(this->d[1]*a->d[2] - this->d[2]*a->d[1],
                        - this->d[0]*a->d[2] + this->d[2]*a->d[0],
                        this->d[0]*a->d[1] - this->d[1]*a->d[0]);
        }
        
#undef foreachcoord_i

};
#endif
