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
	XRUChannelRefresh();
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
	if ( XR_Channel_Listing_Scrolled_Window )
	{
		if ( XR_Channel_Listing_State == 1 )
		{
			gtk_widget_hide( XR_Channel_Listing_Scrolled_Window );
			XR_Channel_Listing_State = 0;
		}
		else
		{
			gtk_widget_show( XR_Channel_Listing_Scrolled_Window );
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
on_scrolledwindow1_realize             (GtkWidget       *widget,
                                        gpointer         user_data)
{
	XR_Channel_Listing_Scrolled_Window = widget;
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
	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		0, 115);
	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		1, 160);
	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		2, 150);
	gtk_clist_set_column_width(GTK_CLIST(XR_Channel_Listing_Window), 
		3, 60);
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

