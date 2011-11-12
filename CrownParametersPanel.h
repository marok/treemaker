
#ifndef CROWNPARAMETERSPANEL_H
#define	CROWNPARAMETERSPANEL_H

#include <gtk/gtk.h>
#include <stdlib.h>

class CrownParametersPanel {
	GtkWidget *xEntry, *yEntry, *zEntry;
	GtkWidget *hEntry, *rEntry;
	GtkWidget *crownTypeCombo;
	GtkWidget *vbox;
	
	GtkWidget *mainWindow;
	Parameters *params;
	
	GtkListStore *store;
	
	static void addCrownClicked( GtkWidget *widget ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		
		float x = atof(gtk_entry_get_text(GTK_ENTRY(panel->xEntry)));
		float y = atof(gtk_entry_get_text(GTK_ENTRY(panel->yEntry)));
		float z = atof(gtk_entry_get_text(GTK_ENTRY(panel->zEntry)));
		
		Subcrown *sub;
		
		gint active=gtk_combo_box_get_active(GTK_COMBO_BOX(panel->crownTypeCombo));
		if(active == 0)
		{
			sub = new SplineCrown(x,y,z, panel->params->methodParams);
			panel->params->crown->subcrowns.push_back(sub);
		} else if(active == 1)
		{
			sub = new CylinderCrown(
				x,y,z,
				atof(gtk_entry_get_text(GTK_ENTRY(panel->rEntry))),
				atof(gtk_entry_get_text(GTK_ENTRY(panel->hEntry)))
			);
			panel->params->crown->subcrowns.push_back(sub);
		}
		
		storeAppandSubcrown(sub,
			panel->params->crown->subcrowns.size(),
			panel->store);
		
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
	
	
	
	static void crownTypeSelected( GtkWidget *widget ,gpointer data )
	{
		
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		gint active=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
		if(active==0)
		{
			gtk_widget_set_sensitive(panel->hEntry, FALSE);
			gtk_widget_set_sensitive(panel->rEntry, FALSE);
		} else if(active == 1)
		{
			gtk_widget_set_sensitive(panel->hEntry, TRUE);
			gtk_widget_set_sensitive(panel->rEntry, TRUE);
		}
	}
	
public:
	
	

	CrownParametersPanel(GtkWidget *window, Parameters *params) {
		this->mainWindow = window;
		this->params = params;
	}

	GtkWidget* createPanel() {
		GtkWidget *crownWidget = gtk_frame_new("Crown Parameters");

		store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_FLOAT, G_TYPE_FLOAT);

		
		for (unsigned int i = 0; i < params->crown->subcrowns.size(); i++) {
			if (params->crown->subcrowns.at(i)->shape == SPLINE) {
				SplineCrown *splineCrown = (SplineCrown*) params->crown->subcrowns.at(i);
				storeAppandSubcrown(splineCrown, params->crown->subcrowns.size(), store);
			} else if (params->crown->subcrowns.at(i)->shape == CYLINDER) {
				CylinderCrown *cylinderCrown = (CylinderCrown*) params->crown->subcrowns.at(i);
				storeAppandSubcrown(cylinderCrown, params->crown->subcrowns.size(), store);
			}
		}
		GtkWidget *list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

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
		
		
		crownTypeCombo = gtk_combo_box_new_text();
		gtk_combo_box_append_text(GTK_COMBO_BOX(crownTypeCombo),"Spline Crown");
		gtk_combo_box_append_text(GTK_COMBO_BOX(crownTypeCombo),"Cylinder Crown");
		gtk_combo_box_set_active(GTK_COMBO_BOX(crownTypeCombo),0);
		g_signal_connect(G_OBJECT(crownTypeCombo),"changed",G_CALLBACK(this->crownTypeSelected),this);
		gtk_box_pack_start(GTK_BOX(vbox),crownTypeCombo,FALSE,FALSE,2);
		
		GtkWidget *hbox = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);
		
		
		GtkWidget *xLabel = gtk_label_new("X");
		xEntry = gtk_entry_new();
		gtk_widget_set_size_request (xEntry,75,-1);
		GtkWidget *yLabel = gtk_label_new("Y");
		yEntry = gtk_entry_new();
		gtk_widget_set_size_request (yEntry,75,-1);
		GtkWidget *zLabel = gtk_label_new("Z");
		zEntry = gtk_entry_new();
		gtk_widget_set_size_request (zEntry,75,-1);
		
		gtk_box_pack_start(GTK_BOX(hbox), xLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), xEntry, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), yLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), yEntry, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), zLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), zEntry, FALSE, FALSE, 2);
		
		hbox = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);
		
		GtkWidget *hLabel = gtk_label_new("H");
		hEntry = gtk_entry_new();
		gtk_widget_set_size_request (hEntry,75,-1);
		gtk_widget_set_sensitive(hEntry,FALSE);
		
		
		GtkWidget *rLabel = gtk_label_new("R");
		rEntry = gtk_entry_new();
		gtk_widget_set_size_request (rEntry,75,-1);
		gtk_widget_set_sensitive(rEntry,FALSE);
		
		
		gtk_box_pack_start(GTK_BOX(hbox), hLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), hEntry, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), rLabel, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(hbox), rEntry, FALSE, FALSE, 2);
		
		GtkWidget *button = gtk_button_new_with_label("Add crown");
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(this->addCrownClicked), this);
		gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 2);
		
		gtk_container_add(GTK_CONTAINER(crownWidget), vbox);
		
		

		
		
		
		gtk_widget_show_all(vbox);
		gtk_widget_show(crownWidget);
		return crownWidget;
	}
};


#endif	/* CROWNPARAMETERSPANEL_H */

