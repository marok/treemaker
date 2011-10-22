#ifndef _METHOD_PARAMETERS_PANEL_H
#define	_METHOD_PARAMETERS_PANEL_H
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <vector>
#include "MethodParameters.h"
#include "ColonizationMethod.h"
#include "Model3d.h"

using namespace std;
class MethodParametersPanel {

	GtkWidget *vbox;
	GtkWidget *hbox;
	ColonizationMethod *cm;
	vector <pair<int,GtkWidget *> > methodWidgets;
	

#define UNROLL_CALLBACK(callbackname,param)\
	static void callbackname( GtkAdjustment *adj, gpointer data){\
             MethodParameters *params = (MethodParameters *)data;\
             params->param = adj->value;\
	}

	UNROLL_CALLBACK(seedChanged,seed);
	UNROLL_CALLBACK(diChanged,di);
	UNROLL_CALLBACK(dkChanged,dk);
	UNROLL_CALLBACK(DChanged,D);
	UNROLL_CALLBACK(pointsChanged,points);
	UNROLL_CALLBACK(crownRadiusChanged,crownRadius);
	UNROLL_CALLBACK(cdChanged,cd);
	UNROLL_CALLBACK(attractionChanged,attraction);

#undef UNROLL_CALLBACK

	static void showEnvelopeClicked(GtkWidget *widget, gpointer data) {
		MethodParameters *params = (MethodParameters *)data;
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
	static void algorithmSelected(GtkWidget *widget, gpointer data) {

		MethodParametersPanel *cpp=(MethodParametersPanel*)data;
		gint active=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
		cpp->hideWidgets(active);
	}
			
	void hideWidgets(gint active){
		    for(unsigned int i=0;i<methodWidgets.size();i++){
		  GtkWidget *w=methodWidgets[i].second;
		  if(methodWidgets[i].first!=active)
		  gtk_widget_hide(w);
		  else 
		    gtk_widget_show(w);
		}
	}


public:
	MethodParametersPanel(ColonizationMethod *cm)
	{
		this->cm = cm;
	}
	GtkWidget *createPanel()
	{
		GtkWidget *paramsWidget;

		paramsWidget=gtk_frame_new("Generation Method");
		GtkTooltips *tooltips=gtk_tooltips_new();


		GtkWidget *label,*scale;
		GtkObject *adj;

		vbox = gtk_vbox_new(FALSE,1);
		gtk_container_add (GTK_CONTAINER (paramsWidget), vbox);

		GtkWidget *combo = gtk_combo_box_new_text();
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo),"Colonization");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo),"Particle");
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo),0);
		g_signal_connect(G_OBJECT(combo),"changed",G_CALLBACK(this->algorithmSelected),this);
		gtk_box_pack_start(GTK_BOX(vbox),combo,FALSE,FALSE,0);
		gtk_widget_show(combo);

#define PACK_LABEL_AND_SLIDER(text,val,min,max,step,func,digits,hint,type)\
    hbox = gtk_hbox_new(FALSE,1);\
    label = gtk_label_new(text);\
    adj=gtk_adjustment_new(cm->params->val,min,max,step,1,0);\
    g_signal_connect(adj,"value_changed",G_CALLBACK(func),cm->params);\
    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
    if(type>=0){\
      methodWidgets.push_back(pair<int,GtkWidget*>(type,scale));\
      methodWidgets.push_back(pair<int,GtkWidget*>(type,label));\
    }\
    gtk_scale_set_digits (GTK_SCALE (scale), digits);\
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,1);\
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
    gtk_tooltips_set_tip(tooltips,scale,hint,NULL);\
    gtk_widget_show(label);\
    gtk_widget_show(hbox);\
    gtk_widget_show(scale);

		// Common Parameters
		PACK_LABEL_AND_SLIDER("Seed:",SEED_DEFAULT,0,100,1,seedChanged,0,"Random number generator seed",-1);
		PACK_LABEL_AND_SLIDER("D:",D_DEFAULT,0.1,1,0.1,DChanged,1,"Step size",-1);
		PACK_LABEL_AND_SLIDER("Points:",POINTS_DEFAULT,10,100,1,pointsChanged,0,"Number of attraction points in crown",-1);
		PACK_LABEL_AND_SLIDER("Crown:",CROWNRADIUS_DEFAULT,2,8,1,crownRadiusChanged,1,"Crown radius",-1);


		// Colonization Parameters	
		PACK_LABEL_AND_SLIDER("di:",DI_DEFAULT,1,10,0.5,diChanged,1,"Influence distance",0);
		PACK_LABEL_AND_SLIDER("dk:",DK_DEFAULT,0.1,1,0.1,dkChanged,1,"Kill distance",0);
	
		// Particle Parameters
		PACK_LABEL_AND_SLIDER("cd:",DI_DEFAULT,0.1,1,0.1,cdChanged,1,"Combine distance",1);
		PACK_LABEL_AND_SLIDER("attraction:",ATTRACTION_DEFAULT,0.1,1,0.1,attractionChanged,1,"Particles attraction",1);


	#undef PACK_LABEL_AND_SLIDER
	
		
		GtkWidget *check = gtk_check_button_new_with_label("Show envelope");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),cm->params->showEnvelope);
		g_signal_connect(check,"clicked",G_CALLBACK(this->showEnvelopeClicked),cm->params);
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

		hideWidgets(cm->params->ACTIVEMETHOD_DEFAULT);
		return paramsWidget;
	}
};


#endif