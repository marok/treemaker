#ifndef _RENDERINGPARAMETERSPANEL_H
#define _RENDERINGPARAMETERSPANEL_H
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "Parameters.h"
#include "RenderingParameters.h"
class RenderingParametersPanel: public IPanel
{
	GtkWidget *vbox;
	GtkWidget *checkShowEnv, *checkShowLeaves, *checkShowBark, *checkShowGrass;

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

		
#define PACK_CHECKBOX(var_name, paramname,label,hint)\
		var_name = gtk_check_button_new_with_label(label);\
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(var_name),params->rp->paramname);\
		g_signal_connect(var_name,"clicked",G_CALLBACK(this->paramname##Clicked),params);\
		gtk_box_pack_start(GTK_BOX(vbox),var_name,FALSE,FALSE,0);\
		gtk_tooltips_set_tip(tooltips,var_name,hint,NULL);\
		gtk_widget_show(var_name);

		PACK_CHECKBOX(checkShowEnv,showEnvelopes,"Show envelopes","Shows tree crown envelopes");
		PACK_CHECKBOX(checkShowLeaves,showLeaves,"Show leaves","Shows leaves on tree model");
		PACK_CHECKBOX(checkShowBark,showBark,"Show bark","Shows bark texture on the trunk");
		PACK_CHECKBOX(checkShowGrass,showGrass,"Show grass","Shows grass texture on the ground");
#undef PACK_CHECKBOX

		gtk_widget_show(paramsWidget);
		gtk_widget_show(vbox);

		return paramsWidget;
	}
	
	void updatePanel()
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowEnv), params->rp->showEnvelopes);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowLeaves), params->rp->showLeaves);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowBark), params->rp->showBark);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowGrass), params->rp->showGrass);
	}
};

#endif