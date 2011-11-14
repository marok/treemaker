
#ifndef CROWNPARAMETERSPANEL_H
#define	CROWNPARAMETERSPANEL_H

#include <gtk/gtk.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class CrownParametersPanel {
	GtkWidget *xEntry, *yEntry, *zEntry;
	GtkWidget *hEntry, *rEntry;
	GtkWidget *crownTypeCombo;
	GtkWidget *vbox;
	
	GtkWidget *xyzWidget;
	GtkWidget *cylinderWidgets;
	GtkWidget *editButton;
	GtkWidget *deleteButton;
	
	GtkWidget *mainWindow;
	Parameters *params;
	
	GtkListStore *store;
	GtkWidget *list;
	
	Subcrown *selectedSubcrown;
	
	//std::vector< std::pair<enum CrownShape, GtkWidget*> > crownWidgets;
	
	static void deleteCrownClicked( GtkWidget *widget ,gpointer data )
	{
		GtkTreeIter iter;
		GtkTreeModel *model;
		int id;
		
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(panel->list));
		
		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
			gtk_tree_model_get (model, &iter, 0, &id, -1);
			panel->selectedSubcrown = panel->params->crown->subcrowns.at(id);
		}
		
		vector< Subcrown* >::iterator i = panel->params->crown->subcrowns.begin();
		i += id;
		panel->params->crown->subcrowns.erase(i);
		
		gtk_list_store_clear(panel->store);
		
		for(unsigned int i=0; i<panel->params->crown->subcrowns.size(); i++)
		{
			storeAppandSubcrown(panel->params->crown->subcrowns.at(i),i,panel->store);
		}
		
	}
	
	static void editCrownClicked( GtkWidget *widget ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		if(panel->selectedSubcrown)
		{
			float x = panel->selectedSubcrown->x = atof(gtk_entry_get_text(GTK_ENTRY(panel->xEntry)));
			float y = panel->selectedSubcrown->y = atof(gtk_entry_get_text(GTK_ENTRY(panel->yEntry)));
			float z = panel->selectedSubcrown->z = atof(gtk_entry_get_text(GTK_ENTRY(panel->zEntry)));
			
			GtkTreeIter iter;
			GtkTreeModel *model;
			GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(panel->list));
			if (gtk_tree_selection_get_selected(selection, &model, &iter))
			{
				gtk_list_store_set(panel->store, &iter, 2, x, 3, y, 4, z, -1);
			}
			
			if(panel->selectedSubcrown->shape == CYLINDER)
			{
				
				CylinderCrown *cyl = (CylinderCrown*)panel->selectedSubcrown;
				cyl->r = atof(gtk_entry_get_text(GTK_ENTRY(panel->rEntry)));
				cyl->h = atof(gtk_entry_get_text(GTK_ENTRY(panel->hEntry)));
			}
		}
		
		DrawMethods::render();
	}
	
	static void addCrownClicked( GtkWidget *widget ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		
		
		Subcrown *sub;
		
		gint active=gtk_combo_box_get_active(GTK_COMBO_BOX(panel->crownTypeCombo));
		if(active == 0)
		{
			sub = new SplineCrown(0,0,0, panel->params->methodParams);
		} else if(active == 1)
		{
			sub = new CylinderCrown(
				0,0,0,
				1,1);
		}
		
		panel->params->crown->subcrowns.push_back(sub);
		storeAppandSubcrown(sub,
			panel->params->crown->subcrowns.size()-1,
			panel->store);
		DrawMethods::render();
	}
	
	static void list_selection_cb(GtkTreeSelection *selection, gpointer data)
	{
		GtkTreeIter iter;
		GtkTreeModel *model;
		int id;
		
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		
		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
			gtk_tree_model_get (model, &iter, 0, &id, -1);
			panel->selectedSubcrown = panel->params->crown->subcrowns.at(id);
		}
		
		if(panel->selectedSubcrown)
		{
			char buff[30];
			sprintf(buff,"%.2f",panel->selectedSubcrown->x);
			gtk_entry_set_text(GTK_ENTRY(panel->xEntry), buff);
			sprintf(buff,"%.2f",panel->selectedSubcrown->y);
			gtk_entry_set_text(GTK_ENTRY(panel->yEntry), buff);
			sprintf(buff,"%.2f",panel->selectedSubcrown->z);
			gtk_entry_set_text(GTK_ENTRY(panel->zEntry), buff);
			
			gtk_widget_show(panel->xyzWidget);
			gtk_widget_show(panel->editButton);
			gtk_widget_show(panel->deleteButton);
			
			if(panel->selectedSubcrown->shape == CYLINDER)
			{
				gtk_widget_show(panel->cylinderWidgets);
				CylinderCrown *cyl = (CylinderCrown*)panel->selectedSubcrown;
				sprintf(buff,"%.2f",cyl->h);
				gtk_entry_set_text(GTK_ENTRY(panel->hEntry), buff);
				sprintf(buff,"%.2f",cyl->r);
				gtk_entry_set_text(GTK_ENTRY(panel->rEntry), buff);
			} else
			{
				gtk_widget_hide(panel->cylinderWidgets);
			}
		}
	}
	
	static void storeAppandSubcrown(Subcrown *sub, gint id, GtkListStore *store)
	{
		GtkTreeIter iter;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
					0, id,
					1, sub->shape == SPLINE?"Spline":"Cylinder",
					2, sub->x,
					3, sub->y,
					4, sub->z,
					-1);
	}
	
