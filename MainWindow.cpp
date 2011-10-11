#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <gtk/gtkgl.h>
#include <gdk/gdkglglext.h>

#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cassert>
#include "ColonizationMethod.h"
using namespace std;


#define DEFAULT_WIDTH  800
#define DEFAULT_HEIGHT 600
#define DEFAULT_TITLE  "CoolWave"

#define TIMEOUT_INTERVAL 10

#define MAXGRID 63
#define SQRTOFTWOINV (1.0 / 1.414213562)


/**************************************************************************
 * Global variable declarations.
 **************************************************************************/

static gboolean animate = TRUE;

static gboolean coordinates = FALSE;

static int grid = (MAXGRID / 2);

static int beginX, beginY;

static float force[MAXGRID][MAXGRID];

static float veloc[MAXGRID][MAXGRID];

static float posit[MAXGRID][MAXGRID];

static float sphi = 90.0;

static float stheta = 45.0;

static float sdepth = 5.0 / 4.0 * (MAXGRID / 2);

static float zNear = (MAXGRID / 2) / 100.0;

static float zFar = (MAXGRID / 2) * 3.0;

static float aspect = 5.0 / 4.0;

static float lightPosition[4] = { 0.0, 0.0, 1.0, 1.0 };

static guint timeout_id = 0;




class MainWindow
{
    GtkWidget *window;
    GdkGLConfig *glconfig;

    static void getforce ()
    {
    } static void getvelocity ()
    {
    }
    static void getposition ()
    {
    }
    static void drawCoordinates ()
    {
	glTranslatef (grid / 2 + 0.5, grid / 2 + 0.5, 0);
	glRotatef (90, 1, 0, 0);
	glRotatef (270, 0, 1, 0);
	if (coordinates) {
	    glBegin (GL_LINES);

	    //os y
	    glColor3f (1, 0, 0);
	    glVertex3f (0, 0, 0);
	    glVertex3f (0, 10, 0);
	    //os x
	    glColor3f (0, 0, 1);
	    glVertex3f (0, 0, 0);
	    glVertex3f (10, 0, 0);
	    glEnd ();

	}
    }

