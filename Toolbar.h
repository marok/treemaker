#ifndef _TOOLBAR_H
#define _TOOLBAR_H
#include <gtk/gtk.h>

class Toolbar {
	Parameters *params;
	ColonizationMethod *cm;


	static void  newClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		//t->params=new Parameters();
		t->params->init();
		//if(model!=NULL)
		//	delete model;
		//model = new  Model3d(t->cm->getRoot(),t->params->tp);
		//model->generateModel();
		//DrawMethods::render();
	}
	static void  openClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		ifstream file("save.txt");
		t->params->load(file);
		file.close();
		//if(model!=NULL)
		//	delete model;
		//model = new  Model3d(t->cm->getRoot(),t->params->tp);
		//model->generateModel();
		//DrawMethods::render();
	}
	static void  saveClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		ofstream file("save.txt");
		t->params->save(file);
		file.close();
	}
	static void  refreshClicked(GtkWidget *widget,gpointer data) {
		Toolbar *t=(Toolbar*)data;
		t->cm->init();
		t->cm->generate();
		if(model!=NULL)
			delete model;
		model = new  Model3d(t->cm->getRoot(),t->params->tp);
		model->generateModel();
		DrawMethods::render();
	}
	static void  convertClicked(GtkWidget *widget,gpointer data) {
		g_print("convertClicked\n");
	}

public:
	Toolbar(Parameters *params,ColonizationMethod *cm) {
		this->params=params;
		this->cm=cm;
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
		ADD_ITEM(GTK_STOCK_REFRESH,refreshClicked,"Generate new model based on current parameters");
		ADD_SEPARATOR
		ADD_ITEM(GTK_STOCK_CONVERT,convertClicked,"Convert model to external format");

#undef ADD_ITEM
#undef ADD_SEPARATOR
		gtk_widget_show_all(toolbar);
		return toolbar;
	}
};
#endif