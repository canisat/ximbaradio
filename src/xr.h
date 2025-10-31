#define XR_VERSION		"Version 0.4"

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

/*
 * How many times through the channel listing updates should the favorite
 * channels be updated too?
 */
#define XR_FAVORITE_REFRESH_COUNT		3

/* Types */
#define XR_ARTIST_TYPE		1
#define XR_CHANNEL_TYPE		2
#define XR_ALL_TYPE			3

/* Default Server information. */
#define SERV_HOST_ADDR		"127.0.0.1"
#define SERV_TCP_PORT		3877

/* Name of file to save favorites to */
#define FAVORITESFILE		".ximbaradiorc"
#define PREFSFILE				".ximbaradioprefs"


/* Debug Macro */
#define DBGPrintf(msg) 		{ if (DEBUG) { printf msg ; } }

/* Typedefs */
typedef struct _categoryt {
	char			*name;
	GtkWidget	*clist;
	int			pagenum;
} CategoryT;
typedef struct _catentryt {
	char		*name;
	int		state;
} CatEntryT;

typedef struct _prefs {
	char			*hostname;
	char			*daemondir;
	gboolean		enable_favorites;
	gboolean		channel_windows;
	int			performance;
	GList			*categories;
} PrefsT;

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
GtkWidget *XR_Favorite_Channel_Window = NULL;
GtkWidget *XR_Favorite_Channel_Label = NULL;
GtkWidget *XR_Delete_Favorite_Popup = NULL;
GtkWidget *XR_Delete_Favorite_Label = NULL;
GtkWidget *XR_Preferences_Window = NULL;
GtkWidget *XR_Preference_Host = NULL;
GtkWidget *XR_Preference_Location = NULL;
GtkWidget *XR_Preference_Channel_Listing = NULL;
GtkWidget *XR_Preference_Enable_Favorites = NULL;
GtkWidget *XR_Preference_Performance = NULL;
GtkWidget *XR_Preference_Clist = NULL;
GtkWidget *XR_Category_Window = NULL;
GtkWidget *XR_Category_State_Label = NULL;

int XR_Clist_Sig_ID = -1;
int XR_Favorites_Clist_Sig_ID = -1;
int XR_Favorites_Artist_Clist_Sig_ID = -1;
int XR_Category_Clist_Sig_ID = -1;
int XR_SockFD;
int XR_Channel_Listing_State = 0;
char *XR_Radio_ID = NULL;
GList *XR_List_Artist = NULL;
GList *XR_List_Channel = NULL;
GList *XR_List_Categories = NULL;
int XR_Favorite_Refresh_Count = XR_FAVORITE_REFRESH_COUNT;
char *delete_favorite_artist;
int delete_favorite_row;
PrefsT prefs;

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
extern GtkWidget *XR_Favorite_Channel_Window;
extern GtkWidget *XR_Favorite_Channel_Label;
extern GtkWidget *XR_Delete_Favorite_Popup;
extern GtkWidget *XR_Delete_Favorite_Label;
extern GtkWidget *XR_Preferences_Window;
extern GtkWidget *XR_Preference_Host;
extern GtkWidget *XR_Preference_Location;
extern GtkWidget *XR_Preference_Channel_Listing;
extern GtkWidget *XR_Preference_Enable_Favorites;
extern GtkWidget *XR_Preference_Performance;
extern GtkWidget *XR_Preference_Clist;
extern GtkWidget *XR_Category_Window;
extern GtkWidget *XR_Category_State_Label;

extern int XR_Clist_Sig_ID;
extern int XR_Favorites_Clist_Sig_ID;
extern int XR_Favorites_Artist_Clist_Sig_ID;
extern int XR_Category_Clist_Sig_ID;
extern int XR_SockFD;
extern int XR_Channel_Listing_State;
extern char *XR_Radio_ID;
extern GList *XR_List_Artist;
extern GList *XR_List_Channel;
extern GList *XR_List_Categories;
extern int XR_Favorite_Refresh_Count;
extern char *delete_favorite_artist;
extern int delete_favorite_row;
extern PrefsT prefs;

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
void XRUSaveFavorites();
void XRUReadFavorites();
void XRUChannelRefreshFavorites();
void XRUSavePreferences();
void XRUSavePrefs();
void XRUReadPrefs();
void XRUShowCategoryState();
void XRUHideCategory();
void XRUShowCategory();

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
extern void XRUSaveFavorites();
extern void XRUReadFavorites();
extern void XRUChannelRefreshFavorites();
extern void XRUSavePreferences();
extern void XRUSavePrefs();
extern void XRUReadPrefs();
extern void XRUShowCategoryState();
extern void XRUHideCategory();
extern void XRUShowCategory();

#endif
