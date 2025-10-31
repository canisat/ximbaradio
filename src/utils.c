/* $Id: $ */

/*========================================================================
 *  Copyright (c) Michael J. Hammel 2004
 *========================================================================
 *              FILE NAME: utils.c
 *            DESCRIPTION: Ximba Radio utility functions
 *      DEFINED CONSTANTS:
 *       TYPE DEFINITIONS:
 *      MACRO DEFINITIONS:
 *       GLOBAL VARIABLES:
 *       PUBLIC FUNCTIONS: XRUMsg, XRUAbout, XRUConnect, XRUDisconnect
 *                       : XRUGetChannelInfo
 *      PRIVATE FUNCTIONS: ReadFromSocket
 *  SOFTWARE DEPENDENCIES:
 *  HARDWARE DEPENDENCIES:
 *                  NOTES:
 *
 * SPECIAL CONSIDERATIONS:
 * Set your tabstops to 3 to make the code more readable.
 *
 *========================================================================
 *
 * MODIFICATION HISTORY:
 * $Log:$
 *
 *========================================================================*/
#define XR_UTIL_C

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "interface.h"
#include "callbacks.h"
#include "xr.h"

/* Locally scoped functions */
static int ReadFromSocket();
static int GetRadioID();
static int GetChannelInfo();
static void AddClistEntry();
static void UpdateClistEntry();
static void UpdateChannelEntry();
static int FavoriteItem();
static void PaintChannelListEntry();
static void SaveArtist();
static void UpdateCategoryTabs();
static int FindCategory();
static void UpdateChannelListings();
static void RefreshFavoriteChannel();
static int FindPrefsCategory();
static void AddPrefsCategory();
static void SavePrefsCategory();

/* Locally scoped variables */

/* Generic buffers for holding incoming or outgoing data. */
static char	cmdbuf[512];
static char	chinfobuf[1024];
static char	lastbuf[512];

/* The timer that updates all windows */
static int	channel_list_timer;

/* Name of the current category when working with preferences */
static char	*category_name;
static int	category_row;

/* State information. */
static int	disconnected = 1;
static int	last_channel = -1;
static int	current_channel = -1;

/* These two are used to prepare data for the CList's */
static char	row_data[6][512];
static char *rows[6];

/* This one holds a channel id when a favorite artist is encountered. */
static int	favorite_channel;

/* These two hold the highlight and normal colors for list windows. */
static GdkColor	gdk_favorite_color;
static GdkColor	gdk_default_color;

/*
 * The arrays that determine the performance of the channel scanning.
 * These arrays are indexed by the prefs.performance value.
 * The idea here is that you fetch "channel_refresh" channels every
 * "channel_time" seconds.
 */
int channel_refresh[10] = {  3,  3,  3,  5,  5, 5, 5, 5, 7, 7 };
int channel_time[10]    = { 20, 17, 15, 13, 10, 7, 5, 3, 3, 2 };

/*========================================================================
 *	Name:		ReadFromSocket
 *	Scope: 	Private
 *					
 *	Description:
 *	Reads data from the SockFD socket up until a newline is received
 * or the specified number of bytes have been read.
 *	Saves data into the specified buffer.
 *
 *	Input Arguments:
 * char		*buf			where the data read from the socket are saved.
 * int		size			maximum number of bytes to read.
 *
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int		Sock_FD			The socket used for connections to OpenXM server.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
int
ReadFromSocket(
	char	*buf,
	int	size
)
{
	char		chr = '\0';
	int		i = 0;

	bzero(buf, size);
	while ((chr != '\n') && (i<size))
	{
		read(XR_SockFD, &chr, 1);
		bcopy((char *)&chr, (char *)(buf+i), 1);
		i++;
	}
	if ( i==size)
	{
		return(1);
	}
	return(0);
}

/*========================================================================
 *	Name:		GetRadioID
 *	Scope: 	Private
 *					
 *	Description:
 *	Retrieves the Radio ID from the server.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int	Sock_FD			The socket used for connections to OpenXM server.
 * char	*cmdbuf			buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
int
GetRadioID()
{
	char	*ptr;

	send(XR_SockFD, XR_CMD_RADIO_ID, strlen(XR_CMD_RADIO_ID), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
	{
		XRUMsg("Server failed to send proper Radio ID.");
		return;
	}

	/* Grab the radio ID and save it. */
	bzero((char *)(cmdbuf + strlen(cmdbuf) - 1), 1);
	ptr = strtok(cmdbuf, "|");
	ptr = strtok(NULL, "|");
	if ( XR_Radio_ID ) free(XR_Radio_ID);
	XR_Radio_ID = (char *)malloc(strlen(ptr)+1);
	sprintf(XR_Radio_ID, ptr);
	DBGPrintf(("GetRadioID(): %s\n", XR_Radio_ID));
}

/*========================================================================
 *	Name:		GetChannelInfo
 *	Scope: 	Private
 *					
 *	Description:
 *	Retrieves channel information.
 *
 *	Input Arguments:
 *	int	ch_id			which channel to fetch, or 0 for current channel.
 *	char	*savebuf		buffer to save return information
 *	int	savesize		maximum size of save buffer.
 *
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int	Sock_FD			The socket used for connections to OpenXM server.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
static int
GetChannelInfo(
	int	ch_id,
	char	*savebuf,
	int	savesize
)
{
	/* Get the channel info for the specified channel */
	char	buf[32];
	bzero(buf, 32);
	if ( ch_id )
		sprintf(buf, "%s%03d\n", XR_CMD_CHA_INFO, ch_id);
	else
		sprintf(buf, "%sXXX\n", XR_CMD_CHA_INFO);
	send(XR_SockFD, buf, strlen(buf), 0);
	if (ReadFromSocket(savebuf, savesize) )
	{
		XRUMsg("Error retrieving channel info.\nClosing connection to server");
		XRUDisconnect();
		return(-1);
	}
	return(0);
}

/*========================================================================
 *	Name:		XRUMsg
 *	Scope: 	Public
 *					
 *	Description:
 * Displays a message in a pop up window.
 *
 *	Input Arguments:
 *	char		*msg			Pointer to message to display.
 *
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUMsg(
	char	*msg
)
{
	if ( XR_Msg_Window == NULL )
	{
		XR_Msg_Window = create_dialog1();
		gtk_widget_show(XR_Msg_Window);
	}

	gtk_label_set_text(GTK_LABEL(XR_Msg), msg);
	gtk_widget_show(XR_Msg_Window);
}


/*========================================================================
 *	Name:		XRUAbout
 *	Scope: 	Public
 *					
 *	Description:
 * Displays the About dialog.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUAbout(
)
{
	if ( XR_About_Window == NULL )
	{
		XR_About_Window = create_dialog2();
		gtk_widget_show(XR_About_Window);
		gtk_label_set_text(GTK_LABEL(XR_About_Version), XR_VERSION);
	}

	gtk_widget_show(XR_About_Window);
}

/*========================================================================
 *	Name:		XRUConnect
 *	Scope: 	Public
 *					
 *	Description:
 * Connects to the OpenXM server.   Displays connect status in a window.
 * Displays IP of server in main window.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int		Sock_FD			The socket used for connections to OpenXM server.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
int
XRUConnect()
{
	int	firsttime = 1;
	struct sockaddr_in	server_addr;

	bzero((char *)&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	server_addr.sin_port = htons(SERV_TCP_PORT);

	if ( (XR_SockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		XRUMsg("Can't connect to XMPCR server");
		printf("Can't open stream socket to XMPCR server\n");
		return(1);
	}

	if (connect(XR_SockFD, (struct sockaddr *) &server_addr,
			sizeof(server_addr)) < 0 )
	{
		XRUMsg("Can't connect to XMPCR server");
		printf("Can't connect to XMPCR server\n");
		return(1);
	}
	gtk_entry_set_text(GTK_ENTRY(XR_Host_IP_Entry), SERV_HOST_ADDR);

	/* Set the "host connected" icon */
	gtk_image_set_from_stock(GTK_IMAGE(XR_Status_Image), 
		GTK_STOCK_APPLY,
		GTK_ICON_SIZE_BUTTON);

	/* Get a background color for favorites */
	if ( firsttime )
	{
		gdk_favorite_color.red   = (guint16)(0*65535.0);
		gdk_favorite_color.green = (guint16)(1*65535.0);
		gdk_favorite_color.blue  = (guint16)(1*65535.0);
		gdk_default_color.red    = (guint16)(1*65535.0);
		gdk_default_color.green  = (guint16)(1*65535.0);
		gdk_default_color.blue   = (guint16)(1*65535.0);
		firsttime = 0;
	}
	return(0);
}

