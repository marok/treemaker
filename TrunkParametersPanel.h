#ifndef _TRUNK_PARAMETERS_PANEL_H
#define _TRUNK_PARAMETERS_PANEL_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "TrunkParameters.h"
#include "Bmp.h"
class TrunkParametersPanel {
	TrunkParameters *tp;
	GtkWidget *vbox;
	GtkWidget *mainWindow;
	GtkWidget *image;

#define UNROLL_CALLBACK(callbackname,param)\
	static void callbackname( GtkAdjustment *adj, gpointer data){\
             TrunkParameters *params = (TrunkParameters *)data;\
             params->param = adj->value;\
		}
	UNROLL_CALLBACK(radiusFactorChanged,radiusFactor);
	UNROLL_CALLBACK(circlePointsChanged,circlePoints);
	UNROLL_CALLBACK(aValueChanged,aValue);
	UNROLL_CALLBACK(mValueChanged,mValue);

#undef UNROLL_CALLBACK
	static void chooseBarkClicked(GtkWidget *w,gpointer data) {
		GtkWidget *dialog;
		TrunkParametersPanel *tpp=(TrunkParametersPanel*)data;
		dialog = gtk_file_chooser_dialog_new("Select bmp file with bark texture",GTK_WINDOW(tpp->mainWindow),GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

			char *tmpname=tpp->generateIcon(filename);
			gtk_image_set_from_file(GTK_IMAGE(tpp->image),tmpname);
			strcpy(tpp->tp->barkPath,filename);
		}
		gtk_widget_destroy(dialog);
	}
	char* generateIcon(char *filename) {
		Bmp bmp;
		bmp.load(filename);
		Bmp *icon=bmp.getIcon();
		static char tmpname[]="/tmp/treemaker_tmpfile";
		icon->save(tmpname);
		return tmpname;
	}

public:
	TrunkParametersPanel(GtkWidget *window,TrunkParameters *tp) {
		this->tp=tp;
		this->mainWindow=window;
	}

	GtkWidget* createPanel() {
		GtkWidget *trunkWidget=gtk_frame_new("Trunk Parameters");

		GtkTooltips *tooltips=gtk_tooltips_new();
		GtkWidget *button,*scale,*label,*hbox;
		GtkObject *adj;
		vbox=gtk_vbox_new(FALSE,1);

		gtk_container_add (GTK_CONTAINER (trunkWidget), vbox);

#define PACK_LABEL_AND_SLIDER(text,val,min,max,step,func,digits,hint)\
    hbox = gtk_hbox_new(FALSE,1);\
    label = gtk_label_new(text);\
    adj=gtk_adjustment_new(tp->val,min,max,step,1,0);\
    g_signal_connect(adj,"value_changed",G_CALLBACK(func),tp);\
    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
    gtk_scale_set_digits (GTK_SCALE (scale), digits);\
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,1);\
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
    gtk_tooltips_set_tip(tooltips,scale,hint,NULL);\
    gtk_widget_show(label);\
    gtk_widget_show(hbox);\
    gtk_widget_show(scale);

		//trunk parameters
		PACK_LABEL_AND_SLIDER("rFactor",RADIUSFACTOR_DEFAULT,1.0,2.0,0.1,radiusFactorChanged,1,"Radius factor");
		PACK_LABEL_AND_SLIDER("aValue",AVALUE_DEFAULT,0.00,0.01,0.0001,aValueChanged,3,"Branch diameter increase value");
		PACK_LABEL_AND_SLIDER("mValue",MVALUE_DEFAULT,1,1.1,0.01,mValueChanged,2,"Branch diameter multiplication value");
		PACK_LABEL_AND_SLIDER("circle",CIRCLEPOINTS_DEFAULT,3,20,1,circlePointsChanged,0,"Points aproximating branch circle");

		//auto refresh checkbox
		//bark texture select
		button=gtk_button_new_with_label("Choose bark texture");
		g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (this->chooseBarkClicked), this);
		label=gtk_label_new("Choose bark texture:");
		gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,1);
		gtk_tooltips_set_tip(tooltips,button,"Opens file chooser dialog",NULL);
		gtk_widget_show(button);


		image=gtk_image_new_from_file(generateIcon(tp->barkPath));
		gtk_box_pack_start(GTK_BOX(vbox),image,FALSE,FALSE,1);
		gtk_widget_show(image);




#undef PACK_LABEL_AND_SLIDER

		gtk_widget_show(trunkWidget);
		gtk_widget_show(vbox);

		return trunkWidget;
	}
};
#endif