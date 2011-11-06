#ifndef DRAWMETHODS_H
#define	DRAWMETHODS_H

#include "GlobalVar.h"
#include "Bmp.h"
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkglglext.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>

GtkWidget *windowWidget;

Model3d *model = NULL;

class DrawMethods {


public:

	static void render() {
		gtk_widget_queue_draw(windowWidget);

	}
	static void drawCoordinates() {
		glBegin(GL_LINES);

		//os y
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);
		//os x
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);
		glEnd();

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

				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
//				int i1 = (i0+1)%tp->circlePoints;
//                                int j1 = (j0+1)%tp->circlePoints;
//
//                                glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
//                                glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
//                                glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
//
//                                glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
//                                glVertex3f(root->segment->circlePts[i1]->x,root->segment->circlePts[i1]->y,root->segment->circlePts[i1]->z);
//                                glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);


			}
			glEnd();

			glPointSize(3);
			glColor3f(1,0,0);
			glBegin(GL_POINTS);
			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				int j0 = (index + i0) % tp->circlePoints;

				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
			}
			glEnd();
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			drawLines(bm->childBranches.at(i), tp);
		}
	}


	static void drawEnvelope() {

		glPushMatrix();

		int n=20;//liczba punktow na krzywej
		int loops=12;//liczba petli

		Crown *c = cm->crown;

		int N = cm->params->crownMainPoints.size();
		float startX = cm->params->crownMainPoints[0]->x;
		float step = (cm->params->crownMainPoints[N - 1]->x - startX) / (float)n;


		for(int j=0; j<loops; j++) {
			glColor3f (0, 0.5, 1);
			glBegin(GL_LINE_STRIP);
			for(int i=0; i<n+1; i++) {
				float x, y;

				if(i == 0)
					x = startX;
				else
					x += step;

				y = c->s->getS(x);

				glVertex3f(y,0,x);
			}
			glEnd();


			glPointSize(4);
			glColor3f (1, 0, 1);
			glBegin(GL_POINTS);
			for(int i=0; i<N; i++)
			{
				glVertex3f(cm->params->crownMainPoints[i]->y,0,cm->params->crownMainPoints[i]->x);
			}
			glEnd();

			float x_max, y_max;
			c->s->globalMax(&x_max, &y_max);
			glColor3f (1, 1, 1);
			glBegin(GL_POINTS);
			glVertex3f(y_max,0,x_max);
			glEnd();

			glRotatef(360/loops,0,0,1);
		}
		glPopMatrix();
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
		}
		int height = bmp.header.biHeight;
		int width = bmp.header.biWidth;
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, bmp.data);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
		glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color

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

	/* Do poprawienia - usunac przezroczystosc lisci - NEHE art 20 */

	static void drawLeaf(Point3d *p, Vector3d *dir) {


		static GLuint texId;
		static Bmp bmp;
		static int initialized=0;
		static char tex_path[] = "textures/leaf_wiki256.bmp";

		if(!initialized) {
			initialized=1;
			bmp.load (tex_path);
			bmp.addAlpha();
			glGenTextures (1, &texId);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glBindTexture (GL_TEXTURE_2D, texId);
		}
		int height = bmp.header.biHeight;
		int width = bmp.header.biWidth;
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,GL_UNSIGNED_BYTE, bmp.data);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
		glColor4f(1.0, 0, 0, 0); // reset gl color

		Vector3d a, b;
		a.d[2] = b.d[2] = dir->d[2]/2;
		a.d[0] = dir->length()*sin(asin(dir->d[1]/dir->length()-M_PI_4));
		b.d[0] = dir->d[0] - a.d[0];

		a.d[1] = dir->length()*cos(asin(dir->d[1]/dir->length()-M_PI_4));
		b.d[1] = dir->d[1] - a.d[1];

		glEnable(GL_BLEND);     // Turn Blending On
		glEnable (GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(p->x,p->y,p->z);
		glBlendFunc(GL_DST_COLOR,GL_ZERO);
		glBegin(GL_QUADS);
		{
			glTexCoord2f (0.0f, 0.0f);
			glVertex3f(0,0,0);

			glTexCoord2f (1.0f, 0.0f);
			glVertex3f(a.d[0],a.d[1],a.d[2]);

			glTexCoord2f (1.0f, 1.0f);
			glVertex3f(dir->d[0],dir->d[1],dir->d[2]);

			glTexCoord2f (0.0f, 1.0f);
			glVertex3f(b.d[0],b.d[1],b.d[2]);
		}
		glEnd();
		glPopMatrix();
		glDisable (GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glColor4f(1.0, 1.0, 1.0, 1.0);

	}

	static void drawLeaves(BranchModel *bm)
	{
		if(!bm) return;

		Point3d *leaf = &bm->nodeModelList.at(bm->nodeModelList.size()-1)->node->point;
		Point3d *befLeaf = &bm->nodeModelList.at(bm->nodeModelList.size()-2)->node->point;

		Vector3d *leafVect = new Vector3d(befLeaf, leaf);
		leafVect->normalize();
		leafVect->mul(1);


		drawLeaf(leaf, leafVect);
		delete leafVect;

		for(unsigned int i = 0; i< bm->childBranches.size(); i++)
		{
			drawLeaves(bm->childBranches.at(i));
		}

	}

	static void drawTreeModel (TrunkParameters *tp) {
		glPushMatrix();

		if(cm->params->showEnvelope)
			drawEnvelope();

		if(cm->params->activeMethod==0) {
			drawLines(bm,tp);
			drawLeaves(bm);
		}
		if(cm->params->activeMethod==1) {
			drawLines(bm,tp);
		}
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