/*========================================================================
 *	Name:		XRUDisconnect
 *	Scope: 	Public
 *					
 *	Description:
 * Disconnect from the remote OpenXM server.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int	Sock_FD			The socket used for connections to OpenXM server.
 * int	disconnected	If set, means we are not connected.
 * char	*cmdbuf			buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUDisconnect()
{
	if ( disconnected ) return;

	send(XR_SockFD, XR_CMD_POWER_OFF, strlen(XR_CMD_POWER_OFF), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XMPCR server failed while closing connection");
	else
		XRUMsg("XMPCR server disconnected");
	disconnected = 1;
	if ( XR_Radio_ID ) free(XR_Radio_ID);
	XR_Radio_ID = NULL;
	close(XR_SockFD);
	gtk_editable_delete_text(GTK_EDITABLE(XR_Host_IP_Entry), 0, -1);

	/* Set the "host connected" icon */
	gtk_image_set_from_stock(GTK_IMAGE(XR_Status_Image), 
		GTK_STOCK_REMOVE,
		GTK_ICON_SIZE_BUTTON);
}

/*========================================================================
 *	Name:		XRURadioOn
 *	Scope: 	Public
 *					
 *	Description:
 * Turns the XM Radio on by sending the appropriate command to the server.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int	Sock_FD			The socket used for connections to OpenXM server.
 * char	*cmdbuf			buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRURadioOn()
{
	char	*ptr;

	if ( XRUConnect() == 1 ) return;

	/* Turn the radio on. */
	bzero(cmdbuf, 512);
	send(XR_SockFD, XR_CMD_POWER_ON, strlen(XR_CMD_POWER_ON), 0);
	if (ReadFromSocket(cmdbuf, 512) )
	{
		XRUMsg("Error retrieving okay from server connection.\nDisconnecting.");
		XRUDisconnect();
		return;
	}

	/* Make sure server returned a "1" - that's means "Okay" */
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XM Radio is already on.");
	else
		XRUMsg("XM Radio has been powered on.");

	disconnected = 0;
	GetRadioID();
	bzero(lastbuf, 512);

	/* Set the timer that will update the channel listings periodically. */
	/* channel_list_timer = gtk_timeout_add (6*1000,
	 */
	channel_list_timer = gtk_timeout_add (channel_time[prefs.performance]*1000,
      (GtkFunction)XRUChannelRefresh, 0);

}

/*========================================================================
 *	Name:		XRUGetChannelInfo
 *	Scope: 	Public
 *					
 *	Description:
 * Retrieves the current channel information.  Saves it to the "chinfobuf"
 * buffer. 
 *
 *	Input Arguments:
 *	int		save_channel		If set, saves the current channel id to
 *										"save_channel" variable.
 *
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int		Sock_FD			The socket used for connections to OpenXM server.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
int
XRUGetChannelInfo(
	int		save_channel
)
{
	char	chr;
	char	*ptr;
	char	*channel, *station;
	char	*artist, *song;
	char	*category;
	char	buf[512];
	int	i;
	int	channel_id;
	int	row;

	static int	running = 0;
	static int	firsttime = 1;

	/* Don't do this if we aren't connected to the server. */
	if ( disconnected ) return(FALSE);

	/* Don't allow this function to be reentrant. */
	if (running) return(TRUE);
	running = 1;

	/* Send the command to get the channel information, then pull the results. */
	if ( GetChannelInfo(0, chinfobuf, 1024) == -1 ) 
	{
		running = 0;
		return(TRUE);
	}

	/* Skip the first four fields - we don't use them for now. */
	DBGPrintf(("Channel info:\n%s", chinfobuf));

	bzero((char *)(chinfobuf + strlen(chinfobuf) - 1), 1);
	ptr = strtok(chinfobuf, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");

	/*
	 * Save the entry temporarily - if its complete, we'll save it in a more
	 * permanent place in a moment.
	 */
	ptr += strlen(ptr) + 1;
	strcpy(buf, ptr);

	/* Grab the fields from the returned data */
	if ( (channel  =strtok(NULL, "~")) == NULL ) {running=0; return(TRUE);}
	if ( (station  =strtok(NULL, "~")) == NULL ) {running=0; return(TRUE);}
	if ( (category =strtok(NULL, "~")) == NULL ) {running=0; return(TRUE);}
	if ( (artist   =strtok(NULL, "~")) == NULL ) {running=0; return(TRUE);}
	if ( (song     =strtok(NULL, "~")) == NULL ) {running=0; return(TRUE);}

	/* If we get here, we got a full entry, so save it. */
	strcpy(lastbuf, buf);

	DBGPrintf((
		"Channel: %s\n"
		"Station: %s\n"
		"Category: %s\n"
		"Artist: %s\n"
		"Song: %s\n",
		channel, station, category, artist, song));

	/* Fill in the display with the retrieved information. */
	if ( ( channel ) && ( station ) )
	{
		if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			sprintf(buf, "%s: %s", channel, station);
		else
			sprintf(buf, "%s: Preview", channel);
		gtk_entry_set_text(GTK_ENTRY(XR_Station_Entry), buf);
	}
	else
		gtk_editable_delete_text(GTK_EDITABLE(XR_Station_Entry), 0, -1);

	if ( artist )
		if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			gtk_entry_set_text(GTK_ENTRY(XR_Artist_Entry), artist);
		else
			gtk_entry_set_text(GTK_ENTRY(XR_Artist_Entry), "XM Preview");
	if ( song )
		if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			gtk_entry_set_text(GTK_ENTRY(XR_Song_Entry), song);
		else
			gtk_editable_delete_text(GTK_EDITABLE(XR_Song_Entry), 0, -1);
	if ( category )
		if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			gtk_entry_set_text(GTK_ENTRY(XR_Category_Entry), category);
		else
			gtk_editable_delete_text(GTK_EDITABLE(XR_Category_Entry), 0, -1);

	/* If requested, save the current channel */
	if ( ( save_channel ) && ( channel ) )
		current_channel = atoi(channel);

	/* Update all the channel listing windows. */
	UpdateChannelListings(current_channel, station, song, artist, category);

	/* Allow other calls to this function again. */
	running = 0;

	/*
	 * All functions that are called by timers need to return this to keep
	 * running (or FALSE to not be run again).
	 */
	return(TRUE);
}

/*========================================================================
 *	Name:		UpdateChannelListings
 *	Scope: 	Private
 *					
 *	Description:
 * Updates the various channel listing windows for the specified channel.
 *
 *	Input Arguments:
 *	int		channel		Channel to update.
 *	char		*station		Station string
 *	char		*song			Song string
 *	char		*artist		Artist string
 *	char		*category	Category string
 *========================================================================*/
