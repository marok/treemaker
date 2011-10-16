#ifndef _COLONIZATION_PARAMETERS_PANEL_H
#define	_COLONIZATION_PARAMETERS_PANEL_H
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "ColonizationParameters.h"
#include "ColonizationMethod.h"
#include "Model3d.h"

class ColonizationParametersPanel{
private:  
	GtkWidget *vbox;
	GtkWidget *hbox;
        ColonizationParameters *params;
        ColonizationMethod *cm;
      
	static void seedChanged( GtkAdjustment *adj , gpointer data )
	{
                ColonizationParameters *params = (ColonizationParameters *)data;
		g_print("seedChanged %lf\n",adj->value);
                params->seed = adj->value;
	}
	static void diChanged( GtkAdjustment *adj , gpointer data )
	{
                ColonizationParameters *params = (ColonizationParameters *)data;
		g_print("diChanged %lf\n",adj->value);
                params->di = adj->value;
	}
	static void dkChanged( GtkAdjustment *adj , gpointer data )
	{
                ColonizationParameters *params = (ColonizationParameters *)data;
                g_print("dkChanged %lf\n",adj->value);
                params->dk = adj->value;
	}
        static void DChanged( GtkAdjustment *adj , gpointer data )
	{
                ColonizationParameters *params = (ColonizationParameters *)data;
		g_print("DChanged %lf\n",adj->value);
                params->D = adj->value;
	}
	static void generateClicked( GtkAdjustment *adj, gpointer data )
	{
		g_print("Quick Regeneration\n");
                
                ColonizationMethod *cm = (ColonizationMethod*) data;
                cm->init();
                cm->generate(60);
                
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

		GtkWidget *label,*scale;
		GtkObject *adj;

		vbox = gtk_vbox_new(FALSE,1);
		gtk_container_add (GTK_CONTAINER (paramsWidget), vbox);

#define PACK_LABEL_AND_SLIDER(text,val,min,max,step,func,digits)\
    hbox = gtk_hbox_new(FALSE,1);\
    label = gtk_label_new(text);\
    adj=gtk_adjustment_new(val,min,max,step,1,0);\
    g_signal_connect(adj,"value_changed",G_CALLBACK(func),cm->params);\
    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
    gtk_scale_set_digits (GTK_SCALE (scale), digits);\
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,1);\
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
    gtk_widget_show(label);\
    gtk_widget_show(hbox);\
    gtk_widget_show(scale);

		PACK_LABEL_AND_SLIDER("Seed:",43,0,100,1,seedChanged,0);
                PACK_LABEL_AND_SLIDER("di:  ",5,1,10,0.5,diChanged,1);
                PACK_LABEL_AND_SLIDER("dk:  ",0.1,0.1,1,0.1,dkChanged,1);
                PACK_LABEL_AND_SLIDER("D:   ",0.2,0.1,1,0.1,DChanged,1);

#undef PACK_LABEL_AND_SLIDER

		GtkWidget *button = gtk_button_new_with_label ("Generate");
		g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (this->generateClicked), cm);
		gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

		gtk_widget_show(paramsWidget);
		gtk_widget_show(button);
		gtk_widget_show(vbox);
                
              
                
                return paramsWidget;
        }
};


#endif