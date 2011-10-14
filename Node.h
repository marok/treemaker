#ifndef _NODE_H
#define _NODE_H
#include "Point3d.h"
#include<vector>
#include "Segment.h"

class Node
{
private:
        std::vector<Node*> children;
        
public:
        Node * next;		// null for leaf
        Node *prev;			// null for root
        Point3d point;
        Segment *segment;
        
        float r; //radius
        
        Node ():point (0, 0, 0) {
                prev = next = NULL;
                r = 0.5;
                segment = NULL;
                children.clear();
        }

        Node (Point3d * p) {
                prev = next = NULL;
                point.x = p->x;
                point.y = p->y;
                point.z = p->z;
                
                r=0.5;
                segment = NULL;
                children.clear();
        }
        
        Node (float x, float y, float z) {
                prev = next = NULL;
                point.x = x;
                point.y = y;
                point.z = z;
                
                r=0.5;
                segment = NULL;
                children.clear();
        }
        
        bool isEqual (Node * b) {
                return point.isEqual (&(b->point));
        }
        
        void print () {
                point.print ();
        }
        
        void addChildren(Node *child)
        {
                children.push_back(child);
                child->prev = this;
        }
        
        Node *getChildAt(int i)
        {
                return children.at(i);
        }
        
        int getChildLen()
        {
                return children.size();
        }
};
#endif
