#ifndef _MODEL3D_H
#define	_MODEL3D_H

#include <cmath>

#define CHILD_RADIUS 0.02
#define RADIUS_FACTOR 2.0


class Model3d {
        Node *root;
  
void computeSegment(Node *node) {
                Vector3d *norm;
                int childLen = node->getChildLen();
                
                if(childLen > 0)
                {
                        Vector3d *fromParent;
                        
                        //Jeśli node jest korzeniem to wektor from parent jest rowny (0,0,0)
                        if(node->prev)
                        {
                                fromParent = new Vector3d(&node->prev->point, &node->point);
                                fromParent->normalize();
                        } else
                                fromParent = new Vector3d();
                        
                        norm = new Vector3d();
                        norm->add(fromParent);
                        delete(fromParent);
                        
                        for(int i=0; i<childLen; i++)
                        {
                                Node *child = node->getChildAt(i);
                                Vector3d *toChild = new Vector3d(&node->point, &child->point );
                                
                                toChild->normalize();
                                norm->add(toChild);
                                delete(toChild);
                        }
                        
                       
                } else if(childLen == 0)//jeżeli node jest liściem
                {
                        norm = new Vector3d(&node->point, &node->prev->point);
                        norm->mul(-1);
                }
                        

                norm->normalize();

                Vector3d *U, *V;
                float factor;

                if (abs(norm->d[0]) >= abs(norm->d[1])) {
                        factor = 1 / sqrt(norm->d[0] * norm->d[0] + norm->d[2] * norm->d[2]);
                        U = new Vector3d(-1 * norm->d[2] * factor, 0, norm->d[0] * factor);
                } else {
                        factor = 1 / sqrt(norm->d[1] * norm->d[1] + norm->d[2] * norm->d[2]);
                        U = new Vector3d(0, norm->d[2] * factor, -1 * norm->d[1] * factor);
                }

                V = norm->crossProduct(U);


                float angle = 2 * M_PI / CIRCLE_PTS_COUNT;

                Segment *segment = new Segment();
                node->segment = segment;
              
                
                for (int i = 0; i < CIRCLE_PTS_COUNT; i++) {
                        Vector3d *u, *v;
                        u = new Vector3d();
                        v = new Vector3d();
                        u->add(U);
                        v->add(V);

                        u->mul(node->r * cos((float) i * angle));
                        v->mul(node->r * sin((float) i * angle));
                        u->add(v);

                        segment->circlePts[i] = new Point3d(&(node->point));
                        segment->circlePts[i]->x += u->d[0];
                        segment->circlePts[i]->y += u->d[1];
                        segment->circlePts[i]->z += u->d[2];

                        //printf("%d. %f %f %f %f %f %f\n",i,node->point.x,node->point.y,node->point.z, node->segment->circlePts[i]->x,node->segment->circlePts[i]->y,node->segment->circlePts[i]->z);
                        
                        delete(u);
                        delete(v);
                }
                delete(U);
                delete(V);
                delete(norm);
                
                for(int i=0; i < childLen; i++)
                {
                        computeSegment(node->getChildAt(i));
                }
        }
        
        
        void computeConnectedPts(Node *root)
        {
                
                int childLen = root->getChildLen();
                
                if(childLen > 0)
                {
                        Segment *rootSegment = root->segment;
                        for(int i =0; i< childLen; i++)
                        {
                                Node *child =root->getChildAt(i);
                                Segment *childSegment = child->segment;
                                
                                Vector3d *r = new Vector3d(&root->point, rootSegment->circlePts[0]);
                                Point3d *p = new Point3d(&child->point);
                                p->x += r->d[0];
                                p->y += r->d[1];
                                p->z += r->d[2];
                                
                                int index;
                                float minDistance;
                                for (int i = 0; i < CIRCLE_PTS_COUNT; i++) {
                                        if (i == 0) {
                                                index = i;
                                                minDistance = p->getDistance(childSegment->circlePts[i]);
                                        } else {
                                                float currentDistance = p->getDistance(childSegment->circlePts[i]);
                                                if (currentDistance < minDistance) {
                                                        minDistance = currentDistance;
                                                        index = i;
                                                }
                                        }
                                }
                                childSegment->index = index;
                                
                                computeConnectedPts(child);
                        }
                }
        }
        
        float computeRadius(Node *root)
        {
                int childLen = root->getChildLen();
                root->r = 0.0;
                if(childLen > 0)
                {
                        for(int i=0; i< childLen; i++)
                        {
                                root->r +=  powf(computeRadius(root->getChildAt(i)), RADIUS_FACTOR);
                        }
                        root->r = powf(root->r, 1.0/RADIUS_FACTOR);
                } else
                {
                        root->r = CHILD_RADIUS;
                }
                
                return root->r;
        }
        public:
        Model3d(Node *root) {
                this->root = root;
        }
       

        void generateModel() {  
                computeRadius(root);
                computeSegment(root);
                computeConnectedPts(root);
                
        }
};
#endif