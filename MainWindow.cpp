#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <gtk/gtkgl.h>
#include <gdk/gdkglglext.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cassert>
#include "ColonizationMethod.h"
#include "ColonizationParameters.h"
#include "Bmp.h"
#include "Model3d.h"



#define DEFAULT_WIDTH  800
#define DEFAULT_HEIGHT 600
#define DEFAULT_TITLE  "TreeMaker"

#define TIMEOUT_INTERVAL 10

#define MAXGRID 63
#define SQRTOFTWOINV (1.0 / 1.414213562)


/**************************************************************************
 * Global variable declarations.
 **************************************************************************/


static gboolean coordinates = FALSE;

static int grid = (MAXGRID / 2);

static int beginX, beginY;

static float sphi = 90.0;

static float stheta = 45.0;

static float sdepth = 5.0 / 4.0 * (MAXGRID / 2);

static float zNear = (MAXGRID / 2) / 100.0;

static float zFar = (MAXGRID / 2) * 3.0;

static float aspect = 5.0 / 4.0;

static float lightPosition[4] = { 0.0, 0.0, 1.0, 1.0 };




class MainWindow
{
	GtkWidget *window;
	GdkGLConfig *glconfig;


	static void drawCoordinates () {
		glTranslatef ((float) grid / 2.0 + 0.5, (float) grid / 2 + 0.5, 0);

		if (coordinates) {
			glBegin (GL_LINES);

			//os y
			glColor3f (1, 0, 0);
			glVertex3f (0, 0, 0);
			glVertex3f (0, 0, 10);
			//os x
			glColor3f (0, 0, 1);
			glVertex3f (0, 0, 0);
			glVertex3f (10, 0, 0);
			glEnd ();

		}
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


	static void drawTreeModel () {
		glPushMatrix();
		glRotatef (90, 1, 0, 0);
		glRotatef (270, 0, 1, 0);

		static int generated = 0;

		static ColonizationMethod cm;

		if (generated == 0) {
			cm.init ();
			cm.generate (60);
			generated = 1;
		}

		glPointSize (3);

		glColor3f (0, 0, 0);
		glPointSize (2);
		glBegin (GL_POINTS);
		for (unsigned int i = 0; i < cm.nodes.size (); i++) {
			Point3d *p = &cm.nodes[i]->point;

			glVertex3f (p->x, p->y, p->z);
		}

		glEnd ();

		glColor3f (1, 0, 0);
		glBegin (GL_POINTS);
		for (unsigned int i = 0; i < cm.aPointsCopy.size (); i++) {
			Point3d *p = &cm.aPointsCopy[i];

			glVertex3f (p->x + 0.1, p->y + 0.1, p->z + 0.1);
		}

		glEnd ();

		Model3d *m = new Model3d(cm.nodes[0]);
		m->generateModel();
		drawLines(cm.nodes[0]);

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

		for (i = 0; i <= grid; i++) {
			glBegin (GL_LINES);
			glVertex3f (i, 0, 0);
			glVertex3f (i, grid, 0);
			glVertex3f (0, i, 0);
			glVertex3f (grid, i, 0);
			glEnd ();
		}
	}

	static void realize (GtkWidget * widget, gpointer data) {
		GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);

		GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

		GdkGLProc proc = NULL;

		/*** OpenGL BEGIN ***/
		if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
			return;

		/* glPolygonOffsetEXT */
		proc = gdk_gl_get_glPolygonOffsetEXT ();
		if (proc == NULL) {
			/* glPolygonOffset */
			proc = gdk_gl_get_proc_address ("glPolygonOffset");
			if (proc == NULL) {
				g_print
				("Sorry, glPolygonOffset() is not supported by this renderer.\n");
				exit (1);
			}
		}

		glEnable (GL_DEPTH_TEST);
		glDepthFunc (GL_LEQUAL);
		glClearColor (0.8, 0.8, 1, 0.0);
		gdk_gl_glPolygonOffsetEXT (proc, 1.0, 1.0);
		//glEnable (GL_CULL_FACE);
		glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		//glEnable (GL_COLOR_MATERIAL);
		//glColorMaterial (GL_FRONT, GL_DIFFUSE);
		glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
		glEnable (GL_LIGHT0);
		glShadeModel (GL_FLAT);

		//glDisable (GL_LIGHTING);

		gdk_gl_drawable_gl_end (gldrawable);
		/*** OpenGL END ***/

		return;
	}



