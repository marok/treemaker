#ifndef LEAFMODEL_H
#define	LEAFMODEL_H

class LeafModel
{
public:
	
	//wektor normalny do powierzchni liścia
	Vector3d *norm;
	
	//wektor określający kierunek liścia na powierzchni
	Vector3d *dir;
	
	//wektory norm i dir muszą być do siebie prostopadłe
	
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