static void
UpdateChannelListings(
	int		channel,
	char		*station,
	char		*song,
	char		*artist,
	char		*category
)
{
	int			row;
	CategoryT	*linkptr;
	GList			*listptr;
	int			index;

	/* Update the channel listing window if this channel already exists there. */
	row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Channel_Listing_Window), (gpointer)channel);
	if ( row != -1 )
	{
		UpdateChannelEntry(row, station, song, artist, category);
		PaintChannelListEntry( XR_Channel_Listing_Window, row, artist);
	}

	/* Update the session listing window. */
	row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Session_Listing_Window), (gpointer)channel);
	if ( row != -1 )
		UpdateClistEntry(XR_Session_Listing_Window,
			row, station, song, artist, category);
	else
	{
		AddClistEntry(
			XR_Session_Listing_Window,
			station, song, artist, category, channel);
		row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Session_Listing_Window), (gpointer)channel);
	}
	PaintChannelListEntry(XR_Session_Listing_Window, row, artist);

	/* Update the category listing window, if any. */
	listptr = g_list_find_custom(XR_List_Categories, category, 
					(gpointer)FindCategory);
	if ( listptr != NULL )
	{
		index = g_list_position(XR_List_Categories, listptr);
		linkptr = (CategoryT *)g_list_nth_data(XR_List_Categories, index);
	
		/* See if this channel is in the exist category list. */
		row = gtk_clist_find_row_from_data(GTK_CLIST(linkptr->clist), 
				(gpointer)channel);
		if ( row != -1 )
		{
			UpdateClistEntry(linkptr->clist, row, station, song, artist, category);
			PaintChannelListEntry(linkptr->clist, row, artist);
		}
	}
}

/*========================================================================
 *	Name:		XRUChannelUp
 *	Scope: 	Public
 *					
 *	Description:
 * Requests the server to change the channel up by one station.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	int	last_channel		used to specify the last channel we were on.
 *	int	current_channel	used to specify the current channel we are on.
 * char	*cmdbuf				buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUChannelUp()
{
	send(XR_SockFD, XR_CMD_CHA_UP, strlen(XR_CMD_CHA_UP), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XMPCR server could not change channel.");
	last_channel = current_channel;
	XRUGetChannelInfo(1);
}

/*========================================================================
 *	Name:		XRUChannelDown
 *	Scope: 	Public
 *					
 *	Description:
 * Requests the server to change the channel down by one station.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	int	last_channel		used to specify the last channel we were on.
 *	int	current_channel	used to specify the current channel we are on.
 * char	*cmdbuf				buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUChannelDown()
{
	send(XR_SockFD, XR_CMD_CHA_DOWN, strlen(XR_CMD_CHA_DOWN), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XMPCR server could not change channel.");
	last_channel = current_channel;
	XRUGetChannelInfo(1);
}

/*========================================================================
 *	Name:		XRUMuteOff
 *	Scope: 	Public
 *					
 *	Description:
 * Requests the server to mute the radio.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * char		*cmdbuf			buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUMuteOff()
{
	send(XR_SockFD, XR_CMD_MUTE_OFF, strlen(XR_CMD_MUTE_OFF), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XMPCR server could not mute radio.");

	/* Set the "host connected" icon */
	gtk_image_set_from_stock(GTK_IMAGE(XR_Status_Image), 
		GTK_STOCK_APPLY,
		GTK_ICON_SIZE_BUTTON);
}

/*========================================================================
 *	Name:		XRUMuteOn
 *	Scope: 	Public
 *					
 *	Description:
 * Requests the server to unmute the radio.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * char		*cmdbuf			buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUMuteOn()
{
	send(XR_SockFD, XR_CMD_MUTE_ON, strlen(XR_CMD_MUTE_ON), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XMPCR server could not mute radio.");

	/* Set the "Mute" icon */
	gtk_image_set_from_stock(GTK_IMAGE(XR_Status_Image), 
		GTK_STOCK_NO,
		GTK_ICON_SIZE_BUTTON);
}

void
XRUChannelJump()
{
	/* Open jump dialog */
}

/*========================================================================
 *	Name:		XRUChannelLast
 *	Scope: 	Public
 *					
 *	Description:
 * Jumps to the last channel we were listening to prior to the current
 * channel.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * char		*cmdbuf			buffer used for all outbound commands.
 * int		Sock_FD			The socket used for connections to OpenXM server.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void
XRUChannelLast()
{
	char	buf[32];
	sprintf(buf, "%s%03d\n", XR_CMD_CHA_SET, last_channel);
	send(XR_SockFD, buf, strlen(buf), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XMPCR server could not change to previous station.");
	else
	{
		last_channel = current_channel;
		XRUGetChannelInfo(1);
	}
}

/*========================================================================
 *	Name:		XRUChannelRefresh
 *	Scope: 	Public
 *					
 *	Description:
 * Refreshes the Channel Listing window by querying for a specified number
 * of channels at a time.  This function runs in a timer and the number of
 * channels should be a configurable parameter to improve response in the GUI.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int		Sock_FD			The socket used for connections to OpenXM server.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
int
XRUChannelRefresh()
{
	static int		channel_id = 1;
	char				buf[512];
	char				*channel, *station;
	char				*artist, *song;
	char				*category;
	char				*ptr;
	int				i;
	int				row;
	static int		running = 0;
	static int		firsttime = 1;
	static int		favorite_count = 0;

	/* Reset most things if we disconnect. */
	if ( disconnected ) {
		bzero(lastbuf, 512);
		channel_id = 1;
		return(FALSE);
	}

	if ( running ) return(TRUE);

	gtk_clist_freeze(GTK_CLIST(XR_Channel_Listing_Window));
	running = 1;
	for (i=channel_id; 
		i<channel_id+channel_refresh[prefs.performance] && i<=XR_MAX_CHANNELS; 
		i++)
	{
		/* Get the channel info for the current channel */
		if ( GetChannelInfo(i, buf, 512) == -1 ) return(TRUE);

		/* Skip the first four fields - we don't use them for now. */
		bzero((char *)(buf + strlen(buf) - 1), 1);
		ptr = strtok(buf, "|"); 
		ptr = strtok(NULL, "|");
		ptr = strtok(NULL, "|");
		ptr = strtok(NULL, "|");
	
		/* Make sure this isn't a copy of the last one.  If it is, skip it.  */
		ptr += strlen(ptr) + 1;
		DBGPrintf(("Channel info:\n%s\n", ptr));
		DBGPrintf(("Lastbuf:\n%s\n", lastbuf));
		if ( strlen(lastbuf) > 0 ) 
			if ( strncmp(lastbuf, ptr, strlen(lastbuf)) == 0 ) continue;
		strcpy(lastbuf, ptr);
	
		/* Fill in the display with current channel info */
		if ( (channel  =strtok(NULL, "~")) == NULL ) continue;
		if ( (station  =strtok(NULL, "~")) == NULL ) continue;
		if ( (category =strtok(NULL, "~")) == NULL ) continue;
		if ( (artist   =strtok(NULL, "~")) == NULL ) continue;
		if ( (song     =strtok(NULL, "~")) == NULL ) continue;

		/* Find the entry in the list, if it exists. */
		row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Channel_Listing_Window), (gpointer)i);

		/* Is this channel already in our link list? */
		if ( row != -1 )
		{
			/* It is, so update its entry and update the display. */
			UpdateChannelEntry(row, station, song, artist, category);
		}
		else
		{
			if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			{
				sprintf((char *)&row_data[0], "%d: %s", i, station);
				sprintf((char *)&row_data[1], "%s", artist);
				sprintf((char *)&row_data[2], "%s", song);
				sprintf((char *)&row_data[3], "%s", category);
				sprintf((char *)&row_data[4], "%03d", i);
				sprintf((char *)&row_data[5], " ");
			}
			else
			{
				sprintf((char *)&row_data[0], "%d: Preview", i);
				sprintf((char *)&row_data[1], "XM Preview");
				sprintf((char *)&row_data[2], "");
				sprintf((char *)&row_data[3], "");
				sprintf((char *)&row_data[4], "%03d", i);
				sprintf((char *)&row_data[5], " ");
			}
			rows[0] = row_data[0];
			rows[1] = row_data[1];
			rows[2] = row_data[2];
			rows[3] = row_data[3];
			rows[4] = row_data[4];
			rows[5] = row_data[5];

			row = 
				gtk_clist_append(GTK_CLIST(XR_Channel_Listing_Window), rows);
			gtk_clist_set_row_data(GTK_CLIST(XR_Channel_Listing_Window),
				row, (gpointer)i);
		}
		PaintChannelListEntry( XR_Channel_Listing_Window, row, artist);

		/* Update the Category Tabs */
		UpdateCategoryTabs(i, station, song, artist, category);
		AddPrefsCategory(category);
	}
	gtk_clist_thaw(GTK_CLIST(XR_Channel_Listing_Window));

	/*
	 * This keeps track of what channel we should start on the next time
	 * were are called.
	 */
	channel_id += channel_refresh[prefs.performance];
	if (channel_id > XR_MAX_CHANNELS) { channel_id = 1; }

	/* Update the Current Channel after we've updated the list. */
	if ( firsttime )
	{
   	XRUGetChannelInfo(1);
		last_channel = current_channel;
		firsttime = 0;
	}
	else
	{
		XRUGetChannelInfo(0);
	}

	/* Do updates on favorite channels a little more often */
	favorite_count++;
	if ( favorite_count == XR_Favorite_Refresh_Count )
	{
		XRUChannelRefreshFavorites();
		favorite_count = 0;
	}

	/*
	 * This variable, if sets, prevents us from being called while we're
	 * already running. 
	 */
	running = 0;
	return(TRUE);
}
/*========================================================================
 *	Name:		XRUChannelRefreshFavorites
 *	Scope: 	Public
 *					
 *	Description:
 * Refreshes all windows by querying for the favorite channels.
 * This function runs as part of XRUChannelRefresh() or by immediate
 * request by the user.
 *========================================================================*/
