#ifndef _NODE_H
#define _NODE_H
#include<vector>

#include "Point3d.h"
#include "Segment.h"

class Node
{
private:
        std::vector<Node*> children;
        
public:
        Node *prev;			// null for root
        Point3d point;
        Segment *segment;
        
        float r; //radius
        
        Node ():point (0, 0, 0) {
                prev = NULL;
                segment = NULL;
                children.clear();
        }

        Node (Point3d * p) {
                prev =  NULL;
                point.x = p->x;
                point.y = p->y;
                point.z = p->z;
                
                segment = NULL;
                children.clear();
        }
        
        Node (float x, float y, float z) {
                prev =  NULL;
                point.x = x;
                point.y = y;
                point.z = z;
                
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