    static void drawTreeModel ()
    {

	static int generated = 0;
	    static ColonizationMethod cm;

	if (generated == 0) {
	  cm.init();
	  cm.generate (20);
	    generated = 1;
	}

	int i;

	glColor3f (1, 0, 0);
	glBegin (GL_POINTS);
	for(int i=0;i<cm.aPoints.size();i++)
	{Point3d *p=&cm.aPoints[i];
	  glVertex3f(p->x,p->y,p->z);
	}

	glEnd ();


	glColor3f (0, 0, 0);
	glPointSize (2);
	glBegin (GL_POINTS);
for(int i=0;i<cm.nodes.size();i++)
	{Point3d *p=&cm.nodes[i].point;
	  glVertex3f(p->x,p->y,p->z);
	}

	glEnd ();

    }
    static void drawWireframe ()
    {
	int i = 0, j = 0;

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
    static void resetWireframe ()
    {
    }

    static void realize (GtkWidget * widget, gpointer data)
    {
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
	glEnable (GL_CULL_FACE);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable (GL_COLOR_MATERIAL);
	glColorMaterial (GL_FRONT, GL_DIFFUSE);
	glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable (GL_LIGHT0);
	glShadeModel (GL_FLAT);
	glDisable (GL_LIGHTING);

	resetWireframe ();

	gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/

	return;
    }



    static gboolean
	configure_event (GtkWidget * widget,
			 GdkEventConfigure * event, gpointer data)
    {
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
		      GdkEventExpose * event, gpointer data)
    {
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

    static gboolean timeout (GtkWidget * widget)
    {
	MainWindow::getforce ();
	MainWindow::getvelocity ();
	MainWindow::getposition ();

	/* Invalidate the whole window. */
	gdk_window_invalidate_rect (widget->window, &widget->allocation,
				    FALSE);

	/* Update synchronously (fast). */
	gdk_window_process_updates (widget->window, FALSE);

	return TRUE;
    }
    static gboolean
	motion_notify_event (GtkWidget * widget,
			     GdkEventMotion * event, gpointer data)
    {
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

	if (redraw && !animate)
	    gdk_window_invalidate_rect (widget->window,
					&widget->allocation, FALSE);

	return TRUE;
    }
    static gboolean
	button_press_event (GtkWidget * widget,
			    GdkEventButton * event, gpointer data)
    {
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
	scroll_event (GtkWidget * widget,
		      GdkEventScroll * event, gpointer data)
    {
	if (event->direction == GDK_SCROLL_UP)
	    sdepth -= 2;
	if (event->direction == GDK_SCROLL_DOWN)
	    sdepth += 2;
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
	    return TRUE;
	}

	return FALSE;
    }

    static gboolean
	key_press_event (GtkWidget * widget,
			 GdkEventKey * event, gpointer data)
    {
	switch (event->keyval) {
	case GDK_r:
	    init_wireframe (widget);
	    break;

	case GDK_a:
	    toggle_animation (widget);
	    break;

	case GDK_w:
	    if (!animate)
		timeout (widget);
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

	if (!animate)
	    gdk_window_invalidate_rect (widget->window,
					&widget->allocation, FALSE);

	return TRUE;
    }

    static void unrealize (GtkWidget * widget, gpointer data)
    {
  /*** Fill in the details here ***/

    }


    static void timeout_add (GtkWidget * widget)
    {
	if (timeout_id == 0) {
	    timeout_id = g_timeout_add (TIMEOUT_INTERVAL,
					(GSourceFunc) timeout, widget);
	}
    }
    static void timeout_remove (GtkWidget * widget)
    {
	if (timeout_id != 0) {
	    g_source_remove (timeout_id);
	    timeout_id = 0;
	}
    }
    static gboolean map_event (GtkWidget * widget, GdkEvent * event,
			       gpointer data)
    {
	if (animate)
	    timeout_add (widget);

	return TRUE;
    }
    static gboolean
	unmap_event (GtkWidget * widget, GdkEvent * event, gpointer data)
    {
	timeout_remove (widget);

	return TRUE;
    }
    static gboolean
	visibility_notify_event (GtkWidget * widget,
				 GdkEventVisibility * event, gpointer data)
    {
	if (animate) {
	    if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
		timeout_remove (widget);
	    else
		timeout_add (widget);
	}

	return TRUE;
    }

    static void toggle_animation (GtkWidget * widget)
    {
	animate = !animate;

	if (animate) {
	    MainWindow::timeout_add (widget);
	}
	else {
	    MainWindow::timeout_remove (widget);
	    gdk_window_invalidate_rect (widget->window,
					&widget->allocation, FALSE);
	}
    }
    static void init_wireframe (GtkWidget * widget)
    {
	MainWindow::resetWireframe ();
	gdk_window_invalidate_rect (widget->window, &widget->allocation,
				    FALSE);
    }
    static void show_coordinates (GtkWidget * widget)
    {
	coordinates = !coordinates;
    }

    GtkWidget *create_popup_menu (GtkWidget * drawing_area)
    {
	GtkWidget *menu;

	GtkWidget *menu_item;

	menu = gtk_menu_new ();

	/* Toggle animation */
	menu_item = gtk_menu_item_new_with_label ("Toggle Animation");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	g_signal_connect_swapped (G_OBJECT (menu_item), "activate",
				  G_CALLBACK (toggle_animation),
				  drawing_area);
	gtk_widget_show (menu_item);

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
    GtkWidget *createWindow (GdkGLConfig * glconfig)
    {
	GtkWidget *window;

	GtkWidget *vbox;

	GtkWidget *drawing_area;

	GtkWidget *menu;

	GtkWidget *button;

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

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), vbox);
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

	/* For timeout function. */
	g_signal_connect (G_OBJECT (drawing_area), "map_event",
			  G_CALLBACK (map_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "unmap_event",
			  G_CALLBACK (unmap_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area),
			  "visibility_notify_event",
			  G_CALLBACK (visibility_notify_event), NULL);

	gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

	gtk_widget_show (drawing_area);

	/*
	 * Popup menu.
	 */

	menu = create_popup_menu (drawing_area);

	g_signal_connect_swapped (G_OBJECT (drawing_area),
				  "button_press_event",
				  G_CALLBACK (button_press_event_popup_menu),
				  menu);

	/*
	 * Simple quit button.
	 */

	button = gtk_button_new_with_label ("Quit");

	g_signal_connect (G_OBJECT (button), "clicked",
			  G_CALLBACK (gtk_main_quit), NULL);

	gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

	gtk_widget_show (button);

	return window;

    }
    GdkGLConfig *configureGL ()
    {
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
    void init (int argc, char *argv[])
    {
	gtk_init (&argc, &argv);
	gtk_gl_init (&argc, &argv);
	glconfig = configureGL ();
	window = createWindow (glconfig);
    }
    void run ()
    {
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
