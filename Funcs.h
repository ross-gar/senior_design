#ifndef __Funcs_H_INCLUDED //gaurd class definition
#define __Funcs_H_INCLUDED

//#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include "RFID_data.h"
//#include "timer.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sstream>
#include <gtk/gtk.h>
#include <string.h>

using namespace std;

/*Global Variables*/
extern int uart0_filestream;
extern RFID_data database;

/* Structs for various operations: */
typedef struct {
GtkTextBuffer* Buffer1;
GtkTextBuffer* Buffer2;
GtkWidget *    AddToggle;
} ViewBuffers;

typedef struct {
GtkTextBuffer* Buffer1;
GtkTextBuffer* Buffer2;
GtkWidget*     Entry;
} Entry_Buffers;

typedef struct {
GAsyncQueue* Queue;
GtkWidget* Timer1_Sec;
GtkWidget* Timer2_Sec;
GtkWidget* Timer1_Min;
GtkWidget* Timer2_Min;
GtkWidget* Up_Toggle1;
GtkWidget* Up_Toggle2;
} Timer_Thread_Items;

typedef struct {
GAsyncQueue* Queue;
GtkTextBuffer* Buffer1;
GtkTextBuffer* Buffer2;
GtkWidget*    AddToggle;
GtkWidget* Entry;
GtkWidget* PopUp_Checkbox;
} RFID_Thread_Items;

typedef struct {
GtkTextBuffer* Buffer1;
GtkTextBuffer* Buffer2;
unsigned long long int final_data;
bool add_remove;
GtkWidget* Entry;
} RFID_PopUp_Items;

typedef struct {
GAsyncQueue * Queue;
GtkWidget* Brightness_Entry;
GtkWidget* Status_Entry;
} Bluetooth_Thread_Items;

/*Various Callback and other Functions*/
void PrintToTextView (GtkTextBuffer *TextBuffer, string OutputString);
void PrintDatabase (GtkWidget *widget, GtkTextBuffer *Buffer);
void PrintDatabase2 (GtkTextBuffer *Buffer);
void DestroyThatWidget (GtkWidget *widget, GtkWidget *Victim);
void AddElement (GtkWidget *widget, Entry_Buffers *Entry_Buffers);
void RemoveElement (GtkWidget *widget, Entry_Buffers *Entry_Buffers);
void ClearDatabase (GtkWidget *widget, ViewBuffers *ViewBuffers);
void ClearDatabasePrompt (GtkWidget *widget, ViewBuffers *ViewBuffers);
void ReadTag (GtkWidget *widget, ViewBuffers *ViewBuffers);
void ReadTag_PopUp (RFID_PopUp_Items* RFID_PopUp_Pack);
void Timer1StartStop (GtkWidget *widget, GAsyncQueue *Queue);
void Timer2StartStop (GtkWidget *widget, GAsyncQueue *Queue);
void Timer1Reset (GtkWidget *widget, GAsyncQueue *Queue);
void Timer2Reset (GtkWidget *widget, GAsyncQueue *Queue);
gpointer TimerThread(Timer_Thread_Items* Timer_Pack);
gpointer RFIDThread(RFID_Thread_Items* RFID_Pack);
gpointer BluetoothThread(Bluetooth_Thread_Items* Bluetooth_Pack);
void ReadTag_PopUp_Callback(GtkWidget *widget, RFID_PopUp_Items* RFID_PopUp_Pack);
void Scalpel_Button_Callback(GtkWidget *widget, RFID_PopUp_Items *RFID_PopUp_Pack);
void Sponge_Button_Callback(GtkWidget *widget, RFID_PopUp_Items *RFID_PopUp_Pack);
void Forceps_Button_Callback(GtkWidget *widget, RFID_PopUp_Items *RFID_PopUp_Pack);
void Scissors_Button_Callback(GtkWidget *widget, RFID_PopUp_Items *RFID_PopUp_Pack);
void Retractor_Button_Callback(GtkWidget *widget, RFID_PopUp_Items *RFID_PopUp_Pack);
void Increase_Brightness_CB (GtkWidget *widget, GAsyncQueue *Queue);
void Decrease_Brightness_CB (GtkWidget *widget, GAsyncQueue *Queue);
void Lamp_On_CB (GtkWidget *widget, GAsyncQueue *Queue);
void Lamp_Off_CB (GtkWidget *widget, GAsyncQueue *Queue);
char retrieve_char(int* brightness);

#endif
