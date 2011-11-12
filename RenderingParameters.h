#ifndef _RENDERINGPARAMETERS_H
#define _RENDERINGPARAMETERS_H

class RenderingParameters
{
public:
	bool showEnvelopes;	//show Tree Crown Envelopes on GUI
	bool showLeaves;	//show Tree Leaves on tree model
	bool showBark;		//show Bark texture on tree model
	bool showGrass;		//show grass on the ground
	static const bool SHOWENVELOPE_DEFAULT=FALSE;
	static const bool SHOWLEAVES_DEFAULT=TRUE;
	static const bool SHOWBARK_DEFAULT=TRUE;
	static const bool SHOWGRASS_DEFAULT=FALSE;

	RenderingParameters() {
		showEnvelopes=SHOWENVELOPE_DEFAULT;
		showLeaves=SHOWLEAVES_DEFAULT;
		showBark=SHOWBARK_DEFAULT;
		showGrass=SHOWGRASS_DEFAULT;
	}
};
#endif