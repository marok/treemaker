#ifndef EDITPANEL_H
#define	EDITPANEL_H

#include <gtk/gtk.h>
#include <vector>


using namespace std;

class EditPanel:public IPanel
{

	vector< pair< SelectionMode ,GtkWidget*> > widgets;


	void activateWidgets(SelectionMode active)
	{
		for(unsigned int i=0; i<widgets.size(); i++)
		{
			if(active <= widgets[i].first)
				gtk_widget_set_sensitive(widgets.at(i).second, TRUE);
			else
				gtk_widget_set_sensitive(widgets.at(i).second, FALSE);
		}
	}

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

	static void selectionModeChanged(GtkWidget *w, gpointer data)
	{
		EditPanel *panel = (EditPanel*) data;

		if(model)
		{
			if(strcmp(gtk_widget_get_name(w),"selection_all") == 0)
				model->selection->setMode(SELECTION_ALL);
			else if(strcmp(gtk_widget_get_name(w), "selection_point_end")==0)
				model->selection->setMode(SELECTION_POINT_END);
			else
				model->selection->setMode(SELECTION_POINT_POINT);

			panel->activateWidgets(model->selection->getMode());

			DrawMethods::render();
		}
	}

	static void applyForChildrenClicked(GtkWidget *w, gpointer data)
	{
		if(model)
		{
			model->selection->applyForChildren = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));
		}
	}

	static void addGravityClicked(GtkWidget *w, gpointer data)
	{
		if(model)
		{
			model->addGravity();
			DrawMethods::render();
		}
	}

	static void subGravityClicked(GtkWidget *w, gpointer data)
	{
		if(model)
		{
			model->subGravity();
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

		GtkWidget *modeVBox = gtk_vbox_new(FALSE, 1);
		gtk_container_add (GTK_CONTAINER (vbox), modeVBox);


		GtkWidget* button = gtk_radio_button_new_with_label(NULL, "All branch");
		gtk_box_pack_start(GTK_BOX(modeVBox),button,FALSE,FALSE,1);
		gtk_widget_set_name(button, "selection_all");
		g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(selectionModeChanged), this);

		GSList *radioList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));

		button = gtk_radio_button_new_with_label(radioList, "From point to end");
		gtk_box_pack_start(GTK_BOX(modeVBox),button,FALSE,FALSE,1);
		gtk_widget_set_name(button, "selection_point_end");
		g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(selectionModeChanged), this);

		radioList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));

		button = gtk_radio_button_new_with_label(radioList, "From point to point");
		gtk_box_pack_start(GTK_BOX(modeVBox),button,FALSE,FALSE,1);
		gtk_widget_set_name(button, "selection_point_point");
		g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(selectionModeChanged), this);

		GtkWidget *check;
		check = gtk_check_button_new_with_label("Applay for all children");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), false);
		g_signal_connect(check,"clicked",G_CALLBACK(applyForChildrenClicked),this);
		gtk_box_pack_start(GTK_BOX(vbox),check,FALSE,FALSE,0);


		GtkWidget *tools = gtk_toolbar_new();
		gtk_box_pack_start(GTK_BOX(vbox),tools,FALSE,FALSE,1);

		GtkToolItem *item;
		GtkTooltips *tooltips=gtk_tooltips_new();


#define ADD_ITEM(icon,callbackname,hint)\
		item = gtk_tool_button_new_from_stock(icon);\
		gtk_toolbar_insert(GTK_TOOLBAR(tools), item, -1);\
		g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(callbackname), NULL);\
		gtk_tooltips_set_tip(tooltips,GTK_WIDGET(item),hint,NULL);


		ADD_ITEM(GTK_STOCK_CUT, deleteBranchClicked, "Delete selected branch. All selected branch's children will be removed");
		widgets.push_back(pair< SelectionMode,GtkWidget* >(SELECTION_POINT_END,GTK_WIDGET(item)));
		ADD_ITEM(GTK_STOCK_ADD, smoothBranchClicked, "Smooth selected branch");
		widgets.push_back(pair< SelectionMode,GtkWidget* >(SELECTION_POINT_POINT,GTK_WIDGET(item)));
		ADD_ITEM(GTK_STOCK_REMOVE, reduceBranchResolutionClicked, "Reduce branch resolution by half");
		widgets.push_back(pair< SelectionMode,GtkWidget* >(SELECTION_POINT_POINT,GTK_WIDGET(item)));
		ADD_ITEM(GTK_STOCK_GOTO_BOTTOM, addGravityClicked, "Add gravity to the selected branch");
		widgets.push_back(pair< SelectionMode,GtkWidget* >(SELECTION_ALL,GTK_WIDGET(item)));
		ADD_ITEM(GTK_STOCK_GOTO_TOP, subGravityClicked, "Substract gravity from the selected branch");
		widgets.push_back(pair< SelectionMode,GtkWidget* >(SELECTION_ALL,GTK_WIDGET(item)));
#undef ADD_ITEM

		gtk_widget_show_all(editWidget);
		return editWidget;
	}

	void updatePanel()
	{

	}
};


#endif	/* EDITPANEL_H */

