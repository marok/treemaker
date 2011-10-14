#ifndef _NODE_H
#define _NODE_H
#include "Point3d.h"
class Node
{
public:
        Node * next;		// null for leaf
        Node *prev;			// null for root
        Point3d point;
        
        float r; //radius
        
        Node ():point (0, 0, 0) {
                prev = next = NULL;
                r = 1;
        };

        Node (Point3d * p) {
                prev = next = NULL;
                point.x = p->x;
                point.y = p->y;
                point.z = p->z;
                
                r=1;
        }
        Node (float x, float y, float z) {
                prev = next = NULL;
                point.x = x;
                point.y = y;
                point.z = z;
                r=1;
        }
        bool isEqual (Node * b) {
                return point.isEqual (&(b->point));
        }
        void print () {
                point.print ();
        }
};
#endif
