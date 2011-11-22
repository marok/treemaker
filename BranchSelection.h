#ifndef BRANCHSELECTION_H
#define	BRANCHSELECTION_H

class BranchSelection
{
	int markedBranchIndex;
	int markedNodeIndex[2];
	
	guint markedNodeCount;
	
public:
	BranchSelection()
	{
		markedBranchIndex = markedNodeIndex[0] = markedNodeIndex[1] = -1;
		markedNodeCount = 0;
	}
	
	void markBranch(int index)
	{
		if(this->markedBranchIndex != index)
			markedNodeCount = 0;
		
		this->markedBranchIndex = index;
	}
	
	void markNode(int index)
	{
		if(markedNodeCount == 0)
		{
			markedNodeCount++;
			markedNodeIndex[0] = index;
		}
		else
			markedNodeIndex[0] = index;
		/*else if(markedNodeCount == 1 && index!=markedNodeIndex[0])
		{
			markedNodeCount++;
			if(index > markedNodeIndex[0])
			{
				markedNodeIndex[1] = index;
			} else
			{
				markedNodeIndex[1] = markedNodeIndex[0];
				markedNodeIndex[0] = index;
			}
		} else 
		{
			if(index != markedNodeIndex[0] && index != markedNodeIndex[1])
			{
				if(index < markedNodeIndex[0])
					markedNodeIndex[0] = index;
				else
					markedNodeIndex[1] = index;
			}
		}*/
	}
	
	void unmarkBranch()
	{
		markedBranchIndex = markedNodeIndex[0] = markedNodeIndex[1] = -1;
		markedNodeCount = 0;
	}
	
	bool isBranchMarked(int index)
	{
		return index == markedBranchIndex;
	}
	
	bool isNodeMarked(int index)
	{
		if(markedNodeCount == 0)
			return true;
		
		if(markedNodeCount == 1 && index >= markedNodeIndex[0])
			return true;
		
		if(markedNodeCount == 2 && index >= markedNodeIndex[0] && index <= markedNodeIndex[1])
			return true;
		
		return false;
	}
	
	int getMarkedBranch()
	{
		return markedBranchIndex;
	}
	
	int getMarkedNode()
	{
		return markedNodeIndex[0];
	}
};


#endif	/* BRANCHSELECTION_H */

