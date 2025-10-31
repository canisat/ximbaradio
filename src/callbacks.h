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

void
on_clist2_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist2_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_clist3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist3_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_notebook1_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist2_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_button16_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button17_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_clist4_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist4_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_setup_realize                       (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_button18_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button19_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_entry3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_favorites_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_entry3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_deletefavorites_realize             (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_deletelabel_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_button20_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button21_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_entry3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_entry4_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_checkbutton1_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_checkbutton2_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_spinbutton1_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist5_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_entry5_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_notebook1_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist5_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_spinbutton1_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_checkbutton1_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_checkbutton2_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_entry5_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_entry4_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist1_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist2_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist3_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_setup_realize                       (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_clist1_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_clist2_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_clist4_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_clist3_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_clist4_realize                      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_categories_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_ChannelState_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_button24_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button22_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button23_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_clist5_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);