public:
	CrownParametersPanel(GtkWidget *window, Parameters *params) {
		this->mainWindow = window;
		this->params = params;
		this->selectedSubcrown = NULL;
	}

	GtkWidget* createPanel() {
		GtkWidget *crownWidget = gtk_frame_new("Crown Parameters");
		
		GtkWidget *hbox;

		store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_FLOAT, G_TYPE_FLOAT);

		
		for (unsigned int i = 0; i < params->crown->subcrowns.size(); i++) {
			if (params->crown->subcrowns.at(i)->shape == SPLINE) {
				SplineCrown *splineCrown = (SplineCrown*) params->crown->subcrowns.at(i);
				storeAppandSubcrown(splineCrown, i, store);
			} else if (params->crown->subcrowns.at(i)->shape == CYLINDER) {
				CylinderCrown *cylinderCrown = (CylinderCrown*) params->crown->subcrowns.at(i);
				storeAppandSubcrown(cylinderCrown, i, store);
			}
		}
		list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

		g_object_unref(G_OBJECT(store));


		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		GtkTreeViewColumn *column;

#define ADD_COLUMN(header, col_num)\
	column = gtk_tree_view_column_new_with_attributes (header, renderer, "text", col_num, NULL);\
	gtk_tree_view_append_column( GTK_TREE_VIEW(list), column);
		ADD_COLUMN("ID", 0);
		ADD_COLUMN("Shape", 1);
		ADD_COLUMN("x", 2);
		ADD_COLUMN("y", 3);
		ADD_COLUMN("z", 4);

#undef ADD_COLUMN

		vbox = gtk_vbox_new(FALSE, 1);
	
		gtk_box_pack_start(GTK_BOX(vbox), list, FALSE, FALSE, 0);
		gtk_widget_show(list);
		
		/*list selection*/
		GtkTreeSelection *select;

		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (list));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (list_selection_cb), this);
		
		
		
		
		xyzWidget = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(vbox), xyzWidget, FALSE, FALSE, 1);
		
		
		GtkWidget *xLabel = gtk_label_new("X");
		xEntry = gtk_entry_new();
		gtk_widget_set_size_request (xEntry,75,-1);
		GtkWidget *yLabel = gtk_label_new("Y");
		yEntry = gtk_entry_new();
		gtk_widget_set_size_request (yEntry,75,-1);
		GtkWidget *zLabel = gtk_label_new("Z");
		zEntry = gtk_entry_new();
		gtk_widget_set_size_request (zEntry,75,-1);
		
		gtk_box_pack_start(GTK_BOX(xyzWidget), xLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(xyzWidget), xEntry, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(xyzWidget), yLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(xyzWidget), yEntry, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(xyzWidget), zLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(xyzWidget), zEntry, FALSE, FALSE, 2);
		gtk_widget_show_all(xyzWidget);
		gtk_widget_hide(xyzWidget);
		
		cylinderWidgets = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(vbox), cylinderWidgets, FALSE, FALSE, 1);
		
		
		GtkWidget *hLabel = gtk_label_new("H");
		hEntry = gtk_entry_new();
		gtk_widget_set_size_request (hEntry,75,-1);
		
		
		GtkWidget *rLabel = gtk_label_new("R");
		rEntry = gtk_entry_new();
		gtk_widget_set_size_request (rEntry,75,-1);
		
		
		gtk_box_pack_start(GTK_BOX(cylinderWidgets), hLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(cylinderWidgets), hEntry, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(cylinderWidgets), rLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(cylinderWidgets), rEntry, FALSE, FALSE, 2);
		
		gtk_widget_show_all(cylinderWidgets);
		gtk_widget_hide(cylinderWidgets);
		
		editButton = gtk_button_new_with_label("Change selected crown");
		gtk_widget_show(editButton);
		g_signal_connect(G_OBJECT(editButton), "clicked", G_CALLBACK(this->editCrownClicked), this);
		gtk_box_pack_start(GTK_BOX(vbox), editButton, FALSE, FALSE, 2);
		
		deleteButton = gtk_button_new_with_label("Delete selected crown");
		gtk_widget_show(deleteButton);
		g_signal_connect(G_OBJECT(deleteButton), "clicked", G_CALLBACK(this->deleteCrownClicked), this);
		gtk_box_pack_start(GTK_BOX(vbox), deleteButton, FALSE, FALSE, 2);
		
		hbox = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);
		gtk_widget_show(hbox);
		
		crownTypeCombo = gtk_combo_box_new_text();
		gtk_widget_show(crownTypeCombo);
		gtk_combo_box_append_text(GTK_COMBO_BOX(crownTypeCombo),"Spline Crown");
		gtk_combo_box_append_text(GTK_COMBO_BOX(crownTypeCombo),"Cylinder Crown");
		gtk_combo_box_set_active(GTK_COMBO_BOX(crownTypeCombo),0);
		gtk_box_pack_start(GTK_BOX(hbox),crownTypeCombo,FALSE,FALSE,2);
		
		GtkWidget *button = gtk_button_new_with_label("Add crown");
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(this->addCrownClicked), this);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);
		
		
		gtk_container_add(GTK_CONTAINER(crownWidget), vbox);
		gtk_widget_show(vbox);
		gtk_widget_show(crownWidget);
		return crownWidget;
	}
};


#endif	/* CROWNPARAMETERSPANEL_H */
