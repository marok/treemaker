#ifndef _MODEL3D_H
#define	_MODEL3D_H

#include <cmath>
#include <vector>
#include "TrunkParameters.h"
#include "LeafModel.h"
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
	BranchModel *bm;
	Parameters *params;
	

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

			computeOneSegment(norm, nm, i!=0?bm->nodeModelList[i-1]:NULL);

			delete(norm);
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			computeSegment(bm->childBranches[i]);
		}

	}

	//liczy pojedynczy segment dla NodeModel
	//norm - wektor normalny do powierzchni wyznaczonej przez segment
	void computeOneSegment(Vector3d *norm, NodeModel *nodeModel, NodeModel *prvNode)
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


		float angle = 2 * M_PI / params->tp->circlePoints;

		Segment *segment = nodeModel->segment;

		float angleDiff = 0;
		bool angleDiffComputed = false;
		
		for (int i = 0; i < params->tp->circlePoints; i++) {
			Vector3d *u, *v;
			u = new Vector3d();
			v = new Vector3d();
			u->add(U);
			v->add(V);

			u->mul(nodeModel->r * cos((float) i * angle+angleDiff));
			v->mul(nodeModel->r * sin((float) i * angle+angleDiff));
			u->add(v);

			segment->circlePts[i] = new Point3d();
			segment->circlePts[i]->x = u->d[0];
			segment->circlePts[i]->y = u->d[1];
			segment->circlePts[i]->z = u->d[2];

			//liczymy różnice pomiędzy obliczonym wektorem a wektorem z poprzedniego segmentu
			if(prvNode!=NULL && i==0 && !angleDiffComputed)
			{
				//wektory a i b wyznaczają płaszczyznę na której powinien się znaleźć punkt z nastepnego segmentu
				Vector3d *a = new Vector3d(prvNode->segment->circlePts[0]);
				Vector3d *b = new Vector3d(prvNode->position, nodeModel->position);
				Vector3d *n = a->crossProduct(b);
				n->normalize();
				
				Vector3d *c = new Vector3d(nodeModel->segment->circlePts[0]);
			
				angleDiff = M_PI_2 - acos(n->dotProduct(c)/(n->length()*c->length()));
				angleDiffComputed = true;
				i--;
				
				delete a;
				delete b;
				delete n;
				delete c;
			}
			
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
                        for (int i = 0; i < params->tp->circlePoints; i++) {
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
				root->r +=  pow(computeRadius(root->getChildAt(i),rec+1), params->tp->radiusFactor);
			}

			root->r = pow(root->r, 1.0/params->tp->radiusFactor);
			root->r = root->r*params->tp->mValue+params->tp->aValue;
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


	//wygładza gałąź od węzła o indeksie start do end włącznie
	//start == -1 - od początku; end == -1 do końca
	//zwraca liczbę dodanych elementów
	int smoothBranch(BranchModel *bm, int start, int end)
	{

		//wywołania rekursywne 
		if(selection->applyForChildren)
		{
			//dla całej gałęzi
			if(start == -1 && end == -1)
			{
				for(unsigned int i=0; i<bm->childBranches.size(); i++)
					smoothBranch(bm->childBranches[i], -1, -1);
			} else //musimy sprawdzić które branche wychodzą z zaznaczonego fragmentu
			{
				int lastNodeIndex = end == -1? bm->nodeModelList.size()-1: end;
				for(int i = start; i <= lastNodeIndex; i++)
				{
					vector<BranchModel*> childBranches = branchesStartedInNode(bm, bm->nodeModelList[i]);
					for(unsigned j = 0; j < childBranches.size(); j++)
						smoothBranch(childBranches[j], -1, -1);
				}
			}
		}
		
		int addedNodes = 0;
		
		Point3d points[3];

		if(start == 0)
			start++;
		
		if(start == -1)
			start = 1;
		if(end == -1)
			end = (int) bm->nodeModelList.size()-1;
		
		if(end < (int)bm->nodeModelList.size()-1)
			end++;
		
		
		
		//pomijamy pierwszy i ostatni element
		for(int i=start; i<end; i++)
		{

			if(i==start)
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

			if(i == 1)
			{
				Point3d *left = new Point3d(
				    0.5*points[0].x + 0.5*points[1].x,
				    0.5*points[0].y + 0.5*points[1].y,
				    0.5*points[0].z + 0.5*points[1].z
				);
				NodeModel *leftNode = new NodeModel(left, (bm->nodeModelList[i]->r+bm->nodeModelList[i-1]->r)/2);
				bm->nodeModelList.insert(bm->nodeModelList.begin()+i, leftNode);
				i++;
				end++;
				addedNodes++;
				
				bm->nodeModelList[i]->position->x = 0.75*points[1].x + 0.25*points[2].x;
				bm->nodeModelList[i]->position->y = 0.75*points[1].y + 0.25*points[2].y;
				bm->nodeModelList[i]->position->z = 0.75*points[1].z + 0.25*points[2].z;

			} else if(i == (int)bm->nodeModelList.size()- 2)
			{
				Point3d *left = new Point3d(
				    0.25*points[0].x + 0.75*points[1].x,
				    0.25*points[0].y + 0.75*points[1].y,
				    0.25*points[0].z + 0.75*points[1].z
				);
				NodeModel *leftNode = new NodeModel(left, (bm->nodeModelList[i]->r+bm->nodeModelList[i-1]->r)/2);
				bm->nodeModelList.insert(bm->nodeModelList.begin()+i, leftNode);
				i++;
				end++;
				addedNodes++;
				
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
				NodeModel *leftNode = new NodeModel(left, (bm->nodeModelList[i]->r+bm->nodeModelList[i-1]->r)/2);
				bm->nodeModelList.insert(bm->nodeModelList.begin()+i, leftNode);
				i++;
				end++;
				addedNodes++;
				
				bm->nodeModelList[i]->position->x = 0.75*points[1].x + 0.25*points[2].x;
				bm->nodeModelList[i]->position->y = 0.75*points[1].y + 0.25*points[2].y;
				bm->nodeModelList[i]->position->z = 0.75*points[1].z + 0.25*points[2].z;
			}
		}
		
		return addedNodes;
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
	//Zwraca liczbę usunietych węzłów, wartosc ma sens tylko w trybie zaznaczania point_point
	int decBranchResolution(BranchModel *bm, int start, int end)
	{
		
		
		int deletedNodes = 0;
		if(selection->applyForChildren)
		{
			if(start == -1 && end == -1)
				for(unsigned int i=0; i<bm->childBranches.size(); i++)
					decBranchResolution(bm->childBranches[i],-1,-1);
			else
			{
				for(int i=start; i<end; i++)
				{
					vector<BranchModel *> b = branchesStartedInNode(bm,bm->nodeModelList[i]);
					for(unsigned int j=0; j<b.size(); j++)
						decBranchResolution(b[j], -1, -1);
				}
			}
		}
		
		
		if(bm->nodeModelList.size() <= 2)
			return 0;
		
		
		if(start == -1)
			start = 0;
		if(end == -1)
			end = bm->nodeModelList.size()-1;
		
		//pętlę inkrementujemy o 1 ponieważ w każdej inkrementacji usuwamy bieżący element
		for(int i=start+1; i < end; i++)
		{
			//sprawdzamy czy z węzła nie wychodzi żadna gałąź
			//jeśli tak to przesuwamy te gałęzie w górę
			vector<BranchModel *> branchesToMove = branchesStartedInNode(bm,bm->nodeModelList[i]);
			for(unsigned int j =0 ; j<branchesToMove.size(); j++)
			{
				branchesToMove[j]->position = bm->nodeModelList[i+1]->position;
			}

			NodeModel *node2delete = bm->nodeModelList[i];
			bm->nodeModelList.erase(bm->nodeModelList.begin()+i);
			delete node2delete;
			
			//zmniejszamy end ponieważ ubywa elementów w kolekcji
			end--;
			deletedNodes++;
		}
		
		return deletedNodes;
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
	}
	
	void cutBranch(BranchModel *bm, int nodeToCutIndex)
	{
		if(nodeToCutIndex == -1)
		{
			removeBranch(bm);
			return;
		}
		
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
	
	void computeLeavesPosition()
	{
		int leavesCnt = params->lp->leavesCount;
		
		while(leavesCnt)
		{
			int branchId = rand() % branches.size();
			BranchModel *branch = branches[branchId];
			int nodeId = sqrt((float)rand()/(float)RAND_MAX)*(branch->nodeModelList.size()-1) + 1; //nie możemy wylosować 0. elementu
			NodeModel *node = branch->nodeModelList[nodeId];
			if(!node->leaf)
			{
				node->leaf = new LeafModel();
				
				Point3d nodeP = branch->getAbsoluteNodePosition(node);
				node->leaf->norm = new Vector3d(branches[0]->position, &nodeP);
				node->leaf->norm->normalize();
				
				Vector3d gravity = Vector3d(0,0,-0.9*(float)rand()/(float)RAND_MAX);
				node->leaf->norm->add(&gravity);
				node->leaf->norm->normalize();
				
				Vector3d v = Vector3d(node->position, branch->nodeModelList[nodeId-1]->position);
				
				node->leaf->dir = node->leaf->norm->crossProduct(&v);
				node->leaf->dir->normalize();
				
				leavesCnt--;
			}
		}
	}
	
	void clearLeaves()
	{
		for(unsigned int i = 0; i < branches.size(); i++)
		{
			BranchModel *branch = branches[i];
			for(unsigned int j=0; j<branch->nodeModelList.size(); j++)
			{
				if(branch->nodeModelList[j]->leaf)
				{
					delete branch->nodeModelList[j]->leaf;
					branch->nodeModelList[j]->leaf = NULL;
				}
			}
		}
	}
	
	void addGravityToTheBranch(BranchModel *bm, float factor)
	{
		for(unsigned int i=0; i<bm->nodeModelList.size(); i++)
		{
			Point3d castOZ = Point3d(0,0,bm->nodeModelList[i]->position->z);
			bm->nodeModelList[i]->position->z -= (factor*(float)i*castOZ.getDistance(bm->nodeModelList[i]->position));
		}
		
		if(selection->applyForChildren)
		{
			for(unsigned int i = 0; i<bm->childBranches.size(); i++)
				addGravityToTheBranch(bm->childBranches[i], factor);
		}
			
	}
	
	void subGravityToTheBranch(BranchModel *bm, float factor)
	{
		for(int i=(int)bm->nodeModelList.size()-1; i >= 0; i--)
		{
			Point3d castOZ = Point3d(0,0,bm->nodeModelList[i]->position->z);
			bm->nodeModelList[i]->position->z += (factor*(float)i*castOZ.getDistance(bm->nodeModelList[i]->position));
		}
		
		if(selection->applyForChildren)
		{
			for(int i = (int) bm->childBranches.size()-1; i>=0; i--)
				subGravityToTheBranch(bm->childBranches[i], factor);
		}
	}
	

public:
	vector<BranchModel *> branches; //tablica zawierająca wszystkie gałęzie

	BranchSelection *selection;
	
	Model3d(Node *root, Parameters *params) {
		this->root = root;
		this->params = params;
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
		computeLeavesPosition();
	}
	
	void refreshModel()
	{
		computeSegment(this->bm);
		computeConnectedPts(this->bm);
		clearLeaves();
		computeLeavesPosition();
	}
	
	void removeMarkedBranch()
	{
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *markedBranch = branches[i];
			
			
			if(selection->getMode() == SELECTION_ALL)
			{
				removeBranch(markedBranch);
			} else
			{
				cutBranch(markedBranch, selection->getMarkedNodes()[0]);
			}
			
			branches.clear();
			populateVectorBranches(this->bm);
			
			selection->unmarkBranch();
		}
	}
	
	void smoothMarkedBranch()
	{
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *marked = branches[i];
			if(selection->getMode() == SELECTION_ALL)
				this->smoothBranch(marked, -1, -1);
			else if(selection->getMode() == SELECTION_POINT_END)
				this->smoothBranch(marked, selection->getMarkedNodes()[0], -1);
			else if(selection->getMode() == SELECTION_POINT_POINT)
			{
				int addedNodes;
				addedNodes = this->smoothBranch(marked, selection->getMarkedNodes()[0], selection->getMarkedNodes()[1]);
				selection->incMarkedArea(addedNodes);
			}
			
			computeSegment(marked);
			computeConnectedPts(marked);
		}
	}
	
	void addGravity()
	{
		const float factor = 0.01;
		
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *marked = branches[i];
			if(selection->getMode() == SELECTION_ALL)
				addGravityToTheBranch(marked,factor);
		}
	}
	
	void subGravity()
	{
		const float factor = 0.01;
		
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *marked = branches[i];
			if(selection->getMode() == SELECTION_ALL)
				subGravityToTheBranch(marked,factor);
		}
	}
	
	void decMarkedBranchResolution()
	{
		int i = selection->getMarkedBranch();
		if(i != -1)
		{
			BranchModel *marked = branches[i];
			if(selection->getMode() == SELECTION_ALL)
				decBranchResolution(marked,-1,-1);
			else if(selection->getMode() == SELECTION_POINT_END)
				decBranchResolution(marked,selection->getMarkedNodes()[0],-1);
			else if(selection->getMode() == SELECTION_POINT_POINT)
			{
				int nodesDeleted;
				nodesDeleted = decBranchResolution(marked,selection->getMarkedNodes()[0],selection->getMarkedNodes()[1]);
				selection->decMarkedArea(nodesDeleted);
			}
			computeSegment(marked);
			computeConnectedPts(marked);
		}
	}
};
#endif