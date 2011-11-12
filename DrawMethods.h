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

GtkWidget *windowWidget;

Model3d *model = NULL;
static int triangles=0;
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
         glPushName (i);\
gluQuadricDrawStyle(quadric, GLU_FILL);\
gluQuadricOrientation(quadric, GLU_INSIDE);\
SOLID_CLOSED_CYLINDER(quadric, 0.1f, 0.1f, 3.0f, 8, 8)\
SOLID_CLOSED_CYLINDER(quadric, 0.2f, 0.01f, 0.5f, 8, 8)\
glTranslatef(0.0,0.0,-3.5);\
gluDeleteQuadric(quadric);\
if(mode==GL_SELECT)\
  glPopName();
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


	static void drawTrunk(BranchModel *bm, Parameters *params)
	{
		TrunkParameters *tp=params->tp;
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
		glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color

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

				glTexCoord2f (0.0f, 0.0f);
				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glTexCoord2f (1.0f, 0.0f);
				glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
				glTexCoord2f (1.0f, 1.0f);
				glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);

				glTexCoord2f (0.0f, 0.0f);
				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glTexCoord2f (0.0f, 1.0f);
				glVertex3f(root->segment->circlePts[i1]->x,root->segment->circlePts[i1]->y,root->segment->circlePts[i1]->z);
				glTexCoord2f (1.0f, 1.0f);
				glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
				triangles+=2;
			}

			glEnd();
			glDisable(GL_TEXTURE_2D);
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			drawTrunk(bm->childBranches.at(i), params);
		}
	}


	static void drawLines(BranchModel *bm, TrunkParameters *tp)
	{
		if(bm==NULL) return;
		glColor3f(1,0.5,0);
		int nodeModelListLen = bm->nodeModelList.size();
		for(int i=0; i<nodeModelListLen-1; i++)
		{
			NodeModel *root= bm->nodeModelList.at(i);
			NodeModel *child = bm->nodeModelList.at(i+1);
			int index = child->segment->index;

			glBegin(GL_LINES);
			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				int j0 = (index + i0) % tp->circlePoints;

				//glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				//glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
				int i1 = (i0+1)%tp->circlePoints;
				int j1 = (j0+1)%tp->circlePoints;

				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
				glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);

				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glVertex3f(root->segment->circlePts[i1]->x,root->segment->circlePts[i1]->y,root->segment->circlePts[i1]->z);
				glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
				triangles+=2;
			}
			glEnd();
			/* Rysowanie pkt nodow
			glPointSize(3);
			glColor3f(1,0,0);
			glBegin(GL_POINTS);
			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				int j0 = (index + i0) % tp->circlePoints;

				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
			}
			glEnd();*/
		}

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
				drawEnvelopeSpline((SplineCrown*)sub);
			} else if(sub->shape == CYLINDER)
			{
				drawEnvelopeCylinder((CylinderCrown*)sub);
			}
		}
	}

	static void drawEnvelopeSpline(SplineCrown *splineCrown)
	{
		glPushMatrix();

		int n=20;//liczba punktow na krzywej
		int loops=12;//liczba petli

		int N = splineCrown->crownMainPoints.size();
		float startX = splineCrown->crownMainPoints[0]->x;
		float step = (splineCrown->crownMainPoints[N - 1]->x - startX) / (float)n;

		glTranslatef(splineCrown->x, splineCrown->y, splineCrown->z);
		for(int j=0; j<loops; j++) {
			glColor3f (0, 0.5, 1);
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

			glPointSize(4);
			glColor3f (1, 0, 1);
			glBegin(GL_POINTS);
			for(int i=0; i<N; i++)
			{
				glVertex3f(splineCrown->crownMainPoints[i]->y,0,splineCrown->crownMainPoints[i]->x);
			}
			glEnd();

			float x_max, y_max;
			splineCrown->s->globalMax(&x_max, &y_max);
			glColor3f (1, 1, 1);
			glBegin(GL_POINTS);
			glVertex3f(y_max,0,x_max);
			glEnd();

			glRotatef(360/loops,0,0,1);
		}
		glPopMatrix();
	}

	static void drawEnvelopeCylinder(CylinderCrown *cylinderCrown)
	{
		glPushMatrix();

		int loops=12;//liczba petli
		glTranslatef(cylinderCrown->x, cylinderCrown->y, 0);
		for(int j=0; j<loops; j++) {
			glColor3f (0, 0.5, 1);
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
		glColor3f (1, 0, 0);
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


	static void drawLeaf(Point3d *p, Vector3d *dir,Parameters *params) {


		static GLuint texId,maskId;

		if(!params->lp->leavesTexInitialized) {
			params->lp->leavesTexInitialized=1;
			Bmp bmp,*mask;
			bmp.load (params->lp->leavesPath);
			mask=bmp.getMask();

			glGenTextures (1, &texId);
			glGenTextures(1,&maskId);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			int height = bmp.header.biHeight;
			int width = bmp.header.biWidth;
			glBindTexture(GL_TEXTURE_2D,maskId);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, bmp.data);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering

			glBindTexture(GL_TEXTURE_2D,texId);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, mask->data);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
			bmp.cleanup();
			mask->cleanup();
			delete mask;

		}
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
		float size=params->lp->leavesSize;

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
		glBindTexture(GL_TEXTURE_2D,texId);
		TEXTURIZE;

		//texture image
		glBlendFunc(GL_ONE,GL_ONE);
		glBindTexture(GL_TEXTURE_2D,maskId);

		TEXTURIZE;

#undef TEXTURIZE

		glPopMatrix();
		glDisable (GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glColor4f(1.0, 1.0, 1.0, 1.0);

	}

	static void drawLeaves(BranchModel *bm,Parameters *params)
	{
		if(!bm) return;

		Point3d *leaf = &bm->nodeModelList.at(bm->nodeModelList.size()-1)->node->point;
		Point3d *befLeaf = &bm->nodeModelList.at(bm->nodeModelList.size()-2)->node->point;

		Vector3d *leafVect = new Vector3d(befLeaf, leaf);
		leafVect->normalize();
		leafVect->mul(1);


		drawLeaf(leaf, leafVect,params);
		delete leafVect;

		for(unsigned int i = 0; i< bm->childBranches.size(); i++)
		{
			drawLeaves(bm->childBranches.at(i),params);
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
		if(cm->params->activeMethod==0) {
			if(params->rp->showBark)
				drawTrunk(bm,params);
			else drawLines(bm,tp);
			//drawLines(bm,tp);
			if(params->rp->showLeaves)
				drawLeaves(bm,params);
		}
		if(cm->params->activeMethod==1) {
			drawLines(bm,tp);
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

