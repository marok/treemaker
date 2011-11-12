#ifndef _LEAVESPARAMETERSPANEL_H
#define _LEAVESPARAMETERSPANEL_H
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "LeavesParameters.h"
#include "Bmp.h"

class LeavesParametersPanel {
	Parameters *params;
	GtkWidget *vbox;
	GtkWidget *image;
	GtkWidget *mainWindow;
#define UNROLL_CALLBACK(param)\
	static void param##Changed( GtkAdjustment *adj, gpointer data){\
             Parameters *params = (Parameters *)data;\
             params->lp->param = adj->value;\
	}
	UNROLL_CALLBACK(leavesSize);
#undef UNROLL_CALLBACK


	static void chooseLeavesClicked(GtkWidget *w,gpointer data) {
		GtkWidget *dialog;
		LeavesParametersPanel *lpp=(LeavesParametersPanel*)data;
		dialog = gtk_file_chooser_dialog_new("Select bmp file with leaves texture",GTK_WINDOW(lpp->mainWindow),GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			char *tmpname=Bmp::generateIcon(filename);
			gtk_image_set_from_file(GTK_IMAGE(lpp->image),tmpname);
			lpp->params->lp->setLeavesPath(filename);
			lpp->params->lp->leavesTexInitialized=FALSE;
		}
		gtk_widget_destroy(dialog);
	}

public:
	LeavesParametersPanel(GtkWidget *window,Parameters *params) {
		this->params=params;
		this->mainWindow=window;
	}
	GtkWidget* createPanel() {
		GtkWidget *trunkWidget=gtk_frame_new("Leaves Parameters");

		GtkTooltips *tooltips=gtk_tooltips_new();
		GtkWidget *button,*hbox;
		vbox=gtk_vbox_new(FALSE,1);
		GtkWidget *label,*scale;
		GtkObject *adj;
		gtk_container_add (GTK_CONTAINER (trunkWidget), vbox);

#define PACK_LABEL_AND_SLIDER(text,val,min,max,step,func,digits,hint)\
    hbox = gtk_hbox_new(FALSE,1);\
    label = gtk_label_new(text);\
    adj=gtk_adjustment_new(params->lp->val,min,max,step,1,0);\
    g_signal_connect(adj,"value_changed",G_CALLBACK(func),params);\
    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
    gtk_scale_set_digits (GTK_SCALE (scale), digits);\
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,1);\
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
    gtk_tooltips_set_tip(tooltips,scale,hint,NULL);\
    gtk_widget_show(label);\
    gtk_widget_show(hbox);\
    gtk_widget_show(scale);

		// Common Parameters
		PACK_LABEL_AND_SLIDER("Leaves size:",LEAVESSIZE_DEFAULT,0.5,1.5,0.1,leavesSizeChanged,1,"Size of a leaves on a tree");

#undef PACK_LABEL_AND_SLIDER

		//leaves texture select
		button=gtk_button_new_with_label("Choose leaves texture");
		g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (this->chooseLeavesClicked), this);
		gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,1);
		gtk_tooltips_set_tip(tooltips,button,"Opens file chooser dialog",NULL);
		gtk_widget_show(button);

		image=gtk_image_new_from_file(Bmp::generateIcon(params->lp->leavesPath));
		gtk_box_pack_start(GTK_BOX(vbox),image,FALSE,FALSE,1);

		gtk_widget_show(image);
		gtk_widget_show(trunkWidget);
		gtk_widget_show(vbox);

		return trunkWidget;
	}

};


#endif