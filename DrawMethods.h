#ifndef DRAWMETHODS_H
#define	DRAWMETHODS_H

#include "GlobalVar.h"
#include "Bmp.h"
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkglglext.h>
#include "Parameters.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <cmath>

#define VERTEX(point) glVertex3f(point.x,point.y,point.z)
#define VERTEX_TRANS(point,trans_point) glVertex3f(point.x+trans_point.x,point.y+trans_point.y,point.z+trans_point.z)

GtkWidget *windowWidget;

static int triangles=0;
static const GLfloat colorActive[] = {1, 1, 1};
static const GLfloat colorActivePoint[] = {1, 0, 0};
static const GLfloat colorUnactive[] = {0, 0.5, 1};
class DrawMethods {


public:

	static void render() {
		gtk_widget_queue_draw(windowWidget);

	}


	static void drawCoordinates(GLenum mode) {


		GLUquadric *quadric;

#define SOLID_CLOSED_CYLINDER(QUAD, BASE, TOP, HEIGHT, SLICES, STACKS) \
gluCylinder(QUAD, BASE, TOP, HEIGHT, SLICES, STACKS); \
glRotatef(180, 1,0,0); \
gluDisk(QUAD, 0.0f, BASE, SLICES, 1); \
glRotatef(180, 1,0,0); \
glTranslatef(0.0f, 0.0f, HEIGHT); \
gluDisk(QUAD, 0.0f, TOP, SLICES,1);

#define DRAW_AXIS(i) \
quadric= gluNewQuadric();\
  if (mode == GL_SELECT)\
gluQuadricDrawStyle(quadric, GLU_FILL);\
gluQuadricOrientation(quadric, GLU_INSIDE);\
SOLID_CLOSED_CYLINDER(quadric, 0.1f, 0.1f, 3.0f, 8, 8)\
SOLID_CLOSED_CYLINDER(quadric, 0.2f, 0.01f, 0.5f, 8, 8)\
glTranslatef(0.0,0.0,-3.5);\
gluDeleteQuadric(quadric);\
if(mode==GL_SELECT)\
		glPushMatrix();
		glColor3f(1,0,0);
		DRAW_AXIS(0);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0,1,0);
		glRotatef(90,0,1,0);
		DRAW_AXIS(1);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0,0,1);
		glRotatef(90,1,0,0);
		DRAW_AXIS(2);
		glPopMatrix();