static void
RefreshFavoriteChannel(
	char			*data,
	gpointer		userdata
)
{
	int		chnum;
	char		buf[1024];
	char		*ptr;
	char		*channel, *station, *artist, *song, *category;

	/* Don't do this if we aren't connected to the server. */
	if ( disconnected ) return;

	/* Retrieve the numeric channel number */
	chnum = atoi(data);

	/* Send the command to get the channel information, then pull the results. */
	if ( GetChannelInfo(chnum, chinfobuf, 1024) == -1 ) return;

	/* Skip the first four fields - we don't use them for now. */
	DBGPrintf(("Channel info:\n%s", chinfobuf));

	bzero((char *)(chinfobuf + strlen(chinfobuf) - 1), 1);
	ptr = strtok(chinfobuf, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");

	/*
	 * Save the entry temporarily - if its complete, we'll save it in a more
	 * permanent place in a moment.
	 */
	ptr += strlen(ptr) + 1;
	strcpy(buf, ptr);

	/* Grab the fields from the returned data */
	if ( (channel  =strtok(NULL, "~")) == NULL ) return;
	if ( (station  =strtok(NULL, "~")) == NULL ) return;
	if ( (category =strtok(NULL, "~")) == NULL ) return;
	if ( (artist   =strtok(NULL, "~")) == NULL ) return;
	if ( (song     =strtok(NULL, "~")) == NULL ) return;

	/* If we get here, we got a full entry, so save it. */
	strcpy(lastbuf, buf);
	UpdateChannelListings(chnum, station, song, artist, category);
}
void
XRUChannelRefreshFavorites()
{
	g_list_foreach(XR_List_Channel, 
		(gpointer)RefreshFavoriteChannel, (gpointer)NULL);
}

/*========================================================================
 *	Name:		UpdateChannelEntry
 *	Scope: 	Private
 *					
 *	Description:
 * Updates an entry in the Channel Listing window.
 *
 *	Input Arguments:
 *	int	row			index into arrays that hold channel and row numbers
 * char	*station		name of current station
 * char	*song			name of current song
 * char	*artist		name of current artist
 * char	*category	name of current category
 *	
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * GtkWidget	*XR_Channel_Listing_Window		The list of channels.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
static void
UpdateChannelEntry(
	int	row,
	char	*station,
	char	*song,
	char	*artist,
	char	*category
)
{
	char	buf[512];
	static int	running = 0;

	if ( running ) return;
	running = 1 ;

	UpdateClistEntry(XR_Channel_Listing_Window,
		row, station, song, artist, category);

	running = 0;
}

/*========================================================================
 *	Name:		AddClistEntry
 *	Scope: 	Private
 *					
 *	Description:
 * Adds a new entry to a CList.
 *
 *	Input Arguments:
 * GtkWidget	*Clist		Listing window
 * char			*station		name of current station
 * char			*song			name of current song
 * char			*artist		name of current artist
 * char			*category	name of current category
 *	
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * GtkWidget	*Clist		The list of channels.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
static void
AddClistEntry(
	GtkWidget	*Clist,
	char			*station,
	char			*song,
	char			*artist,
	char			*category,
	int			chnum
)
{
	int			row;

	if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
	{
		sprintf((char *)&row_data[0], "%d: %s", chnum, station);
		sprintf((char *)&row_data[1], "%s", artist);
		sprintf((char *)&row_data[2], "%s", song);
		sprintf((char *)&row_data[3], "%s", category);
		sprintf((char *)&row_data[4], "%03d", chnum);
		sprintf((char *)&row_data[5], " ");
	}
	else
	{
		sprintf((char *)&row_data[0], "%d: Preview", chnum);
		sprintf((char *)&row_data[1], "XM Preview");
		sprintf((char *)&row_data[2], "");
		sprintf((char *)&row_data[3], "");
		sprintf((char *)&row_data[4], "%03d", chnum);
		sprintf((char *)&row_data[5], " ");
	}
	rows[0] = row_data[0];
	rows[1] = row_data[1];
	rows[2] = row_data[2];
	rows[3] = row_data[3];
	rows[4] = row_data[4];
	rows[5] = row_data[5];

	row = gtk_clist_append(GTK_CLIST(Clist), rows);
	gtk_clist_set_row_data(GTK_CLIST(Clist), row, (gpointer)chnum);
}

/*========================================================================
 *	Name:		UpdateClistEntry
 *	Scope: 	Private
 *					
 *	Description:
 * Updates an entry in the Clist Listing window.
 *
 *	Input Arguments:
 *	int	row			index into arrays that hold Clist and row numbers
 * char	*station		name of current station
 * char	*song			name of current song
 * char	*artist		name of current artist
 * char	*category	name of current category
 *	
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * GtkWidget	*Clist		The list of channels.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
static void
UpdateClistEntry(
	GtkWidget	*clist,
	int			row,
	char			*station,
	char			*song,
	char			*artist,
	char			*category
)
{
	char			buf[512];
	static int	running = 0;

	if ( running ) return;
	running = 1 ;

	sprintf(buf, "%d: %s", 
		(int)gtk_clist_get_row_data(GTK_CLIST(clist), row), station);
	DBGPrintf(("UpdateSessionInfo(row/Ch: station): %d/%s\n", row, buf));

	gtk_clist_set_text(GTK_CLIST(clist), row, 0, (char *)buf);
	gtk_clist_set_text(GTK_CLIST(clist), row, 2, (char *)song);

	if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
	{
		gtk_clist_set_text(GTK_CLIST(clist), row, 1, (char *)artist);
		gtk_clist_set_text(GTK_CLIST(clist), row, 3, (char *)category);
	}
	else
	{
		gtk_clist_set_text(GTK_CLIST(clist), row, 1, "");
		gtk_clist_set_text(GTK_CLIST(clist), row, 3, "");
	}

	running = 0;
}

/*========================================================================
 *	Name:		FavoriteItem
 *	Scope: 	Private
 *					
 *	Description:
 * This function compares two strings to see if they are equivalent.
 * It is used to search GList's to find an element with a matching string.
 *
 * Note: ptr2 points to the string we're looking for.
 * ptr1 points to a string taken from the current GList entry.
 *========================================================================*/
static int 
FavoriteItem (
	char	*ptr1,
	char	*ptr2
)
{
	if ( strlen(ptr2) >= strlen(ptr1) )
		if ( strncmp(ptr2, (char *)ptr1, strlen(ptr1)) == 0 )
			return(0);
	return(1);
}

static void
PaintChannelListEntry(
	GtkWidget	*clist,
	int			row,
	char			*artist
)
{
	GList		*found;
	char		buf[256];

	found = g_list_find_custom(XR_List_Artist, artist, (gpointer)FavoriteItem);
	if ( found != NULL ) 
	{
		gtk_clist_set_background(GTK_CLIST(clist), row, &gdk_favorite_color);
		favorite_channel = (int)gtk_clist_get_row_data(GTK_CLIST(clist), row);

		if (favorite_channel == current_channel) return;
		if ( ! prefs.enable_favorites ) return;

		if ( XR_Favorite_Channel_Window == NULL )
		{
			XR_Favorite_Channel_Window = create_favorites();
			gtk_widget_realize(XR_Favorite_Channel_Window);
		}

		/* Don't update the window is its already visible */
		if ( ! GTK_WIDGET_VISIBLE(XR_Favorite_Channel_Window) ) 
		{
			sprintf(buf, "%s\nis now plaing on\nchannel %d", 
				artist, favorite_channel);
			gtk_label_set_text(GTK_LABEL(XR_Favorite_Channel_Label), buf);
			gtk_widget_show(XR_Favorite_Channel_Window);
		}
	}
	else
	{
		gtk_clist_set_background(GTK_CLIST(clist), row, &gdk_default_color);
	}
}



/*========================================================================
 *	Name:		XRUChannelListingSelected
 *	Scope: 	Public
 *					
 *	Description:
 * This function takes the row selected from our Channel Listing and
 * requests the server to jump to the specified station.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int		Sock_FD			The socket used for connections to OpenXM server.
 * char		*cmdbuf			buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUChannelListingSelected (
   GtkWidget      *widget,
   gint           row,
   gint           column,
   GdkEventButton *event,
   gpointer       data
)
{
	char				buf[32];
	int				channel_id;

	if (disconnected) return;

	channel_id = (int)gtk_clist_get_row_data(GTK_CLIST(widget), row);

	if ( channel_id )
	{
		sprintf(buf, "%s%03d\n", XR_CMD_CHA_SET, channel_id);
		send(XR_SockFD, buf, strlen(buf), 0);
		ReadFromSocket(cmdbuf, 512);
		if ( strncmp(cmdbuf, "1", 1)  != 0 )
			XRUMsg("XMPCR server could not change to that station.");
		else
		{
			last_channel = current_channel;
			XRUGetChannelInfo(1);
		}
	}
}

/*========================================================================
 *	Name:		XRUJumpToOpen
 *	Scope: 	Public
 *					
 *	Description:
 * Opens the JumpTo dialog.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUJumpToOpen()
{
	if ( XR_JumpTo_Window == NULL )
		XR_JumpTo_Window = create_dialog3();
	gtk_widget_show(XR_JumpTo_Window);
}

/*========================================================================
 *	Name:		XRUJumpToClose
 *	Scope: 	Public
 *					
 *	Description:
 * Closes the JumpTo dialog.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUJumpToClose()
{
	if ( XR_JumpTo_Window )
		gtk_widget_hide(XR_JumpTo_Window);
}

/*========================================================================
 *	Name:		XRUJumpToStationID
 *	Scope: 	Public
 *					
 *	Description:
 * Jump to the specified station retrieved from the JumpTo dialog.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUJumpToStationID()
{
	char	*channel;
	int	channel_id;
	char	buf[32];
	int	row;

	if ( XR_JumpTo_StationID )
	{
		channel = 
			gtk_editable_get_chars(GTK_EDITABLE(XR_JumpTo_StationID), 0, -1);
		channel_id = atoi(channel);
		free(channel);
		bzero(buf, 32);
		sprintf(buf, "%s%03d\n", XR_CMD_CHA_SET, channel_id);
		send(XR_SockFD, buf, strlen(buf), 0);
		ReadFromSocket(cmdbuf, 512);
		if ( strncmp(cmdbuf, "1", 1)  != 0 )
			XRUMsg("XMPCR server could not change to previous station.");
		else
		{
			last_channel = current_channel;
			XRUGetChannelInfo(1);
			row = gtk_clist_find_row_from_data(
				GTK_CLIST(XR_Channel_Listing_Window), (gpointer)current_channel);
			if ( row != -1 )
			{
				gtk_signal_handler_block (
					GTK_OBJECT (XR_Channel_Listing_Window), 
						XR_Clist_Sig_ID);

				gtk_clist_select_row( GTK_CLIST(XR_Channel_Listing_Window), row, 0);

				gtk_signal_handler_unblock (
					GTK_OBJECT (XR_Channel_Listing_Window), 
						XR_Clist_Sig_ID);
			}
		}
	}
	else
	{
		XRUMsg("Error: Can't get station id from dialog!");
	}
	XRUJumpToClose();
}

/*========================================================================
 *	Name:		XRUJumpToFavoriteChannel
 *	Scope: 	Public
 *					
 *	Description:
 * Jumps to the current favorite channel based which is set when a favorite
 * artist is recognized during a channel update or add to the main listing
 * window.
 *
 *========================================================================*/
void 
XRUJumpToFavoriteChannel()
{
	char	buf[32];
	int	row;

	if ( XR_Favorite_Channel_Window == NULL )
		return;

	bzero(buf, 32);
	sprintf(buf, "%s%03d\n", XR_CMD_CHA_SET, favorite_channel);
	send(XR_SockFD, buf, strlen(buf), 0);
	ReadFromSocket(cmdbuf, 512);
	if ( strncmp(cmdbuf, "1", 1)  != 0 )
		XRUMsg("XMPCR server could not change to previous station.");
	else
	{
		last_channel = current_channel;
		XRUGetChannelInfo(1);
		row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Channel_Listing_Window), (gpointer)current_channel);
		if ( row != -1 )
		{
			gtk_signal_handler_block (
				GTK_OBJECT (XR_Channel_Listing_Window), 
					XR_Clist_Sig_ID);

			gtk_clist_select_row( GTK_CLIST(XR_Channel_Listing_Window), row, 0);

			gtk_signal_handler_unblock (
				GTK_OBJECT (XR_Channel_Listing_Window), 
					XR_Clist_Sig_ID);
		}
	}
	gtk_widget_hide(XR_Favorite_Channel_Window);
}

