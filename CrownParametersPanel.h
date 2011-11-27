
#ifndef CROWNPARAMETERSPANEL_H
#define	CROWNPARAMETERSPANEL_H

#include <gtk/gtk.h>
#include <stdlib.h>
#include <vector>
#include "SplineCrownParametersPanel.h"
using namespace std;

class CrownParametersPanel {
	GtkObject *xAdj, *yAdj, *zAdj;
	GtkObject *hAdj, *rDownAdj, *rUpAdj;

	GtkWidget *crownTypeCombo;

	GtkWidget *editButton;
	GtkWidget *deleteButton;

	GtkWidget *mainWindow;
	Parameters *params;

	GtkListStore *crownStore;
	GtkWidget *crownList;

	Subcrown *selectedSubcrown;

	//kontrolki przyporządkowane odpowiedniej koronie, -1 znaczy, że kontrolka pasuje do wszystkich koron
	vector< pair<int, GtkWidget*> > crownWidgets;

	SplineCrownParametersPanel *scpp;

	//jeżeli active równa się -1, ukrywamy wszystkie kontrolki(usuwanie, edytowanie)
	void showWidgets(int active)
	{
		for(unsigned int i=0; i<crownWidgets.size(); i++)
		{
			if((crownWidgets.at(i).first == -1 || crownWidgets.at(i).first == active) && active != -1)
				gtk_widget_show(crownWidgets.at(i).second);
			else
				gtk_widget_hide(GTK_WIDGET(crownWidgets.at(i).second));
		}
	}

	void populateCrownList()
	{
		for(unsigned int i=0; i< params->crown->subcrowns.size(); i++)
		{
			Subcrown *sub = params->crown->subcrowns.at(i);
			storeAppendSubcrown(sub, i, crownStore);
		}
	}

