#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define XR_CB_C

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "xr.h"


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	exit(0);
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	XRUAbout();
}


void
XRConnect                              (GtkButton       *button,
                                        gpointer         user_data)
{
	XRURadioOn();
}


void
XRDisconnect                           (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUDisconnect();
}


void
XRChannelUp                            (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUChannelUp();
}


void
XRChannelDown                          (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUChannelDown();
}


void
XRChannelJump                          (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUJumpToOpen();
}


void
XRChannelLast                          (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUChannelLast();
}


void
XRChannelRefresh                       (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUChannelRefreshFavorites();
}


void
XRMuteOff                              (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUMuteOff();
}


void
XRMuteOn                               (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUMuteOn();
}


void
XRChannelListToggle                    (GtkButton       *button,
                                        gpointer         user_data)
{
	if ( XR_Channel_Listing_Notebook_Window )
	{
		if ( XR_Channel_Listing_State == 1 )
		{
			gtk_widget_hide( XR_Channel_Listing_Notebook_Window );
			XR_Channel_Listing_State = 0;
		}
		else
		{
			gtk_widget_show( XR_Channel_Listing_Notebook_Window );
			XR_Channel_Listing_State = 1;
		}
	}
}


void
XRPreferences                          (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_artist_entry_realize                (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Artist_Entry = widget;
}


void
on_song_entry_realize                  (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Song_Entry = widget;
}


void
on_station_entry_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Station_Entry = widget;
}


void
on_category_entry_realize              (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Category_Entry = widget;
}


void
on_host_ip_entry_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Host_IP_Entry = widget;
}


void
on_host_status_image_realize           (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Status_Image = widget;
}


void
on_XR_Msg_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Msg = widget;
}


void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	if ( XR_Msg_Window )
		gtk_widget_hide(XR_Msg_Window);
}


void
on_notebook1_realize                   (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Channel_Listing_Notebook_Window = widget;
	XR_Channel_Listing_State = 1;
}


void
on_label6_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_About_Version = widget;
}


void
on_closebutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	if ( XR_About_Window )
		gtk_widget_hide(XR_About_Window);
}



void
on_button14_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUJumpToStationID();
}


void
on_button15_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUJumpToStationName();
}


void
on_entry1_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_JumpTo_StationID = widget;
}


void
on_entry2_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_JumpTo_StationName = widget;
}


void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUJumpToClose();
}



void
on_show_radio_id1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	XRUShowRadioID();
}


void
on_clist1_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Channel_Listing_Window = widget;
	gtk_clist_set_sort_column(GTK_CLIST(XR_Channel_Listing_Window), 4);
	gtk_clist_set_auto_sort(GTK_CLIST(XR_Channel_Listing_Window), TRUE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Channel_Listing_Window), 
		4, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Channel_Listing_Window), 
		5, FALSE);

	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		0, 115);
	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		1, 160);
	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		2, 150);
	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		3, 60);

	XR_Clist_Sig_ID = 
		g_signal_handler_find((gpointer)XR_Channel_Listing_Window,
			G_SIGNAL_MATCH_FUNC, 
			0, 0, NULL, 
			on_clist1_select_row,
			0);
}


void
on_clist1_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	XRUChannelListingSelected(clist, row, column, event, user_data);
}


gboolean
on_entry1_key_press_event              (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{

	XRUJumpKeyPress(event);
	return FALSE;
}


void
on_clist2_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Favorites_Listing_Window = widget;
	gtk_clist_set_sort_column(GTK_CLIST(XR_Favorites_Listing_Window), 4);
	gtk_clist_set_auto_sort(GTK_CLIST(XR_Favorites_Listing_Window), TRUE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Listing_Window), 
		1, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Listing_Window), 
		2, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Listing_Window), 
		3, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Listing_Window), 
		4, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Listing_Window), 
		5, FALSE);

	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Listing_Window), 
		0, 115);
	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Listing_Window), 
		1, 160);
	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Listing_Window), 
		2, 150);
	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Listing_Window), 
		3, 60);

	XR_Favorites_Clist_Sig_ID = 
		g_signal_handler_find((gpointer)XR_Favorites_Listing_Window,
			G_SIGNAL_MATCH_FUNC, 
			0, 0, NULL, 
			on_clist2_select_row,
			0);
}


void
on_clist2_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	XRUChannelListingSelected(clist, row, column, event, user_data);
}


