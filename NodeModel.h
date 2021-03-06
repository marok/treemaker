#ifndef MODELNODE_H
#define	MODELNODE_H

#include <vector>

//dla jednego Node może być kilka ModelNode
class NodeModel
{
public:
	//Pozycja względem początka gałęzi(branch->position)
	Point3d *position;
	Segment *segment;
	LeafModel *leaf;
	float r;

	NodeModel(Point3d *relativePosition, float r)
	{
		this->position = relativePosition;
		this->r = r;
		this->segment = new Segment();
		this->leaf = NULL;
	}

	NodeModel(Point3d absoultePosition, Point3d branchAbsolutePosition, float r)
	{
		this->position = new Point3d(&absoultePosition);
		this->position->sub(&branchAbsolutePosition);
		this->segment = new Segment();

		this->r = r;
		this->leaf = NULL;
	}

	~NodeModel()
	{
		delete position;
		delete segment;
		if(leaf)
			delete leaf;
	}

	//zwraca punkt segmentu, względem początka gałęzi
	Point3d getSegmentPointRel2BranchAt(int i)
	{
		Point3d result;
		result.add(*position);
		result.add(segment->circlePts[i]);
		return result;
	}
};


#endif	/* MODELNODE_H */

