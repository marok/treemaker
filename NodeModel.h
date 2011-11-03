#ifndef MODELNODE_H
#define	MODELNODE_H

#include <vector>

//dla jednego Node może być kilka ModelNode
class NodeModel
{
public:
	NodeModel(Node *node)
	{
		this->node = node;
	}

	Node *node;
	Segment *segment;
};


#endif	/* MODELNODE_H */

