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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "interface.h"
#include "xr.h"

/* Locally scoped functions */
static int ReadFromSocket();
static int GetRadioID();
static int GetChannelInfo();
static void AddClistEntry();
static void UpdateClistEntry();
static void UpdateChannelEntry();

/* Locally scoped variables */
static char	cmdbuf[512];
static char	chinfobuf[1024];
static int	channel_info_timer;
static int	channel_list_timer;
static int	disconnected = 1;
static int	last_channel = -1;
static int	current_channel = -1;
static char	lastbuf[512];

static char	row_data[6][512];
static char *rows[6];


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
	bzero((char *)(cmdbuf + strlen(cmdbuf) - 2), 1);
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
	gtk_entry_set_text(GTK_ENTRY(XR_Host_IP_Entry), "");

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
	channel_list_timer = gtk_timeout_add (6*1000,
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
	static int firsttime = 1;

	/* Don't do this if we aren't connected to the server. */
	if ( disconnected ) return(FALSE);

	/* Send the command to get the channel information, then pull the results. */
	if ( GetChannelInfo(0, chinfobuf, 1024) == -1 ) return(TRUE);

	/* Skip the first four fields - we don't use them for now. */
	DBGPrintf(("Channel info:\n%s", chinfobuf));

	bzero((char *)(chinfobuf + strlen(chinfobuf) - 2), 1);
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
	if ( (channel  =strtok(NULL, "~")) == NULL ) return(TRUE);
	if ( (station  =strtok(NULL, "~")) == NULL ) return(TRUE);
	if ( (category =strtok(NULL, "~")) == NULL ) return(TRUE);
	if ( (artist   =strtok(NULL, "~")) == NULL ) return(TRUE);
	if ( (song     =strtok(NULL, "~")) == NULL ) return(TRUE);

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
		gtk_entry_set_text(GTK_ENTRY(XR_Station_Entry), "");

	if ( artist )
		if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			gtk_entry_set_text(GTK_ENTRY(XR_Artist_Entry), artist);
		else
			gtk_entry_set_text(GTK_ENTRY(XR_Artist_Entry), "XM Preview");
	if ( song )
		if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			gtk_entry_set_text(GTK_ENTRY(XR_Song_Entry), song);
		else
			gtk_entry_set_text(GTK_ENTRY(XR_Song_Entry), "");
	if ( category )
		if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
			gtk_entry_set_text(GTK_ENTRY(XR_Category_Entry), category);
		else
			gtk_entry_set_text(GTK_ENTRY(XR_Category_Entry), "");

	/* If requested, save the current channel */
	if ( ( save_channel ) && ( channel ) )
		current_channel = atoi(channel);

	/* Update the channel listing window if this channel already exists there. */
	row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Channel_Listing_Window), (gpointer)current_channel);
	if ( row != -1 )
		UpdateChannelEntry(row, station, song, artist, category);

	/* Update the session listing window. */
	row = gtk_clist_find_row_from_data(
			GTK_CLIST(XR_Session_Listing_Window), (gpointer)current_channel);
	if ( row != -1 )
		UpdateClistEntry(XR_Session_Listing_Window,
			row, station, song, artist, category);
	else
		AddClistEntry(
			XR_Session_Listing_Window,
			station, song, artist, category, current_channel);

	/*
	 * All functions that are called by timers need to return this to keep
	 * running (or FALSE to not be run again).
	 */
	return(TRUE);
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

	/* Reset most things if we disconnect. */
	if ( disconnected ) {
		bzero(lastbuf, 512);
		channel_id = 1;
		return(FALSE);
	}

	if ( running ) return(TRUE);

	gtk_clist_freeze(GTK_CLIST(XR_Channel_Listing_Window));
	running = 1;
	for (i=channel_id; i<channel_id+XR_REFRESH_BLOCK && i<=XR_MAX_CHANNELS; i++)
	{
		/* Get the channel info for the current channel */
		if ( GetChannelInfo(i, buf, 512) == -1 ) return(TRUE);

		/* Skip the first four fields - we don't use them for now. */
		bzero((char *)(buf + strlen(buf) - 2), 1);
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

		/* Find the entry if the list, if it exists. */
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
	}
	gtk_clist_thaw(GTK_CLIST(XR_Channel_Listing_Window));

	/*
	 * This keeps track of what channel we should start on the next time
	 * were are called.
	 */
	channel_id += XR_REFRESH_BLOCK;
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

	/*
	 * This variable, if sets, prevents us from being called while we're
	 * already running. 
	 */
	running = 0;
	return(TRUE);
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

#if 0
	sprintf(buf, "%d: %s", 
		(int)gtk_clist_get_row_data(GTK_CLIST(XR_Channel_Listing_Window), row), 
		station);
	DBGPrintf(("UpdateChInfo(row/Ch: station): %d/%s\n", row, buf));

	gtk_clist_set_text(GTK_CLIST(XR_Channel_Listing_Window), 
		row, 0, (char *)buf);
	gtk_clist_set_text(GTK_CLIST(XR_Channel_Listing_Window), 
		row, 2, (char *)song);

	if (strncmp(station, "XM Prev", strlen("XM Prev")) != 0)
	{
		gtk_clist_set_text(GTK_CLIST(XR_Channel_Listing_Window), 
			row, 1, (char *)artist);
		gtk_clist_set_text(GTK_CLIST(XR_Channel_Listing_Window), 
			row, 3, (char *)category);
	}
	else
	{
		gtk_clist_set_text(GTK_CLIST(XR_Channel_Listing_Window), row, 1, "");
		gtk_clist_set_text(GTK_CLIST(XR_Channel_Listing_Window), row, 3, "");
	}
#endif

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
	char	buf[512];
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

/*
	channel_id = 
		(int)gtk_clist_get_row_data(GTK_CLIST(XR_Channel_Listing_Window), row);
*/
	channel_id = (int)gtk_clist_get_row_data(GTK_CLIST(widget), row);
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

	/* Get the current channel info */
	if ( GetChannelInfo(0, buf, 1024) == -1 ) return;

	bzero((char *)(buf + strlen(buf) - 2), 1);
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
		AddClistEntry(
			XR_Favorites_Artist_Listing_Window,
			station, song, artist, category, chnum);
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

	/* Get the current channel info */
	if ( GetChannelInfo(0, buf, 1024) == -1 ) return;

	bzero((char *)(buf + strlen(buf) - 2), 1);
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
		AddClistEntry(
			XR_Favorites_Listing_Window,
			station, song, artist, category, chnum);
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
