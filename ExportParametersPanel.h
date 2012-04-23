#ifndef _EXPORTPARAMETERSPANEL_H
#define _EXPORTPARAMETERSPANEL_H
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <algorithm>
#include "Parameters.h"
#include "RenderingParameters.h"
#include "ExportParameters.h"
class ExportParametersPanel: public IPanel
{
	GtkWidget *vbox;
	GtkWidget *checkInvert;
	GtkWidget *mainWindow;
	GtkWidget *label;
	GtkWidget *labelTriangles;
	Parameters *params;
#define UNROLL_CALLBACK(paramname)\
	static void paramname##Clicked( GtkWidget *widget, gpointer data){\
             Parameters *params = (Parameters *)data;\
             params->xp->paramname = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));\
	}

	UNROLL_CALLBACK(invert);
#undef UNROLL_CALLBACK

	static void chooseExportFilenameClicked(GtkWidget *w,gpointer data) {
		ExportParametersPanel *xpp=(ExportParametersPanel*)data;
		GtkWidget *dialog = gtk_file_chooser_dialog_new("Select file to save OBJ",GTK_WINDOW(xpp->mainWindow),GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);
		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			gtk_widget_destroy(dialog);
			xpp->params->xp->filename=filename;
			xpp->updatePanel();
		}
	}

public:
	ExportParametersPanel(GtkWidget *window,Parameters *p) {
		this->params=p;
		this->mainWindow=window;

	}
	GtkWidget *createPanel()
	{
		GtkWidget *paramsWidget;

		paramsWidget=gtk_frame_new("Export");
		GtkTooltips *tooltips=gtk_tooltips_new();

		vbox = gtk_vbox_new(FALSE,1);
		gtk_container_add (GTK_CONTAINER (paramsWidget), vbox);


#define PACK_CHECKBOX(var_name, paramname,label,hint)\
		var_name = gtk_check_button_new_with_label(label);\
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(var_name),params->xp->paramname);\
		g_signal_connect(var_name,"clicked",G_CALLBACK(this->paramname##Clicked),params);\
		gtk_box_pack_start(GTK_BOX(vbox),var_name,FALSE,FALSE,0);\
		gtk_tooltips_set_tip(tooltips,var_name,hint,NULL);\
		gtk_widget_show(var_name);

		PACK_CHECKBOX(checkInvert,invert,"Invert Y","Inverts Y axis for Blender");
#undef PACK_CHECKBOX

		label = gtk_label_new(params->xp->filename.c_str());
		gtk_widget_show(label);
		gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

		GtkWidget *button=gtk_button_new_with_label("Change export filename");
		g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (this->chooseExportFilenameClicked), this);
		gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,1);
		gtk_tooltips_set_tip(tooltips,button,"Opens file chooser dialog",NULL);
		
		char tri[64];
		sprintf (tri, "triangles: %d",params->xp->triangles);
		labelTriangles = gtk_label_new(tri);
		gtk_widget_show(labelTriangles);
		gtk_box_pack_start(GTK_BOX(vbox),labelTriangles,FALSE,FALSE,0);
		
		
		gtk_widget_show(button);
		gtk_widget_show(paramsWidget);
		gtk_widget_show(vbox);

		return paramsWidget;
	}
	string cutText(string s) {
		if(s.size()<20) return s;
		string r("...");
		for(unsigned int i=s.size()-20; i<s.size(); i++) {
			r+=s[i];
		}
		return r;
	}
	void updatePanel()
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkInvert), params->xp->invert);
		gtk_label_set_text(GTK_LABEL(label),cutText(params->xp->filename).c_str());
		char tri[64];
		sprintf (tri, "triangles: %d",params->xp->triangles);
		gtk_label_set_text(GTK_LABEL(labelTriangles),tri);

	}
};

#endif