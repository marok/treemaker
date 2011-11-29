#ifndef _EXPORTER_H
#define _EXPORTER_H
#include <fstream>
#include <iostream>
#include <vector>
#include "Point3d.h"
#include "Face.h"
using namespace std;

// OBJ Model Exporter
class Exporter {
	Parameters *params;
	ofstream file;
	vector<Point3d> v;//vertex coordinates
	vector<Point2d> vt;//texture coordinates
	vector<Face> faces;
	void writeVertices() {
		for(unsigned int i=0; i<v.size(); i++)
			file<<"v "<<v[i].x<<" "<<v[i].y<<" "<<v[i].z<<endl;
	}
	void writeTextures() {
		for(unsigned int i=0; i<vt.size(); i++)
			file<<"vt "<<vt[i].x<<" "<<vt[i].y<<endl;
	}
	void writeFaces() {
//#define OBJGEOMETRYONLY
		for(unsigned int i=0; i<faces.size(); i++) {
			file<<"f ";
			for(unsigned int j=0; j<3; j++)
			{
			// assert(faces[i].vt[j]<=4);
			  #ifdef OBJGEOMETRYONLY
			  file<<faces[i].v[j]<<" ";
			#else
			  file<<faces[i].v[j]<<"/"<<faces[i].vt[j]<<" ";
			#endif
			}  file<<endl;

		}
	}
	int findPoint(Point3d *a) { //O(1)
		int found=0;
		unsigned int prevmax=params->tp->circlePoints*4;
		unsigned int start=MAX(v.size()-prevmax,0);
		for(unsigned int i=start; i<v.size(); i++)
			if(a->x==v[i].x&&a->y==v[i].y&&a->z==v[i].z)
			{
				found=i+1;//zapisz id znalezionego powiekszone o 1 bo .obj numeruje od 1
				break;
			}
		return found;
	}
	int findTexture(Point2d *a){
	    int found=0;
	    //	if(a->x>1)
	//	  a->x=a->x-(int)(a->x);
	//	if(a->y>1)
	//	  a->y=a->y-(int)(a->y);
		if(abs(0-a->x)<0.01) a->x=0;
		if(abs(0-a->y)<0.01) a->y=0;
		
		if(abs(1-a->x)<0.01) a->x=1;
		if(abs(1-a->y)<0.01) a->y=1;
	    for(unsigned int i=0;i<vt.size();i++)
	      if(a->x==vt[i].x&&a->y==vt[i].y){
		found=i+1;
		break;
	      }
	    return found;
	}
	//Dodaje trojkat do listy faces bez normalnych i wsp. tekstury
	void addTriangleVertex(Point3d &a,Point3d &b,Point3d &c,Face &f) {
		//dla kazdego pkt trojkata sprawdz czy istnieje juz na liscie v
		//bo obj nie lubi powtorzen wierzcholkow
		Point3d* p[3]= {&a,&b,&c};
		int ids[3];
		for(unsigned int i=0; i<3; i++)
			ids[i]=findPoint(p[i]);
		for(unsigned int i=0; i<3; i++) {
			if(ids[i]==0) { //not found
				switch(i) {
				case 0:
					v.push_back(a);
					break;
				case 1:
					v.push_back(b);
					break;
				case 2:
					v.push_back(c);
					break;
				}
				f.v[i]=v.size();
			} else f.v[i]=ids[i];
		}
	}
	void addTriangleTexture(Point2d &at,Point2d &bt,Point2d &ct,Face &f){
	  Point2d *p[3]={&at,&bt,&ct};
	  int ids[3];
	  for(unsigned int i=0;i<3;i++)
	    ids[i]=findTexture(p[i]);
	  for(unsigned int i=0;i<3;i++) {
			if(ids[i]==0) { //not found
				switch(i) {
				case 0:
					vt.push_back(at);
					break;
				case 1:
					vt.push_back(bt);
					break;
				case 2:
					vt.push_back(ct);
					break;
				}
				f.vt[i]=vt.size();
			} else f.vt[i]=ids[i];
		}
	}
	void addTriangle(Point3d a,Point3d b,Point3d c,Point2d at,Point2d bt,Point2d ct) {
	  Face f;
	  addTriangleVertex(a,b,c,f);
	  addTriangleTexture(at,bt,ct,f);
	  faces.push_back(f);
	}
	void exportBranch(BranchModel *bm)
	{
		TrunkParameters *tp=params->tp;
		float kx=tp->kx;//3;//ile razy ma sie owinac tekstura
		float ky=tp->ky;//0.5;//ile jednostek tekstury na 1 jednostke dlugosci pnia
		int nodeModelListLen = bm->nodeModelList.size();
		static vector<Point2d> prevCoords[2];
		prevCoords[0].clear();
		prevCoords[1].clear();
		// i - numer okregu w galezi
		for(int i=0; i<nodeModelListLen-1; i++)
		{
			NodeModel *root= bm->nodeModelList.at(i);
			NodeModel *child = bm->nodeModelList.at(i+1);
			int index = child->segment->index;

			// Obliczanie wspolrzednych tekstury na pniu
			if(i==0) {
				float dx=root->segment->circlePts[0]->getDistance(root->segment->circlePts[1]);
				float D=tp->circlePoints*dx;//oblicz obwod pnia
				for(int k=0; k<=tp->circlePoints; k++) {
					float txcoord=(k)*dx*kx/D;
					Point2d tcoord(txcoord,0);
					prevCoords[1].push_back(tcoord);
				}
			}
			prevCoords[i&1].clear();
			float dx=child->segment->circlePts[0]->getDistance(child->segment->circlePts[1]);
			float D=tp->circlePoints*dx;

			for(int k=0; k<=tp->circlePoints; k++) {
				float txcoord=(k*dx)*kx/D;
				float tycoord=ky*(i+1);
				Point2d tcoord(txcoord,tycoord);
				prevCoords[i&1].push_back(tcoord);
			}



#define GV(point,tpoint) point.x+tpoint.x,point.y+tpoint.y,point.z+tpoint.z
			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				int j0 = (index + i0) % tp->circlePoints;
				int i1 = (i0+1)%tp->circlePoints;
				int j1 = (j0+1)%tp->circlePoints;
				int tj1= (j0+1);
				int ti1= (i0+1);

				Point3d rootAbs, childAbs;
				rootAbs = bm->getAbsoluteNodePosition(root);
				childAbs = bm->getAbsoluteNodePosition(child);

				Point2d at(prevCoords[!(i&1)][i0].x,prevCoords[!(i&1)][i0].y);
				//Point2d at(0,0);
				Point3d a(GV(rootAbs, (*root->segment->circlePts[i0]) ));

				Point2d bt(prevCoords[i&1][j0].x,prevCoords[i&1][j0].y);
				//Point2d bt(0,1);
				Point3d b(GV(childAbs, (*child->segment->circlePts[j0])));

				Point2d ct(prevCoords[i&1][tj1].x,prevCoords[i&1][tj1].y);
				//Point2d ct(1,1);
				Point3d c(GV(childAbs, (*child->segment->circlePts[j1])));
				//addTriangle(a,b,c);
				addTriangle(a,b,c,at,bt,ct);

				Point2d dt(prevCoords[!(i&1)][ti1].x,prevCoords[!(i&1)][ti1].y);
				//Point2d dt(1,0);
				Point3d d(GV(rootAbs, (*root->segment->circlePts[i1])));

				Point2d et(prevCoords[(i&1)][tj1].x,prevCoords[(i&1)][tj1].y);
				//Point2d et(1,1);
				Point3d e(GV(childAbs, (*child->segment->circlePts[j1])));

				Point2d ft(prevCoords[!(i&1)][i0].x,prevCoords[!(i&1)][i0].y);
				//Point2d ft(0,0);
				Point3d f(GV(rootAbs, (*root->segment->circlePts[i0])));
				//addTriangle(d,e,f);
				addTriangle(d,e,f,dt,et,ft);
			}
#undef GV
		}
	}
	void exportLeaf(Point3d *p, Vector3d *dir, Vector3d *norm,Parameters *params,unsigned int cnt){

		unsigned int lastTypeId=cnt;
		//float size=params->lp->leaves[0].leafSize;

		unsigned int type=params->lp->types[lastTypeId].first;
		//float size=params->lp->types[lastTypeId].second;
		
		Point2d petiole = params->lp->leaves[type].petiole;
		
		Vector3d a, b, *tmp1, *tmp2;
		tmp1 = dir->crossProduct(norm);
		tmp1->normalize();
		tmp1->mul(petiole.x);
		
		tmp2 = dir->crossProduct(norm);
		tmp2->normalize();
		tmp2->mul(petiole.x-1);
		
		a.add(dir);
		a.add(tmp1);
		
		
		b.add(dir);
		b.add(tmp2);
		
		#define FT(va,s,t,u) Point3d va(p->x+s,p->y+t,p->z+u); 
		#define TT(va,u,v) Point2d va(u,v);
		TT(at,petiole.x, 1.0f);
		FT(av,0,0,0);
		TT(bt,1.0f, 0.0f);
		FT(bv,a.d[0],a.d[1],a.d[2]);
		TT(ct,0.0f, 0.0f);
		FT(cv,b.d[0],b.d[1],b.d[2]);
		
		TT(dt,petiole.x, 1.0f);
		FT(dv,0,0,0);
		TT(et,0.0f, 0.0f);
		FT(ev,b.d[0],b.d[1],b.d[2]);
		TT(ft,0.0f, 1.0f);
		FT(fv,tmp2->d[0],tmp2->d[1],tmp2->d[2]);
		
		TT(gt,petiole.x, 1.0f);
		FT(gv,0,0,0);
		TT(ht,1.0f, 1.0f);
		FT(hv,tmp1->d[0],tmp1->d[1],tmp1->d[2]);
		TT(it,1.0f, 0.0f);
		FT(iv,a.d[0],a.d[1],a.d[2]);
		#undef FT
		#undef TT
		
		addTriangle(av,bv,cv,at,bt,ct);
		addTriangle(dv,ev,fv,dt,et,ft);
		addTriangle(gv,hv,iv,gt,ht,it);

	}
	
	
	void clean() {
		v.clear();
		vt.clear();
		faces.clear();
	}

  public:
	void init(char *filename) {
		file.open(filename);
		printf("saving...");
		if(file.is_open()==false) {
			printf("Exporter error file is not opened");
			return;
		}
		file<<"# This model was generated by Treemaker O&O."<<endl;
		file<<"mtllib tree0.mtl"<<endl;
		file<<"o Tree"<<endl;
	}
	void exportTrunk(Model3d *model, Parameters *params)
	{
		this->params=params;
		for(unsigned int i=0; i<model->branches.size(); i++)
			exportBranch(model->branches[i]);
	}
	void exportLeaves(BranchModel *bm, Parameters *params,unsigned int cnt)
	{
		if(!bm) return;
		
		for(unsigned int i=0; i<bm->nodeModelList.size(); i++)
			if(bm->nodeModelList[i]->leaf)
			{
				NodeModel *node = bm->nodeModelList[i];
				Point3d leafPoint = bm->getAbsoluteNodePosition(node);
				exportLeaf(&leafPoint, node->leaf->dir, node->leaf->norm,params,cnt++);
				if(cnt >= params->lp->types.size()) cnt=0;
			}

		for(unsigned int i = 0; i< bm->childBranches.size(); i++)
		{
			exportLeaves(bm->childBranches.at(i),params,cnt);
		}


	}
	void saveTrunk(){
		file<<"usemtl bark"<<endl;
		file<<"g Trunk"<<endl;
		file<<"# List of Vertices, with (x,y,z) coordinates"<<endl;
		writeVertices();
		file<<"# Texture coordinates, with (x,y) coordinates"<<endl;
		writeTextures();
		file<<"# Face Definitions (v1-id v2-id v3-id)"<<endl;
		writeFaces();  
		clean();
	}
	void saveLeaves(){
		file<<"usemtl leaf"<<endl;
		file<<"g Leaves"<<endl;
		file<<"# List of Vertices, with (x,y,z) coordinates"<<endl;
		writeVertices();
		file<<"# Texture coordinates, with (x,y) coordinates"<<endl;
		writeTextures();
		file<<"# Face Definitions (v1-id v2-id v3-id)"<<endl;
		writeFaces();
		clean();
	}
	void close() {
		file<<"# End of file"<<endl;
		file.close();
		clean();
		printf("done\n");
	}
};
#endif