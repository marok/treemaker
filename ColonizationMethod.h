#ifndef _COLONIZATIONMETHOD_H
#define _COLONIZATIONMETHOD_H
#include <cstdio>
#include <cstdlib>
#include "Node.h"
#include "Vector3d.h"
#include <vector>
#include <cassert>
#include "MethodParameters.h"
#include "Crown.h"
using namespace std;

class GenerationMethod
{
public:
	void init ();
	void generate ();
	Node *getRoot ();
};

class ColonizationMethod:public GenerationMethod
{

	int points;//attraction points


	void removeAPoint (int index)
	{
		Point3d *p = aPoints[index];
		delete p;
		aPoints.erase (aPoints.begin () + index);
	}
	void colonize ()
	{
		unsigned int i, j, k, left = aPoints.size ();

		int runda = 0;

		while (left && runda++ < 2000 && nodes.size () < 2000) {
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
				Point3d *ap = aPoints[i];

				for (j = 0; j < nodes.size (); j++) {
					float newDistance = ap->getDistance (&nodes[j]->point);

					if (newDistance <= params->di && newDistance <= dis[i]) {
						dis[i] = newDistance;
						ids[i] = j;
					}
				}
			}

			static vector < int >N[2000];

			for (i = 0; i < 2000; i++)
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

						Point3d *s = aPoints[index];

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
							if (newNode->point.getDistance (aPoints[j]) <= params->dk) {
								removeAPoint (j--);
							}
						}
					} else
						delete newNode;

				}

			if (added == 0) {
				//puts ("nie dodano nic nowego");
				break;
			}
		}
	}

public:
	vector < Node *> nodes;	//tree nodes
	vector < Point3d *> aPoints;	//attraction points

	MethodParameters *params;
	Crown *crown;

	ColonizationMethod(MethodParameters *params)
	{
		this->params = params;
		crown = new Crown(params);
	}

	void init ()
	{
		for(unsigned int i=0; i<nodes.size(); i++)
			delete nodes[i];
		nodes.clear();
		aPoints.clear();
		srand (params->seed);
	}

	void generate ()
	{
		delete crown;
		crown = new Crown(params);

		aPoints = crown->generatePoints();

		int i;

		Node *prvNode = NULL;
		for (i = 0; i < 10; i++) {
			Node *newNode = new Node(0,0,i*0.1);
			newNode->prev = prvNode;

			if(prvNode)
				prvNode->addChildren(newNode);

			nodes.push_back (newNode);
			prvNode = newNode;
		}
		colonize ();
	}
	Node *getRoot() {
		return (nodes.size()==0)?NULL:nodes[0];
	}


};
#endif
