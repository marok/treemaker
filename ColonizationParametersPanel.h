#ifndef _COLONIZATION_PARAMETERS_PANEL_H
#define	_COLONIZATION_PARAMETERS_PANEL_H
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "ColonizationParameters.h"
#include "ColonizationMethod.h"
#include "Model3d.h"

class ColonizationParametersPanel {

	GtkWidget *vbox;
	GtkWidget *hbox;
	ColonizationMethod *cm;

#define UNROLL_CALLBACK(callbackname,param)\
	static void callbackname( GtkAdjustment *adj, gpointer data){\
             ColonizationParameters *params = (ColonizationParameters *)data;\
             params->param = adj->value;\
	}

	UNROLL_CALLBACK(seedChanged,seed);
	UNROLL_CALLBACK(diChanged,di);
	UNROLL_CALLBACK(dkChanged,dk);
	UNROLL_CALLBACK(DChanged,D);
	UNROLL_CALLBACK(pointsChanged,points);
	UNROLL_CALLBACK(crownRadiusChanged,crownRadius);

#undef UNROLL_CALLBACK

	static void showPointsClicked(GtkWidget *widget, gpointer data) {
		ColonizationParameters *params = (ColonizationParameters *)data;
		params->showEnvelope=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	}
	static void generateClicked( GtkWidget *widget ,gpointer data )
	{
		ColonizationMethod *cm = (ColonizationMethod*) data;
		cm->init();
		cm->generate();

		Model3d *m = new  Model3d(cm->nodes[0]);
		m->generateModel();
	}


public:
	ColonizationParametersPanel(ColonizationMethod *cm)
	{
		this->cm = cm;
	}
	GtkWidget *createPanel()
	{
		GtkWidget *paramsWidget;

		paramsWidget=gtk_frame_new("Colonization Algorithm");
		GtkTooltips *tooltips=gtk_tooltips_new();

		
		GtkWidget *label,*scale;
		GtkObject *adj;

		vbox = gtk_vbox_new(FALSE,1);
		gtk_container_add (GTK_CONTAINER (paramsWidget), vbox);

#define PACK_LABEL_AND_SLIDER(text,val,min,max,step,func,digits,hint)\
    hbox = gtk_hbox_new(FALSE,1);\
    label = gtk_label_new(text);\
    adj=gtk_adjustment_new(cm->params->val,min,max,step,1,0);\
    g_signal_connect(adj,"value_changed",G_CALLBACK(func),cm->params);\
    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
    gtk_scale_set_digits (GTK_SCALE (scale), digits);\
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,1);\
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
    gtk_tooltips_set_tip(tooltips,scale,hint,NULL);\
    gtk_widget_show(label);\
    gtk_widget_show(hbox);\
    gtk_widget_show(scale);

		PACK_LABEL_AND_SLIDER("Seed:",SEED_DEFAULT,0,100,1,seedChanged,0,"Random number generator seed");
		PACK_LABEL_AND_SLIDER("di:",DI_DEFAULT,1,10,0.5,diChanged,1,"Influence distance");
		PACK_LABEL_AND_SLIDER("dk:",DK_DEFAULT,0.1,1,0.1,dkChanged,1,"Kill distance");
		PACK_LABEL_AND_SLIDER("D:",D_DEFAULT,0.1,1,0.1,DChanged,1,"Step size");
		PACK_LABEL_AND_SLIDER("Points:",POINTS_DEFAULT,10,100,1,pointsChanged,0,"Number of attraction points in crown");
		PACK_LABEL_AND_SLIDER("Crown:",CROWNRADIUS_DEFAULT,2,8,1,crownRadiusChanged,1,"Crown radius");

#undef PACK_LABEL_AND_SLIDER


		
		GtkWidget *check = gtk_check_button_new_with_label("Show envelope");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),cm->params->showEnvelope);
		g_signal_connect(check,"clicked",G_CALLBACK(this->showPointsClicked),cm->params);
		gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);
		gtk_tooltips_set_tip(tooltips,check,"Shows crown envelope",NULL);
		
		
		
		GtkWidget *button = gtk_button_new_with_label ("Generate");
		g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (this->generateClicked), cm);
		gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
		gtk_tooltips_set_tip(tooltips,button,"Generates new tree model",NULL);
		
		gtk_widget_show(paramsWidget);
		gtk_widget_show(button);
		gtk_widget_show(check);
		gtk_widget_show(vbox);

		return paramsWidget;
	}
};


#endif