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

	static void drawPoints(BranchModel *bm)
	{
		Point3d branchAbs  = bm->getAbsolutePosition();

		glPointSize(4);
		glColor3f(1, 1, 1);
		glBegin(GL_POINTS);
		for (unsigned int j = 0; j < bm->nodeModelList.size(); j++) {
			NodeModel *node = bm->nodeModelList[j];
			glVertex3f(branchAbs.x + node->position->x, branchAbs.y + node->position->y, branchAbs.z + node->position->z);
		}
		glEnd();
	}

	static void
	drawEnvelopeSpline(SplineCrown *splineCrown, bool active, int activeMainPoint) {
		glPushMatrix();

		int n = 20; //liczba punktow na krzywej
		int loops = 12; //liczba petli

		int N = splineCrown->crownMainPoints.size();
		float startX = splineCrown->crownMainPoints[0]->x;
		float step = (splineCrown->crownMainPoints[N - 1]->x - startX) / (float) n;

		glTranslatef(splineCrown->x, splineCrown->y, splineCrown->z);
		for (int j = 0; j < loops; j++) {
			glColor3fv(active ? colorActive : colorUnactive);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < n + 1; i++) {
				float x, y;

				if (i == 0)
					x = startX;
				else
					x += step;

				y = splineCrown->s->getS(x);

				glVertex3f(y, 0, x);
			}
			glEnd();

			if (active) {
				glPointSize(4);
				glColor3fv(colorUnactive);
				glBegin(GL_POINTS);
				for (int i = 0; i < N; i++) {
					if (activeMainPoint == i) {
						glColor3fv(colorActivePoint);
						glVertex3f(splineCrown->crownMainPoints[i]->y, 0, splineCrown->crownMainPoints[i]->x);
						glColor3fv(colorUnactive);
					} else
						glVertex3f(splineCrown->crownMainPoints[i]->y, 0, splineCrown->crownMainPoints[i]->x);
				}
				glEnd();
			}

			glRotatef(360 / loops, 0, 0, 1);
		}
		glPopMatrix();
	}

	static void
	drawEnvelopeTruncatedCone(TruncatedConeCrown *truncatedConeCrown, bool active) {
		glPushMatrix();

		int loops = 12; //liczba petli
		glTranslatef(truncatedConeCrown->x, truncatedConeCrown->y, 0);
		for (int j = 0; j < loops; j++) {
			glColor3fv(active ? colorActive : colorUnactive);
			glBegin(GL_LINE_STRIP);
			glVertex3f(0, 0, truncatedConeCrown->z);
			glVertex3f(truncatedConeCrown->r_down, 0, truncatedConeCrown->z);
			glVertex3f(truncatedConeCrown->r_up, 0, truncatedConeCrown->z + truncatedConeCrown->h);
			glVertex3f(0, 0, truncatedConeCrown->z + truncatedConeCrown->h);
			glEnd();
			glRotatef(360 / loops, 0, 0, 1);
		}
		glPopMatrix();
	}

	static void
	drawEnvelopeSphere(SphereCrown *sphereCrown, bool active)
	{
		glPushMatrix();

		int loops = 6; //liczba petli
		int n = 20; //liczba punktów na półokręgu
		glTranslatef(sphereCrown->x, sphereCrown->y, sphereCrown->z);
		for (int j = 0; j < loops; j++) {
			glColor3fv(active ? colorActive : colorUnactive);
			glBegin(GL_LINE_STRIP);
			{
				for(int i=0; i<=n; i++)
				{
					float x = sphereCrown->r*cos(2.0*(float)i*M_PI/(float)n);
					float y = sphereCrown->r*sin(2.0*(float)i*M_PI/(float)n);
					glVertex3f(x,0,y);
				}
			}
			glEnd();
			glRotatef(180 / loops, 0, 0, 1);
		}
		glPopMatrix();
	}

	static void
	drawTrunk(BranchModel *bm, Parameters *params)
	{
		float kx=params->tp->kx;//3;//ile razy ma sie owinac tekstura
		float ky=params->tp->ky;//0.5;//ile jednostek tekstury na 1 jednostke dlugosci pnia
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

		glBindTexture(GL_TEXTURE_2D,texId);
		if(bm==NULL) return;
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

			glEnable (GL_TEXTURE_2D);
			glBegin(GL_TRIANGLES);
			// Obliczanie wspolrzednych tekstury na pniu
			if(i==0) { //pierwszy okrag galezi wiec tycoord=0
				float dx=root->segment->circlePts[0]->getDistance(root->segment->circlePts[1]);
				float D=tp->circlePoints*dx;;//oblicz obwod pnia
				for(int k=0; k<=tp->circlePoints; k++) {
					float txcoord=(k)*dx*kx/D; //obliczam wspolrzedna x tekstury
					Point2d tcoord(txcoord,0);
					prevCoords[1].push_back(tcoord);
				}
			}
			prevCoords[i&1].clear();// czyszcze wspolrzedne n-2 iteracji
			float dx=child->segment->circlePts[0]->getDistance(child->segment->circlePts[1]);
			float D=tp->circlePoints*dx;

			for(int k=0; k<=tp->circlePoints; k++) {
				// int j0 = (index + k) % tp->circlePoints;
				//  int j1 = (j0+1)%tp->circlePoints;
				float txcoord=(k*dx)*kx/D;
				//float dy=root->segment->circlePts[k%tp->circlePoints]->getDistance(child->segment->circlePts[j0]);
				//float tycoord=dy/ky+prevCoords[!(i&1)][j0].y;
				float tycoord=ky*(i+1);
				Point2d tcoord(txcoord,tycoord);
				prevCoords[i&1].push_back(tcoord);
			}

			// Laczymy pkty okregu root z pktami okregu child trojkatami
			//i0 - indeks pkt root
			//j0 - indeks pkt child
			//i1 - indeks nastepnego pkt root
			//j1 - indeks nastepnego pkt child
			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				// printf("i0=%d\n",i0);
				triangles+=2;
				int j0 = (index + i0) % tp->circlePoints;

				int i1 = (i0+1)%tp->circlePoints;
				int j1 = (j0+1)%tp->circlePoints;
				int tj1= (j0+1);
				int ti1= (i0+1);

				Point3d rootAbs, childAbs;
				rootAbs = bm->getAbsoluteNodePosition(root);
				childAbs = bm->getAbsoluteNodePosition(child);

				//glTexCoord2f (0.0f, 0.0f);
				glTexCoord2f(prevCoords[!(i&1)][i0].x,prevCoords[!(i&1)][i0].y);
				VERTEX_TRANS(rootAbs, (*root->segment->circlePts[i0]));

				//glTexCoord2f (0.0f, 1.0f);
				glTexCoord2f(prevCoords[i&1][j0].x,prevCoords[i&1][j0].y);
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j0]));

				//glTexCoord2f (1.0f, 1.0f);
				glTexCoord2f(prevCoords[i&1][tj1].x,prevCoords[i&1][tj1].y);
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j1]));

				//glTexCoord2f (0.0f, 1.0f);
				glTexCoord2f(prevCoords[!(i&1)][ti1].x,prevCoords[!(i&1)][ti1].y);
				VERTEX_TRANS(rootAbs, (*root->segment->circlePts[i1]));

				//glTexCoord2f (1.0f, 1.0f);
				glTexCoord2f(prevCoords[(i&1)][tj1].x,prevCoords[(i&1)][tj1].y);
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j1]));

				//glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
				glTexCoord2f(prevCoords[!(i&1)][i0].x,prevCoords[!(i&1)][i0].y);
				VERTEX_TRANS(rootAbs, (*root->segment->circlePts[i0]));

			}
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		glPopName();
	}

	static void
	drawLines(BranchModel *bm, TrunkParameters *tp)
	{
		if(bm==NULL) return;

		int nodeModelListLen = bm->nodeModelList.size();
		Point3d branchAbs  = bm->getAbsolutePosition();


		glColor3f(0.5,0,0);
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


			for (int i0 = 0; i0 < tp->circlePoints; i0++) {
				int j0 = (index + i0) % tp->circlePoints;
				triangles+=2;

				//glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				//glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
				int i1 = (i0+1)%tp->circlePoints;
				int j1 = (j0+1)%tp->circlePoints;
				glBegin(GL_LINES);
				VERTEX_TRANS(nodeAbs, (*node->segment->circlePts[i0]));
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j0]));
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j1]));
				VERTEX_TRANS(nodeAbs, (*node->segment->circlePts[i0]));
				glEnd();

				glBegin(GL_LINES);
				VERTEX_TRANS(nodeAbs, (*node->segment->circlePts[i0]));
				VERTEX_TRANS(nodeAbs, (*node->segment->circlePts[i1]));
				VERTEX_TRANS(childAbs, (*child->segment->circlePts[j1]));
				VERTEX_TRANS(nodeAbs, (*node->segment->circlePts[i0]));
				glEnd();
			}
		}

		int len = bm->childBranches.size();
		for (int i = 0; i < len; i++) {
			drawLines(bm->childBranches.at(i), tp);
		}
	}

