#ifndef _TOOLBAR_H
#define _TOOLBAR_H
#include <gtk/gtk.h>
#include "Exporter.h"
#include <time.h>
#include "Panels.h"
class Toolbar {
	Parameters *params;
	ColonizationMethod *cm;
	GtkWidget *mainWindow;
	Panels *panels;
	gboolean working;

	static gboolean inc_progress(gpointer data) {
		Toolbar *t=(Toolbar*)data;
		GtkWidget *progress_bar=(GtkWidget*)t->progress_bar;

		gtk_progress_bar_pulse((GTK_PROGRESS_BAR(progress_bar)));
		return t->working;
	}

	static void  newClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		t->params->setDefaults();
		t->panels->updatePanels();
	}
	static void  openClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		GtkWidget *dialog = gtk_file_chooser_dialog_new("Select file to load parameters",GTK_WINDOW(t->mainWindow),GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			ifstream file(filename);
			t->params->load(file);
			file.close();
		}
		gtk_widget_destroy(dialog);
		t->panels->updatePanels();
	}
	static void  saveClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		GtkWidget *dialog = gtk_file_chooser_dialog_new("Select file to save parameters",GTK_WINDOW(t->mainWindow),GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);
		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			gtk_widget_destroy(dialog);
			ofstream file(filename);
			t->params->save(file);
			file.close();
		}
	}

	static void  generateClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		t->working=1;
		g_timeout_add(100,inc_progress,t);
		t->cm->init();
		t->cm->setBar(t->progress_bar);

		t->cm->generate();
		if(model!=NULL)
			delete model;
		t->params->tp->setCirclePointsValue();
		model = new  Model3d(t->cm->getRoot(),t->params);
		model->generateModel();
		DrawMethods::render();
		t->working=0;
	}


	static void  refreshClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;

		if(!model)
			return;

		t->params->tp->setCirclePointsValue();
		model->refreshModel();

		DrawMethods::render();
	}
	static void  convertClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		g_print("convertClicked\n");
		char filename[]="models/tree0.obj";
		Exporter ex;
		ex.init(filename);
		ex.exportTrunk(model, t->params);
		ex.saveTrunk();
		ex.exportLeaves(model ? model->getRootBranch() : NULL, t->params, 0);
		ex.saveLeaves();
		ex.close();
	}

public:
	GtkWidget *progress_bar;

	Toolbar(GtkWidget *mainWindow,Parameters *params,ColonizationMethod *cm, Panels *panels) {
		this->params=params;
		this->cm=cm;
		this->mainWindow=mainWindow;
		this->panels = panels;
	}
	GtkWidget* createToolbar() {
		GtkWidget *toolbar;
		GtkToolItem *item;
		toolbar = gtk_toolbar_new();
		gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
		gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);
		GtkTooltips *tooltips=gtk_tooltips_new();


#define ADD_ITEM(icon,callbackname,hint)\
		item = gtk_tool_button_new_from_stock(icon);\
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);\
		g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(callbackname), this);\
		gtk_tooltips_set_tip(tooltips,GTK_WIDGET(item),hint,NULL);


#define ADD_SEPARATOR\
		{GtkToolItem *sep = gtk_separator_tool_item_new();\
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep, -1);}

		ADD_ITEM(GTK_STOCK_NEW,newClicked,"Restore default parameters");
		ADD_ITEM(GTK_STOCK_OPEN,openClicked,"Read parameters from file");
		ADD_ITEM(GTK_STOCK_SAVE,saveClicked,"Save current parameters to file");
		ADD_SEPARATOR
		ADD_ITEM(GTK_STOCK_EXECUTE,generateClicked,"Generates new model based on current parameters");
		ADD_ITEM(GTK_STOCK_REFRESH,refreshClicked,"Refreshes current model");
		ADD_SEPARATOR
		ADD_ITEM(GTK_STOCK_CONVERT,convertClicked,"Convert model to external format");

#undef ADD_ITEM
#undef ADD_SEPARATOR
		gtk_widget_show_all(toolbar);
		//Tworzymy progress_bar
		progress_bar =gtk_progress_bar_new ();
		gtk_progress_bar_set_pulse_step((GtkProgressBar*) progress_bar,0.05);


		return toolbar;
	}
};
#endif