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
  /*
  static const float di = 5.0;	//influence distance
    static const float dk = 0.6;	//kill distance
    static const float D = 0.3;	//node length
*/
    static const float di = 5.0;	//influence distance
    static const float dk = 0.1;	//kill distance
    static const float D = 0.2;	//node length

    void removeAPoint (int index)
    {
	aPoints.erase (aPoints.begin () + index);
    }
    void colonize ()
    {
	int i, j, k, left = aPoints.size ();

	int runda = 0;

	while (left && runda++ < 1000 && nodes.size () < 1000) {
	    left=aPoints.size();
	    printf ("runda %d left=%d\n", runda, left);
	    printf ("old nodes size = %d\n", nodes.size ());

	    bool added = 0;

	    int ids[left];	//index najblizszego noda

	    float dis[left];	//odl do najblizszego noda

	    for (i = 0; i < left; i++) {
		dis[i] = 10000;
		ids[i] = -1;
	    }
	    //znajdz nablizszego noda dla kazdego apointa
	    // O(n*m)
	    for (i = 0; i < left; i++) {
		Point3d *ap = &aPoints[i];

		for (j = 0; j < nodes.size (); j++) {
		    float newDistance = ap->getDistance (&nodes[j].point);
		  //  printf("odl(%d,%d)=%f\n",i,j,newDistance);
		    if (newDistance <= di && newDistance <= dis[i]) {
			dis[i] = newDistance;
			//printf("newdist=%f\n",dis[i]);
			ids[i] = j;
		    }
		}
	    }

	    static vector < int >N[1000];

	    for (i = 0; i < 1000; i++)
		N[i].clear ();

	    for (i = 0; i < left; i++)
	    {
	    //  printf("ids[%d] =  %d\n",i,ids[i]);
	      if(ids[i]!=-1)
	     N[ids[i]].push_back (i);
	    }
	    int oldNodesSize = nodes.size();
	    
	    for (i = 0; i < oldNodesSize; i++)
		if (N[i].size () != 0) {
		    puts("dla noda");
		    nodes[i].print();
		    Vector3d sum (0, 0, 0);

		    puts("znalazlem");
		    for (j = 0; j < N[i].size (); j++) {
			
			int index = N[i][j];
			aPoints[index].print();
			Point3d *s = &aPoints[index];

			Vector3d p (&nodes[i].point);

			Vector3d tmp = Vector3d (s);

			Vector3d tmp2 = Vector3d (p);

			tmp.sub (&tmp2);
			sum.add (&tmp);
		    }
		    sum.normalize ();
		    sum.mul (D);

		    Node newNode = nodes[i];

		    newNode.point.x += sum.d[0];
		    newNode.point.y += sum.d[1];
		    newNode.point.z += sum.d[2];

		    added = 1;
		    int duplication = 0;

		    for (k = 0; k < nodes.size (); k++)
			if (newNode.isEqual (&nodes[k])) {
			    duplication = 1;
			    added = 0;

			}
		    if (duplication == 0) {
			nodes.push_back (newNode);
			puts ("utworzony pkt");
			newNode.point.print ();
			
			for (j = 0; j < aPoints.size(); j++) {
			    if (newNode.point.getDistance (&aPoints[j]) <= dk) {
				puts ("usuwam");
				removeAPoint (j--);
			    }

			}
		    }

		}

	    if (added == 0) {
		puts ("nie dodano nic nowego");
		break;
	    }
	}
	//end while(left)

	/*   for (i = 0; i < oldNodesSize; i++) {
	   vector < int >influenting;

	   for (j = 0; j < oldleft; j++) {
	   if (aPoints[j].getDistance (&nodes[i].point) <= di) // if it is close enough
	   influenting.push_back (j);

	   }
	   printf ("influenting %d\n", influenting.size ());
	   if (influenting.size ()) {  //if there is at least one close to node v
	   puts ("na wezel");
	   nodes[i].point.print ();
	   puts ("wplywaja:");
	   Point3d p (0, 0, 0);

	   Node n (&p);

	   Vector3d sum (0, 0, 0);     //wyznacza kierunek nowego noda

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

	   } */
    }

  public:
    vector < Node > nodes;	//tree nodes
    vector < Point3d > aPoints;	//attraction points
    vector < Point3d > aPointsCopy;	//attraction points


    void createAttractionPoints (Point3d * crownCenter, float crownRadius,
				 int points)
    {
	aPoints.clear ();
	while (points) {
#define MRAND (((random() % 2000) - 1000) / (float) 1000) * crownRadius
	    float rx = MRAND + crownCenter->x;

	    float ry = MRAND + crownCenter->y;

	    float rz = MRAND + crownCenter->z;

#undef MRAND
	    Point3d a (rx, ry, rz);

	    if (a.getDistance (crownCenter) <= crownRadius) {
		aPoints.push_back (a);
		aPointsCopy.push_back(a);
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

	//puts ("root:");
	int i;

	for (i = 0; i < 6; i++) {
	    nodes.push_back (Node (0, i, 0));
	//    nodes[nodes.size () - 1].print ();
	}
	//puts ("apoints:\n");
	//for (i = 0; i < aPoints.size (); i++)
	//    printf ("glVertex3f(%f,%f,%f);\n", aPoints[i].x,
	//	    aPoints[i].y, aPoints[i].z);

	//getchar ();
	puts ("");
	colonize ();
	puts ("colonized");
	//for (i = 0; i < nodes.size (); i++)
	//    printf ("glVertex3f(%f,%f,%f);\n", nodes[i].point.x,
	//	    nodes[i].point.y, nodes[i].point.z);
	//nodes[i].point.print ();
    }

};
#endif