public:

	static void
	render() {
		gtk_widget_queue_draw(windowWidget);

	}


	static void
	drawCoordinates(GLenum mode) {


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
		glRotatef(-90,1,0,0);
		DRAW_AXIS(2);
		glPopMatrix();
#undef DRAW_AXIS
#undef SOLID_CLOSED_CYLINDER


	}

	static void drawTrunks(Model3d *model, Parameters *params)
	{
		if(!model)
			return;

		glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color

		for(unsigned int i=0; i<model->branches.size(); i++)
		{
			drawTrunk(model->branches[i], params);
		}
	}

	static void drawTrunksEditMode(Model3d *model, Parameters *params)
	{
		if(!model)
			return;

		glInitNames();
		glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color

		for(unsigned int i=0; i<model->branches.size(); i++)
		{
			glPushName(i);
			if(model->selection->isBranchMarked(i))
			{
				BranchModel *bm = model->branches[i];
				Point3d branchAbs  = bm->getAbsolutePosition();

				glPointSize(6);
				for(unsigned int j=0; j< bm->nodeModelList.size(); j++)
				{
					if(model->selection->isNodeMarked(j))
					{
						glColor3f(1,0,0);
					} else
					{
						glColor3f(1,1,1);
					}
					glPushName(j);
					glBegin(GL_POINTS);
					VERTEX_TRANS(branchAbs, (*bm->nodeModelList[j]->position));
					glEnd();
					glPopName();
				}


				//drawPoints(bm);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(0, 1, 0, 0.3);
			} else
			{
				glDisable(GL_BLEND);
				glColor4f(1.0, 1.0, 1.0, 1.0);
			}

			drawTrunk(model->branches[i], params);
			glPopName();
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



	static void drawTrunksLines(Model3d *model, Parameters *params)
	{
		if(!model)
			return;

		for(unsigned int i=0; i<model->branches.size(); i++)
		{
			drawLines(model->branches[i], params->tp);
		}
	}


	static void drawEnvelopes(Crown *crown) {
		Crown *c = crown;

		for(unsigned int i=0; i<c->subcrowns.size(); i++)
		{
			Subcrown *sub = c->subcrowns.at(i);
			switch(sub->shape)
			{
			case SPLINE:
				drawEnvelopeSpline(SPLINE_CROWN(sub), crown->activeSubcrown == (int)i, SPLINE_CROWN(sub)->activePoint);
				break;
			case TRUNCATEDCONE:
				drawEnvelopeTruncatedCone(TRUNCATEDCONE_CROWN(sub),crown->activeSubcrown == (int)i);
				break;
			case SPHERE:
				drawEnvelopeSphere(SPHERE_CROWN(sub), crown->activeSubcrown == (int)i);
				break;
			default:
				break;
			}
		}
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
	static void drawLeaf(Point3d *p, Vector3d *dir, Vector3d *norm,Parameters *params,unsigned int cnt) {
		//nie ma zadnego aktywnego typu liscia
		if(params->lp->types.size()==0) return;
		initLeavesTextures(params);

		glColor4f(0.0, 0, 0, 0); // reset gl color
		triangles+=2;

		glEnable(GL_BLEND);// Turn Blending On
		glDisable(GL_DEPTH_TEST);
		glEnable (GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(p->x,p->y,p->z);
		unsigned int lastTypeId=cnt;
		//float size=params->lp->leaves[0].leafSize;

		unsigned int type=params->lp->types[lastTypeId].first;
		//float size=params->lp->types[lastTypeId].second;

		Point2d petiole = params->lp->leaves[type].petiole;

		/*ustalenie położenia liścia*/
		Vector3d a, b, *tmp1, *tmp2;
		tmp1 = dir->crossProduct(norm);
		tmp1->normalize();
		tmp1->mul(1-petiole.x);

		tmp2 = dir->crossProduct(norm);
		tmp2->normalize();
		tmp2->mul(-1.0*petiole.x);

		a.add(dir);
		a.add(tmp1);


		b.add(dir);
		b.add(tmp2);



#define TEXTURIZE\
		glBegin(GL_TRIANGLES);\
		{\
			glTexCoord2f (petiole.x, 1.0f);\
			glVertex3f(0,0,0);\
			glTexCoord2f (1.0f, 0.0f);\
			glVertex3f(a.d[0],a.d[1],a.d[2]);\
			glTexCoord2f (0.0f, 0.0f);\
			glVertex3f(b.d[0],b.d[1],b.d[2]);\
			glTexCoord2f (petiole.x, 1.0f);\
			glVertex3f(0,0,0);\
			glTexCoord2f (0.0f, 0.0f);\
			glVertex3f(b.d[0],b.d[1],b.d[2]);\
			glTexCoord2f (0.0f, 1.0f);\
			glVertex3f(tmp2->d[0],tmp2->d[1],tmp2->d[2]);\
			glTexCoord2f (petiole.x, 1.0f);\
			glVertex3f(0,0,0);\
			glTexCoord2f (1.0f, 1.0f);\
			glVertex3f(tmp1->d[0],tmp1->d[1],tmp1->d[2]);\
			glTexCoord2f (1.0f, 0.0f);\
			glVertex3f(a.d[0],a.d[1],a.d[2]);\
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
		delete tmp1;
		delete tmp2;
	}

	static void drawLeaves(BranchModel *bm, Parameters *params,unsigned int cnt)
	{
		if(!bm) return;

		for(unsigned int i=0; i<bm->nodeModelList.size(); i++)
			if(bm->nodeModelList[i]->leaf)
			{
				NodeModel *node = bm->nodeModelList[i];
				Point3d leafPoint = bm->getAbsoluteNodePosition(node);
				drawLeaf(&leafPoint, node->leaf->dir, node->leaf->norm,params,cnt++);
				if(cnt >= params->lp->types.size()) cnt=0;
			}

		for(unsigned int i = 0; i< bm->childBranches.size(); i++)
		{
			drawLeaves(bm->childBranches.at(i),params,cnt);
		}

	}
	static void
	drawTreeModel(Parameters *params, bool generatorMode)
	{
		glPushMatrix();
		TrunkParameters *tp = params->tp;
		params->xp->triangles=triangles=0;
		if (generatorMode) {
			if (params->rp->showGrass)
				drawGrass();

			if (params->rp->showEnvelopes)
				drawEnvelopes(params->crown);

			if (cm->params->methodParams->activeMethod == 0) {
				if (params->rp->showBark)
					drawTrunks(model, params);
				else drawLines(model ? model->getRootBranch() : NULL, tp);

				if (params->rp->showLeaves)
					drawLeaves(model ? model->getRootBranch() : NULL, params, 0);
			}
			if (cm->params->methodParams->activeMethod == 1) {
				drawLines(model ? model->getRootBranch() : NULL, tp);
			}
		} else {
			drawTrunksEditMode(model, params);
		}
		params->xp->triangles=triangles;
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