/*========================================================================
 *	Name:		XRUJumpToStationName
 *	Scope: 	Public
 *					
 *	Description:
 * Closes the JumpTo dialog.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUJumpToStationName()
{
}

/*========================================================================
 *	Name:		XRUShowRadioID
 *	Scope: 	Public
 *					
 *	Description:
 * Closes the JumpTo dialog.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUShowRadioID()
{
	char	buf[256];
	if ( XR_Radio_ID != NULL )
		sprintf(buf, "Your XM Radio ID: %s", XR_Radio_ID);
	else
		sprintf(buf, "You're not connected to an XM Radio.");
	XRUMsg(buf);
}

/*========================================================================
 *	Name:		XRUShowRadioID
 *	Scope: 	Public
 *					
 *	Description:
 * Closes the JumpTo dialog.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUJumpKeyPress(
	GdkEventKey	*event
)
{
	switch (event->keyval)
   {
      case GDK_Return:
			XRUJumpToStationID();
         break;
      default:
         break;
   }

}

/*========================================================================
 *	Name:		XRUAddFavoriteArtist
 *	Scope: 	Public
 *					
 *	Description:
 * Add the current artist to the favorites list.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUAddFavoriteArtist()
{
	char	buf[1024];
	char	*ptr;
	char	*channel, *station;
	char	*artist, *song;
	char	*category;
	int	chnum;
	int	row;
	char	*linkptr;

	/* Get the current channel info */
	if ( GetChannelInfo(0, buf, 1024) == -1 ) return;

	bzero((char *)(buf + strlen(buf) - 1), 1);
	ptr = strtok(buf, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");

	/* Grab the fields from the returned data */
	if ( (channel  =strtok(NULL, "~")) == NULL ) return;
	if ( (station  =strtok(NULL, "~")) == NULL ) return;
	if ( (category =strtok(NULL, "~")) == NULL ) return;
	if ( (artist   =strtok(NULL, "~")) == NULL ) return;
	if ( (song     =strtok(NULL, "~")) == NULL ) return;

	/* Add it to the clist */
	chnum = atoi(channel);
	row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Favorites_Artist_Listing_Window), (gpointer)chnum);
	if ( row == -1 )
	{
		/* 
		 * Add an entry to the link list of favorite artists and save the
		 * link id.
		 */
		linkptr = (char *)malloc(strlen(artist) + 1);
		sprintf(linkptr, artist);
		XR_List_Artist = g_list_append(XR_List_Artist, linkptr);

		XRUSaveFavorites();

		/* Then add the new entry to the CList. */
		AddClistEntry(
			XR_Favorites_Artist_Listing_Window,
			station, song, artist, category, (int)linkptr);
	}
}


