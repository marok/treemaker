#ifndef _MODEL3D_H
#define	_MODEL3D_H

#include <cmath>
#include <vector>
#include "TrunkParameters.h"
#include "NodeModel.h"
#include "BranchModel.h"
#include "BranchSelection.h"

using namespace std;

#define CHILD_RADIUS 0.01

//Klasa Model3d zawiera obiekty typu Node(wygenerowane przez odpowednią metodę generowania drzew).
//Składa się z obiektów BranchModel. Każdy obiekt BranchModel zawiera vector obiektów NodeModel. Branch model jest gałęzią.
//Gałąź nie rozgałęzia się. Każde rozgałęzienie jest symbolizowane przez kolejne obiekty BranchModel.
//Obiekty Node są zamienione z reprezentacji bezwzględnej(współrzędne względem (0,0,0)) na reprezentację względną(współrzędne względem pola position)
class Model3d {
	Node *root;
	TrunkParameters *tp;
	BranchModel *bm;

	void computeSegment(BranchModel *bm) {
		Vector3d *norm;

		int nodeModelListLen = bm->nodeModelList.size();
		for(int i=0; i<nodeModelListLen; i++)
		{
			NodeModel *nm = bm->nodeModelList[i];
			if(i==0)
			{
				norm = new Vector3d(nm->position, bm->nodeModelList[1]->position);
			} else if(i == nodeModelListLen-1)
			{
				norm = new Vector3d(bm->nodeModelList[i-1]->position, nm->position);
			} else
			{
				Vector3d *fromParent = new Vector3d(bm->nodeModelList[i-1]->position, nm->position);
				fromParent->normalize();
				Vector3d *toChild = new Vector3d(nm->position, bm->nodeModelList[i+1]->position);
				toChild->normalize();

				norm = new Vector3d();
				norm->add(fromParent);
				norm->add(toChild);

				delete(fromParent);
				delete(toChild);
			}
			norm->normalize();

			computeOneSegment(norm, nm);

			delete(norm);
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			computeSegment(bm->childBranches[i]);
		}

	}

	//liczy pojedynczy segment dla NodeModel
	//norm - wektor normalny do powierzchni wyznaczonej przez segment
	void computeOneSegment(Vector3d *norm, NodeModel *nodeModel)
	{
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


		float angle = 2 * M_PI / tp->circlePoints;

		Segment *segment = nodeModel->segment;

		for (int i = 0; i < tp->circlePoints; i++) {
			Vector3d *u, *v;
			u = new Vector3d();
			v = new Vector3d();
			u->add(U);
			v->add(V);

			u->mul(nodeModel->r * cos((float) i * angle));
			v->mul(nodeModel->r * sin((float) i * angle));
			u->add(v);

			segment->circlePts[i] = new Point3d();
			segment->circlePts[i]->x = u->d[0];
			segment->circlePts[i]->y = u->d[1];
			segment->circlePts[i]->z = u->d[2];

			//printf("%d. %f %f %f %f %f %f\n",i,node->point.x,node->point.y,node->point.z, node->segment->circlePts[i]->x,node->segment->circlePts[i]->y,node->segment->circlePts[i]->z);

			delete(u);
			delete(v);
		}
		delete(U);
		delete(V);
	}

	void computeConnectedPts(BranchModel *bm)
	{

		int nodeModelListLen = bm->nodeModelList.size();

		for(int i=0; i<nodeModelListLen - 1; i++)
		{
			NodeModel *root = bm->nodeModelList.at(i);
			NodeModel *child = bm->nodeModelList.at(i+1);

			Vector3d *r = new Vector3d(root->segment->circlePts[0]);
			Point3d *p = new Point3d(child->position);
			p->x += r->d[0];
			p->y += r->d[1];
			p->z += r->d[2];

			int index;
			float minDistance;
			for (int i = 0; i < tp->circlePoints; i++) {
				if (i == 0) {
					index = i;
					Point3d segPoint = child->getSegmentPointRel2BranchAt(i);
					minDistance = p->getDistance(&segPoint);
				} else {
					Point3d segPoint = child->getSegmentPointRel2BranchAt(i);
					float currentDistance = p->getDistance(&segPoint);
					if (currentDistance < minDistance) {
						minDistance = currentDistance;
						index = i;
					}
				}
			}
			child->segment->index = index;
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			computeConnectedPts(bm->childBranches.at(i));
		}
	}


	float computeRadius(Node *root,int rec)
	{
		int childLen = root->getChildLen();
		root->r = 0.0;
		if(childLen > 0)
		{
			for(int i=0; i< childLen; i++)
			{
				root->r +=  pow(computeRadius(root->getChildAt(i),rec+1), tp->radiusFactor);
			}

			root->r = pow(root->r*tp->mValue+tp->aValue, 1.0/tp->radiusFactor);

		} else
		{
			root->r = CHILD_RADIUS;
		}
		if(rec==0)
			root->r*=1.3;
		if(rec==1)
			root->r*=1.2;
		if(rec==2)
			root->r*=1.1;
		return root->r;
	}