	void storeAppendSubcrown(Subcrown *sub, gint id, GtkListStore *store)
	{
		char buffX[6], buffY[6], buffZ[6];
		sprintf(buffX, "%.1f", sub->x);
		sprintf(buffY, "%.1f", sub->y);
		sprintf(buffZ, "%.1f", sub->z);

		GtkTreeIter iter;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
		                   0, id,
		                   1, sub->shape == SPLINE?"Spline":"Truncated Cone",
		                   2, buffX,
		                   3, buffY,
		                   4, buffZ,
		                   -1);
	}

	void updateSelectedCrownItem()
	{
		char buffX[6], buffY[6], buffZ[6];
		sprintf(buffX, "%.1f", selectedSubcrown->x);
		sprintf(buffY, "%.1f", selectedSubcrown->y);
		sprintf(buffZ, "%.1f", selectedSubcrown->z);

		GtkTreeIter iter;
		GtkTreeModel *model;
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(crownList));
		if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
			gtk_list_store_set(crownStore, &iter,
			                   2, buffX,
			                   3, buffY,
			                   4, buffZ,
			                   -1);
		}
	}


	static void crownXChanged( GtkAdjustment *adj ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		panel->selectedSubcrown->x = gtk_adjustment_get_value(adj);
		panel->updateSelectedCrownItem();
		DrawMethods::render();
	}

	static void crownYChanged( GtkAdjustment *adj ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		panel->selectedSubcrown->y = gtk_adjustment_get_value(adj);
		panel->updateSelectedCrownItem();
		DrawMethods::render();
	}

	static void crownZChanged( GtkAdjustment *adj ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		panel->selectedSubcrown->z = gtk_adjustment_get_value(adj);
		panel->updateSelectedCrownItem();
		DrawMethods::render();
	}

	static void crownHChanged( GtkAdjustment *adj ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		if(panel->selectedSubcrown->shape == TRUNCATEDCONE)
		{
			TRUNCATEDCONE_CROWN(panel->selectedSubcrown)->h = gtk_adjustment_get_value(adj);
		}
		DrawMethods::render();
	}

	static void crownRDownChanged( GtkAdjustment *adj ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		if(panel->selectedSubcrown->shape == TRUNCATEDCONE)
		{
			TRUNCATEDCONE_CROWN(panel->selectedSubcrown)->r_down = gtk_adjustment_get_value(adj);
		}
		DrawMethods::render();
	}
	
	static void crownRUpChanged( GtkAdjustment *adj ,gpointer data )
	{
		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		if(panel->selectedSubcrown->shape == TRUNCATEDCONE)
		{
			TRUNCATEDCONE_CROWN(panel->selectedSubcrown)->r_up = gtk_adjustment_get_value(adj);
		}
		DrawMethods::render();
	}

	static void deleteCrownClicked( GtkWidget *widget ,gpointer data )
	{
		GtkTreeIter iter;
		GtkTreeModel *model;
		int id;

		CrownParametersPanel *panel = (CrownParametersPanel*) data;
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(panel->crownList));

		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
			gtk_tree_model_get (model, &iter, 0, &id, -1);
		}

		vector< Subcrown* >::iterator i = panel->params->crown->subcrowns.begin();
		i += id;
		panel->params->crown->subcrowns.erase(i);

		gtk_list_store_clear(panel->crownStore);
		panel->populateCrownList();
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
			sub = new TruncatedConeCrown(
			    0,0,0,
			    1,1,1);
		}

		panel->params->crown->subcrowns.push_back(sub);
		panel->storeAppendSubcrown(sub,
		                           panel->params->crown->subcrowns.size()-1,
		                           panel->crownStore);
		DrawMethods::render();
	}

	static void crownListSelectinChanged(GtkTreeSelection *selection, gpointer data)
	{
		GtkTreeIter iter;
		GtkTreeModel *model;
		int id;

		CrownParametersPanel *panel = (CrownParametersPanel*) data;

		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
			gtk_tree_model_get (model, &iter, 0, &id, -1);
			panel->selectedSubcrown = panel->params->crown->subcrowns.at(id);

			panel->params->crown->activeSubcrown = id;

			gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->xAdj), panel->selectedSubcrown->x);
			gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->yAdj), panel->selectedSubcrown->y);
			gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->zAdj), panel->selectedSubcrown->z);

			switch(panel->selectedSubcrown->shape)
			{
			case TRUNCATEDCONE:
				gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->hAdj), TRUNCATEDCONE_CROWN(panel->selectedSubcrown)->h);
				gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->rDownAdj), TRUNCATEDCONE_CROWN(panel->selectedSubcrown)->r_down);
				gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->rUpAdj), TRUNCATEDCONE_CROWN(panel->selectedSubcrown)->r_up);
				break;
			case SPLINE:
				panel->scpp->updatePanel(SPLINE_CROWN(panel->selectedSubcrown));
				break;
			default:
				break;
			}

			panel->showWidgets(panel->selectedSubcrown->shape);
		} else
		{
			panel->selectedSubcrown = NULL;
			panel->showWidgets(-1);
			panel->params->crown->activeSubcrown = -1;
		}

		DrawMethods::render();
	}

public:
	CrownParametersPanel(GtkWidget *window, Parameters *params) {
		this->mainWindow = window;
		this->params = params;
		this->selectedSubcrown = NULL;
	}

	GtkWidget* createPanel() {
		GtkWidget *crownWidget = gtk_frame_new("Crown Parameters");

		scpp = new SplineCrownParametersPanel();

		GtkWidget *hbox;
		GtkWidget *vbox = gtk_vbox_new(FALSE, 1);

		hbox = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);
		gtk_widget_show(hbox);

		crownTypeCombo = gtk_combo_box_new_text();
		gtk_widget_show(crownTypeCombo);
		gtk_combo_box_append_text(GTK_COMBO_BOX(crownTypeCombo),"Spline Crown");
		gtk_combo_box_append_text(GTK_COMBO_BOX(crownTypeCombo),"Truncated Cone Crown");
		gtk_combo_box_set_active(GTK_COMBO_BOX(crownTypeCombo),0);
		gtk_box_pack_start(GTK_BOX(hbox),crownTypeCombo,FALSE,FALSE,2);

		GtkWidget *button = gtk_button_new_with_label("Add crown");
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(this->addCrownClicked), this);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);

		crownStore = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);


		for (unsigned int i = 0; i < params->crown->subcrowns.size(); i++) {
			storeAppendSubcrown(params->crown->subcrowns.at(i), i, crownStore);
		}
		crownList = gtk_tree_view_new_with_model(GTK_TREE_MODEL(crownStore));

		g_object_unref(G_OBJECT(crownStore));

		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		GtkTreeViewColumn *column;

