#ifndef SPLINECROWNPARAMETERSPANEL_H
#define	SPLINECROWNPARAMETERSPANEL_H

#include <gtk/gtk.h>
#include <vector>

using namespace std;

class SplineCrownParametersPanel
{
	GtkListStore *splineCrownStore;
	GtkWidget *splineCrownList;
	
	GtkWidget *splinePointVBox;
	GtkWidget* deleteSplinePointButton;

	GtkObject *splineXAdj, *splineYAdj;
	
	SplineCrown *selectedSplineCrown;
	
	void populateSplineCrownList(SplineCrown *splineCrown) {
		for (unsigned int i = 0; i < splineCrown->crownMainPoints.size(); i++) {
			Point2d *p = splineCrown->crownMainPoints.at(i);

			GtkTreeIter iter;
			gtk_list_store_append(splineCrownStore, &iter);
			gtk_list_store_set(splineCrownStore, &iter,
				0, i,
				1, p->x,
				2, p->y,
				-1);
		}
	}
	
	void updateSelectedSplinePointItem()
	{
		Point2d* p = selectedSplineCrown->crownMainPoints[selectedSplineCrown->activePoint];


		GtkTreeIter iter;
		GtkTreeModel *model;
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(splineCrownList));
		if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
			gtk_list_store_set(splineCrownStore, &iter,
			                   1, p->x,
			                   2, p->y,
			                   -1);
		}
	}
	
	static void addSplinePoint( GtkWidget *widget ,gpointer data )
	{
		SplineCrownParametersPanel *panel = (SplineCrownParametersPanel*) data;

		float size = panel->selectedSplineCrown->crownMainPoints.size();

		float x = panel->selectedSplineCrown->crownMainPoints.at(size/2)->x+0.01;
		float y = panel->selectedSplineCrown->crownMainPoints.at(size/2)->y+0.01;;

		printf("%f\n",x);

		panel->selectedSplineCrown->addCrownMainPoint(new Point2d(x,y));

		gtk_list_store_clear(panel->splineCrownStore);
		panel->populateSplineCrownList(panel->selectedSplineCrown);
		DrawMethods::render();
	}
	
	static void deleteSplinePoint( GtkWidget *widget ,gpointer data )
	{
		SplineCrownParametersPanel *panel = (SplineCrownParametersPanel*) data;

		GtkTreeIter iter;
		GtkTreeModel *model;
		int id;


		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(panel->splineCrownList));

		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
			gtk_tree_model_get (model, &iter, 0, &id, -1);
		}

		vector< Point2d* >::iterator i = panel->selectedSplineCrown->crownMainPoints.begin();
		i += id;
		panel->selectedSplineCrown->deleteCrownMainPoint(i);

		gtk_list_store_clear(panel->splineCrownStore);
		panel->populateSplineCrownList(panel->selectedSplineCrown);
		DrawMethods::render();
	}

	static void splineXChanged( GtkAdjustment *adj ,gpointer data )
	{
		SplineCrownParametersPanel *panel = (SplineCrownParametersPanel*) data;
		panel->selectedSplineCrown->changeCrownMainPoint(panel->selectedSplineCrown->activePoint,
		        gtk_adjustment_get_value(adj),
		        panel->selectedSplineCrown->crownMainPoints[panel->selectedSplineCrown->activePoint]->y);

		panel->updateSelectedSplinePointItem();

		DrawMethods::render();
		printf("x %f\n",gtk_adjustment_get_value(adj));
	}

	static void splineYChanged( GtkAdjustment *adj ,gpointer data )
	{
		SplineCrownParametersPanel *panel = (SplineCrownParametersPanel*) data;
		panel->selectedSplineCrown->changeCrownMainPoint(panel->selectedSplineCrown->activePoint,
		        panel->selectedSplineCrown->crownMainPoints[panel->selectedSplineCrown->activePoint]->x,
		        gtk_adjustment_get_value(adj));

		panel->updateSelectedSplinePointItem();

		DrawMethods::render();
	}

	static void splinePointsListSelectionChanged(GtkTreeSelection *selection, gpointer data)
	{
		SplineCrownParametersPanel *panel = (SplineCrownParametersPanel*) data;
		GtkTreeIter iter;
		GtkTreeModel *model;

		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
			gtk_tree_model_get (model, &iter, 0, &panel->selectedSplineCrown->activePoint, -1);
			gtk_widget_show(panel->splinePointVBox);
			if(panel->selectedSplineCrown->crownMainPoints.size() > 2)
				gtk_widget_set_sensitive(panel->deleteSplinePointButton, TRUE);
			
			float size = panel->selectedSplineCrown->crownMainPoints.size();
			float activePoint = panel->selectedSplineCrown->activePoint;
			
			if(activePoint > 0 && activePoint < size-1)
			{
				float xPrev = panel->selectedSplineCrown->crownMainPoints[activePoint-1]->x;
				float xNext = panel->selectedSplineCrown->crownMainPoints[activePoint+1]->x;

				gtk_adjustment_set_lower(GTK_ADJUSTMENT(panel->splineXAdj), xNext + 0.1);
				gtk_adjustment_set_upper(GTK_ADJUSTMENT(panel->splineXAdj), xPrev - 0.1);
			} else if(activePoint == size - 1)
			{
				float xPrev = panel->selectedSplineCrown->crownMainPoints[size - 2]->x;
				float x = panel->selectedSplineCrown->crownMainPoints[size - 1]->x;
				gtk_adjustment_set_lower(GTK_ADJUSTMENT(panel->splineXAdj), x - 5);
				gtk_adjustment_set_upper(GTK_ADJUSTMENT(panel->splineXAdj), xPrev - 0.1);
			} else //panel->selectedSplinePointId == 0
			{
				float xNext = panel->selectedSplineCrown->crownMainPoints[1]->x;
				float x = panel->selectedSplineCrown->crownMainPoints[0]->x;
				gtk_adjustment_set_lower(GTK_ADJUSTMENT(panel->splineXAdj), xNext + 0.1);
				gtk_adjustment_set_upper(GTK_ADJUSTMENT(panel->splineXAdj), x + 5);
			}
			
			Point2d* p = panel->selectedSplineCrown->crownMainPoints[activePoint];
			gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->splineXAdj), p->x);
			gtk_adjustment_set_value(GTK_ADJUSTMENT(panel->splineYAdj), p->y);

		} else
		{
			gtk_widget_hide(panel->splinePointVBox);
			gtk_widget_set_sensitive(panel->deleteSplinePointButton, FALSE);
			panel->selectedSplineCrown->activePoint = -1;
		}
	}
