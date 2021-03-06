#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <gtk/gtkgl.h>
#include <gdk/gdkglglext.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cassert>
#include "MethodParameters.h"
#include "ColonizationMethod.h"
#include "ParticleMethod.h"
#include "Model3d.h"
#include "IPanel.h"
#include "MethodParametersPanel.h"
#include "TrunkParametersPanel.h"
#include "RenderingParametersPanel.h"
#include "LeavesParametersPanel.h"
#include "CrownParametersPanel.h"
#include "EditPanel.h"
#include "ExportParameters.h"
#include "ExportParametersPanel.h"
#include "Toolbar.h"

#include "Spline.h"

#include "Bmp.h"

#include "DrawMethods.h"
#include "Crown.h"
#include "Panels.h"

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

int grid = (MAXGRID / 2);

static int beginX, beginY;

static float sphi = 90.0;

static float stheta = 45.0;

static float sdepth = 5.0 / 4.0 * (MAXGRID / 2);

static float zNear = (MAXGRID / 2) / 100.0;

static float zFar = (MAXGRID / 2) * 3.0;

static float aspect = 5.0 / 4.0;

static float lightPosition[4] = { 100.0, -100.0, 100.0, 100.0 };


ColonizationMethod *cm = NULL;
ParticleMethod *pm = NULL;


Model3d *model = NULL;
Parameters *parameters = NULL;
Panels *panels;

enum TreemakerMode
{
	MODE_GENERATOR,
	MODE_EDITOR
};

TreemakerMode tmMode = MODE_GENERATOR;



class MainWindow
{
	GtkWidget *window;
	GdkGLConfig *glconfig;

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
	//glClearColor (1, 1, 1, 0.0);
		
		gdk_gl_glPolygonOffsetEXT (proc, 1.0, 1.0);
		//glEnable (GL_CULL_FACE);
		glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable (GL_COLOR_MATERIAL);
		glColorMaterial (GL_FRONT, GL_DIFFUSE);
		glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
		glEnable (GL_LIGHT0);
		glShadeModel (GL_SMOOTH);

		//glEnable (GL_LIGHTING);

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

		
		DrawMethods::drawWireframe();
	
		DrawMethods::drawTreeModel (parameters, tmMode == MODE_GENERATOR?TRUE:FALSE);
		panels->updatePanels();
		
		if(coordinates) {
			DrawMethods::drawCoordinates (GL_RENDER);

		}
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
			
			if(tmMode == MODE_EDITOR)
				selection(event->x, event->y);
			