	//zwraca dziecko o największym promieniu
	static Node* findMaxChild(Node *parent)
	{
		float maxR;
		Node* result = NULL;
		for(int j=0; j<parent->getChildLen(); j++)
		{
			if(!result)
			{
				result = parent->getChildAt(j);
				maxR = result->r;
			} else if(parent->getChildAt(j)->r > maxR)
			{
				result = parent->getChildAt(j);
				maxR = result->r;
			}
		}
		return result;
	}

	BranchModel* node2BranchModel(Node *root, BranchModel *parentBranch, Point3d* branchPosition)
	{
		Node *node = root;

		BranchModel *branch = new BranchModel(parentBranch, branchPosition);
		branches.push_back(branch);

		if(node->prev)
		{
			branch->addNewNodeModel(node->prev);
		}

		while(node)
		{
			Point3d* p = branch->addNewNodeModel(node);

			int nodeChildrenLen = node->getChildLen();

			if(nodeChildrenLen > 1)
			{
				Node *maxNode = findMaxChild(node);
				for(int i=0; i<nodeChildrenLen; i++)
				{
					Node *childNode = node->getChildAt(i);
					if(childNode != maxNode)
						branch->addChildBranch(node2BranchModel(childNode, branch, p));
				}
				node = maxNode;

			} else if(nodeChildrenLen == 1)
			{
				node = node->getChildAt(0);
			}
			else
				node = NULL;
		}

		if(parentBranch)
			branch->nodeModelList[0]->r = branch->nodeModelList[1]->r;
		return branch;
	}


	void smoothBranch(BranchModel *bm)
	{

		Point3d points[3];

		//pomijamy pierwszy i ostatni element
		for(unsigned int i=1; i<bm->nodeModelList.size()-1; i++)
		{

			if(i==1)
			{
				points[0] = *bm->nodeModelList[i-1]->position;
				points[1] = *bm->nodeModelList[i]->position;
				points[2] = *bm->nodeModelList[i+1]->position;
			} else
			{
				points[0] = points[1];
				points[1] = points[2];
				points[2] = *bm->nodeModelList[i+1]->position;
			}

//			printf("%d\n",bm->nodeModelList.size());
//			for(unsigned int j=0; j<bm->nodeModelList.size(); j++)
//			{
//				bm->nodeModelList[j]->position->print();
//			}

			if(i == 1)
			{
				Point3d *left = new Point3d(
				    0.5*points[0].x + 0.5*points[1].x,
				    0.5*points[0].y + 0.5*points[1].y,
				    0.5*points[0].z + 0.5*points[1].z
				);
				NodeModel *leftNode = new NodeModel(left, bm->nodeModelList[i]->r);
				bm->nodeModelList.insert(bm->nodeModelList.begin()+i, leftNode);
				i++;

				bm->nodeModelList[i]->position->x = 0.75*points[1].x + 0.25*points[2].x;
				bm->nodeModelList[i]->position->y = 0.75*points[1].y + 0.25*points[2].y;
				bm->nodeModelList[i]->position->z = 0.75*points[1].z + 0.25*points[2].z;

			} else if(i == bm->nodeModelList.size()- 2)
			{
				Point3d *left = new Point3d(
				    0.25*points[0].x + 0.75*points[1].x,
				    0.25*points[0].y + 0.75*points[1].y,
				    0.25*points[0].z + 0.75*points[1].z
				);
				NodeModel *leftNode = new NodeModel(left, bm->nodeModelList[i]->r);
				bm->nodeModelList.insert(bm->nodeModelList.begin()+i, leftNode);
				i++;

				bm->nodeModelList[i]->position->x = 0.5*points[1].x + 0.5*points[2].x;
				bm->nodeModelList[i]->position->y = 0.5*points[1].y + 0.5*points[2].y;
				bm->nodeModelList[i]->position->z = 0.5*points[1].z + 0.5*points[2].z;
			} else
			{
				Point3d *left = new Point3d(
				    0.25*points[0].x + 0.75*points[1].x,
				    0.25*points[0].y + 0.75*points[1].y,
				    0.25*points[0].z + 0.75*points[1].z
				);
				NodeModel *leftNode = new NodeModel(left, bm->nodeModelList[i]->r);
				bm->nodeModelList.insert(bm->nodeModelList.begin()+i, leftNode);
				i++;

				bm->nodeModelList[i]->position->x = 0.75*points[1].x + 0.25*points[2].x;
				bm->nodeModelList[i]->position->y = 0.75*points[1].y + 0.25*points[2].y;
				bm->nodeModelList[i]->position->z = 0.75*points[1].z + 0.25*points[2].z;
			}
		}
	}
	
