#ifndef BRANCHSELECTION_H
#define	BRANCHSELECTION_H

enum SelectionMode
{
	SELECTION_ALL,
	SELECTION_POINT_END,
	SELECTION_POINT_POINT
};

class BranchSelection
{
	int markedBranchIndex;
	int markedNodeIndex[2];

	SelectionMode mode;

	//raczej index do modyfikacji
	int lastModifiedNodeIndex;
	SelectionMode prevMode;

public:
	bool applyForChildren;
	BranchSelection()
	{
		markedBranchIndex = markedNodeIndex[0] = markedNodeIndex[1] = -1;
		mode = SELECTION_ALL;
		prevMode = SELECTION_ALL;
		applyForChildren = false;
	}

	void markBranch(int index)
	{
		if(this->markedBranchIndex != index)
			markedBranchIndex = markedNodeIndex[0] = markedNodeIndex[1] = -1;

		this->markedBranchIndex = index;
	}
	void markNode(int index) {
		if(index == markedNodeIndex[0] || index == markedNodeIndex[1])
			return;

		if(mode == SELECTION_POINT_END)
		{
			markedNodeIndex[0] = index;
			markedNodeIndex[1] = -1;
		} else if(mode == SELECTION_POINT_POINT)
		{
			if(markedNodeIndex[0]==-1)
				markedNodeIndex[0] = index;
			else if(markedNodeIndex[1]==-1)
				if(markedNodeIndex[0] < index)
					markedNodeIndex[1] = index;
				else
				{
					markedNodeIndex[1] = markedNodeIndex[0];
					markedNodeIndex[0] = index;
					lastModifiedNodeIndex--;
				}

			else {
				if(lastModifiedNodeIndex == 0)
				{
					if(index > markedNodeIndex[1])
					{
						markedNodeIndex[0] = markedNodeIndex[1];
						markedNodeIndex[1] = index;
						lastModifiedNodeIndex--;
					} else
						markedNodeIndex[0] = index;
				} else
				{
					if(index < markedNodeIndex[0])
					{
						markedNodeIndex[1] = markedNodeIndex[0];
						markedNodeIndex[0] = index;
						lastModifiedNodeIndex--;
					} else
						markedNodeIndex[1] = index;
				}
			}

			lastModifiedNodeIndex++;
			lastModifiedNodeIndex%=2;
		}
	}

	void unmarkBranch()
	{
		markedBranchIndex = -1;
		unmarkNodes();
	}

	void unmarkNodes()
	{
		markedNodeIndex[0] = markedNodeIndex[1] = -1;
	}

	bool isBranchMarked(int index)
	{
		return index == markedBranchIndex;
	}

	bool isNodeMarked(int index)
	{

		if(mode == SELECTION_ALL)
			return true;
		if(mode == SELECTION_POINT_END)
		{
			if(index >= markedNodeIndex[0])
				return true;
		}
		if(mode == SELECTION_POINT_POINT)
		{
			if(index >= markedNodeIndex[0] && (index<=markedNodeIndex[1] || markedNodeIndex[1]==-1))
				return true;
		}
		return false;
	}

	int getMarkedBranch()
	{
		return markedBranchIndex;
	}

	int* getMarkedNodes()
	{
		return markedNodeIndex;
	}

	void setMode(SelectionMode mode)
	{
		if (mode != prevMode) {
			this->prevMode = this->mode;
			this->mode = mode;
			if (mode == SELECTION_ALL) {
				unmarkNodes();
			} else if (mode == SELECTION_POINT_END) {
				markedNodeIndex[1] = -1;
			}
		}
	}

	//zwiększa zaznaczony obszar o i węzłów
	//ma sens tylko jeśli jesteśmy w trybie point_point
	void incMarkedArea(int i)
	{
		if(markedNodeIndex[1]!=-1)
			markedNodeIndex[1]+=i;
	}

	void decMarkedArea(int i)
	{
		if(markedNodeIndex[1]!=-1)
			markedNodeIndex[1]-=i;
	}

	SelectionMode getMode()
	{
		return mode;
	}
};


#endif	/* BRANCHSELECTION_H */