void 
XRUAddFavoriteStation()
{
	char	buf[1024];
	char	*ptr;
	char	*channel, *station;
	char	*artist, *song;
	char	*category;
	int	chnum;
	int	row;
	char	*linkptr;

	/* Get the current channel info */
	if ( GetChannelInfo(0, buf, 1024) == -1 ) return;

	bzero((char *)(buf + strlen(buf) - 1), 1);
	ptr = strtok(buf, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");
	ptr = strtok(NULL, "|");

	/* Grab the fields from the returned data */
	if ( (channel  =strtok(NULL, "~")) == NULL ) return;
	if ( (station  =strtok(NULL, "~")) == NULL ) return;
	if ( (category =strtok(NULL, "~")) == NULL ) return;
	if ( (artist   =strtok(NULL, "~")) == NULL ) return;
	if ( (song     =strtok(NULL, "~")) == NULL ) return;

	/* Add it to the clist */
	chnum = atoi(channel);
	row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Favorites_Listing_Window), (gpointer)chnum);
	if ( row == -1 )
	{
		/* 
		 * Add an entry to the link list of favorite artists and save the
		 * link id.
		 */
		linkptr = (char *)malloc(strlen(channel) + 1);
		sprintf(linkptr, "%s: %s", channel, station);
		XR_List_Channel = g_list_append(XR_List_Channel, linkptr);

		XRUSaveFavorites();

		/* Then add the new entry to the CList. */
		AddClistEntry(
			XR_Favorites_Listing_Window,
			station, song, artist, category, chnum);
	}
}

/*========================================================================
 *	Name:		XRUChannelListingSelected
 *	Scope: 	Public
 *					
 *	Description:
 * This function takes the row selected from our Channel Listing and
 * requests the server to jump to the specified station.
 *
 *	Input Arguments:
 *	Output Arguments:
 *	Return Values:
 *	Global Variables:
 * int		Sock_FD			The socket used for connections to OpenXM server.
 * char		*cmdbuf			buffer used for all outbound commands.
 *
 *	External Routines:
 *	Method:
 *	Restrictions:
 *	Notes:
 *========================================================================*/
void 
XRUFavoritesListingSelected (
   GtkWidget      *widget,
   gint           row,
   gint           column,
   GdkEventButton *event,
   gpointer       data
)
{
	char				buf[32];
	int				channel_id;

	channel_id = 
		(int)gtk_clist_get_row_data(GTK_CLIST(XR_Favorites_Listing_Window), row);
	printf("Channel selected: %d\n", channel_id);

	if ( channel_id )
	{
		sprintf(buf, "%s%03d\n", XR_CMD_CHA_SET, channel_id);
		send(XR_SockFD, buf, strlen(buf), 0);
		ReadFromSocket(cmdbuf, 512);
		if ( strncmp(cmdbuf, "1", 1)  != 0 )
			XRUMsg("XMPCR server could not change to that station.");
		else
		{
			last_channel = current_channel;
			XRUGetChannelInfo(1);
		}
	}
}

/*========================================================================
 *	Name:		XRUSaveFavorites, ReadFavorites, SaveArtist, SaveChannels
 *	Scope: 	Private
 *					
 *	Description:
 * Manage the Favorites file for channels and artists.
 *========================================================================*/
void
SaveArtist(
	char			*data,
	gpointer		fd
)
{
	fprintf(fd, "artist: %s\n", data);
}
void
SaveChannel(
	char			*data,
	gpointer		fd
)
{
	fprintf(fd, "channel: %s\n", data);
}

void 
XRUSaveFavorites (
	int	type
)
{
	FILE			*fd;
	static char	*home;
	static int	firsttime = 1;
	static char	buf[1024];

	if ( firsttime )
	{
		home = getenv("HOME");
		firsttime = 0;
		sprintf(buf, "%s/%s", home, FAVORITESFILE);
	}

	if ( (fd = fopen(buf, "w")) == NULL )
	{
		printf("Can't open for writing %s\n", buf);
		return;
	}
	g_list_foreach(XR_List_Artist, (gpointer)SaveArtist, (gpointer)fd);
	g_list_foreach(XR_List_Channel, (gpointer)SaveChannel, (gpointer)fd);
	fclose(fd);
}

void 
XRUReadFavorites ()
{
	FILE	*fd;
	char	*type;
	char	*value;
	char	*linkptr;
	char	*station;
	int	chnum;

	static char	*home;
	static int	firsttime = 1;
	static char	buf[1024];

	if ( firsttime )
	{
		home = getenv("HOME");
		firsttime = 0;
		sprintf(buf, "%s/%s", home, FAVORITESFILE);
	}

	if ( (fd = fopen(buf, "r")) == NULL )
	{
		printf("Can't open for reading %s\n", buf);
		return;
	}

	while ( fgets(buf, 1024, fd) )
	{
		type = strtok(buf, ":");
		value = (char *)(buf+strlen(type)+1);
		while ( *type == ' ' ) type++;
		while ( *value == ' ' ) value++;
		bzero((char *)(value+strlen(value)-1), 1);
		linkptr = (char *)malloc(strlen(value) + 1);

		sprintf(linkptr, value);
		if ( strncasecmp(type, "Artist", strlen("Artist")) == 0 )
		{
			XR_List_Artist = g_list_append(XR_List_Artist, linkptr);
			AddClistEntry(
				XR_Favorites_Artist_Listing_Window,
				"", "", value, "", (int)linkptr);
		}
		else
		{
			chnum = atoi(strtok(value, ":"));
			station = strtok(NULL, ":");
			XR_List_Channel = g_list_append(XR_List_Channel, linkptr);
			AddClistEntry(
				XR_Favorites_Listing_Window,
				station, "", "", "", chnum);
		}
	}

	fclose(fd);
}

/*========================================================================
 *	Name:		UpdateCategoryTabs
 *	Scope: 	Private
 *					
 *	Description:
 * Update the Category tabs with the just retrieved channel data.
 *========================================================================*/
static int
FindCategory(
	CategoryT	*category_data,
	char			*src_category
)
{
	if ( strcmp(category_data->name, src_category) == 0 )
		return(0);
	return(1);
}

