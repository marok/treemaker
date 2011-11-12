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
		GtkWidget *button;
		vbox=gtk_vbox_new(FALSE,1);

		gtk_container_add (GTK_CONTAINER (trunkWidget), vbox);

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