	static gboolean
	configure_event (GtkWidget * widget,
	                 GdkEventConfigure * event, gpointer data) {
		GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);

		GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

		GLfloat w = widget->allocation.width;

		GLfloat h = widget->allocation.height;

		/*** OpenGL BEGIN ***/
		if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
			return FALSE;

		aspect = (float) w / (float) h;
		glViewport (0, 0, w, h);

		gdk_gl_drawable_gl_end (gldrawable);
		/*** OpenGL END ***/

		return TRUE;
	}

	static gboolean
	expose_event (GtkWidget * widget,
	              GdkEventExpose * event, gpointer data) {
		GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);

		GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

		/*** OpenGL BEGIN ***/
		if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
			return FALSE;
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		gluPerspective (64.0, aspect, zNear, zFar);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();

		glTranslatef (0.0, 0.0, -sdepth);
		glRotatef (-stheta, 1.0, 0.0, 0.0);
		glRotatef (sphi, 0.0, 0.0, 1.0);
		glTranslatef (-(float) ((grid + 1) / 2 - 1),
		              -(float) ((grid + 1) / 2 - 1), 0.0);

		MainWindow::drawWireframe ();
		MainWindow::drawCoordinates ();
		MainWindow::drawTreeModel ();

		/* Swap buffers */
		if (gdk_gl_drawable_is_double_buffered (gldrawable))
			gdk_gl_drawable_swap_buffers (gldrawable);
		else
			glFlush ();

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gdk_gl_drawable_gl_end (gldrawable);
		/*** OpenGL END ***/

		return TRUE;
	}

	static gboolean
	motion_notify_event (GtkWidget * widget,
	                     GdkEventMotion * event, gpointer data) {
		gboolean redraw = FALSE;

		if (event->state & GDK_BUTTON1_MASK) {
			sphi += (float) (event->x - beginX) / 4.0;
			stheta += (float) (beginY - event->y) / 4.0;

			redraw = TRUE;
		}

		/*if (event->state & GDK_BUTTON2_MASK)
		   {
		   sdepth -= ((event->y - beginY)/(widget->allocation.height))*(MAXGRID/2);

		   redraw = TRUE;
		   } */

		beginX = event->x;
		beginY = event->y;

		if (redraw)
			gdk_window_invalidate_rect (widget->window,
			                            &widget->allocation, FALSE);

		return TRUE;
	}
	static gboolean
	button_press_event (GtkWidget * widget,
	                    GdkEventButton * event, gpointer data) {
		if (event->button == 1) {
			beginX = event->x;
			beginY = event->y;
			return TRUE;
		}

		if (event->button == 2) {
			beginX = event->x;
			beginY = event->y;
			return TRUE;
		}

		return FALSE;
	}


	static gboolean
	key_press_event (GtkWidget * widget,
	                 GdkEventKey * event, gpointer data) {
		switch (event->keyval) {
		case GDK_r:
			init_wireframe (widget);
			break;

		case GDK_plus:
			sdepth -= 2.0;
			break;

		case GDK_minus:
			sdepth += 2.0;
			break;

		case GDK_Escape:
			gtk_main_quit ();
			break;

		default:
			return FALSE;
		}

		gdk_window_invalidate_rect (widget->window,
		                            &widget->allocation, FALSE);

		return TRUE;
	}

	static void unrealize (GtkWidget * widget, gpointer data) {
		/*** Fill in the details here ***/

	}
	static gboolean
	scroll_event (GtkWidget * widget,
	              GdkEventScroll * event, gpointer data)
	{
		gboolean redraw = FALSE;

		if (event->direction == GDK_SCROLL_UP)
		{
			sdepth -= 2;
			redraw = TRUE;
		}
		if (event->direction == GDK_SCROLL_DOWN)
		{
			sdepth += 2;
			redraw = TRUE;
		}

		if(redraw)
			gdk_window_invalidate_rect (widget->window,
			                            &widget->allocation, FALSE);

		return TRUE;
	}
	static gboolean
	button_press_event_popup_menu (GtkWidget * widget,
	                               GdkEventButton * event, gpointer data)
	{
		if (event->button == 3) {
			/* Popup menu. */
			gtk_menu_popup (GTK_MENU (widget), NULL, NULL, NULL,
			                NULL, event->button, event->time);
		}
		return TRUE;
	}


	static void init_wireframe (GtkWidget * widget) {
		gdk_window_invalidate_rect (widget->window, &widget->allocation,
		                            FALSE);
	}
	static void show_coordinates (GtkWidget * widget) {
		coordinates = !coordinates;
		gdk_window_invalidate_rect (widget->window,
		                            &widget->allocation, FALSE);
	}

	GtkWidget *create_popup_menu (GtkWidget * drawing_area) {
		GtkWidget *menu;

		GtkWidget *menu_item;

		menu = gtk_menu_new ();


		/* Init wireframe model */
		menu_item = gtk_menu_item_new_with_label ("Initialize");
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		g_signal_connect_swapped (G_OBJECT (menu_item), "activate",
		                          G_CALLBACK (init_wireframe), drawing_area);
		gtk_widget_show (menu_item);

		/* Show coordinates */
		menu_item = gtk_menu_item_new_with_label ("Show Coordinates");
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		g_signal_connect_swapped (G_OBJECT (menu_item), "activate",
		                          G_CALLBACK (show_coordinates),
		                          drawing_area);
		gtk_widget_show (menu_item);

		/* Quit */
		menu_item = gtk_menu_item_new_with_label ("Quit");
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		g_signal_connect (G_OBJECT (menu_item), "activate",
		                  G_CALLBACK (gtk_main_quit), NULL);
		gtk_widget_show (menu_item);

		return menu;
	}
	GtkWidget *createWindow (GdkGLConfig * glconfig) {
		GtkWidget *window;

		GtkWidget *hbox;
		GtkWidget *vbox;

		GtkWidget *drawing_area;

		GtkWidget *menu;

		//GtkWidget *button;

		/*
		 * Top-level window.
		 */

		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), DEFAULT_TITLE);

		/* Get automatically redrawn if any of their children changed allocation. */
		gtk_container_set_reallocate_redraws (GTK_CONTAINER (window), TRUE);

		/* Connect signal handlers to the window */
		g_signal_connect (G_OBJECT (window), "delete_event",
		                  G_CALLBACK (gtk_main_quit), NULL);

		/*
		 * VBox.
		 */

		hbox = gtk_hbox_new (FALSE, 0);
		vbox = gtk_vbox_new(FALSE,1);
		gtk_container_add (GTK_CONTAINER (window), hbox);
		//gtk_container_add(GTK_CONTAINER(window),vbox);
		gtk_widget_show (hbox);
		gtk_widget_show (vbox);

		/*
		 * Drawing area to draw OpenGL scene.
		 */

		drawing_area = gtk_drawing_area_new ();
		gtk_widget_set_size_request (drawing_area, DEFAULT_WIDTH,
		                             DEFAULT_HEIGHT);

		/* Set OpenGL-capability to the widget */
		gtk_widget_set_gl_capability (drawing_area,
		                              glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);

		gtk_widget_add_events (drawing_area,
		                       GDK_BUTTON1_MOTION_MASK |
		                       GDK_BUTTON2_MOTION_MASK |
		                       GDK_BUTTON_PRESS_MASK |
		                       GDK_SCROLL_MASK | GDK_VISIBILITY_NOTIFY_MASK);

		/* Connect signal handlers to the drawing area */

		g_signal_connect_after (G_OBJECT (drawing_area), "realize",
		                        G_CALLBACK (realize), NULL);
		g_signal_connect (G_OBJECT (drawing_area), "configure_event",
		                  G_CALLBACK (configure_event), NULL);
		g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		                  G_CALLBACK (expose_event), NULL);

		g_signal_connect (G_OBJECT (drawing_area),
		                  "motion_notify_event",
		                  G_CALLBACK (motion_notify_event), NULL);

		g_signal_connect (G_OBJECT (drawing_area), "scroll_event",
		                  G_CALLBACK (scroll_event), NULL);


		g_signal_connect (G_OBJECT (drawing_area), "button_press_event",
		                  G_CALLBACK (button_press_event), NULL);
		g_signal_connect (G_OBJECT (drawing_area), "unrealize",
		                  G_CALLBACK (unrealize), NULL);

		/* key_press_event handler for top-level window */
		g_signal_connect_swapped (G_OBJECT (window), "key_press_event",
		                          G_CALLBACK (key_press_event), drawing_area);


		gtk_box_pack_start (GTK_BOX (hbox), drawing_area, TRUE, TRUE, 0);

		gtk_widget_show (drawing_area);

		/*
		 * Popup menu.
		 */

		menu = create_popup_menu (drawing_area);

		g_signal_connect_swapped (G_OBJECT (drawing_area),
		                          "button_press_event",
		                          G_CALLBACK (button_press_event_popup_menu),
		                          menu);

		/* Colonization algorithm parameters */
		//	GtkWidget *colonizationParameters;
		GtkWidget *otherParameters;

		otherParameters=gtk_frame_new("Other Parameters");
		//gtk_frame_set_shadow_type(GTK_FRAME(colonizationParameters),GTK_SHADOW_IN);

		//
		gtk_box_pack_start(GTK_BOX(hbox),vbox,FALSE,FALSE,1);

		ColonizationParameters cparams(vbox);
		//	gtk_box_pack_start(GTK_BOX(vbox),colonizationParameters,FALSE,FALSE,1);
		gtk_box_pack_start(GTK_BOX(vbox),otherParameters,FALSE,FALSE,1);