			gdk_window_invalidate_rect (widget->window,
			                            &widget->allocation, FALSE);
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
			sdepth -= 1;
			redraw = TRUE;
		}
		if (event->direction == GDK_SCROLL_DOWN)
		{
			sdepth += 1;
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

	static void
	selection(int x, int y) {

		const int BUFFER_LENGTH = 64;
		GLuint select_buffer [BUFFER_LENGTH];
		glSelectBuffer(BUFFER_LENGTH, select_buffer);
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glMatrixMode(GL_PROJECTION);

		glPushMatrix();

		glLoadIdentity();

		gluPickMatrix(x, viewport [3] - y, 4, 4, viewport);

		gluPerspective (64.0, aspect, zNear, zFar);

		glRenderMode(GL_SELECT);

		DrawMethods::drawTreeModel(parameters, FALSE);


		GLint hits = glRenderMode(GL_RENDER);

		glMatrixMode(GL_PROJECTION);

		glPopMatrix();

		if (hits == 0)
		{
//			printf("Nic nie trafiono!\n");
			if(model)
			{
				model->selection->unmarkBranch();
			}
		}
		else
		{
			
//			printf("Trafiono w %d obiekt/obiektów!\n",hits);
//			for(int i=0; i<4*hits; i++)
//				printf("%d ",select_buffer[i]);
//			printf("\n");
			
			
			if(model)
			{
				model->selection->markBranch(select_buffer[3]);
			
				//mamy dwie nazwy na stosie (gałąź jest zaznaczona i trafiliśmy na punkt)
				if(select_buffer[0]==2)
				{
					model->selection->markNode(select_buffer[4]);
				}
			}
		}
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
		gtk_widget_show(hbox);
		gtk_widget_show(vbox);

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

		
		/*** Panels initialization ***/
		panels = new Panels(
			new MethodParametersPanel(cm,pm,parameters->tp),
			new CrownParametersPanel(window,parameters),
			new RenderingParametersPanel(parameters),
			new TrunkParametersPanel(window,parameters),
			new LeavesParametersPanel(window,parameters),
			new EditPanel(),
			new ExportParametersPanel(window,parameters)
			);
		
		
		// Toolbar
		Toolbar *tbar=new Toolbar(window,parameters,cm, panels);
		GtkWidget *toolsPanel = tbar->createToolbar();

		gtk_box_pack_start(GTK_BOX(vbox), toolsPanel, FALSE, FALSE, 5);


		/*
		 * Popup menu.
		 */

		menu = create_popup_menu (drawing_area);

		g_signal_connect_swapped (G_OBJECT (drawing_area),
		                          "button_press_event",
		                          G_CALLBACK (button_press_event_popup_menu),
		                          menu);

		
		gtk_box_pack_start(GTK_BOX(hbox),vbox,FALSE,FALSE,1);

		GtkWidget *notebook=gtk_notebook_new();
		g_signal_connect (G_OBJECT (notebook), "switch-page",
		                  G_CALLBACK (notebookSwitchPage), NULL);
		
		GtkWidget *label;
		
		label=gtk_label_new("Algorithms");
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),panels->getMPP()->createPanel(),label);

		
		label=gtk_label_new("C");
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),panels->getCPP()->createPanel(),label);
		
		
		label=gtk_label_new("R");
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),panels->getRPP()->createPanel(),label);


		label=gtk_label_new("T");
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),panels->getTPP()->createPanel(),label);

		
		label=gtk_label_new("L");
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),panels->getLPP()->createPanel(),label);

		
		label = gtk_label_new("E");
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),panels->getEP()->createPanel(),label);
		
		label = gtk_label_new("X");
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook),panels->getXP()->createPanel(),label);
		
		
		gtk_widget_show(notebook);
		gtk_box_pack_start(GTK_BOX(vbox),notebook,FALSE,FALSE,1);
		
		
		//gtk_progress_bar_update((GtkProgressBar*)progress_bar,1);
		//gtk_progress_set_show_text ((GtkProgressBar*)progress_bar,gint 1);
		gtk_widget_show(tbar->progress_bar);
		gtk_box_pack_start(GTK_BOX(vbox),tbar->progress_bar,FALSE,FALSE,1);


		windowWidget=window;
		return window;

	}
	
	static void
	notebookSwitchPage(GtkWidget *w, gpointer page, guint pageId, gpointer userData)
	{
		bool draw = FALSE;
		
		if((tmMode == MODE_GENERATOR && pageId == 5) ||
			(tmMode == MODE_EDITOR && pageId !=5))
			draw = TRUE;
		
		if(pageId == 5)
		{
			tmMode = MODE_EDITOR;
			
		}
		else
			tmMode = MODE_GENERATOR;
		
		if(draw)
			DrawMethods::render();
		
	}

	static gboolean
	timeout (GtkWidget *widget)
	{
		//g_print("timeout");
		gtk_widget_queue_draw(widget);
		/* Invalidate the whole window. */
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

		/* Update synchronously (fast). */
		gdk_window_process_updates (widget->window, FALSE);

		return TRUE;
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
		parameters=new Parameters();
		cm = new ColonizationMethod(parameters);
		pm = new ParticleMethod(parameters);
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
	glutInit(&argc, argv);
	MainWindow mw;
	mw.init (argc, argv);
	mw.run ();
	return 0;
}
