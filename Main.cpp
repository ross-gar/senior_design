/*All includes and functions defined in Funcs.h*/

#include "Funcs.h"

using namespace std;

/*Global Variables*/
extern int uart0_filestream;
extern RFID_data database;

int main(int argc, char** argv)
{
  /* Define Widget Pointers: */
  GtkWidget *MainWindow;
  GtkWidget *Grid, *Timer1_Grid, *Timer2_Grid, *Lamp_Control_Grid;
  GtkWidget *UpdateButton, *ClearButton, *QuitButton, *ReadRFID;
  GtkWidget *TextOut;
  GtkTextBuffer *TextOutBuffer;
  GtkWidget *TextOutScroll;
  GtkWidget *MssgOut;
  GtkTextBuffer *MssgOutBuffer;
  GtkWidget *ManualEntry, *ManualEntryGo, *ManualEntryLabel;
  GtkWidget *ManualRemove, *ManualRemoveGo, *ManualRemoveLabel;
  GtkWidget *DatabaseLabel;
  GtkWidget *SystemMssgLabel;
  GtkTextIter iter1, iter2;
  GtkWidget *RadioBox, *Radio1, *Radio2;
  GtkWidget *Timer1_RadioBox, *Timer1_Radio1, *Timer1_Radio2;
  GtkWidget *Timer2_RadioBox, *Timer2_Radio1, *Timer2_Radio2;
  GtkWidget *Timer1_Button, *Timer2_Button, *Timer1_ResetButton, *Timer2_ResetButton;
  GtkWidget *Timer1_Sec, *Timer2_Sec, *Timer1_Min, *Timer2_Min;
  GtkWidget *Timer1_Label, *Timer2_Label, *Timer1_Colon, *Timer2_Colon;
  GThread *TimerThreadPointer, *RFIDThreadPointer, *BluetoothThreadPointer;
  GtkWidget *PopUp_Checkbox;
  GtkWidget *Increase_Brightness, *Decrease_Brightness, *Brightness_Label, *Brightness_Entry;
  GtkWidget *Status_Label, *Status_Entry;
  GtkWidget *Lamp_On_Button, *Lamp_Off_Button;
  
  /* RFID Terminal Setup */
  uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
        if (uart0_filestream == -1)
        {
                printf("Error - Unable to open UART\n");
        }
        struct termios options;
        tcgetattr(uart0_filestream, &options);
        options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
        options.c_iflag = IGNPAR;
        options.c_oflag = 0;
        options.c_lflag = 0;
        tcflush(uart0_filestream, TCIFLUSH);
        tcsetattr(uart0_filestream, TCSANOW, &options);

  /* Load test data into database */
  unsigned long long id1 = 345678913;
  unsigned long long id2 = 345678911;
  unsigned long long id3 = 345678912;
  database.add_element(id1,"Test Data Entry 1");
  database.add_element(id2,"Test Data Entry 2");
  database.add_element(id3,"Test Data Entry 3");

  /* Set up multithreading */
        if (!g_thread_supported())
          g_thread_init(NULL);
        gdk_threads_init();

  /* Obtain Gtk global lock */
  gdk_threads_enter();

  gtk_init(&argc, &argv);

  /* Main Window Definitions */
  MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(MainWindow), "Smart Lamp Application"); //define main window
  gtk_window_set_default_size(GTK_WINDOW(MainWindow), 1100,600); //size main window
  gtk_window_set_position(GTK_WINDOW(MainWindow), GTK_WIN_POS_CENTER); //set window position
  g_signal_connect(G_OBJECT(MainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);  //code for closing window

  /* Grid Setup */
  Grid      = gtk_grid_new();
  Timer1_Grid = gtk_grid_new();
  Timer2_Grid = gtk_grid_new();
  Lamp_Control_Grid = gtk_grid_new();
  gtk_container_add (GTK_CONTAINER (MainWindow), Grid);  //pack grid into Main Window container

  /* Define Buttons: */
  UpdateButton        = gtk_button_new_with_label("Update Database Contents");
  ClearButton         = gtk_button_new_with_label("Clear Database Contents");
  QuitButton          = gtk_button_new_with_label("Quit");
  ReadRFID            = gtk_button_new_with_label("Read In RFID Information");
  ManualEntryGo       = gtk_button_new_with_label("Add");
  ManualRemoveGo      = gtk_button_new_with_label("Remove");
  Timer1_Button       = gtk_button_new_with_label("Start/Stop");
  Timer2_Button       = gtk_button_new_with_label("Start/Stop");
  Timer1_ResetButton  = gtk_button_new_with_label("Reset");
  Timer2_ResetButton  = gtk_button_new_with_label("Reset");
  Increase_Brightness = gtk_button_new_with_label("+");
  Decrease_Brightness = gtk_button_new_with_label("-");
  Lamp_On_Button      = gtk_button_new_with_label("Turn Lamp On");
  Lamp_Off_Button     = gtk_button_new_with_label("Turn Lamp Off");

  /* Define Labels */
  DatabaseLabel     = gtk_label_new("Database Contents:");
  SystemMssgLabel   = gtk_label_new("System Messages:");
  ManualEntryLabel  = gtk_label_new("Enter A Description of the Item:");
  ManualRemoveLabel = gtk_label_new("Enter The Index Number of the Item:");
  Timer1_Label      = gtk_label_new("Timer 1");
  Timer2_Label      = gtk_label_new("Timer 2");
  Timer1_Colon      = gtk_label_new(":");
  Timer2_Colon      = gtk_label_new(":");
  Brightness_Label  = gtk_label_new("Lamp Brightness (%):");
  Status_Label      = gtk_label_new("Lamp Status:");

  /* Define Manual Text Entry */
  ManualEntry      = gtk_entry_new();
  ManualRemove     = gtk_entry_new();
  Timer1_Min       = gtk_entry_new();
  Timer2_Min       = gtk_entry_new();
  Timer1_Sec       = gtk_entry_new();
  Timer2_Sec       = gtk_entry_new();
  Brightness_Entry = gtk_entry_new();
  Status_Entry     = gtk_entry_new();

  /* Set Timer Text Boxes to 0 */
  gtk_entry_set_text(GTK_ENTRY(Timer1_Sec), "0");
  gtk_entry_set_text(GTK_ENTRY(Timer2_Sec), "0");
  gtk_entry_set_text(GTK_ENTRY(Timer1_Min), "0");
  gtk_entry_set_text(GTK_ENTRY(Timer2_Min), "0");

  /* Set Default Brightness of Lamp */
  gtk_entry_set_text(GTK_ENTRY(Brightness_Entry), "90");
  gtk_entry_set_text(GTK_ENTRY(Status_Entry), "Lamp connection not made");

  /* Define Text Output Box and its Attributes */
  TextOutBuffer = gtk_text_buffer_new(NULL);  //creates text buffer
  MssgOutBuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_create_tag(TextOutBuffer, "writeonly", "editable", FALSE, NULL);  //creates tag for read only
  gtk_text_buffer_create_tag(MssgOutBuffer, "writeonly", "editable", FALSE, NULL);
  TextOut = gtk_text_view_new_with_buffer(TextOutBuffer);  //creates text view and sets TextOutBuffer as the buffer
  MssgOut = gtk_text_view_new_with_buffer(MssgOutBuffer);
  gtk_text_buffer_get_iter_at_offset(TextOutBuffer, &iter1, 0);  //gets position of begining of text
  gtk_text_buffer_get_iter_at_offset(MssgOutBuffer, &iter2, 0);
  gtk_text_buffer_insert_with_tags_by_name (TextOutBuffer, &iter1, "Welcome to the Smart Lamp Interface Program!", -1, "writeonly", NULL); //inserts read only text
  gtk_text_buffer_insert_with_tags_by_name (MssgOutBuffer, &iter2, "Here's Where Messages Will Be Displayed", -1, "writeonly", NULL);

  /* Code for scrolled window */
  TextOutScroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add (GTK_CONTAINER(TextOutScroll), TextOut);

  /* Radio Button Setup */
  Radio1   = gtk_radio_button_new_with_label (NULL, "Add RFID Tags to Database");
  Radio2   = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(Radio1), "Remove RFID Tags from Database");
  RadioBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  gtk_box_pack_start (GTK_BOX(RadioBox), Radio1, TRUE, TRUE, 1);
  gtk_box_pack_start (GTK_BOX(RadioBox), Radio2, TRUE, TRUE, 1);

  Timer1_Radio1   = gtk_radio_button_new_with_label (NULL, "Count Up");
  Timer1_Radio2   = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(Timer1_Radio1), "Count Down");
  Timer1_RadioBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  gtk_box_pack_start (GTK_BOX(Timer1_RadioBox), Timer1_Radio1, TRUE, TRUE, 1);
  gtk_box_pack_start (GTK_BOX(Timer1_RadioBox), Timer1_Radio2, TRUE, TRUE, 1);

  Timer2_Radio1   = gtk_radio_button_new_with_label (NULL, "Count Up");
  Timer2_Radio2   = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(Timer2_Radio1), "Count Down");
  Timer2_RadioBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  gtk_box_pack_start (GTK_BOX(Timer2_RadioBox), Timer2_Radio1, TRUE, TRUE, 1);
  gtk_box_pack_start (GTK_BOX(Timer2_RadioBox), Timer2_Radio2, TRUE, TRUE, 1);

  /* Define check box */
  PopUp_Checkbox = gtk_check_button_new_with_label("Display Pop Up When Removing Items from Database");

  /*Request Size for main text view */
  gtk_widget_set_size_request(TextOutScroll, 500,200);
  //gtk_widget_set_size_request(MssgOut, 500, 50);

  /* Timer1 Grid Setup */
  gtk_grid_attach (GTK_GRID(Timer1_Grid), Timer1_Label,       0,0,1,1);
  gtk_grid_attach (GTK_GRID(Timer1_Grid), Timer1_Min,         0,1,1,1);
  gtk_grid_attach (GTK_GRID(Timer1_Grid), Timer1_Colon,       1,1,1,1);
  gtk_grid_attach (GTK_GRID(Timer1_Grid), Timer1_Sec,         2,1,1,1);
  gtk_grid_attach (GTK_GRID(Timer1_Grid), Timer1_Button,      3,1,2,1);
  gtk_grid_attach (GTK_GRID(Timer1_Grid), Timer1_ResetButton, 5,1,2,1);
  gtk_grid_attach (GTK_GRID(Timer1_Grid), Timer1_RadioBox,    0,2,1,1);

  /* Timer2 Grid Setup */
  gtk_grid_attach (GTK_GRID(Timer2_Grid), Timer2_Label,       0,0,1,1);
  gtk_grid_attach (GTK_GRID(Timer2_Grid), Timer2_Min,         0,1,1,1);
  gtk_grid_attach (GTK_GRID(Timer2_Grid), Timer2_Colon,       1,1,1,1);
  gtk_grid_attach (GTK_GRID(Timer2_Grid), Timer2_Sec,         2,1,1,1);
  gtk_grid_attach (GTK_GRID(Timer2_Grid), Timer2_Button,      3,1,2,1);
  gtk_grid_attach (GTK_GRID(Timer2_Grid), Timer2_ResetButton, 5,1,2,1);
  gtk_grid_attach (GTK_GRID(Timer2_Grid), Timer2_RadioBox,    0,2,1,1);

  /* Lamp Control Elements Grid Setup */
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Brightness_Label,    0,0,1,1);
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Brightness_Entry,    0,1,1,1);
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Increase_Brightness, 1,1,1,1);
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Decrease_Brightness, 2,1,1,1);
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Status_Label,        0,2,1,1);
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Status_Entry,        0,3,1,1);
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Lamp_On_Button,      1,3,1,1);
  gtk_grid_attach (GTK_GRID(Lamp_Control_Grid), Lamp_Off_Button,     2,3,1,1);

  /*Attach All Elements to Grid: */
  gtk_grid_attach (GTK_GRID(Grid), UpdateButton,        0,0,1,1);
  gtk_grid_attach (GTK_GRID(Grid), ClearButton,         2,0,1,1);
  gtk_grid_attach (GTK_GRID(Grid), QuitButton,          3,0,1,1);
  gtk_grid_attach (GTK_GRID(Grid), ReadRFID,            1,0,1,1);
  gtk_grid_attach (GTK_GRID(Grid), DatabaseLabel,       0,1,1,1);
  gtk_grid_attach (GTK_GRID(Grid), TextOutScroll,       0,2,5,4);
  gtk_grid_attach (GTK_GRID(Grid), SystemMssgLabel,     0,7,1,1);
  gtk_grid_attach (GTK_GRID(Grid), MssgOut,             0,8,4,4);
  gtk_grid_attach (GTK_GRID(Grid), ManualEntryGo,       6,1,1,1);
  gtk_grid_attach (GTK_GRID(Grid), ManualEntry,         5,1,1,1);
  gtk_grid_attach (GTK_GRID(Grid), ManualEntryLabel,    5,0,1,1);
  gtk_grid_attach (GTK_GRID(Grid), ManualRemove,        5,3,1,1);
  gtk_grid_attach (GTK_GRID(Grid), ManualRemoveGo,      6,3,1,1);
  gtk_grid_attach (GTK_GRID(Grid), ManualRemoveLabel,   5,2,1,1);
  gtk_grid_attach (GTK_GRID(Grid), RadioBox,            5,4,1,1);
  gtk_grid_attach (GTK_GRID(Grid), PopUp_Checkbox,      5,5,1,1);
  gtk_grid_attach (GTK_GRID(Grid), Timer1_Grid,         0,13,2,2);
  gtk_grid_attach (GTK_GRID(Grid), Timer2_Grid,         2,13,2,2);
  gtk_grid_attach (GTK_GRID(Grid), Lamp_Control_Grid,   0,15,2,2);

  ViewBuffers Output_Pack;
  Output_Pack.Buffer1 = TextOutBuffer;
  Output_Pack.Buffer2 = MssgOutBuffer;
  Output_Pack.AddToggle = Radio1;

  Entry_Buffers Entry_Pack;
  Entry_Pack.Buffer1 = TextOutBuffer;
  Entry_Pack.Buffer2 = MssgOutBuffer;
  Entry_Pack.Entry = ManualEntry;

  Entry_Buffers Remove_Pack;
  Remove_Pack.Buffer1 = TextOutBuffer;
  Remove_Pack.Buffer2 = MssgOutBuffer;
  Remove_Pack.Entry = ManualRemove;

  GAsyncQueue* Timer_Queue = g_async_queue_new();
  Timer_Thread_Items Timer_Pack;
  Timer_Pack.Queue = Timer_Queue;
  Timer_Pack.Timer1_Min = Timer1_Min; Timer_Pack.Timer2_Min = Timer2_Min;
  Timer_Pack.Timer1_Sec = Timer1_Sec; Timer_Pack.Timer2_Sec = Timer2_Sec;
  Timer_Pack.Up_Toggle1 = Timer1_Radio1;
  Timer_Pack.Up_Toggle2 = Timer2_Radio1;

  GAsyncQueue* RFID_Queue = g_async_queue_new();
  RFID_Thread_Items RFID_Pack;
  RFID_Pack.Buffer1 = TextOutBuffer;
  RFID_Pack.Buffer2 = MssgOutBuffer;
  RFID_Pack.AddToggle = Radio1;
  RFID_Pack.Queue = RFID_Queue;
  RFID_Pack.PopUp_Checkbox = PopUp_Checkbox;

  GAsyncQueue* Bluetooth_Queue = g_async_queue_new();
  Bluetooth_Thread_Items Bluetooth_Pack;
  Bluetooth_Pack.Queue = Bluetooth_Queue;
  Bluetooth_Pack.Brightness_Entry = Brightness_Entry;
  Bluetooth_Pack.Status_Entry = Status_Entry;

  /* Callback connections: */
  g_signal_connect (QuitButton,         "clicked",   G_CALLBACK (gtk_main_quit),         NULL);
  g_signal_connect (UpdateButton,       "clicked",   G_CALLBACK (PrintDatabase),         (gpointer) TextOutBuffer);
  g_signal_connect (ReadRFID,           "clicked",   G_CALLBACK (ReadTag),               (gpointer) &Output_Pack);
  g_signal_connect (ClearButton,        "clicked",   G_CALLBACK (ClearDatabasePrompt),   (gpointer) &Output_Pack);
  g_signal_connect (ManualEntryGo,      "clicked",   G_CALLBACK (AddElement),            (gpointer) &Entry_Pack);
  g_signal_connect (ManualRemoveGo,     "clicked",   G_CALLBACK (RemoveElement),         (gpointer) &Remove_Pack);
  g_signal_connect (ManualEntry,        "activate",  G_CALLBACK (AddElement),            (gpointer) &Entry_Pack);
  g_signal_connect (ManualRemove,       "activate",  G_CALLBACK (RemoveElement),         (gpointer) &Remove_Pack);
  g_signal_connect (Timer1_Button,      "clicked",   G_CALLBACK (Timer1StartStop),       (gpointer) Timer_Queue);
  g_signal_connect (Timer2_Button,      "clicked",   G_CALLBACK (Timer2StartStop),       (gpointer) Timer_Queue);
  g_signal_connect (Timer1_ResetButton, "clicked",   G_CALLBACK (Timer1Reset),           (gpointer) Timer_Queue);
  g_signal_connect (Timer2_ResetButton, "clicked",   G_CALLBACK (Timer2Reset),           (gpointer) Timer_Queue);
  g_signal_connect (Increase_Brightness,"clicked",   G_CALLBACK (Increase_Brightness_CB),(gpointer) Bluetooth_Queue);
  g_signal_connect (Decrease_Brightness,"clicked",   G_CALLBACK (Decrease_Brightness_CB),(gpointer) Bluetooth_Queue);
  g_signal_connect (Lamp_On_Button,     "clicked",   G_CALLBACK (Lamp_On_CB),            (gpointer) Bluetooth_Queue);
  g_signal_connect (Lamp_Off_Button,    "clicked",   G_CALLBACK (Lamp_Off_CB),           (gpointer) Bluetooth_Queue);

  /* Extra Thread Definitions */
  TimerThreadPointer = g_thread_new("Timer Thread", (GThreadFunc) TimerThread, (gpointer) &Timer_Pack);
  RFIDThreadPointer  = g_thread_new("RFID Thread" , (GThreadFunc) RFIDThread,  (gpointer) &RFID_Pack);
  BluetoothThreadPointer = g_thread_new("Bluetooth Thread", (GThreadFunc) BluetoothThread, (gpointer) &Bluetooth_Pack);

  //Show all widgets
  gtk_widget_show_all(MainWindow);

  //Main Gtk Loop:
  gtk_main();

  //Release gtk global lock
  gdk_threads_leave();
  
  return 0;
}
