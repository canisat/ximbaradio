#define XR_VERSION		"Version 0.3"

/* All commands to the server must end in a newline */
#define XR_CMD_POWER_ON		"xmPOW:ON0:XXX\n"
#define XR_CMD_POWER_OFF	"xmPOW:OFF:XXX\n"
#define XR_CMD_CHA_UP		"xmCHA:UP0:XXX\n"
#define XR_CMD_CHA_DOWN		"xmCHA:DOW:XXX\n"
#define XR_CMD_GET_ALL		"xmCHA:GET:XXX\n"
#define XR_CMD_MUTE_ON		"xmMUT:ON0:XXX\n"
#define XR_CMD_MUTE_OFF		"xmMUT:OFF:XXX\n"
#define XR_CMD_PCR_DATA		"xmPCR:DAT:XXX\n"
#define XR_CMD_RADIO_ID		"xmPCR:DAT:RID\n"

/* These commands require a 3 character argument to be added to it. */
#define XR_CMD_CHA_SET		"xmCHA:NEW:"
#define XR_CMD_CHA_INFO		"xmCHA:NFO:"

/* How many channels to fetch at any one time during a refresh. */
#define XR_REFRESH_BLOCK	5

/* What is the maximum number of channels I can handle? */
#define XR_MAX_CHANNELS		255

/* Default Server information. */
#define SERV_HOST_ADDR		"127.0.0.1"
#define SERV_TCP_PORT		3877


/* Debug Macro */
#define DBGPrintf(msg) 		{ if (DEBUG) { printf msg ; } }

#ifdef XR_CB_C

GtkWidget *XR_Msg_Window = NULL;
GtkWidget *XR_Msg = NULL;
GtkWidget *XR_Channel_Listing_Window = NULL;
GtkWidget *XR_Channel_Listing_Notebook_Window = NULL;
GtkWidget *XR_Favorites_Listing_Window = NULL;
GtkWidget *XR_Favorites_Artist_Listing_Window = NULL;
GtkWidget *XR_Session_Listing_Window = NULL;
GtkWidget *XR_About_Window = NULL;
GtkWidget *XR_About_Version = NULL;
GtkWidget *XR_Artist_Entry = NULL;
GtkWidget *XR_Song_Entry = NULL;
GtkWidget *XR_Station_Entry = NULL;
GtkWidget *XR_Category_Entry = NULL;
GtkWidget *XR_Host_IP_Entry = NULL;
GtkWidget *XR_Status_Image = NULL;
GtkWidget *XR_JumpTo_Window = NULL;
GtkWidget *XR_JumpTo_StationID = NULL;
GtkWidget *XR_JumpTo_StationName = NULL;

int XR_Clist_Sig_ID = -1;
int XR_Favorites_Clist_Sig_ID = -1;
int XR_Favorites_Artist_Clist_Sig_ID = -1;
int XR_Category_Clist_Sig_ID = -1;
int XR_SockFD;
int XR_Channel_Listing_State = 0;
char *XR_Radio_ID = NULL;

int DEBUG = 0;

#else

extern GtkWidget *XR_Msg_Window;
extern GtkWidget *XR_Msg;
extern GtkWidget *XR_Channel_Listing_Window;
extern GtkWidget *XR_Channel_Listing_Notebook_Window;
extern GtkWidget *XR_Favorites_Listing_Window;
extern GtkWidget *XR_Favorites_Artist_Listing_Window;
extern GtkWidget *XR_Session_Listing_Window;
extern GtkWidget *XR_About_Window;
extern GtkWidget *XR_About_Version;
extern GtkWidget *XR_Artist_Entry;
extern GtkWidget *XR_Song_Entry;
extern GtkWidget *XR_Station_Entry;
extern GtkWidget *XR_Category_Entry;
extern GtkWidget *XR_Host_IP_Entry;
extern GtkWidget *XR_Status_Image;
extern GtkWidget *XR_JumpTo_Window;
extern GtkWidget *XR_JumpTo_StationID;
extern GtkWidget *XR_JumpTo_StationName;

extern int XR_Clist_Sig_ID;
extern int XR_Favorites_Clist_Sig_ID;
extern int XR_Favorites_Artist_Clist_Sig_ID;
extern int XR_Category_Clist_Sig_ID;
extern int XR_SockFD;
extern int XR_Channel_Listing_State;
extern char *XR_Radio_ID;

extern int DEBUG;

#endif


#ifdef XR_UTIL_C

void XRUMsg();
void XRUInit();
int XRUConnect();
void XRURadioOn();
void XRUDisconnect();
void XRUChannelUp();
void XRUChannelDown();
void XRUChannelJump();
void XRUMuteOff();
void XRUMuteOn();
void XRUChannelLast();
void XRUChannelLast();
int XRUGetChannelInfo();
int XRUChannelRefresh();
void XRUChannelListingSelected();
void XRUFavoritesListingSelected();
void XRUJumpToStationID();
void XRUJumpToStationName();
void XRUJumpToClose();
void XRUJumpToOpen();
void XRUShowRadioID();
void XRUJumpKeyPress();
void XRUAddFavoriteArtist();
void XRUAddFavoriteStation();

#else

extern void XRUMsg();
extern void XRUInit();
extern int XRUConnect();
extern void XRURadioOn();
extern void XRUDisconnect();
extern void XRUChannelUp();
extern void XRUChannelDown();
extern void XRUChannelJump();
extern void XRUMuteOff();
extern void XRUMuteOn();
extern void XRUChannelLast();
extern int XRUGetChannelInfo();
extern int XRUChannelRefresh();
extern void XRUChannelListingSelected();
extern void XRUFavoritesListingSelected();
extern void XRUJumpToStationID();
extern void XRUJumpToStationName();
extern void XRUJumpToClose();
extern void XRUJumpToOpen();
extern void XRUShowRadioID();
extern void XRUJumpKeyPress();
extern void XRUAddFavoriteArtist();
extern void XRUAddFavoriteStation();

#endif
