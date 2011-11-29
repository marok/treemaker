#ifndef _LEAVESPARAMETERSPANEL_H
#define _LEAVESPARAMETERSPANEL_H
#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <gdk/gdkkeysyms.h>
#include "LeavesParameters.h"
#include "Bmp.h"
using namespace std;
class LeavesParametersPanel: public IPanel{
	Parameters *params;
	GtkWidget *vbox;
	GtkWidget *image;
	GtkWidget *mainWindow;
	GtkWidget *list;
	vector<pair<string,GtkObject *> > adjustments;
	vector<GtkWidget *> scales;
	
	GtkObject* adjLeavesCount;
	
#define UNROLL_CALLBACK(param)\
	static void param##Changed( GtkAdjustment *adj, gpointer data){\
             LeavesParametersPanel *lpp = (LeavesParametersPanel *)data;\
             int active=lpp->params->lp->activeLeaf;\
             if(active!=-1){\
             lpp->params->lp->leaves[active].param = adj->value;\
             lpp->params->lp->generateTypes();\
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
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			char *tmpname=Bmp::generateIcon(filename);
			gtk_image_set_from_file(GTK_IMAGE(lpp->image),tmpname);
			Leaf leaf(filename);
			lpp->params->lp->leaves.push_back(leaf);
			lpp->add_to_list(lpp->list,leaf.getTexName());
			lpp->activateLeaf(leaf.getTexName());
		}
		gtk_widget_destroy(dialog);
	}
	
	static void leavesCountChanged( GtkAdjustment *adj, gpointer data)
	{
		LeavesParametersPanel *lpp=(LeavesParametersPanel*)data;
		lpp->params->lp->leavesCount = gtk_adjustment_get_value(adj);
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
		populate_list(list);
	}
	
	void
	populate_list(GtkWidget *list)
	{
		for (unsigned int i = 0; i < params->lp->leaves.size(); i++)
			add_to_list(list, params->lp->leaves[i].getTexName());
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
			draw_petiole(id);
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
	static gint
	image_pressed (GtkWidget *widget, GdkEventButton *event,gpointer data)
	{
		LeavesParametersPanel *lpp=(LeavesParametersPanel*)data;
		if (event->button == 1)
		{
			int x=event->x;
			int y=event->y;
			x-=100;
			int id=lpp->params->lp->activeLeaf;
			if(x>=0&&x<128&&id!=-1)
			{
				//obliczanie nowej pozycji szypulki
				Point2d *petiole=&lpp->params->lp->leaves[id].petiole;
				petiole->x=float(x)/float(128);
				petiole->y=float(y)/float(128);
				gtk_image_set_from_file(GTK_IMAGE(lpp->image),Bmp::generateIcon(lpp->params->lp->leaves[id].leafTexPath));
				lpp->draw_petiole(id);
			}
			gtk_widget_queue_draw(lpp->image);
		}
		return TRUE;
	}
	void draw_petiole(int id) {

		int x=params->lp->leaves[id].petiole.x*127;
		int y=params->lp->leaves[id].petiole.y*127;
		int width, height, rowstride, n_channels;
		guchar *pixels, *p;
		GdkPixbuf * pixbuf=gtk_image_get_pixbuf(GTK_IMAGE(image));
		n_channels = gdk_pixbuf_get_n_channels (pixbuf);

		g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
		g_assert (gdk_pixbuf_get_bits_per_sample (pixbuf) == 8);
		g_assert (n_channels == 3);

		width = gdk_pixbuf_get_width (pixbuf);
		height = gdk_pixbuf_get_height (pixbuf);

		g_assert (x >= 0 && x < width);
		g_assert (y >= 0 && y < height);

		rowstride = gdk_pixbuf_get_rowstride (pixbuf);
		pixels = gdk_pixbuf_get_pixels (pixbuf);

		//draw rectangle
		for(int dx=-2; dx<=2; dx++)
			for(int dy=-2; dy<=2; dy++)
				for(int i=0; i<9; i++) {
					if(x+dx>=0&&x+dx<width)
						if(y+dy>=0&&y+dy<height) {
							p = pixels + (y+dy) * rowstride + (x+dx) * n_channels;
							p[0] = 0;
							p[1] = 120;
							p[2] = 0;
						}
				}
	}
public:
	LeavesParametersPanel(GtkWidget *window,Parameters *params) {
		this->params=params;
		this->mainWindow=window;
	}
	GtkWidget* createPanel() {
		GtkWidget *leavesWidget=gtk_frame_new("Leaves Parameters");
		gtk_widget_show(leavesWidget);
		GtkTooltips *tooltips=gtk_tooltips_new();
		GtkWidget *button,*hbox;
		vbox=gtk_vbox_new(FALSE,1);
		GtkWidget *label,*scale;
		GtkObject* adj;
	
		gtk_container_add (GTK_CONTAINER (leavesWidget), vbox);

		hbox = gtk_hbox_new(FALSE, 1);
		label = gtk_label_new("Leaves count:");
		adjLeavesCount = gtk_adjustment_new(params->lp->leavesCount, 0, 400, 5, 1, 0);
		g_signal_connect(adjLeavesCount, "value_changed", G_CALLBACK(leavesCountChanged),this);
		scale = gtk_hscale_new(GTK_ADJUSTMENT(adjLeavesCount));
		gtk_scale_set_digits(GTK_SCALE(scale), 0);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), scale, TRUE, TRUE, 0);
		gtk_tooltips_set_tip(tooltips, scale, "Leaves count", NULL);
		gtk_widget_show(label);
		gtk_widget_show(hbox);
		gtk_widget_show(scale);
		
		list=gtk_tree_view_new();
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
		PACK_LABEL_AND_SLIDER("Size deriviation:",0,0.9,0.1,sizeDeriviation,1,"Deriviation of a leaves size");
		PACK_LABEL_AND_SLIDER("Amount:",0,1,0.01,amount,2,"Amount of leaves that type");

#undef PACK_LABEL_AND_SLIDER
		//leaf image
		GtkWidget *eventBox=gtk_event_box_new();
		gtk_box_pack_start(GTK_BOX(vbox),eventBox,FALSE,FALSE,1);
		gtk_signal_connect (GTK_OBJECT (eventBox), "button_press_event",
		                    (GtkSignalFunc)image_pressed, this);
		gtk_widget_show(eventBox);
		image=gtk_image_new_from_icon_name ("broken image" ,  GTK_ICON_SIZE_LARGE_TOOLBAR);
		gtk_tooltips_set_tip(tooltips,eventBox,"Click on the image to select petiole",NULL);
		gtk_container_add(GTK_CONTAINER(eventBox),image);

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
	
	void updatePanel()
	{
		GtkListStore *store;

		store = GTK_LIST_STORE(gtk_tree_view_get_model
		                       (GTK_TREE_VIEW(list)));
		gtk_list_store_clear(store);
		populate_list(list);
		activateWidgets(FALSE);
	}

};


#endif