void
on_clist3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Session_Listing_Window = widget;
	gtk_clist_set_sort_column(GTK_CLIST(XR_Session_Listing_Window), 4);
	gtk_clist_set_auto_sort(GTK_CLIST(XR_Session_Listing_Window), TRUE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Session_Listing_Window), 
		4, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Session_Listing_Window), 
		5, FALSE);

	gtk_clist_set_column_width(GTK_CLIST(XR_Session_Listing_Window), 
		0, 115);
	gtk_clist_set_column_width(GTK_CLIST(XR_Session_Listing_Window), 
		1, 160);
	gtk_clist_set_column_width(GTK_CLIST(XR_Session_Listing_Window), 
		2, 150);
	gtk_clist_set_column_width(GTK_CLIST(XR_Session_Listing_Window), 
		3, 60);

	XR_Category_Clist_Sig_ID = 
		g_signal_handler_find((gpointer)XR_Session_Listing_Window,
			G_SIGNAL_MATCH_FUNC, 
			0, 0, NULL, 
			on_clist3_select_row,
			0);
}


void
on_clist3_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	XRUChannelListingSelected(clist, row, column, event, user_data);
}




void
on_button16_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUAddFavoriteArtist();
}


void
on_button17_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUAddFavoriteStation();
}


void
on_clist4_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Favorites_Artist_Listing_Window = widget;
	gtk_clist_set_sort_column(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 1);
	gtk_clist_set_auto_sort(GTK_CLIST(XR_Favorites_Artist_Listing_Window), TRUE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		0, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		2, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		3, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		4, FALSE);
	gtk_clist_set_column_visibility(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		5, FALSE);

	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		0, 115);
	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		1, 160);
	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		2, 150);
	gtk_clist_set_column_width(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
		3, 60);

	XR_Favorites_Artist_Clist_Sig_ID = 
		g_signal_handler_find((gpointer)XR_Favorites_Artist_Listing_Window,
			G_SIGNAL_MATCH_FUNC, 
			0, 0, NULL, 
			on_clist4_select_row,
			0);
}


void
on_clist4_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	int	channel;
	char	msg[1024];

	/* Don't do this if the window is already visible. */
	if ( XR_Delete_Favorite_Popup != NULL )
		if ( GTK_WIDGET_VISIBLE(XR_Delete_Favorite_Popup) ) 
			return;

	/* Save the data for the current selection. */
	delete_favorite_artist = gtk_clist_get_row_data(GTK_CLIST(clist), row);
	delete_favorite_row = row;

	/* If its not yet available, create the popup dialog. */
	if ( XR_Delete_Favorite_Popup == NULL )
	{
		XR_Delete_Favorite_Popup = create_deletefavorites();
		gtk_widget_realize(XR_Delete_Favorite_Popup);
	}

	/* Populate the dialog and display it. */
	sprintf(msg, "Delete\n%s\nfrom Favorites?", delete_favorite_artist);
	gtk_label_set_text(GTK_LABEL(XR_Delete_Favorite_Label), msg);
	gtk_label_set_justify(GTK_LABEL(XR_Delete_Favorite_Label), 
		GTK_JUSTIFY_CENTER);
	gtk_widget_show(XR_Delete_Favorite_Popup);
}


void
on_setup_realize                       (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XRUReadFavorites();
}


void
on_button18_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	XRUJumpToFavoriteChannel();
}


void
on_button19_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide(XR_Favorite_Channel_Window);
}


void
on_entry3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Favorite_Channel_Label = widget;
}


void
on_favorites_realize                   (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Favorite_Channel_Window = widget;
}



void
on_deletefavorites_realize             (GtkWidget       *widget,
                                        gpointer         user_data)
{

}


void
on_deletelabel_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Delete_Favorite_Label = widget;
}


void
on_button20_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	if ( XR_Delete_Favorite_Popup != NULL )
	{
		gtk_clist_remove(GTK_CLIST(XR_Favorites_Artist_Listing_Window), 
			delete_favorite_row);
		g_list_remove( XR_List_Artist, delete_favorite_artist);
		gtk_widget_hide(XR_Delete_Favorite_Popup);
		XRUSaveFavorites();
	}
}


void
on_button21_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	if ( XR_Delete_Favorite_Popup != NULL )
		gtk_widget_hide(XR_Delete_Favorite_Popup);
}

