/* Klasa zawiera definicje trojkata opisanego wspolrzednymi wierzcholkow i tekstury */
#ifndef _FACE_H
#define _FACE_H

class Face {
public:
	int v[3];
	int vt[3];
	Face() {
		v[0]=v[1]=v[2]=0;
		vt[0]=vt[1]=vt[2]=0;
	}
	Face(int v0,int v1,int v2) {
		v[0]=v0;
		v[1]=v1;
		v[2]=v2;
		vt[0]=vt[1]=vt[2]=0;
	}
	Face(int v0,int v1,int v2,int vt0,int vt1,int vt2) {
		v[0]=v0;
		v[1]=v1;
		v[2]=v2;
		vt[0]=vt0;
		vt[1]=vt1;
		vt[2]=vt2;
	}
};
#endif