	//funkcja zwraca wszystkie gałązie startujące w danym węźle
	//parentBranch - gałąź na której leży węzeł nm
	//WARNING! szybkie to to nie jest xD - aby ulepszyć trzeba trochę przebudować Model3d
	vector<BranchModel *> branchesStartedInNode(BranchModel *parentBranch, NodeModel *nm)
	{
		vector<BranchModel *> result;
		for(unsigned int i=0; i<parentBranch->childBranches.size(); i++)
		{
			if(parentBranch->childBranches[i]->position == nm->position)
				result.push_back(parentBranch->childBranches[i]);
		}
		
		return result;
	}
	
	//Usuwa co drugi węzeł gałęzi. Jeśli z węzła wychodzi inna gałąź to gałąź jest przesuwana w górę gałęzi.
	void decBranchResolution(BranchModel *bm)
	{
		
		//pętlę inkrementujemy o 1 ponieważ w każdej inkrementacji usuwamy bieżący element
		for(unsigned int i=1; i < bm->nodeModelList.size() - 1; i++)
		{
			//sprawdzamy czy z węzła nie wychodzi żadna gałąź
			//jeśli tak to przesuwamy te gałęzie w górę
			vector<BranchModel *> branchesToMove = branchesStartedInNode(bm,bm->nodeModelList[i]);
			for(unsigned int j =0 ; j<branchesToMove.size(); j++)
				branchesToMove[j]->position = bm->nodeModelList[i+1]->position;

			NodeModel *node2delete = bm->nodeModelList[i];
			bm->nodeModelList.erase(bm->nodeModelList.begin()+i);
			delete node2delete;
		}
	}

	void printBranches(BranchModel *bm, int ind)
	{
		printf("%d branch: ",ind);
		bm->position->print();
		Point3d abs = bm->getAbsolutePosition();
		printf("branch abs: ");
		abs.print();

		for (unsigned int j = 0; j < bm->nodeModelList.size(); j++) {
			NodeModel *node = bm->nodeModelList[j];
			printf("Node ");
			node->position->print();
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			printBranches(bm->childBranches.at(i), ind+1);
		}
	}
	
	void populateVectorBranches(BranchModel *bm)
	{
		if(!bm)
			return;
		branches.push_back(bm);
		for(unsigned int i=0; i<bm->childBranches.size(); i++)
			populateVectorBranches(bm->childBranches[i]);
	}
	
	void removeBranch(BranchModel *branch)
	{
		BranchModel *parentBranch = branch->parentBranch;
		if (parentBranch) {
			for (unsigned int i = 0; i < parentBranch->childBranches.size(); i++)
				if (parentBranch->childBranches[i] == branch) {
					parentBranch->childBranches.erase(parentBranch->childBranches.begin() + i);
					break;
				}
		} else
			this->bm = NULL;

		delete branch;

		branches.clear();
		populateVectorBranches(this->bm);
	}
	
	void cutBranch(BranchModel *bm, int nodeToCutIndex)
	{
		
		for(unsigned int i = nodeToCutIndex; i<bm->nodeModelList.size(); i++)
		{
			NodeModel *nodeToCut = bm->nodeModelList[i];
			vector<BranchModel *> branchesToRemove = branchesStartedInNode(bm, nodeToCut);
			for(unsigned int j=0; j<branchesToRemove.size(); j++)
			{
				removeBranch(branchesToRemove[j]);
			}
			delete bm->nodeModelList[i];
		}
		bm->nodeModelList.erase(bm->nodeModelList.begin()+nodeToCutIndex, bm->nodeModelList.end());
	}
	

public:
	vector<BranchModel *> branches; //tablica zawierająca wszystkie gałęzie

	BranchSelection *selection;
	
	Model3d(Node *root,TrunkParameters *tp) {
		this->root = root;
		this->tp=tp;
		this->bm = NULL;
		
		selection = new BranchSelection();
	}

	~Model3d()
	{
		delete this->bm;
		delete this->selection;
	}

	BranchModel *getRootBranch()
	{
		return this->bm;
	}

	void generateModel()
	{
		if(this->bm)
			delete this->bm;
		branches.clear();

		computeRadius(root,0);
		this->bm = node2BranchModel(root, NULL, new Point3d());
		
		computeSegment(this->bm);
		computeConnectedPts(this->bm);
	}
	
	void removeMarkedBranch()
	{
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *markedBranch = branches[i];
			
			int j = selection->getMarkedNode();
			if(j <= 1)
			{
				removeBranch(markedBranch);
			} else
			{
				cutBranch(markedBranch, j);
			}
			
			selection->unmarkBranch();
		}
	}
	
	void smoothMarkedBranch()
	{
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *marked = branches[i];
			this->smoothBranch(marked);
			computeSegment(marked);
			computeConnectedPts(marked);
		}
	}
	
	void decMarkedBranchResolution()
	{
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *marked = branches[i];
			decBranchResolution(marked);
			computeSegment(marked);
			computeConnectedPts(marked);
		}
	}
};
#endif