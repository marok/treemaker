
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
	
	void addNodeModel(NodeModel *nodeModel)
	{
		nodeModelList.push_back(nodeModel);
	}
	
	void addChildBranch(BranchModel *branch)
	{
		childBranches.push_back(branch);
	}
};


#endif	/* MODELBRANCH_H */

