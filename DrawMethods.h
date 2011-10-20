#ifndef DRAWMETHODS_H
#define	DRAWMETHODS_H

#include "GlobalVar.h"

class DrawMethods {
public:
        
        static void drawCoordinates() {
                glTranslatef((float) grid / 2.0 + 0.5, (float) grid / 2 + 0.5, 0);


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

	static void drawCircle(Node *root)
	{
		Segment *s = root->segment;
		glPointSize(4);
		glColor3f (1, 0, 0);

		glBegin(GL_POINTS);
		glVertex3f(root->point.x,
		           root->point.y,
		           root->point.z);
		glEnd();

		glColor3f (1, 1, 0);
		glBegin(GL_POINTS);
		for (int i = 0; i < CIRCLE_PTS_COUNT; i++) {

			glVertex3f(s->circlePts[i]->x,
			           s->circlePts[i]->y,
			           s->circlePts[i]->z);

		}
		glEnd();

		int childLen = root->getChildLen();

		for(int i=0; i< childLen; i++)
		{
			drawCircle(root->getChildAt(i));
		}
	}

	static void drawLines(Node *root)
	{
		glColor3f(1,0.5,0);
		int childLen = root->getChildLen();
		for(int i=0; i<childLen; i++)
		{
			Node *child = root->getChildAt(i);
			int index = child->segment->index;
			glBegin(GL_LINES);
			for(int i0=0; i0<CIRCLE_PTS_COUNT; i0++)
			{
				int j0 = (index + i0)%CIRCLE_PTS_COUNT;

				glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
				glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);

//                                int i1 = (i0+1)%CIRCLE_PTS_COUNT;
//                                int j1 = (j0+1)%CIRCLE_PTS_COUNT;
//
//                                Vector3d *v1 = new Vector3d(root->segment->circlePts[i0],root->segment->circlePts[i1]);
//                                Vector3d *v2 = new Vector3d(root->segment->circlePts[i0], child->segment->circlePts[j1]);
//                                Vector3d *normal1 = v1->crossProduct(v2);
//                                normal1->mul(-1);
//
//                                glNormal3f(normal1->d[0],normal1->d[1],normal1->d[2]);
//                                glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
//                                glVertex3f(child->segment->circlePts[j0]->x,child->segment->circlePts[j0]->y,child->segment->circlePts[j0]->z);
//                                glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
//
//                                glVertex3f(root->segment->circlePts[i0]->x,root->segment->circlePts[i0]->y,root->segment->circlePts[i0]->z);
//                                glVertex3f(root->segment->circlePts[i1]->x,root->segment->circlePts[i1]->y,root->segment->circlePts[i1]->z);
//                                glVertex3f(child->segment->circlePts[j1]->x,child->segment->circlePts[j1]->y,child->segment->circlePts[j1]->z);
			}

			glEnd();
			drawLines(child);
		}
	}


	static void drawEnvelope() {

		glPushMatrix();
		glColor3f (0, 0.2, 1);

		float r=cm->params->crownRadius;
		int n=20;//liczba punktow na okregu
		int loops=6;//liczba petli

		for(int j=0; j<loops; j++) {
			glBegin(GL_LINE_STRIP);
			for(int i=0; i<n+1; i++) {
				float oldX=0;
				float oldY=0;
				float oldZ=9;
				float newY=oldY;
				float newX=oldX+r*cos(6.28318 * i / n);
				float newZ=oldZ+r*sin(6.28318 * i / n);
				glVertex3f(newX,newY,newZ);
			}
			glEnd();
			glRotatef(180/loops,0,0,1);
		}
		glPopMatrix();
	}
        
        static void drawEnvelope2() {

		glPushMatrix();
		
                
		int n=20;//liczba punktow na okregu
		int loops=12;//liczba petli

                int N = 4;
                float x[] = {6,7,10,12};
                float y[] = {0,2,5,0};
                Spline *s = new Spline(x,y,N);
        
                
                float startX = x[0];
                float step = (x[N-1] - x[0]) / (float)n;
                
                
		for(int j=0; j<loops; j++) {
                        glColor3f (0, 0.5, 1);
			glBegin(GL_LINE_STRIP);
			for(int i=0; i<n+1; i++) {
                                float x, y;
                                
                                if(i == 0)
                                        x = startX;
                                else
                                        x += step;
                                
                                y = s->getS(x);
                                
				glVertex3f(y,0,x);
			}
			glEnd();
                        
                        
                        glPointSize(4);
                        glColor3f (1, 0, 1);
                        glBegin(GL_POINTS);
                        for(int i=0; i<N; i++)
                        {
                                glVertex3f(y[i],0,x[i]);
                        }
                        glEnd();
                        
			glRotatef(360/loops,0,0,1);
		}
                delete s;
		glPopMatrix();
	}

	static void drawTreeModel () {
		glPushMatrix();
		//glRotatef (90, 1, 0, 0);
		//glRotatef (270, 0, 1, 0);

		if(cm->params->showEnvelope)
			drawEnvelope();

                drawEnvelope2();
                
		if(cm!=NULL && cm->nodes.size() > 0 && cm->nodes[0]!=NULL)
			drawLines(cm->nodes[0]);

		glPopMatrix();


		//drawing textures
//		static int tgen = 0;
//
//		GLuint texId;
//
//		if (tgen == 0) {
//			static Bmp bmp;
//
//			//char tex_path[] = "textures/leaf_wiki256.bmp";
//			char tex_path[]="textures/bark_256.bmp";
//
//
//			bmp.load (tex_path);
//			// bmp.addAlpha();//
//			g_print ("loaded");
//			tgen = 1;
//
//			int height = bmp.header.biHeight;
//
//			int width = bmp.header.biWidth;
//
//			glGenTextures (1, &texId);
//			glBindTexture (GL_TEXTURE_2D, texId);
//			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
//			              GL_UNSIGNED_BYTE, bmp.data);
//			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
//			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
//
//
//		}
//
//		vector<Point3d> circles[10];
//		glColor4f (1, 1, 1, 0);
//
//		int n=8;//liczba punktow na okregu
//		int m=6;//liczba punktow pnia
//		float r=1;
//		//oblicz coordynaty pnia
//		for (int i = 0; i < m; i++) {
//			Point3d *p = &cm.nodes[i].point;
//			for(int j=0; j<n; j++) {
//				int oldX=p->x;
//				int oldY=p->z;
//				float newZ=p->y;
//				float newX=oldX+r*cos(6.28318 * j / n);
//				float newY=oldY+r*sin(6.28318 * j / n);
//				circles[i].push_back(Point3d(newX,newY,newZ));
//
//			}
//			r*=0.8;
//		}
//		glEnable (GL_TEXTURE_2D);
//		for(int i=0; i<m-1; i++)
//			for(int j=0; j<n; j++) {
//				Point3d a=circles[i][j];
//				Point3d b=circles[i][(j+1)%n];
//				Point3d c=circles[i+1][(j+1)%n];
//				Point3d d=circles[i+1][j];
//
//				glBegin(GL_QUADS);
//				glTexCoord2f (0.0f, 0.0f);
//				glVertex3f(a.x,a.y,a.z);
//				glTexCoord2f (1.0f, 0.0f);
//
//				glVertex3f(b.x,b.y,b.z);
//				glTexCoord2f (1.0f, 1.0f);
//
//				glVertex3f(c.x,c.y,c.z);
//				glTexCoord2f (0.0f, 1.0f);
//
//				glVertex3f(d.x,d.y,d.z);
//				glEnd();
//			}
//		glDisable (GL_TEXTURE_2D);


//glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color
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
