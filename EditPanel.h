#ifndef EDITPANEL_H
#define	EDITPANEL_H

#include <gtk/gtk.h>
#include "GlobalVar.h"
class EditPanel
{
	static void deleteBranchClicked(GtkWidget *w, gpointer data)
	{
		if(model)
		{
			model->removeMarkedBranch();
			DrawMethods::render();
		}
	}
	static void smoothBranchClicked(GtkWidget *w, gpointer data)
	{
		if(model)
		{
			model->smoothMarkedBranch();
			DrawMethods::render();
		}
	}
	
	static void reduceBranchResolutionClicked(GtkWidget *w, gpointer data)
	{
		if(model)
		{
			model->decMarkedBranchResolution();
			DrawMethods::render();
		}
	}
	
public:
	EditPanel()
	{
		
	}
	
	GtkWidget* createPanel()
	{
		GtkWidget *editWidget = gtk_frame_new("Editor");

		GtkWidget *vbox = gtk_vbox_new(FALSE, 1);
		gtk_container_add (GTK_CONTAINER (editWidget), vbox);
		
		GtkWidget *tools = gtk_toolbar_new();
		gtk_box_pack_start(GTK_BOX(vbox),tools,FALSE,FALSE,1);
		
		
		GtkToolItem *item;
		GtkTooltips *tooltips=gtk_tooltips_new();
		

#define ADD_ITEM(icon,callbackname,hint)\
		item = gtk_tool_button_new_from_stock(icon);\
		gtk_toolbar_insert(GTK_TOOLBAR(tools), item, -1);\
		g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(callbackname), NULL);\
		gtk_tooltips_set_tip(tooltips,GTK_WIDGET(item),hint,NULL);

		
		ADD_ITEM(GTK_STOCK_REMOVE, deleteBranchClicked, "Delete selected branch. All selected branch's children will be removed");
		ADD_ITEM(GTK_STOCK_ABOUT, smoothBranchClicked, "Smooth selected branch");
		ADD_ITEM(GTK_STOCK_ABOUT, reduceBranchResolutionClicked, "Reduce branch resolution by half");
#undef ADD_ITEM
		
		gtk_widget_show_all(editWidget);
		return editWidget;
	}
	
	
};


#endif	/* EDITPANEL_H */