public:
	SplineCrownParametersPanel()
	{
		this->selectedSplineCrown = NULL;
	}
	
	void updatePanel(SplineCrown *selectedSplineCrown)
	{
		this->selectedSplineCrown = selectedSplineCrown;
		
		gtk_list_store_clear(this->splineCrownStore);
		populateSplineCrownList(selectedSplineCrown);
	}
	
	GtkWidget* createPanel()
	{
		GtkWidget *resultVBox = gtk_vbox_new(FALSE,1);
		GtkWidget *hbox = gtk_hbox_new(FALSE, 1);
		gtk_box_pack_start(GTK_BOX(resultVBox), hbox, FALSE, FALSE, 1);
		
		splineCrownStore = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_FLOAT, G_TYPE_FLOAT);
		splineCrownList = gtk_tree_view_new_with_model(GTK_TREE_MODEL(splineCrownStore));
		
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		GtkTreeViewColumn *column;
#define ADD_COLUMN(header, col_num)\
	column = gtk_tree_view_column_new_with_attributes (header, renderer, "text", col_num, NULL);\
	gtk_tree_view_append_column( GTK_TREE_VIEW(splineCrownList), column);
		ADD_COLUMN("ID", 0);
		ADD_COLUMN("X", 1);
		ADD_COLUMN("Y", 2);
#undef ADD_COLUMN

		gtk_box_pack_start(GTK_BOX(hbox), splineCrownList, FALSE, FALSE, 2);
		g_object_unref(G_OBJECT(splineCrownStore));
		
		/*list selection*/
		GtkTreeSelection *selectSplinePoint;

		selectSplinePoint = gtk_tree_view_get_selection (GTK_TREE_VIEW (splineCrownList));
		gtk_tree_selection_set_mode (selectSplinePoint, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (selectSplinePoint), "changed", G_CALLBACK (splinePointsListSelectionChanged), this);

		GtkWidget *splineButtonsVBox = gtk_vbox_new(FALSE,1);
		gtk_box_pack_start(GTK_BOX(hbox), splineButtonsVBox, FALSE, FALSE, 2);

		GtkWidget* addSplinePointButton = gtk_button_new_with_label("Add");
		gtk_box_pack_start(GTK_BOX(splineButtonsVBox), addSplinePointButton, FALSE, FALSE, 2);
		g_signal_connect (G_OBJECT (addSplinePointButton), "clicked", G_CALLBACK (addSplinePoint), this);

		deleteSplinePointButton = gtk_button_new_with_label("Del");
		gtk_box_pack_start(GTK_BOX(splineButtonsVBox), deleteSplinePointButton, FALSE, FALSE, 2);
		g_signal_connect (G_OBJECT (deleteSplinePointButton), "clicked", G_CALLBACK (deleteSplinePoint), this);
		gtk_widget_set_sensitive(deleteSplinePointButton, FALSE);

		splinePointVBox = gtk_vbox_new(FALSE,1);
		gtk_box_pack_start(GTK_BOX(resultVBox), splinePointVBox, FALSE, FALSE, 2);

		GtkTooltips *tooltips=gtk_tooltips_new();
		GtkWidget *label, *scale;
#define PACK_LABEL_AND_SLIDER(text, adj, cb, hint)\
    hbox = gtk_hbox_new(FALSE,1);\
    label = gtk_label_new(text);\
    adj=gtk_adjustment_new(0, 0, 10, 0.1 ,1,0);\
    g_signal_connect(adj,"value_changed",G_CALLBACK(cb),this);\
    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
    gtk_scale_set_digits (GTK_SCALE (scale), 1);\
    gtk_box_pack_start(GTK_BOX(splinePointVBox),hbox,FALSE,FALSE,1);\
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
    gtk_tooltips_set_tip(tooltips,scale,hint,NULL);\

		PACK_LABEL_AND_SLIDER("X:", splineXAdj, splineXChanged, "sdfgdfg");
		PACK_LABEL_AND_SLIDER("Y:", splineYAdj, splineYChanged, "sdfgdfg");


#undef PACK_LABEL_AND_SLIDER
		gtk_widget_show_all(resultVBox);
		gtk_widget_hide(resultVBox);
		gtk_widget_hide(splinePointVBox);
		
		return resultVBox;
	}
};



#endif	/* SPLINECROWNPARAMETERSPANEL_H */

