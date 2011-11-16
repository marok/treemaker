#ifndef _LEAVESPARAMETERSPANEL_H
#define _LEAVESPARAMETERSPANEL_H
#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <gdk/gdkkeysyms.h>
#include "LeavesParameters.h"
#include "Bmp.h"
using namespace std;
class LeavesParametersPanel {
	Parameters *params;
	GtkWidget *vbox;
	GtkWidget *image;
	GtkWidget *mainWindow;
	//int activeLeaf;
	vector<pair<string,GtkObject *> > adjustments;
	vector<GtkWidget *> scales;

#define UNROLL_CALLBACK(param)\
	static void param##Changed( GtkAdjustment *adj, gpointer data){\
             LeavesParametersPanel *lpp = (LeavesParametersPanel *)data;\
             int active=lpp->params->lp->activeLeaf;\
             if(active!=-1){\
             lpp->params->lp->leaves[active].param = adj->value;\
	  }\
	}
	UNROLL_CALLBACK(leafSize);
	UNROLL_CALLBACK(sizeDeriviation);
	UNROLL_CALLBACK(amount);
#undef UNROLL_CALLBACK


	static void addLeafClicked(GtkWidget *w,gpointer data) {
		GtkWidget *dialog;
		LeavesParametersPanel *lpp=(LeavesParametersPanel*)data;
		dialog = gtk_file_chooser_dialog_new("Select bmp file with leaves texture",GTK_WINDOW(lpp->mainWindow),GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
			//char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			//char *tmpname=Bmp::generateIcon(filename);
			//gtk_image_set_from_file(GTK_IMAGE(lpp->image),tmpname);
			//		lpp->params->lp->setLeavesPath(filename);
			//	lpp->params->lp->leavesTexInitialized=FALSE;
		}
		gtk_widget_destroy(dialog);


	}
	void
	init_list(GtkWidget *list)
	{

		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;
		GtkListStore *store;

		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Leaves list:",
		         renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

		store = gtk_list_store_new(1, G_TYPE_STRING);

		gtk_tree_view_set_model(GTK_TREE_VIEW(list),
		                        GTK_TREE_MODEL(store));

		g_object_unref(store);
		for(unsigned int i=0; i<params->lp->leaves.size(); i++)
			add_to_list(list,params->lp->leaves[i].getTexName());
	}

	void
	add_to_list(GtkWidget *list, const gchar *str)
	{

		GtkListStore *store;
		GtkTreeIter iter;

		store = GTK_LIST_STORE(gtk_tree_view_get_model
		                       (GTK_TREE_VIEW(list)));

		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, str, -1);
	}
	static void on_changed(GtkWidget *widget,gpointer data) {

		LeavesParametersPanel *lpp=(LeavesParametersPanel*)data;
		GtkTreeIter iter;
		GtkTreeModel *model;
		char *value;

		if (gtk_tree_selection_get_selected(
		            GTK_TREE_SELECTION(widget), &model, &iter)) {
			gtk_tree_model_get(model, &iter, 0, &value,  -1);
			lpp->activateWidgets(1);

			lpp->activateLeaf(value);
			g_free(value);
		}

	}
	void activateLeaf(char *texName) {
		int id=params->lp->getId(texName);
		params->lp->activeLeaf=id;
		if(id!=-1)
		{
			Leaf* act=&params->lp->leaves[id];
			//load proper sliders values
			for(unsigned int i=0; i<adjustments.size(); i++) {
#define CHECK_ARG(argName)\
	      if(adjustments[i].first.compare(#argName)==0)\
	       GTK_ADJUSTMENT(adjustments[i].second)->value=act->argName;
				CHECK_ARG(leafSize);
				CHECK_ARG(sizeDeriviation);
				CHECK_ARG(amount);
#undef CHECK_ARG
			}
			refreshScales();
			//load image
			gtk_image_set_from_file(GTK_IMAGE(image),Bmp::generateIcon(params->lp->leaves[id].leafTexPath));
			gtk_widget_show(image);
		}
	}
	void activateWidgets(bool state) {
		for(unsigned int i=0; i<scales.size(); i++)
			gtk_widget_set_sensitive(scales[i],state);

	}
	void refreshScales() {
		for(unsigned int i=0; i<scales.size(); i++)
			gtk_widget_queue_draw(scales[i]);

	}
public:
	LeavesParametersPanel(GtkWidget *window,Parameters *params) {
		this->params=params;
		this->mainWindow=window;
		//activeLeaf=-1;
	}
	GtkWidget* createPanel() {
		GtkWidget *leavesWidget=gtk_frame_new("Leaves Parameters");
		gtk_widget_show(leavesWidget);
		GtkTooltips *tooltips=gtk_tooltips_new();
		GtkWidget *button,*hbox;
		vbox=gtk_vbox_new(FALSE,1);
		GtkWidget *label,*scale;
		GtkObject *adj;
		gtk_container_add (GTK_CONTAINER (leavesWidget), vbox);


		GtkWidget *list=gtk_tree_view_new();
		gtk_box_pack_start(GTK_BOX(vbox),list,FALSE,FALSE,1);

		init_list(list);

		GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
		g_signal_connect(selection,"changed",G_CALLBACK(on_changed),this);

#define PACK_LABEL_AND_SLIDER(text,min,max,step,paramName,digits,hint)\
		    hbox = gtk_hbox_new(FALSE,1);\
		    label = gtk_label_new(text);\
		    adj=gtk_adjustment_new(min,min,max,step,1,0);\
		    g_signal_connect(adj,"value_changed",G_CALLBACK(paramName##Changed),this);\
		    scale=gtk_hscale_new(GTK_ADJUSTMENT(adj));\
		    gtk_scale_set_digits (GTK_SCALE (scale), digits);\
		    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,1);\
		    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);\
		    gtk_box_pack_start(GTK_BOX(hbox),scale,TRUE,TRUE,0);\
		    gtk_tooltips_set_tip(tooltips,scale,hint,NULL);\
		    adjustments.push_back(pair<string,GtkObject*>(#paramName,adj));\
		    scales.push_back(scale);\
		    gtk_widget_show(label);\
		    gtk_widget_show(hbox);\
		    gtk_widget_show(scale);

		PACK_LABEL_AND_SLIDER("Leaf size:",1,2,0.1,leafSize,1,"Size of a leaves on a tree");
		PACK_LABEL_AND_SLIDER("Size deriviation:",0,0.5,0.1,sizeDeriviation,1,"Deriviation of a leaves size");
		PACK_LABEL_AND_SLIDER("Amount:",0,1,0.01,amount,2,"Amount of leaves that type");

#undef PACK_LABEL_AND_SLIDER
		//leaf image
		image=gtk_image_new_from_icon_name ("broken image" ,  GTK_ICON_SIZE_LARGE_TOOLBAR);
		gtk_box_pack_start(GTK_BOX(vbox),image,FALSE,FALSE,1);

		//leaf add button
		button=gtk_button_new_with_label("Add leaf");
		g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (this->addLeafClicked), this);
		gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,1);
		gtk_tooltips_set_tip(tooltips,button,"Opens file chooser dialog",NULL);
		gtk_widget_show(button);

		activateWidgets(0);
		gtk_widget_show(leavesWidget);
		gtk_widget_show(vbox);
		gtk_widget_show(list);
		return leavesWidget;
	}

};


#endif