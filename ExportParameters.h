#ifndef _EXPORTPARAMETERS_H
#define _EXPORTPARAMETERS_H

class ExportParameters
{
public:
	bool invert;	//invert ? axis for Blender
	string filename;
	int triangles;

	ExportParameters() {
		setDefault();
	}

	void setDefault()
	{
		invert=1;
		triangles=0;
		filename="models/tree0.obj";
	}

	/* Serialization methods */
	void save(ofstream &s)
	{
#define SAVE(param) s<<param<<endl;
		SAVE(invert);
		SAVE(filename);
#undef SAVE
	}
	void load(ifstream &s)
	{
#define LOAD(param) s>>param;
		LOAD(invert);
		LOAD(filename);
#undef LOAD
	}
};
#endif