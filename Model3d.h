#ifndef _MODEL3D_H
#define	_MODEL3D_H

#include <cmath>
#include "TrunkParameters.h"
#include "NodeModel.h"
#include "BranchModel.h"

#define CHILD_RADIUS 0.02

class Model3d {
	Node *root;
	TrunkParameters *tp;

	void computeSegment(BranchModel *bm) {
		Vector3d *norm;

		int nodeModelListLen = bm->nodeModelList.size();
		for(int i=0; i<nodeModelListLen; i++)
		{
			NodeModel *nm = bm->nodeModelList.at(i);
			if(i==0)
			{
				norm = new Vector3d(&nm->node->point, &bm->nodeModelList.at(1)->node->point);
			} else if(i == nodeModelListLen-1)
			{
				norm = new Vector3d(&bm->nodeModelList.at(i-1)->node->point, &nm->node->point);
			} else
			{
				Vector3d *fromParent = new Vector3d(&bm->nodeModelList.at(i-1)->node->point, &nm->node->point);
				Vector3d *toChild = new Vector3d(&nm->node->point, &bm->nodeModelList.at(i+1)->node->point);

				norm = new Vector3d();
				norm->add(fromParent);
				norm->add(toChild);

				delete(fromParent);
				delete(toChild);
			}
			norm->normalize();

			//jezeli segment jest pierwszy w gałęzi to ma promień drugiego segmentu
			float R = i==0?bm->nodeModelList.at(i+1)->node->r:nm->node->r;

			computeOneSegment(norm, nm, R);

			delete(norm);
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			computeSegment(bm->childBranches.at(i));
		}

	}

	//liczy pojedynczy segment dla NodeModel
	//norm - wektor normalny do powierzchni wyznaczonej przez segment
	void computeOneSegment(Vector3d *norm, NodeModel *nodeModel, float R)
	{
		Node *node = nodeModel->node;

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

		Segment *segment = new Segment();
		nodeModel->segment = segment;


		for (int i = 0; i < tp->circlePoints; i++) {
			Vector3d *u, *v;
			u = new Vector3d();
			v = new Vector3d();
			u->add(U);
			v->add(V);

			u->mul(R * cos((float) i * angle));
			v->mul(R * sin((float) i * angle));
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
	}

	void computeConnectedPts(BranchModel *bm)
	{

		int nodeModelListLen = bm->nodeModelList.size();

		for(int i=0; i<nodeModelListLen - 1; i++)
		{
			NodeModel *root = bm->nodeModelList.at(i);
			NodeModel *child = bm->nodeModelList.at(i+1);

			Vector3d *r = new Vector3d(&root->node->point, root->segment->circlePts[0]);
			Point3d *p = new Point3d(&child->node->point);
			p->x += r->d[0];
			p->y += r->d[1];
			p->z += r->d[2];

			int index;
			float minDistance;
			for (int i = 0; i < tp->circlePoints; i++) {
				if (i == 0) {
					index = i;
					minDistance = p->getDistance(child->segment->circlePts[i]);
				} else {
					float currentDistance = p->getDistance(child->segment->circlePts[i]);
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
			root->r*=1.6;
		if(rec==1)
			root->r*=1.4;
		if(rec==2)
			root->r*=1.2;
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

	BranchModel* node2BranchModel(Node *root)
	{
		Node *node = root;

		BranchModel *branch = new BranchModel();
		if(node->prev)
		{
			branch->addNodeModel(new NodeModel(node->prev));
		}

		while(node)
		{
			NodeModel *nodeModel = new NodeModel(node);
			int nodeChildrenLen = node->getChildLen();

			if(nodeChildrenLen > 1)
			{
				Node *maxNode = findMaxChild(node);
				for(int i=0; i<nodeChildrenLen; i++)
				{
					Node *childNode = node->getChildAt(i);
					if(childNode != maxNode)
						branch->addChildBranch(node2BranchModel(childNode));
				}
				node = maxNode;

			} else if(nodeChildrenLen == 1)
			{
				node = node->getChildAt(0);
			}
			else
				node = NULL;

			branch->addNodeModel(nodeModel);
		}


		return branch;
	}

public:
	Model3d(Node *root,TrunkParameters *tp) {
		this->root = root;
		this->tp=tp;
	}


	BranchModel* generateModel() {
		BranchModel *result;

		computeRadius(root,0);
		result = node2BranchModel(root);
		computeSegment(result);
		computeConnectedPts(result);

		return result;
	}
};
#endif