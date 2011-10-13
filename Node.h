#ifndef _NODE_H
#define _NODE_H
#include "Point3d.h"
class Node
{
  public:
    Node * next;		// null for leaf
    Node *prev;			// null for root
    Point3d point;
      Node ():point (0, 0, 0)
    {
	prev = next = NULL;
    };

    Node (Point3d * p) {
	prev = next = NULL;
	point.x = p->x;
	point.y = p->y;
	point.z = p->z;
    }
    Node (float x, float y, float z)
    {
	prev = next = NULL;
	point.x = x;
	point.y = y;
	point.z = z;
    }
    bool isEqual (Node * b)
    {
	return point.isEqual (&(b->point));
    }
    void print ()
    {
	point.print ();
    }
};
#endif
