#ifndef LEAFMODEL_H
#define	LEAFMODEL_H

class LeafModel
{
public:
	Vector3d *norm;
	Vector3d *dir;
	
	LeafModel()
	{
		norm = NULL;
		dir = NULL;
	}
	
	~LeafModel()
	{
		if(norm)
			delete norm;
		if(dir)
			delete dir;
	}
};


#endif	/* LEAFMODEL_H */