static void
UpdateCategoryTabs(
	int	chnum,
	char	*station,
	char	*song,
	char	*artist,
	char	*category
)
{
	CategoryT	*linkptr;
	int			row;
	GtkWidget	*clist;
	GList			*listptr;
	int			index;
	GtkWidget	*scrolledwindow;
	GtkWidget	*label;
	int			pagenum;
	int			position;
	CatEntryT	*catentry;
	int			state;

	int	clist_widths[6] = {115, 160, 150, 60, 80, 80};
	char	*clist_labels[] = {
				"Channel",
				"Artist",
				"Song",
				"Category",
				"Ch #",
				"Usage",
				NULL
				};

	/* First, find out if this category's display state has been set */
	listptr = g_list_find_custom(prefs.categories, category, 
					(gpointer)FindPrefsCategory);
	if ( listptr != NULL )
	{
		position = g_list_position(prefs.categories, listptr);
		catentry = (CatEntryT *)g_list_nth_data(prefs.categories, position);

		/* Don't add this tab if its been disabled by the user */
		if ( catentry->state == 0 )
			return;
	}

	/* Find a matching entry, if any, for the category. */
	listptr = g_list_find_custom(XR_List_Categories, category, 
					(gpointer)FindCategory);

	if ( listptr == NULL )
	{
		/* Add a category tab - a scrolled clist widget */
		scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
		gtk_widget_set_name (scrolledwindow, "scrolledwindow");
		gtk_widget_show (scrolledwindow);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

		label = gtk_label_new (category);
		gtk_widget_show (label);
		gtk_notebook_append_page(
			GTK_NOTEBOOK (XR_Channel_Listing_Notebook_Window), 
			scrolledwindow,
			label);
		pagenum = gtk_notebook_page_num(
						GTK_NOTEBOOK (XR_Channel_Listing_Notebook_Window), 
						scrolledwindow);

  		clist = gtk_clist_new (6);
  		gtk_widget_show (clist);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), clist);
		gtk_widget_set_size_request (clist, 525, 100);
		for(index=0;index<6;index++)
			gtk_clist_set_column_width (GTK_CLIST (clist), index,
				clist_widths[index]);
		gtk_clist_column_titles_show (GTK_CLIST (clist));
		gtk_clist_set_sort_column(GTK_CLIST(clist), 4);
		gtk_clist_set_auto_sort(GTK_CLIST(clist), TRUE);
		gtk_clist_set_column_visibility(GTK_CLIST(clist), 4, FALSE);
		gtk_clist_set_column_visibility(GTK_CLIST(clist), 5, FALSE);
		g_signal_connect ((gpointer) clist, "select_row",
			G_CALLBACK (on_clist1_select_row),
			NULL);

		index=0;
		while(clist_labels[index] != NULL)
		{
			label = gtk_label_new (clist_labels[index]);
			gtk_widget_show (label);
			gtk_clist_set_column_widget (GTK_CLIST (clist), index, label);
			gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
			index++;
		}

		/* Allocate and fill the structure for Category tabs */
		linkptr = (CategoryT *)malloc(sizeof(CategoryT));
		linkptr->name = (char *)malloc(strlen(category)+1);
		sprintf(linkptr->name, "%s", category);
		linkptr->clist = clist;
		linkptr->pagenum = pagenum;

		/* Add link to categories list */
		XR_List_Categories = g_list_append(XR_List_Categories, linkptr);

		/* Add an entry for this channel */
		AddClistEntry(clist, station, song, artist, category, chnum);

		/* Now find that new entry. */
		listptr = g_list_find_custom(XR_List_Categories, category, 
					(gpointer)FindCategory);
	}

	/* Get the Category structure data */
	index = g_list_position(XR_List_Categories, listptr);
	linkptr = (CategoryT *)g_list_nth_data(XR_List_Categories, index);

	/* Find out if we need to hide this page */
	
	/* See if this channel is in the exist category list. */
	row = gtk_clist_find_row_from_data(GTK_CLIST(linkptr->clist), 
				(gpointer)chnum);
	if ( row != -1 )
		UpdateClistEntry(linkptr->clist, row, station, song, artist, category);
	else
		AddClistEntry( linkptr->clist, station, song, artist, category, chnum);
}

/*========================================================================
 *	Name:		XRUSavePreferences
 *	Scope: 	Public
 *					
 *	Description:
 * Retrieve values from the Preferences dialog, update the runtime system
 * and save to file.
 *========================================================================*/
void
XRUSavePreferences()
{
	gboolean		channel_windows;
	int			performance;

	/* This one tells us where the XMDaemon will be running */
	if (prefs.hostname) free(prefs.hostname);
	prefs.hostname = 
		gtk_editable_get_chars(GTK_EDITABLE(XR_Preference_Host), 0, -1);

	/*
	 * This one tells use where to find the XMDaemon program, in case we
	 * want to start it locally
	 */
	if (prefs.daemondir) free(prefs.daemondir);
	prefs.daemondir = 
		gtk_editable_get_chars(GTK_EDITABLE(XR_Preference_Location),0,-1);

	/*
	 * This one sets whether or not we show the channel listing windows.
	 */
	prefs.channel_windows = (gboolean)
		gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(XR_Preference_Channel_Listing));
	if ( XR_Channel_Listing_Notebook_Window )
	{
		if ( prefs.channel_windows )
		{
			gtk_widget_show( XR_Channel_Listing_Notebook_Window );
			XR_Channel_Listing_State = 1;
		}
		else
		{
			gtk_widget_hide( XR_Channel_Listing_Notebook_Window );
			XR_Channel_Listing_State = 0;
		}
	}

	/*
	 * This one determines if we want to be notified when a favorite artist
	 * is playing.
	 */
	prefs.enable_favorites = (gboolean)
		gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(XR_Preference_Enable_Favorites));

	/*
	 * This one determines how often and how much we query for new 
	 * station information.
	 */
	prefs.performance = 
		gtk_spin_button_get_value_as_int(
			GTK_SPIN_BUTTON(XR_Preference_Performance));

	/* Reset the refresh timer. */
	gtk_timeout_remove ( channel_list_timer );
	channel_list_timer = gtk_timeout_add (channel_time[prefs.performance]*1000,
      (GtkFunction)XRUChannelRefresh, 0);

	/* Save to file */
	XRUSavePrefs();
}


/*========================================================================
 *	Name:		XRUSavePrefs
 *	Scope: 	Public
 *					
 *	Description:
 * Save the preferences to file.
 *========================================================================*/
void
XRUSavePrefs()
{
	FILE			*fd;
	static char	*home;
	static int	firsttime = 1;
	static char	buf[1024];
	GList			*catlistitem;
	int			position;

	/* First time - build output file name */
	if ( firsttime )
	{
		home = getenv("HOME");
		firsttime = 0;
		sprintf(buf, "%s/%s", home, PREFSFILE);
	}

	/* Open the output file */
	if ( (fd = fopen(buf, "w")) == NULL )
	{
		printf("Can't open for writing %s\n", buf);
		return;
	}

	/* Write the preferences to it. */
	fprintf(fd, "hostname:%s\n", prefs.hostname);
	if ( prefs.daemondir )
		fprintf(fd, "daemondir:%s\n", prefs.daemondir);
	else
		fprintf(fd, "daemondir:\n");
	fprintf(fd, "favorites:%d\n", (int)prefs.enable_favorites);
	fprintf(fd, "channels:%d\n", (int)prefs.channel_windows);
	fprintf(fd, "performance:%d\n", prefs.performance);

	/* Run the list of categories and save them and their states */
	g_list_foreach(prefs.categories, SavePrefsCategory, fd);

	/* Close the output file. */
	fclose(fd);
}

static void
SavePrefsCategory(
	CatEntryT	*catentry,
	FILE			*fd
)
{
	fprintf(fd, "category:%s:%d\n", catentry->name, catentry->state);
}

/*========================================================================
 *	Name:		XRUReadPrefs
 *	Scope: 	Public
 *					
 *	Description:
 * Read the preferences to file.  Only call this once, at init time.
 *========================================================================*/
