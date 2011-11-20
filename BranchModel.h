
#ifndef MODELBRANCH_H
#define	MODELBRANCH_H

#include <vector>

/*
 * ModelBranch składa się z listy ModelNode. W skład gałęzi wchodzą kolejne Nody, mające największy promień.
 * Pozostałe Nody tworzą kolejne gałęzie.
 */
class BranchModel
{
public:
	std::vector<BranchModel*> childBranches;
	std::vector<NodeModel*> nodeModelList;

	BranchModel *parentBranch;

	//Pozycja gałęzi względem gałęzi rodzica;
	Point3d *position;

	BranchModel(BranchModel *parentBranch)
	{
		this->parentBranch = parentBranch;
		position = NULL;
	}

	~BranchModel()
	{
		delete position;
		for(unsigned int i=0; i<nodeModelList.size(); i++)
		{
			delete nodeModelList[i];
		}

		for(unsigned int i=0; i<childBranches.size(); i++)
		{
			delete childBranches[i];
		}
	}

	void addNewNodeModel(Node *node)
	{
		if(!position)
		{
			if(parentBranch == NULL)
			{
				position = new Point3d(); //(0,0,0)
			} else
			{
				position = new Point3d(&node->point);
				position->sub(parentBranch->position);
			}
		}

		NodeModel *nodeModel = new NodeModel(node->point, getAbsolutePosition(), node->r);

		nodeModelList.push_back(nodeModel);
	}

	void addChildBranch(BranchModel *branch)
	{
		childBranches.push_back(branch);
	}

	Point3d getAbsolutePosition()
	{
		BranchModel *parent = this->parentBranch;
		Point3d result;
		result.add(*this->position);
		while(parent)
		{
			result.add(*parent->position);
			parent = parent->parentBranch;
		}
		return result;
	}

	Point3d getAbsoluteNodePosition(NodeModel *node)
	{
		Point3d result;
		result.add(getAbsolutePosition());
		result.add(*node->position);
		return result;
	}
};


#endif	/* MODELBRANCH_H */

