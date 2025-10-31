#include <gtk/gtk.h>


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
XRConnect                              (GtkButton       *button,
                                        gpointer         user_data);

void
XRDisconnect                           (GtkButton       *button,
                                        gpointer         user_data);

void
XRChannelUp                            (GtkButton       *button,
                                        gpointer         user_data);

void
XRChannelDown                          (GtkButton       *button,
                                        gpointer         user_data);

void
XRChannelJump                          (GtkButton       *button,
                                        gpointer         user_data);

void
XRChannelLast                          (GtkButton       *button,
                                        gpointer         user_data);

void
XRChannelRefresh                       (GtkButton       *button,
                                        gpointer         user_data);

void
XRMuteOff                              (GtkButton       *button,
                                        gpointer         user_data);

void
XRMuteOn                               (GtkButton       *button,
                                        gpointer         user_data);

void
XRChannelListToggle                    (GtkButton       *button,
                                        gpointer         user_data);

void
XRPreferences                          (GtkButton       *button,
                                        gpointer         user_data);

void
on_artist_entry_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_song_entry_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_station_entry_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_category_entry_realize              (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_host_ip_entry_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_host_status_image_realize           (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_XR_Msg_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_scrolledwindow1_realize             (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_label6_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_closebutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_XR_Msg_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_button14_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button15_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_entry1_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_entry2_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_show_radio_id1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clist1_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_scrolledwindow1_realize             (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist1_click_column                 (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_clist1_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist1_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_entry1_key_press_event              (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);