void
XRUReadPrefs()
{
	FILE			*fd;
	struct stat	stat_buf;
	char			buf[1024];
	char			*name;
	char			*value;
	char			*state;
	CatEntryT	*catentry;
	static char	*home;
	static char	filename[1024];

	/* Init filename */
	home = getenv("HOME");
	sprintf(filename, "%s/%s", home, PREFSFILE);
	bzero(buf, 1024);

	/* Init preferences to null or empty values. */
	prefs.hostname = (char *)malloc(strlen(SERV_HOST_ADDR)+1);
	strcpy(prefs.hostname, SERV_HOST_ADDR);
	prefs.daemondir = NULL;
	prefs.enable_favorites = 1;
	prefs.channel_windows = 0;
	prefs.performance = 5;
	prefs.categories = NULL;

	/* Don't do anything if the file isn't there! */
	if ( stat(filename, &stat_buf) != 0 ) return;

	/* Open the input file */
	if ( (fd = fopen(filename, "r")) == NULL )
	{
		printf("Can't open for reading %s\n", filename);
		return;
	}

	/* Write the preferences to it. */
	while(fgets(buf, 1023, fd) != NULL)
	{
		/* Remove newline. */
		bzero((char *)(buf+strlen(buf)-1), 1);

		/* Grab the name/value pair for the current line */
		name = strtok(buf, ":");
		value = strtok(NULL, ":");

		/* Update preferences structure based on name */
		if ( strcasecmp(name, "hostname") == 0 )
		{
			if (prefs.hostname) free(prefs.hostname);
			prefs.hostname = (char *)malloc(strlen(value)+1);
			strcpy(prefs.hostname, value);
		}
		if ( strcasecmp(name, "daemondir") == 0 )
		{
			if (prefs.daemondir) free(prefs.daemondir);
			if ( value )
			{
				prefs.daemondir = (char *)malloc(strlen(value)+1);
				strcpy(prefs.daemondir, value);
			}
			else
				prefs.daemondir = NULL;
		}
		if ( strcasecmp(name, "favorites") == 0 )
			prefs.enable_favorites = atoi(value);
		if ( strcasecmp(name, "channels") == 0 )
			prefs.channel_windows = atoi(value);
		if ( strcasecmp(name, "performance") == 0 )
			prefs.performance = atoi(value);

		if ( strcasecmp(name, "category") == 0 )
		{
			/* Categories have three fields. */
			state = strtok(NULL, ":");

			/* Save new entry to categories GList with value and state */
			catentry = (CatEntryT *)malloc(sizeof(CatEntryT));
			catentry->name = (char *)malloc(strlen(value)+1);
			strcpy(catentry->name, value);
			catentry->state = atoi(state);
			prefs.categories = 
				g_list_append(prefs.categories, (gpointer)catentry);
		}
	}

	/* Close the output file. */
	fclose(fd);
}

/*========================================================================
 *	Name:		FindPrefsCategory, AddPrefsCategory
 *	Scope: 	Private
 *					
 *	Description:
 * AddPrefsCategory will add an entry to the list of categories in the
 * Preferences dialog where the user can determine if that category should be
 * displayed or not.
 * FindPrefsCategory is a utility function that searchs a GList for an
 * existing category name.  It is called only by AddPrefsCategory via glib.
 *========================================================================*/
static int
FindPrefsCategory(
	CatEntryT	*category_data,
	char			*src_category
)
{
	if ( strcmp(category_data->name, src_category) == 0 )
		return(0);
	return(1);
}
static void
AddPrefsCategory(
	char	*category
)
{
	GList			*listptr;
	GList			*catlistitem;
	CatEntryT	*catentry;
	char			row_data[2][512];
	char 			*rows[2];
	int			row, position;

	listptr = g_list_find_custom(prefs.categories, category, 
					(gpointer)FindPrefsCategory);
	if ( listptr == NULL )
	{
		printf("Adding %s to prefs categories\n", category);

		/* Save new entry to categories GList with value and state */
		catentry = (CatEntryT *)malloc(sizeof(CatEntryT));
		catentry->name = (char *)malloc(strlen(category)+1);
		strcpy(catentry->name, category);
		catentry->state = 1;
		prefs.categories = g_list_append(prefs.categories, (gpointer)catentry);
		catlistitem = g_list_find(prefs.categories, (gpointer)catentry);
		position = g_list_position(prefs.categories, catlistitem);

		/* Update the category list in the preferences dialog */
		if ( XR_Preference_Clist != NULL )
		{
			sprintf((char *)&row_data[0], "%s", catentry->name);
			sprintf((char *)&row_data[1], "Yes");
			rows[0] = row_data[0];
			rows[1] = row_data[1];
			row = gtk_clist_append(GTK_CLIST(XR_Preference_Clist), rows);
			gtk_clist_set_row_data(GTK_CLIST(XR_Preference_Clist), row,
				(gpointer)position);
		}
	}
}

/*========================================================================
 *	Name:		XRUShowCategoryState
 *	Scope: 	Public
 *					
 *	Description:
 * Pops up the category state selection window that allows the user to 
 * decide if the specified category should have its tab displayed or not.
 *========================================================================*/
void
XRUShowCategoryState(
	int	row
)
{
	static int	firsttime = 1;

	if ( firsttime ) 
	{
		category_name = (char *)malloc(256);
		firsttime = 0;
	}
	if ( XR_Category_Window == NULL )
	{
		XR_Category_Window = create_categories();
		gtk_widget_show(XR_Category_Window);
	}
	gtk_clist_get_text(GTK_CLIST(XR_Preference_Clist), row, 0, &category_name);
	gtk_label_set_text(GTK_LABEL(XR_Category_State_Label), category_name);
	gtk_widget_show(XR_Category_Window);
	category_row = row;
}

/*========================================================================
 *	Name:		XRUHideCategory
 *	Scope: 	Public
 *					
 *	Description:
 * Removes the Category tab if it exists and sets its state to off.
 *========================================================================*/
void
XRUHideCategory(
	int	row
)
{
	CategoryT	*linkptr;
	GList			*listptr;
	int			position;
	CatEntryT	*catentry;

	/* Find this entry in our preferences list and mark it as "off" */
	listptr = g_list_find_custom(prefs.categories, category_name, 
					(gpointer)FindPrefsCategory);
	position = g_list_position(prefs.categories, listptr);
	catentry = (CatEntryT *)g_list_nth_data(prefs.categories, position);
	catentry->state = 0;
	gtk_clist_set_text(GTK_CLIST(XR_Preference_Clist), category_row, 1, "No");

	/* If a page tab exists for this entry, delete it. */
	listptr = g_list_find_custom(XR_List_Categories, category_name, 
					(gpointer)FindCategory);
	if ( listptr != NULL )
	{
		position = g_list_position(XR_List_Categories, listptr);
		linkptr = (CategoryT *)g_list_nth_data(XR_List_Categories, position);
		gtk_notebook_remove_page(
			GTK_NOTEBOOK (XR_Channel_Listing_Notebook_Window), 
			linkptr->pagenum);
		free(linkptr->name);
		free(linkptr);
		XR_List_Categories = g_list_delete_link(XR_List_Categories, listptr);
	}
}

/*========================================================================
 *	Name:		XRUShowCategory
 *	Scope: 	Public
 *					
 *	Description:
 * Allow the Category tab to be displayed from now on.
 *========================================================================*/
void
XRUShowCategory(
	int	row
)
{
	CategoryT	*linkptr;
	GList			*listptr;
	int			position;
	CatEntryT	*catentry;

	/* Find this entry in our preferences list and mark it as "off" */
	listptr = g_list_find_custom(prefs.categories, category_name, 
					(gpointer)FindPrefsCategory);
	position = g_list_position(prefs.categories, listptr);
	catentry = (CatEntryT *)g_list_nth_data(prefs.categories, position);
	catentry->state = 1;
	gtk_clist_set_text(GTK_CLIST(XR_Preference_Clist), category_row, 1, "Yes");
}
