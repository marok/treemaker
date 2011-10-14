#ifndef MODEL3D_H
#define	MODEL3D_H

#include "Segment.h"

class Model3d {
public:
        Node *root;
        
        Model3d(Node *root) {
                this->root = root;
        }
       

        Segment * generateModel() {
                Node *node = root;
                Segment *prvSegment = NULL;
                Segment *firstSegment = NULL;
                while(node)
                {
                        prvSegment = computeSegment(prvSegment, node);
                        if(firstSegment ==NULL)
                                firstSegment = prvSegment;
                        
                        
                        node = node->next;
                }
                
                prvSegment = firstSegment;
                while(prvSegment)
                {
                        computeConnectedPts(prvSegment);
                        prvSegment = prvSegment->next;
                }
                
                return firstSegment;
        }
private:
        Segment * computeSegment(Segment * prvSegment, Node *node) {
                Vector3d *norm;
                
                if(node->prev && node->next)
                {
                        Vector3d *AB = new Vector3d(&node->prev->point, &node->point);
                        AB->normalize();
                        
                        Vector3d *BC = new Vector3d(&node->point, &node->next->point );
                        BC->normalize();
                        
                        norm = new Vector3d();
                        norm->add(AB);
                        norm->add(BC);
                        
                        delete(AB);
                        delete(BC);
                       
                } else if(node->next== NULL)
                {
                        norm = new Vector3d(&node->point, &node->prev->point);
                        norm->mul(-1);
                } else
                {
                        norm = new Vector3d(&node->point, &node->next->point);
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

                Segment * result = new Segment(node);
                if(prvSegment)
                        prvSegment->next = result;
                
                result->next = NULL;
                
                for (int i = 0; i < CIRCLE_PTS_COUNT; i++) {
                        Vector3d *u, *v;
                        u = new Vector3d();
                        v = new Vector3d();
                        u->add(U);
                        v->add(V);

                        u->mul(node->r * cos((float) i * angle));
                        v->mul(node->r * sin((float) i * angle));
                        u->add(v);

                        result->circlePts[i] = new Point3d(&(node->point));
                        result->circlePts[i]->x += u->d[0];
                        result->circlePts[i]->y += u->d[1];
                        result->circlePts[i]->z += u->d[2];

                        delete(u);
                        delete(v);
                }
                delete(U);
                delete(V);
                delete(norm);
                
                return result;
        }
        
        
        void computeConnectedPts(Segment *s)
        {
                
                Segment * nextS = s->next;
                if(nextS)
                {
                        Vector3d *r = new Vector3d(&s->node->point, s->circlePts[0]);
                        Point3d *p = new Point3d(&nextS->node->point);
                        p->x += r->d[0];
                        p->y += r->d[1];
                        p->z += r->d[2];
                        
                        int index;
                        float minDistance;
                        for(int i = 0; i<CIRCLE_PTS_COUNT; i++)
                        {
                                if(i == 0)
                                {
                                        index = i;
                                        minDistance = p->getDistance(nextS->circlePts[i]);
                                }
                                else
                                {
                                        float currentDistance = p->getDistance(nextS->circlePts[i]);
                                        if(currentDistance < minDistance)
                                        {
                                                minDistance = currentDistance;
                                                index = i;
                                        }
                                }
                        }
                        s->index = index;
                        
                }
        }
};
#endif