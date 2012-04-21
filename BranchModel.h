
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

	//tablica z węzłami gałęzi. Zawsza posiada przynajmniej 2 elementy.
	std::vector<NodeModel*> nodeModelList;

	BranchModel *parentBranch;

	//Pozycja gałęzi względem gałęzi rodzica;
	//UWAGA: to jest ten sam obiekt co w obiekcie NodeModel w gałęzi-rodzicu
	Point3d *position;

	BranchModel(BranchModel *parentBranch, Point3d* branchPosition)
	{
		this->parentBranch = parentBranch;
		position = branchPosition;
	}

	~BranchModel()
	{
		for(unsigned int i=0; i<nodeModelList.size(); i++)
		{
			delete nodeModelList[i];
		}

		for(unsigned int i=0; i<childBranches.size(); i++)
		{
			delete childBranches[i];
		}
	}

	//zwraca wskaźnik na punkt, do którego został doczepiony NodeModel
	Point3d* addNewNodeModel(Node *node)
	{
		Point3d* result = new Point3d();
		result->add(node->point);
		result->sub(getAbsolutePosition());
		NodeModel *nodeModel = new NodeModel(result, node->r);
		nodeModelList.push_back(nodeModel);
		return result;
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

