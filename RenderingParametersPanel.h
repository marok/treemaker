#ifndef _RENDERINGPARAMETERSPANEL_H
#define _RENDERINGPARAMETERSPANEL_H
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "Parameters.h"
#include "RenderingParameters.h"
class RenderingParametersPanel
{
	GtkWidget *vbox;

	Parameters *params;
#define UNROLL_CALLBACK(paramname)\
	static void paramname##Clicked( GtkWidget *widget, gpointer data){\
             Parameters *params = (Parameters *)data;\
             params->rp->paramname = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));\
             DrawMethods::render();\
	}

	UNROLL_CALLBACK(showEnvelopes);
	UNROLL_CALLBACK(showLeaves);
	UNROLL_CALLBACK(showBark);
	UNROLL_CALLBACK(showGrass);

#undef UNROLL_CALLBACK
public:
	RenderingParametersPanel(Parameters *p) {
		params=p;
	}
	GtkWidget *createPanel()
	{
		GtkWidget *paramsWidget;

		paramsWidget=gtk_frame_new("Rendering");
		GtkTooltips *tooltips=gtk_tooltips_new();

		vbox = gtk_vbox_new(FALSE,1);
		gtk_container_add (GTK_CONTAINER (paramsWidget), vbox);

		GtkWidget *check;
#define PACK_CHECKBOX(paramname,label,hint)\
		check = gtk_check_button_new_with_label(label);\
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),params->rp->paramname);\
		g_signal_connect(check,"clicked",G_CALLBACK(this->paramname##Clicked),params);\
		gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);\
		gtk_tooltips_set_tip(tooltips,check,hint,NULL);\
		gtk_widget_show(check);

		PACK_CHECKBOX(showEnvelopes,"Show envelopes","Shows tree crown envelopes");
		PACK_CHECKBOX(showLeaves,"Show leaves","Shows leaves on tree model");
		PACK_CHECKBOX(showBark,"Show bark","Shows bark texture on the trunk");
		PACK_CHECKBOX(showGrass,"Show grass","Shows grass texture on the ground");


#undef PACK_CHECKBOX

		gtk_widget_show(paramsWidget);
		gtk_widget_show(vbox);

		return paramsWidget;
	}
};

#endif