#ifndef _COLONIZATIONMETHOD_H
#define _COLONIZATIONMETHOD_H
#include <cstdio>
#include <cstdlib>
#include "Node.h"
#include "Vector3d.h"
#include <vector>
#include <cassert>
using namespace std;

class GenerationMethod
{
  public:
    void init ();
    void generate ();
    Node *getNodes ();
};

class ColonizationMethod:public GenerationMethod
{
    static const float di = 4.0;	//influence distance
    static const float dk = 0.8;	//kill distance
    static const float D = 0.2;		//node length

   
 
    void removeAPoint (int index)
    {
	aPoints.erase (aPoints.begin () + index);
    }
    void colonize ()
    {
	int i, j, k, left = aPoints.size ();

	int runda = 0;

	while (left) {
	    printf ("runda %d left=%d\n", runda++, left);
	    printf ("old nodes size = %d\n", nodes.size ());
	    int added = 0;

	    int oldNodesSize = nodes.size ();

	    int oldleft = left;

	    for (i = 0; i < oldNodesSize; i++) {
		vector < int >influenting;

		for (j = 0; j < oldleft; j++) {
		    if (aPoints[j].getDistance (&nodes[i].point) <= di)	// if it is close enough
			influenting.push_back (j);

		}
		printf ("influenting %d\n", influenting.size ());
		if (influenting.size ()) {	//if there is at least one close to node v
		    puts ("na wezel");
		    nodes[i].point.print ();
		    puts ("wplywaja:");
		    Point3d p (0, 0, 0);

		    Node n (&p);

		    Vector3d sum (0, 0, 0);	//wyznacza kierunek nowego noda

		    for (k = 0; k < influenting.size (); k++) {
			int index = influenting[k];

			Point3d *s = &aPoints[index];

			s->print ();
			Vector3d p (&nodes[i].point);

			Vector3d tmp = Vector3d (s);

			Vector3d tmp2 = Vector3d (p);

			tmp.sub (&tmp2);
			sum.add (&tmp);
		    }
		    //  puts ("sum init:");
		    //  sum.print ();
		    //normalizacja
		    sum.normalize ();
		    //przeskalowanie do porzadanej dlugosci
		    sum.mul (D);
		    //   puts ("sum przeskalowany");
		    //    sum.print ();

		    Node newNode = nodes[i];

		    newNode.point.x += sum.d[0];
		    newNode.point.y += sum.d[1];
		    newNode.point.z += sum.d[2];
		    puts ("");
		    puts ("utworzony pkt");
		    newNode.point.print ();
		    //    getchar ();
		    added = 1;

		    int duplication = 0;

		    for (k = 0; k < nodes.size (); k++)
			if (newNode.isEqual (&nodes[k])) {
			    duplication = 1;
			    added = 0;

			}
		    if (duplication == 0)
			nodes.push_back (newNode);
		    assert (di >= dk);
		    for (k = 0; k < left; k++) {
			//      attractionPoints[influenting[k]].print();
			printf ("dist: %f\n", newNode.point.getDistance
				(&aPoints[k]));
			if (newNode.point.getDistance (&aPoints[k]) <= dk) {
			    puts ("usuwam");
			    removeAPoint (k);
			}
		    }
		    influenting.clear ();

		}


	    }
	    if (added == 0) {
		puts ("nie dodano nic nowego");
		break;
	    }

	}
    }

  public:
      vector < Node > nodes;		//tree nodes
      vector < Point3d > aPoints;	//attraction points


    void createAttractionPoints (Point3d * crownCenter, float crownRadius,
				 int points)
    {
	aPoints.clear ();
	while (points)
	{
#define MRAND (((random() % 2000) - 1000) / (float) 1000) * crownRadius
	    float rx = MRAND + crownCenter->x;
	    float ry = MRAND + crownCenter->y;
	    float rz = MRAND + crownCenter->z;
#undef MRAND
	    Point3d a (rx, ry, rz);
	    if (a.getDistance (crownCenter) <= crownRadius)
	    {
		aPoints.push_back (a);
		points--;
	    }
	}
    }
    void init ()
    {
	srand (42);
    }

    void generate (int aPointsNum)
    {
	Point3d crownCenter (0, 9.0, 0);

	float crownRadius = 5.0;

	createAttractionPoints (&crownCenter, crownRadius, aPointsNum);

	puts ("root:");
	int i;

	for (i = 0; i < 6; i++) {
	    nodes.push_back (Node (0, i, 0));
	    nodes[nodes.size () - 1].print ();
	}
	puts ("apoints:\n");
	for (i = 0; i < aPoints.size (); i++)
	    printf ("glVertex3f(%f,%f,%f);\n", aPoints[i].x,
		    aPoints[i].y, aPoints[i].z);

	getchar ();
	puts ("");
	colonize ();
	puts ("colonized");
	for (i = 0; i < nodes.size (); i++)
	    printf ("glVertex3f(%f,%f,%f);\n", nodes[i].point.x,
		    nodes[i].point.y, nodes[i].point.z);
	//nodes[i].point.print ();
    }

};
#endif