#define ADD_COLUMN(header, col_num)\
	column = gtk_tree_view_column_new_with_attributes (header, renderer, "text", col_num, NULL);\
	gtk_tree_view_append_column( GTK_TREE_VIEW(crownList), column);
		ADD_COLUMN("ID", 0);
		ADD_COLUMN("Shape", 1);
		ADD_COLUMN("x", 2);
		ADD_COLUMN("y", 3);
		ADD_COLUMN("z", 4);

#undef ADD_COLUMN




		gtk_box_pack_start(GTK_BOX(vbox), crownList, FALSE, FALSE, 0);
		gtk_widget_show(crownList);

		/*list selection*/
		GtkTreeSelection *select;

		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (crownList));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (crownListSelectinChanged), this);


		hbox = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);
		gtk_widget_show(hbox);

		GtkTooltips *tooltips=gtk_tooltips_new();
		GtkWidget *label, *scale;

#define PACK_LABEL_AND_SLIDER(text, adj , min,cb,hint,type)\
    hbox = gtk_hbox_new(FALSE,1);\
    label = gtk_label_new(text);\
    adj=gtk_adjustment_new(0, min, 20, 0.1 ,1,0);\
    g_signal_connect(adj,"value_changed",G_CALLBACK(cb),this);\
    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
    crownWidgets.push_back(pair<int, GtkWidget *>(type, scale));\
    crownWidgets.push_back(pair<int, GtkWidget *>(type, label));\
    gtk_scale_set_digits (GTK_SCALE (scale), 1);\
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,1);\
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
    gtk_tooltips_set_tip(tooltips,scale,hint,NULL);\
    gtk_widget_show(hbox);

		PACK_LABEL_AND_SLIDER("X:", xAdj, -20, crownXChanged, "sdfgdfg", -1);
		PACK_LABEL_AND_SLIDER("Y:", yAdj, -20, crownYChanged, "sdfgdfg", -1);
		PACK_LABEL_AND_SLIDER("Z:", zAdj, 0, crownZChanged, "sdfgdfg", -1);
		PACK_LABEL_AND_SLIDER("H:", hAdj, 0, crownHChanged, "sdfgdfg", TRUNCATEDCONE);
		PACK_LABEL_AND_SLIDER("R UP:", rUpAdj, 0, crownRUpChanged, "sdfgdfg", TRUNCATEDCONE);
		PACK_LABEL_AND_SLIDER("R DOWN:", rDownAdj, 0, crownRDownChanged, "sdfgdfg", TRUNCATEDCONE);

#undef PACK_LABEL_AND_SLIDER

		GtkWidget *splineCrownPanel  = scpp->createPanel();
		gtk_box_pack_start(GTK_BOX(vbox), splineCrownPanel, FALSE, FALSE, 2);
		crownWidgets.push_back(pair<int, GtkWidget *>(SPLINE, splineCrownPanel));

		deleteButton = gtk_button_new_with_label("Delete selected crown");
		g_signal_connect(G_OBJECT(deleteButton), "clicked", G_CALLBACK(this->deleteCrownClicked), this);
		gtk_box_pack_start(GTK_BOX(vbox), deleteButton, FALSE, FALSE, 2);
		crownWidgets.push_back(pair<int, GtkWidget *>(-1, deleteButton));


		gtk_container_add(GTK_CONTAINER(crownWidget), vbox);
		gtk_widget_show(vbox);
		gtk_widget_show(crownWidget);
		return crownWidget;
	}
	
	void updatePanel()
	{
		gtk_tree_store_clear(GTK_TREE_STORE(crownStore));
		populateCrownList();
		showWidgets(-1);
	}
	
};


#endif	/* CROWNPARAMETERSPANEL_H */