//		gtk_widget_show(colonizationParameters);
		gtk_widget_show(otherParameters);

		/*
		 * Simple quit button.
		 */

		//button = gtk_button_new_with_label ("Quit");

		//g_signal_connect (G_OBJECT (button), "clicked",
		//                 G_CALLBACK (gtk_main_quit), NULL);

		//gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);

		//gtk_widget_show (button);

		return window;

	}
	GdkGLConfig *configureGL () {
		GdkGLConfig *glconfig;

		/* Try double-buffered visual */
		glconfig =
		    gdk_gl_config_new_by_mode (GdkGLConfigMode (GDK_GL_MODE_RGB |
		                               GDK_GL_MODE_DEPTH |
		                               GDK_GL_MODE_DOUBLE));
		if (glconfig == NULL) {
			g_print ("\n*** Cannot find the double-buffered visual.\n");
			g_print ("\n*** Trying single-buffered visual.\n");

			/* Try single-buffered visual */
			glconfig =
			    gdk_gl_config_new_by_mode (GdkGLConfigMode
			                               (GDK_GL_MODE_RGB |
			                                GDK_GL_MODE_DEPTH));
			if (glconfig == NULL) {
				g_print ("*** No appropriate OpenGL-capable visual found.\n");
				exit (1);
			}
		}

		return glconfig;
	}
public:
	void init (int argc, char *argv[]) {
		gtk_init (&argc, &argv);
		gtk_gl_init (&argc, &argv);
		glconfig = configureGL ();
		window = createWindow (glconfig);
	}
	void run () {
		gtk_widget_show (window);
		gtk_main ();
	}
};

int
main (int argc, char *argv[])
{
	MainWindow mw;

	mw.init (argc, argv);
	mw.run ();
	return 0;
}