#undef DRAW_AXIS
#undef SOLID_CLOSED_CYLINDER


	}


	static void drawTrunk(BranchModel *bm, unsigned int branchId, Parameters *params)
	{
		TrunkParameters *tp=params->tp;
		glPushName(branchId);
		static GLuint texId;
		if(params->tp->barkTexInitialized==FALSE) {
			params->tp->barkTexInitialized=TRUE;
			Bmp bmp;
			bmp.load(params->tp->barkPath);
			glGenTextures(1,&texId);
			glBindTexture(GL_TEXTURE_2D,texId);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, bmp.header.biWidth, bmp.header.biHeight, 0, GL_RGB,GL_UNSIGNED_BYTE, bmp.data);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
			bmp.cleanup();
		}

		glBindTexture(GL_TEXTURE_2D,texId);
		if(bm==NULL) return;
		int nodeModelListLen = bm->nodeModelList.size();
		for(int i=0; i<nodeModelListLen-1; i++)
		{
			NodeModel *root= bm->nodeModelList.at(i);
			NodeModel *child = bm->nodeModelList.at(i+1);
			int index = child->segment->index;

			glEnable (GL_TEXTURE_2D);

			glBegin(GL_TRIANGLES);

			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				int j0 = (index + i0) % tp->circlePoints;

				int i1 = (i0+1)%tp->circlePoints;
				int j1 = (j0+1)%tp->circlePoints;

				Point3d rootAbs, childAbs;
				rootAbs = bm->getAbsoluteNodePosition(root);
				childAbs = bm->getAbsoluteNodePosition(child);

				glTexCoord2f (0.0f, 0.0f);
				VERTEX_TRANS(rootAbs, (*root->segment->circlePts[i0]));
				//glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glTexCoord2f (1.0f, 0.0f);
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j0]));
				//glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
				glTexCoord2f (1.0f, 1.0f);
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j1]));
				//glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);

				glTexCoord2f (0.0f, 0.0f);
				VERTEX_TRANS(rootAbs, (*root->segment->circlePts[i0]));
				//glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glTexCoord2f (0.0f, 1.0f);
				VERTEX_TRANS(rootAbs, (*root->segment->circlePts[i1]));
				//glVertex3f(root->segment->circlePts[i1]->x,root->segment->circlePts[i1]->y,root->segment->circlePts[i1]->z);
				glTexCoord2f (1.0f, 1.0f);
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j1]));
				//glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
				triangles+=2;
			}
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		glPopName();
	}

	static void drawTrunks(Model3d *model, Parameters *params)
	{
		if(!model)
			return;

		for(unsigned int i=0; i<model->branches.size(); i++)
		{
			if(model->markedBranchIndex != -1 && (model->markedBranchIndex == int(i)))
				glColor3f(0,1,0);
			else
				glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color
			drawTrunk(model->branches[i], i, params);
		}
	}

	static void drawNodes(Node *node)
	{
		if(!node)
			return;

		glPointSize(3);
		glColor3f(1,0,1);
		glBegin(GL_POINTS);
		glVertex3f(node->point.x, node->point.y, node->point.z);
		glEnd();
		for(int i=0; i< node-> getChildLen(); i++)
		{
			drawNodes(node->getChildAt(i));
		}
	}

	static void drawLines(BranchModel *bm, TrunkParameters *tp)
	{
		if(bm==NULL) return;

		glColor3f(1,0.5,0);
		int nodeModelListLen = bm->nodeModelList.size();
		Point3d branchAbs  = bm->getAbsolutePosition();
		for(int i=0; i<nodeModelListLen-1; i++)
		{
			NodeModel *node= bm->nodeModelList.at(i);
			NodeModel *child = bm->nodeModelList.at(i+1);
			int index = child->segment->index;

			Point3d nodeAbs;
			nodeAbs.add(branchAbs);
			nodeAbs.add(*node->position);

			Point3d childAbs;
			childAbs.add(branchAbs);
			childAbs.add(*child->position);

			glBegin(GL_POINTS);
			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				int j0 = (index + i0) % tp->circlePoints;

				Point3d circAbs;
				circAbs.add(nodeAbs);
				circAbs.add(*node->segment->circlePts[i0]);
				glVertex3f(circAbs.x, circAbs.y, circAbs.z);


				Point3d circChildAbs;
				circChildAbs.add(childAbs);
				circChildAbs.add(*child->segment->circlePts[j0]);
				glVertex3f(circChildAbs.x, circChildAbs.y, circChildAbs.z);

				//glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				//glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
//				int i1 = (i0+1)%tp->circlePoints;
//				int j1 = (j0+1)%tp->circlePoints;
//
//				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
//				glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
//				glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
//
//				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
//				glVertex3f(root->segment->circlePts[i1]->x,root->segment->circlePts[i1]->y,root->segment->circlePts[i1]->z);
//				glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
//				triangles+=2;
			}
			glEnd();


		}
		//Rysowanie pkt nodow
		Point3d abs = bm->getAbsolutePosition();

		glPointSize(3);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		for (unsigned int j = 0; j < bm->nodeModelList.size(); j++) {
			NodeModel *node = bm->nodeModelList[j];
			glVertex3f(abs.x + node->position->x, abs.y + node->position->y, abs.z + node->position->z);
		}
		glEnd();

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			drawLines(bm->childBranches.at(i), tp);
		}
	}


	static void drawEnvelopes(Crown *crown) {
		Crown *c = crown;

		for(unsigned int i=0; i<c->subcrowns.size(); i++)
		{
			Subcrown *sub = c->subcrowns.at(i);
			if(sub->shape == SPLINE)
			{
				drawEnvelopeSpline((SplineCrown*)sub, crown->activeSubcrown == (int)i, SPLINE_CROWN(sub)->activePoint);
			} else if(sub->shape == CYLINDER)
			{
				drawEnvelopeCylinder((CylinderCrown*)sub,crown->activeSubcrown == (int)i);
			}
		}
	}

	static void drawEnvelopeSpline(SplineCrown *splineCrown, bool active, int activeMainPoint)
	{
		glPushMatrix();

		int n=20;//liczba punktow na krzywej
		int loops=12;//liczba petli

		int N = splineCrown->crownMainPoints.size();
		float startX = splineCrown->crownMainPoints[0]->x;
		float step = (splineCrown->crownMainPoints[N - 1]->x - startX) / (float)n;

		glTranslatef(splineCrown->x, splineCrown->y, splineCrown->z);
		for(int j=0; j<loops; j++) {
			glColor3fv(active?colorActive:colorUnactive);
			glBegin(GL_LINE_STRIP);
			for(int i=0; i<n+1; i++) {
				float x, y;

				if(i == 0)
					x = startX;
				else
					x += step;

				y = splineCrown->s->getS(x);

				glVertex3f(y,0,x);
			}
			glEnd();

			if(active)
			{
				glPointSize(4);
				glColor3fv(colorUnactive);
				glBegin(GL_POINTS);
				for(int i=0; i<N; i++)
				{
					if(activeMainPoint == i)
					{
						glColor3fv(colorActivePoint);
						glVertex3f(splineCrown->crownMainPoints[i]->y,0,splineCrown->crownMainPoints[i]->x);
						glColor3fv(colorUnactive);
					} else
						glVertex3f(splineCrown->crownMainPoints[i]->y,0,splineCrown->crownMainPoints[i]->x);
				}
				glEnd();
			}

			glRotatef(360/loops,0,0,1);
		}
		glPopMatrix();
	}

	static void drawEnvelopeCylinder(CylinderCrown *cylinderCrown, bool active)
	{
		glPushMatrix();

		int loops=12;//liczba petli
		glTranslatef(cylinderCrown->x, cylinderCrown->y, 0);
		for(int j=0; j<loops; j++) {
			glColor3fv(active?colorActive:colorUnactive);
			glBegin(GL_LINE_STRIP);
			glVertex3f(0, 0, cylinderCrown->z);
			glVertex3f(cylinderCrown->r, 0, cylinderCrown->z);
			glVertex3f(cylinderCrown->r, 0, cylinderCrown->z+cylinderCrown->h);
			glVertex3f(0, 0, cylinderCrown->z+cylinderCrown->h);
			glEnd();
			glRotatef(360/loops,0,0,1);
		}
		glPopMatrix();
	}

	static void drawPoints(std::vector<Point3d *> points)
	{
		glPointSize(3);
		glColor3f (1, 0, 1);
		glBegin(GL_POINTS);
		for(unsigned int i=0; i<points.size(); i++)
		{
			glVertex3f(points.at(i)->x,points.at(i)->y,points.at(i)->z);
		}
		glEnd();
	}

	static void drawGrass() {
		//
		static GLuint texId;
		static Bmp bmp;
		static int initialized=0;
		static char tex_path[] = "textures/grass_512.bmp";

		if(!initialized) {
			initialized=1;
			bmp.load (tex_path);
			glGenTextures (1, &texId);
			glBindTexture (GL_TEXTURE_2D, texId);
			int height = bmp.header.biHeight;
			int width = bmp.header.biWidth;
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, bmp.data);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
		}
		glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color
		glBindTexture (GL_TEXTURE_2D, texId);

		float W=14;
		float H=0.1;
		glEnable (GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		{
			glTexCoord2f (0.0f, 0.0f);
			glVertex3f(-W,-W,H);

			glTexCoord2f (1.0f, 0.0f);
			glVertex3f(W,-W,H);

			glTexCoord2f (1.0f, 1.0f);
			glVertex3f(W,W,H);

			glTexCoord2f (0.0f, 1.0f);
			glVertex3f(-W,W,H);
		}
		glEnd();

		glDisable (GL_TEXTURE_2D);
		glColor4f(1.0, 1.0, 1.0, 1.0);
	}

	static void initLeavesTextures(Parameters *params) {
		/* Dla kazdej tekstury liscia, jesli nie zostala zainicjowana to ja inicjuj */
		for(unsigned int i=0; i<params->lp->leaves.size(); i++) {
			Leaf *leaf=&params->lp->leaves[i];
			if(!leaf->leafTexInitialized) {
				leaf->leafTexInitialized=TRUE;
				Bmp bmp,*mask;
				bmp.load (leaf->leafTexPath);
				mask=bmp.getMask();
				glGenTextures (1, &leaf->texId);
				glGenTextures(1,&leaf->maskId);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glBindTexture(GL_TEXTURE_2D,leaf->maskId);
				glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, bmp.header.biWidth, bmp.header.biHeight, 0, GL_RGB,GL_UNSIGNED_BYTE, bmp.data);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
				glBindTexture(GL_TEXTURE_2D,leaf->texId);
				glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, bmp.header.biWidth, bmp.header.biHeight, 0, GL_RGB,GL_UNSIGNED_BYTE, mask->data);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
				bmp.cleanup();
				mask->cleanup();
				delete mask;
			}
		}
	}
	static void drawLeaf(Point3d *p, Vector3d *dir,Parameters *params,unsigned int cnt) {
		//nie ma zadnego aktywnego typu liscia
		if(params->lp->types.size()==0) return;
		initLeavesTextures(params);

		glColor4f(0.0, 0, 0, 0); // reset gl color

		Vector3d a, b;
		a.d[2] = b.d[2] = dir->d[2]/2;
		a.d[0] = dir->length()*sin(asin(dir->d[1]/dir->length()-M_PI_4));
		b.d[0] = dir->d[0] - a.d[0];

		a.d[1] = dir->length()*cos(asin(dir->d[1]/dir->length()-M_PI_4));
		b.d[1] = dir->d[1] - a.d[1];

		glEnable(GL_BLEND);// Turn Blending On
		glDisable(GL_DEPTH_TEST);
		glEnable (GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(p->x,p->y,p->z);
		unsigned int lastTypeId=cnt;
		//float size=params->lp->leaves[0].leafSize;

		unsigned int type=params->lp->types[lastTypeId].first;
		float size=params->lp->types[lastTypeId].second;
#define TEXTURIZE\
		glBegin(GL_QUADS);\
		{\
			glTexCoord2f (0.0f, 0.0f);\
			glVertex3f(0,0,0);\
			glTexCoord2f (1.0f, 0.0f);\
			glVertex3f(a.d[0]*size,a.d[1]*size,a.d[2]*size);\
			glTexCoord2f (1.0f, 1.0f);\
			glVertex3f(dir->d[0]*size,dir->d[1]*size,dir->d[2]*size);\
			glTexCoord2f (0.0f, 1.0f);\
			glVertex3f(b.d[0]*size,b.d[1]*size,b.d[2]*size);\
		}\
		glEnd();
		//texture mask
		glBlendFunc(GL_DST_COLOR,GL_ZERO);
		glBindTexture(GL_TEXTURE_2D,params->lp->leaves[type].texId);
		TEXTURIZE;

		//texture image
		glBlendFunc(GL_ONE,GL_ONE);
		glBindTexture(GL_TEXTURE_2D,params->lp->leaves[type].maskId);

		TEXTURIZE;

#undef TEXTURIZE

		glPopMatrix();
		glDisable (GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glColor4f(1.0, 1.0, 1.0, 1.0);

	}

	static void drawLeaves(BranchModel *bm, Parameters *params,unsigned int cnt)
	{
		if(!bm) return;

		Point3d *leaf = bm->nodeModelList.at(bm->nodeModelList.size()-1)->position;
		Point3d *befLeaf = bm->nodeModelList.at(bm->nodeModelList.size()-2)->position;

		Vector3d *leafVect = new Vector3d(befLeaf, leaf);
		leafVect->normalize();
		leafVect->mul(1);

		Point3d leafPoint = bm->getAbsoluteNodePosition(bm->nodeModelList.at(bm->nodeModelList.size()-1));
		drawLeaf(&leafPoint, leafVect,params,cnt++);
		if(cnt>=params->lp->types.size()) cnt=0;
		delete leafVect;

		for(unsigned int i = 0; i< bm->childBranches.size(); i++)
		{
			drawLeaves(bm->childBranches.at(i),params,cnt);
		}

	}

	static void drawTreeModel (Parameters *params) {
		glPushMatrix();
		TrunkParameters *tp=params->tp;
		//triangles=0;

		if(params->rp->showGrass)
			drawGrass();
		if(params->rp->showEnvelopes)
			drawEnvelopes(params->crown);
		if(cm->params->methodParams->activeMethod==0) {
			if(params->rp->showBark)
				drawTrunks(model, params);
			else drawLines(model?model->getRootBranch():NULL, tp);

			if(params->rp->showLeaves)
				drawLeaves(model?model->getRootBranch():NULL, params,0);
		}
		if(cm->params->methodParams->activeMethod==1) {
			drawLines(model?model->getRootBranch():NULL,tp);
		}

		//g_print("triangles: %d\n",triangles);
		glPopMatrix();
	}

	static void drawWireframe () {
		int i;

		glColor3f (0.0, 0.6, 0);

		for (i = -grid/2; i <= grid/2; i++) {
			glBegin (GL_LINES);
			glVertex3f (i, -grid/2, 0);
			glVertex3f (i, grid/2, 0);
			glVertex3f (-grid/2, i, 0);
			glVertex3f (grid/2, i, 0);
			glEnd ();
		}
	}
};


#endif	/* DRAWMETHODS_H */

