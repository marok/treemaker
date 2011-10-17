#ifndef _COLONIZATIONMETHOD_H
#define _COLONIZATIONMETHOD_H
#include <cstdio>
#include <cstdlib>
#include "Node.h"
#include "Vector3d.h"
#include <vector>
#include <cassert>
#include "ColonizationParameters.h"
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

	int points;//attraction points


	void removeAPoint (int index)
	{
		aPoints.erase (aPoints.begin () + index);
	}
	void colonize ()
	{
		unsigned int i, j, k, left = aPoints.size ();

		int runda = 0;

		while (left && runda++ < 1000 && nodes.size () < 1000) {
			left = aPoints.size ();

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
					float newDistance = ap->getDistance (&nodes[j]->point);

					if (newDistance <= params->di && newDistance <= dis[i]) {
						dis[i] = newDistance;
						ids[i] = j;
					}
				}
			}

			static vector < int >N[1000];

			for (i = 0; i < 1000; i++)
				N[i].clear ();

			for (i = 0; i < left; i++) {
				if (ids[i] != -1)
					N[ids[i]].push_back (i);
			}
			unsigned int oldNodesSize = nodes.size ();

			for (i = 0; i < oldNodesSize; i++)
				if (N[i].size () != 0) {
					Vector3d sum (0, 0, 0);


					for (j = 0; j < N[i].size (); j++) {

						int index = N[i][j];

						Point3d *s = &aPoints[index];

						Vector3d p (&nodes[i]->point);

						Vector3d tmp = Vector3d (s);

						Vector3d tmp2 = Vector3d (p);

						tmp.sub (&tmp2);
						sum.add (&tmp);
					}
					sum.normalize ();
					sum.mul (params->D);

					Node *newNode = new Node(&nodes[i]->point);

					newNode->point.x += sum.d[0];
					newNode->point.y += sum.d[1];
					newNode->point.z += sum.d[2];

					added = 1;
					int duplication = 0;

					for (k = 0; k < nodes.size (); k++)
						if (newNode->isEqual (nodes[k])) {
							duplication = 1;
							added = 0;

						}
					if (duplication == 0) {
						nodes.push_back (newNode);

						nodes.at(nodes.size()-1)->prev = nodes[i];
						nodes[i]->addChildren(nodes.at(nodes.size()-1));

						for (j = 0; j < aPoints.size (); j++) {
							if (newNode->point.getDistance (&aPoints[j]) <= params->dk) {
								//                      puts ("usuwam");
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
	}

public:
	vector < Node *> nodes;	//tree nodes
	vector < Point3d > aPoints;	//attraction points

	ColonizationParameters *params;

	ColonizationMethod(ColonizationParameters *params)
	{
		this->params = params;
	}

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
				points--;
			}
		}

	}
	void init ()
	{
		nodes.clear();
		aPoints.clear();
		srand (params->seed);
	}

	void generate ()
	{
		Point3d crownCenter (0, 9.0, 0);

		createAttractionPoints (&crownCenter, params->crownRadius, params->points);

		int i;

		Node *prvNode = NULL;
		for (i = 0; i < 6; i++) {
			Node *newNode = new Node(0,i,0);
			newNode->prev = prvNode;

			if(prvNode)
				prvNode->addChildren(newNode);

			nodes.push_back (newNode);
			prvNode = newNode;
		}
		colonize ();
	}

};
#endif
