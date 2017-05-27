#include "Funcs.h"

using namespace std;

/* Global Variables */
int uart0_filestream = -1;
RFID_data database;

/* 
Message chars to facillitate communication between threads 
A = timer 1 start/stop	B = timer 1 reset
C = timer 2 start/stop	D = timer 2 reset
*/
const char A_Mssg    = 'A';
const char B_Mssg    = 'B';
const char C_Mssg    = 'C';
const char D_Mssg    = 'D';
const char Zero_Char = '0';

/* Messages for bluetooth communication stuff */
const char increase_Mssg = 'i';
const char decrease_Mssg = 'd';
const char Lamp_On_Mssg = 'x';
const char Lamp_Off_Mssg = 'y';

/*This function converts a pair of ascii chars from the RFID reader to their integer value*/
int char_convert(char a, char b)
{
        unsigned int a_intermediate = a;  //get char values into integer form
        unsigned int b_intermediate = b;
        unsigned int first_half = 0;  //initialize
        unsigned int second_half = 0;
        if ( (a_intermediate <= 57) && (a_intermediate >= 48) )  //check if char was an ascii number
        {
                first_half = a_intermediate - 48; //convert to actual number
        }
        else if ( (a_intermediate <= 70) && (a_intermediate >= 65) ) //check if char was ascii A-F
        {
                first_half = a_intermediate - 55; //convert to actual number
        }
        if ( (b_intermediate <= 57) && (b_intermediate >= 48) )  //check if char was an ascii number
        {
                second_half = b_intermediate - 48; //convert to actual number
        }
        else if ( (b_intermediate <= 70) && (b_intermediate >= 65) ) //check if char was ascii A-F
        {
                second_half = b_intermediate - 55; //convert to actual number
        }
        return ( (first_half << 4) | (second_half) ); //return OR of two numbers with second half shifted
}

/*This function prints out text to whichever TextView the calling functions wants it to*/
void PrintToTextView (GtkTextBuffer *TextBuffer, string OutputString)
{
	//Generate chars from input string
	char OutputChars[1024];
        strncpy(OutputChars, OutputString.c_str(), sizeof(OutputChars));
        OutputChars[sizeof(OutputChars) - 1] = 0;

        GtkTextIter start, end;

        //Get text buffer used for text output box
        //GtkTextBuffer *Buffer = gtk_text_view_get_buffer(TextView);

        //Retrieve Start and End Iterators
        gtk_text_buffer_get_start_iter (TextBuffer, &start);
        gtk_text_buffer_get_end_iter (TextBuffer, &end);

        //Delete Previous Contents
        gtk_text_buffer_delete (TextBuffer, &start, &end);

        //Get Start Location After Deletion
        gtk_text_buffer_get_start_iter (TextBuffer, &start);
        gtk_text_buffer_insert_with_tags_by_name (TextBuffer, &start, OutputChars, -1, "writeonly", NULL);

}

/*This function is called when the user presses the button to query the database.
It reads the database contents, converts it to an array of chars, and then sends
that to the text buffer inserted in the main text output window*/
void PrintDatabase (GtkWidget *widget, GtkTextBuffer *Buffer)
{
	stringstream OutputStringStream;
	OutputStringStream<<hex;
	for (int i=0; i<database.size(); ++i)
        {
		element tmp = database.read_element(i);
		OutputStringStream<<dec<<i+1<<") "<<tmp.id_number<<"\t\t"<<tmp.item_name<<"\n";
        }
	if (database.size() == 0)
		OutputStringStream<<"The Database is Empty!\n";
	string OutputString = OutputStringStream.str();
	PrintToTextView(Buffer, OutputString);
}
void PrintDatabase2 (GtkTextBuffer *Buffer)
{
	stringstream OutputStringStream;
	OutputStringStream<<hex;
	for (int i=0; i<database.size(); ++i)
        {
		element tmp = database.read_element(i);
		OutputStringStream<<dec<<i+1<<") "<<tmp.id_number<<"\t\t"<<tmp.item_name<<"\n";
        }
	if (database.size() == 0)
		OutputStringStream<<"The Database is Empty!\n";
	string OutputString = OutputStringStream.str();
	PrintToTextView(Buffer, OutputString);
}

void DestroyThatWidget (GtkWidget *widget, GtkWidget *Victim)
{
	gtk_widget_destroy(Victim);
}

void AddElement (GtkWidget *widget, Entry_Buffers *Entry_Buffers)
{
	srand(time(0)); //seed random number generator
	string text = gtk_entry_get_text( GTK_ENTRY(Entry_Buffers->Entry) );
	if (text.size() == 0)
	{
		PrintToTextView( Entry_Buffers->Buffer2, "Entry Box is Empty, Please Enter A Name");
	}
	else
	{
		unsigned long long int randnum = rand() % 1000000000; //generate random integer
		bool passfail = database.add_element(randnum,text);
		if (passfail == FALSE)
			cout<<"Error! Couldn't add manual element to database\n";
		else
		{
			stringstream tmp1;
			tmp1<<"ID: "<<randnum<<" with Name: "<<text<<" added to database.";
			PrintToTextView( Entry_Buffers->Buffer2, tmp1.str());
		}
		gtk_entry_set_text( GTK_ENTRY(Entry_Buffers->Entry) , "");
	}
	PrintDatabase(widget, Entry_Buffers->Buffer1);
}

void RemoveElement (GtkWidget *widget, Entry_Buffers *Entry_Buffers)
{
	string choice = gtk_entry_get_text( GTK_ENTRY(Entry_Buffers->Entry) );
	stringstream tmp2;
	tmp2<<choice;
	int index_choice;
	tmp2>>index_choice;
	bool inrange = ( ((index_choice-1) < database.size()) && (index_choice > 0));
	if (inrange == false)
	{
		PrintToTextView( Entry_Buffers->Buffer2, "That Index Number Does Not Exist or is Invalid");
	}
	else
	{
		element temp = database.read_element(index_choice-1);
		bool passfail = database.remove_index(index_choice-1);
		if (passfail == true)
		{
		stringstream tmp1;
		tmp1<<"Element #"<<choice<<" removed. ID: "<<temp.id_number<<" Name: "<<temp.item_name;
		PrintToTextView( Entry_Buffers->Buffer2, tmp1.str());
		}
	}
	PrintDatabase(widget, Entry_Buffers->Buffer1);
	gtk_entry_set_text( GTK_ENTRY(Entry_Buffers->Entry) ,"");
}

void ClearDatabase (GtkWidget *widget, ViewBuffers *ViewBuffers)
{
        bool passfail = database.clear_database();
        if (passfail)
        {
                stringstream tmp;
                tmp<<"Database Cleared!";
                PrintToTextView( ViewBuffers->Buffer2  ,tmp.str());
        }
        else
        {
                stringstream tmp;
                tmp<<"Database Not Cleared. Something's Wrong!\n";
                PrintToTextView( ViewBuffers->Buffer2 ,tmp.str());
        }
	PrintDatabase(widget, ViewBuffers->Buffer1);

}
void ClearDatabasePrompt (GtkWidget *widget, ViewBuffers *ViewBuffers)
{
	GtkWidget *PopUpWindow;
	GtkWidget *Grid2;
	GtkWidget *OkButton;
	GtkWidget *CancelButton;
	GtkWidget *PopUpLabel;
        PopUpWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(PopUpWindow), "Clear Database Confirmation"); //define main window
        gtk_window_set_default_size(GTK_WINDOW(PopUpWindow), 100,75); //size main window
        gtk_window_set_position(GTK_WINDOW(PopUpWindow), GTK_WIN_POS_CENTER); //set window position

        OkButton     = gtk_button_new_with_label("Ok");
        CancelButton = gtk_button_new_with_label("Cancel");
	PopUpLabel = gtk_label_new("Are you sure that you want to clear the database contents?");

        g_signal_connect(G_OBJECT(PopUpWindow), "destroy", G_CALLBACK(gtk_widget_destroy), NULL);  //code for closing window
        g_signal_connect(CancelButton,          "clicked", G_CALLBACK(DestroyThatWidget), (gpointer) PopUpWindow);
	g_signal_connect(OkButton,              "clicked", G_CALLBACK(ClearDatabase),     (gpointer) ViewBuffers);
	g_signal_connect(OkButton,              "clicked", G_CALLBACK(DestroyThatWidget), (gpointer) PopUpWindow);

        Grid2 =  gtk_grid_new(); //create grid
	gtk_grid_attach (GTK_GRID(Grid2), PopUpLabel,    0,0,2,1);
	gtk_grid_attach (GTK_GRID(Grid2), OkButton,      0,1,1,1);
        gtk_grid_attach (GTK_GRID(Grid2), CancelButton,  1,1,1,1);
        gtk_container_add (GTK_CONTAINER (PopUpWindow), Grid2);  //pack grid into Main Window container
	gtk_widget_show_all(PopUpWindow); //show pop up window
}

/*This function reads tag information into the database and displays the result
to the text box on the main window*/
void ReadTag (GtkWidget *widget, ViewBuffers *ViewBuffers)
{
	unsigned char rx_buffer[20];
	stringstream tmp;
	int rx_length = read(uart0_filestream, (void*) rx_buffer, 16); //read 16 data bytes (the size of one transmission from RFID reader
	if (rx_length <=0)
	{
		PrintToTextView( ViewBuffers->Buffer2 , "There is no data in the buffer");
	}
	else
	{
		rx_buffer[rx_length] = '\0';
                if (rx_length!=16)  //check if correct amount of data was read in
                {
			tmp<<"Error Reading Card, Recieved Inforrect Number of Bytes: "<<rx_length;
                	PrintToTextView( ViewBuffers->Buffer2 , tmp.str());
                }
                else
                {
                        // This code converts all char pairs into integers
                        unsigned long long int digit1=char_convert(rx_buffer[1], rx_buffer[2]);
                        unsigned long long int digit2=char_convert(rx_buffer[3], rx_buffer[4]);
                        unsigned long long int digit3=char_convert(rx_buffer[5], rx_buffer[6]);
                        unsigned long long int digit4=char_convert(rx_buffer[7], rx_buffer[8]);
                        unsigned long long int digit5=char_convert(rx_buffer[9], rx_buffer[10]);
                        unsigned long long int check_digit=char_convert(rx_buffer[11], rx_buffer[12]);
                        //Check for check sum confimation
                        bool check_sum = ( (digit1^digit2^digit3^digit4^digit5) == check_digit);
			unsigned long long int final_data = ( (digit1<<32) | (digit2<<24) | (digit3<<16) | (digit4<<8) | (digit5) );
			if (check_sum == true)
                        {
				tmp<<"Check Sum Success!\n";
				bool add_or_remove = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( ViewBuffers->AddToggle ));
				if ( add_or_remove == true )
				{
                        		//unsigned long long int final_data = ( (digit1<<32) | (digit2<<24) | (digit3<<16) | (digit4<<8) | (digit5) );
                                	bool passfail=database.add_element(final_data,"Example");
                                	if (passfail == true)
                                		tmp<<"ID added to database: "<<final_data;
                                	else
                                       		tmp<<"That tag was already in database\n";
					PrintToTextView( ViewBuffers->Buffer2 , tmp.str());
				}
				else
				{
					bool passfail=database.remove_element(final_data);
					if (passfail == true)
						tmp<<"Data Removed from database: "<<final_data;
					else
						tmp<<"That tag wasn't in the database\n";
					PrintToTextView (ViewBuffers->Buffer2, tmp.str());
				}
			}
		}
	}
	PrintDatabase(widget, ViewBuffers->Buffer1 );
}

/* The following 4 callback functions push messages to the timer thread */
void Timer1StartStop (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &A_Mssg);
}
void Timer2StartStop (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &C_Mssg);
}
void Timer1Reset (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &B_Mssg);
}
void Timer2Reset (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &D_Mssg);
}

gpointer TimerThread (Timer_Thread_Items* Timer_Pack)
{
	/* Timer Counts for Both Timers */
	int timer1_count_min = 0;
	int timer2_count_min = 0;
	int timer1_count_sec = 0;
	int timer2_count_sec = 0;
	//int temp_count= 0;

	/* start_stop = false for stopped, true for running */
	bool start_stop1 = false;
	bool start_stop2 = false;

	/* Variables for generating output */
	stringstream ss_min, ss_sec;
	char const *charbuf_min, *charbuf_sec;
	string temp_string_sec, temp_string_min;
	string entry_input_sec, entry_input_min;

	/* Communication variables */
	char Mssg_temp = 0;
	char* Mssg_Point_temp = NULL;
	while (TRUE)
	{
		/* See if there are any new messages */
		if (g_async_queue_length(Timer_Pack->Queue) > 0)
		{
			/* Pop Message */
			Mssg_Point_temp = (char*) g_async_queue_try_pop(Timer_Pack->Queue);
			Mssg_temp = *Mssg_Point_temp;

			/* Modify State if neccessary */
			if (Mssg_temp == 'A')
			{
				if (start_stop1 == true){ start_stop1 = false; }
				else 			{ start_stop1 = true; }
			}
			else if (Mssg_temp == 'C')
			{
				if (start_stop2 == true){ start_stop2 = false; }
				else 			{ start_stop2 = true; }
			}
			else if (Mssg_temp == 'B')
			{
				gdk_threads_enter();
				gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer1_Sec), &Zero_Char);
				gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer1_Min), &Zero_Char);
				gdk_threads_leave();		
			}
			else if (Mssg_temp == 'D')
			{
				gdk_threads_enter();
				gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer2_Sec), &Zero_Char);
				gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer2_Min), &Zero_Char);
				gdk_threads_leave();
			}
		}

		/* Check State to see if Output Necessary on Timer 1 */
		if (start_stop1 == true)
		{
			/* Query Entry for Current Value */
			entry_input_sec = gtk_entry_get_text( GTK_ENTRY(Timer_Pack->Timer1_Sec) );
			entry_input_min = gtk_entry_get_text( GTK_ENTRY(Timer_Pack->Timer1_Min) );
			ss_sec << entry_input_sec;
			ss_min << entry_input_min;
			ss_sec >> timer1_count_sec;
			ss_min >> timer1_count_min;
			timer1_count_sec = timer1_count_sec + (60*timer1_count_min);
			
			/* Modify Count */
			if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( Timer_Pack->Up_Toggle1 ) ) ) { timer1_count_sec++; }
			else { timer1_count_sec--; }

			/* Generate Entry Output */
			ss_sec.str(std::string() ); ss_sec.clear();  //clear stringstream & errors
			ss_min.str(std::string() ); ss_min.clear();  //clear stringstream & errors
			timer1_count_min = timer1_count_sec/60;
			timer1_count_sec = timer1_count_sec - (timer1_count_min*60);
			ss_sec << timer1_count_sec;
			ss_min << timer1_count_min;
			temp_string_sec = ss_sec.str();
			temp_string_min = ss_min.str();
			ss_sec.str(std::string() ); ss_sec.clear();  //clear stringstream & errors
			ss_min.str(std::string() ); ss_min.clear();  //clear stringstream & errors
			charbuf_min = (char*) temp_string_min.c_str();
			charbuf_sec = (char*) temp_string_sec.c_str();

			/* Write to Interface Element */
			gdk_threads_enter();
			gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer1_Sec), charbuf_sec);
			gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer1_Min), charbuf_min);
			gdk_threads_leave();

		}

		/* Check State to see if Output Necessary on Timer 2*/
		if (start_stop2 == true)
		{
			/* Query Entry for Current Value */
			entry_input_sec = gtk_entry_get_text( GTK_ENTRY(Timer_Pack->Timer2_Sec) );
			entry_input_min = gtk_entry_get_text( GTK_ENTRY(Timer_Pack->Timer2_Min) );
			ss_sec << entry_input_sec;
			ss_min << entry_input_min;
			ss_sec >> timer2_count_sec;
			ss_min >> timer2_count_min;
			timer2_count_sec = timer2_count_sec + (60*timer2_count_min);			

			/* Modify Count */
			if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( Timer_Pack->Up_Toggle2 ) ) ) { timer2_count_sec++; }
			else { timer2_count_sec--; }

			/* Generate Entry Output */
			ss_sec.str(std::string() ); ss_sec.clear();  //clear stringstream & errors
			ss_min.str(std::string() ); ss_min.clear();  //clear stringstream & errors
			timer2_count_min = timer2_count_sec/60;
			timer2_count_sec = timer2_count_sec - (timer2_count_min*60);
			ss_sec << timer2_count_sec;
			ss_min << timer2_count_min;
			temp_string_sec = ss_sec.str();
			temp_string_min = ss_min.str();
			ss_sec.str(std::string() ); ss_sec.clear();  //clear stringstream & errors
			ss_min.str(std::string() ); ss_min.clear();  //clear stringstream & errors
			charbuf_min = (char*) temp_string_min.c_str();
			charbuf_sec = (char*) temp_string_sec.c_str();

			/* Write to Interface Element */
			gdk_threads_enter();
			gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer2_Sec), charbuf_sec);
			gtk_entry_set_text(GTK_ENTRY(Timer_Pack->Timer2_Min), charbuf_min);
			gdk_threads_leave();
		}
		sleep(1.5);
	}
	return (NULL);
}

gpointer RFIDThread (RFID_Thread_Items *RFID_Pack)
{
	/* Variables used by RFID process */
	unsigned char rx_buffer[20];
	stringstream tmp;
	int rx_length;
	unsigned long long int final_data = 0;
	bool add_or_remove = true, passfail = false, existing = true;

	/* Set up package of info to send to callback */
	RFID_PopUp_Items RFID_PopUp_Pack;
	RFID_PopUp_Pack.Buffer1 = RFID_Pack->Buffer1;
	RFID_PopUp_Pack.Buffer2 = RFID_Pack->Buffer2;
	RFID_PopUp_Pack.final_data = final_data;
	RFID_PopUp_Pack.add_remove = true;

	while(true)
	{
		/* Attempt to read in from UART */ 
		rx_length = read(uart0_filestream, (void*) rx_buffer, 16);
		/* Check for correct rx length */
		if (rx_length == 16 )
		{
			/* Convert characters into their integer values */
			rx_buffer[rx_length] = '\0';
			unsigned long long int digit1=char_convert(rx_buffer[1], rx_buffer[2]);
                        unsigned long long int digit2=char_convert(rx_buffer[3], rx_buffer[4]);
                        unsigned long long int digit3=char_convert(rx_buffer[5], rx_buffer[6]);
                        unsigned long long int digit4=char_convert(rx_buffer[7], rx_buffer[8]);
                        unsigned long long int digit5=char_convert(rx_buffer[9], rx_buffer[10]);
                        unsigned long long int check_digit=char_convert(rx_buffer[11], rx_buffer[12]);

			/* Perform Check Sum on Received Data */
			bool check_sum = ( (digit1^digit2^digit3^digit4^digit5) == check_digit);
			if (check_sum)
			{
				gdk_threads_enter();
				/* Generate final tag ID */
				final_data = ( (digit1<<32) | (digit2<<24) | (digit3<<16) | (digit4<<8) | (digit5) );

				/* Checks if that element is already in database */
				existing = database.check_id(final_data);

				/* Check if adding or removing from database */
				add_or_remove = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( RFID_Pack->AddToggle ));

				/* Code for if the tag is already in the database */
				if (existing)
				{
					/* Adding element */
					if (add_or_remove)
					{ 
						tmp<<"That tag is already in the database."; 
						PrintToTextView(RFID_Pack->Buffer2, tmp.str());
					}
					/* Removing */
					else
					{
						/* User Wants Pop up when removing items */
						if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( RFID_Pack->PopUp_Checkbox)))
						{
							/* Indicate Removing Elements from Databse */
							RFID_PopUp_Pack.add_remove = false;
	
							/* Display Pop Up */
							ReadTag_PopUp( &RFID_PopUp_Pack );	
						}
						/* User doesn't want pop up window */
						else
						{
							passfail = database.remove_element(final_data);
							if (passfail == true)
							{
								tmp<<"Tag "<< final_data <<" removed from database.";
	
								/* Output new stuff to main window */
								PrintDatabase2( RFID_Pack->Buffer1 );
								PrintToTextView(RFID_Pack->Buffer2, tmp.str());
							}
							else
							{
								tmp<<"That tag wasn't in the database.";
								PrintToTextView(RFID_Pack->Buffer2, tmp.str());
							}
						}
					}
				}
				/* Tag isn't already in database */
				else
				{
					/* Add new final data value to pack */
					RFID_PopUp_Pack.final_data = final_data;

					/* Adding element */
					if ( add_or_remove )
					{
						/* Indicate Adding Elements to Databse */
						RFID_PopUp_Pack.add_remove = true;

						/* Display Pop Up */
						ReadTag_PopUp( &RFID_PopUp_Pack );                                                                                                                                                                            
					}
					else
					{
						
						tmp<<"That tag wasn't in the database.";
						PrintToTextView(RFID_Pack->Buffer2, tmp.str());		
					}

				}
				tmp.str(std::string() ); tmp.clear();  //clear stringstream & errors
				gdk_threads_leave();
					
			}//check sum
		}//rx length check
		/* Sleep for some time before checking for more tags */
		sleep(2);
	} //while loop
	return (NULL);
}

void ReadTag_PopUp (RFID_PopUp_Items* RFID_PopUp_Pack)
{
	/* Pop up window set up code */
	GtkWidget *PopUpWindow, *Grid, *OkButton, *CancelButton, *PopUpLabel;
	GtkWidget *Entry;
	GtkWidget *Scalpel_Button, *Sponge_Button, *Forceps_Button, *Scissors_Button, *Retractor_Button;
        PopUpWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(PopUpWindow), "Read in RFID Tag"); //define pop up window
        gtk_window_set_default_size(GTK_WINDOW(PopUpWindow), 175,100); //size pop up window
        gtk_window_set_position(GTK_WINDOW(PopUpWindow), GTK_WIN_POS_CENTER); //set window position

	/* Initialize Elements and pack grid */
	Grid =  gtk_grid_new();
	Entry = gtk_entry_new();
	if (RFID_PopUp_Pack->add_remove == true)
	{
		PopUpLabel = gtk_label_new("Please Enter a Description for that RFID Tag or Select a Predefined One");
		OkButton     = gtk_button_new_with_label("Ok");
        	CancelButton = gtk_button_new_with_label("Cancel");
		Scalpel_Button = gtk_button_new_with_label("Scalpel");
		Sponge_Button = gtk_button_new_with_label("Sponge");
		Forceps_Button = gtk_button_new_with_label("Forceps");
		Scissors_Button = gtk_button_new_with_label("Scissors");
		Retractor_Button = gtk_button_new_with_label("Retractor");

		/* Define Grid Locations */
		gtk_grid_attach (GTK_GRID(Grid), PopUpLabel,       0,0,2,1);
		gtk_grid_attach (GTK_GRID(Grid), Entry,            0,1,2,1);
		gtk_grid_attach (GTK_GRID(Grid), OkButton,         0,2,1,1);
        	gtk_grid_attach (GTK_GRID(Grid), CancelButton,     1,2,1,1);
		gtk_grid_attach (GTK_GRID(Grid), Scalpel_Button,   2,0,1,1);
		gtk_grid_attach (GTK_GRID(Grid), Sponge_Button,    3,0,1,1);
		gtk_grid_attach (GTK_GRID(Grid), Forceps_Button,   2,1,1,1);
		gtk_grid_attach (GTK_GRID(Grid), Scissors_Button,  3,1,1,1);
		gtk_grid_attach (GTK_GRID(Grid), Retractor_Button, 2,2,1,1);

		//Establish connections
		g_signal_connect(Scalpel_Button,        "clicked", G_CALLBACK(Scalpel_Button_Callback),  (gpointer) RFID_PopUp_Pack);
		g_signal_connect(Scalpel_Button,        "clicked", G_CALLBACK(DestroyThatWidget),        (gpointer) PopUpWindow);
		g_signal_connect(Sponge_Button,         "clicked", G_CALLBACK(Sponge_Button_Callback),   (gpointer) RFID_PopUp_Pack);
		g_signal_connect(Sponge_Button,         "clicked", G_CALLBACK(DestroyThatWidget),        (gpointer) PopUpWindow);
		g_signal_connect(Forceps_Button,        "clicked", G_CALLBACK(Forceps_Button_Callback),  (gpointer) RFID_PopUp_Pack);
		g_signal_connect(Forceps_Button,        "clicked", G_CALLBACK(DestroyThatWidget),        (gpointer) PopUpWindow);
		g_signal_connect(Scissors_Button,       "clicked", G_CALLBACK(Scissors_Button_Callback), (gpointer) RFID_PopUp_Pack);
		g_signal_connect(Scissors_Button,       "clicked", G_CALLBACK(DestroyThatWidget),        (gpointer) PopUpWindow);
		g_signal_connect(Retractor_Button,      "clicked", G_CALLBACK(Retractor_Button_Callback),(gpointer) RFID_PopUp_Pack);
		g_signal_connect(Retractor_Button,      "clicked", G_CALLBACK(DestroyThatWidget),        (gpointer) PopUpWindow);

	}

	else
	{
		PopUpLabel = gtk_label_new("Are you sure that you want to remove that tag?");
		OkButton     = gtk_button_new_with_label("Yes");
        	CancelButton = gtk_button_new_with_label("No");
		gtk_grid_attach (GTK_GRID(Grid), PopUpLabel,    0,0,2,1);
		gtk_grid_attach (GTK_GRID(Grid), OkButton,      0,1,1,1);
        	gtk_grid_attach (GTK_GRID(Grid), CancelButton,  1,1,1,1);
	}

	/* Pack Grid into pop up window */
        gtk_container_add (GTK_CONTAINER (PopUpWindow), Grid);

	/* Put entry into information pack */
	RFID_PopUp_Pack->Entry = Entry;

	/* Callback functions setup */
        g_signal_connect(G_OBJECT(PopUpWindow), "destroy", G_CALLBACK(gtk_widget_destroy),     NULL);  //code for closing window
        g_signal_connect(CancelButton,          "clicked", G_CALLBACK(DestroyThatWidget),      (gpointer) PopUpWindow);
	g_signal_connect(OkButton, 		"clicked", G_CALLBACK(ReadTag_PopUp_Callback), (gpointer) RFID_PopUp_Pack);
	g_signal_connect(Entry,    		"activate",G_CALLBACK(ReadTag_PopUp_Callback), (gpointer) RFID_PopUp_Pack);
	g_signal_connect(OkButton,              "clicked", G_CALLBACK(DestroyThatWidget),        (gpointer) PopUpWindow);
	g_signal_connect(Entry,                 "activate",G_CALLBACK(DestroyThatWidget),        (gpointer) PopUpWindow);
	gtk_widget_show_all(PopUpWindow);
}

void ReadTag_PopUp_Callback (GtkWidget *widget, RFID_PopUp_Items* RFID_PopUp_Pack)
{
	stringstream tmp;
	/* Get description entered by user */
	string user_description = gtk_entry_get_text( GTK_ENTRY(RFID_PopUp_Pack->Entry) );

	/* Check if adding or removing elements */
	bool add_remove = RFID_PopUp_Pack->add_remove;

	if (add_remove == true)
	{
		/* Attempt to add element to database */
		bool passfail=database.add_element( RFID_PopUp_Pack->final_data , user_description);
		if (passfail == true)
		{
			//cout<<"Element "<< RFID_PopUp_Pack->final_data <<" with description "<<user_description<<" added to database.\n";
			tmp<<"Element "<< RFID_PopUp_Pack->final_data <<" with description "<<user_description<<" added to database.";
	
			/* Output new stuff to main window */
			PrintDatabase2( RFID_PopUp_Pack->Buffer1 );
			PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
		}
		else
		{
			tmp<<"That tag was already in the database.";
			PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
		}
	}
	else
	{
		/* Attempt to remove element from database */
		bool passfail=database.remove_element(RFID_PopUp_Pack->final_data);
		if (passfail == true)
		{
			tmp<<"Tag "<< RFID_PopUp_Pack->final_data <<" removed from database.";
	
			/* Output new stuff to main window */
			PrintDatabase2( RFID_PopUp_Pack->Buffer1 );
			PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
		}
		else
		{
			tmp<<"That tag wasn't in the database.";
			PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
		}
	}
}

void Scalpel_Button_Callback(GtkWidget *widget, RFID_PopUp_Items* RFID_PopUp_Pack)
{
  stringstream tmp;
  /* Attempt to add element to database */
  bool passfail=database.add_element( RFID_PopUp_Pack->final_data , "Scalpel");
  if (passfail == true)
    {
      tmp<<"Element "<< RFID_PopUp_Pack->final_data <<" with description "<<"Scalpel"<<" added to database.";
	
      /* Output new stuff to main window */
      PrintDatabase2( RFID_PopUp_Pack->Buffer1 );
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }
  else
    {
      tmp<<"That tag was already in the database.";
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }  
}
void Sponge_Button_Callback(GtkWidget *widget, RFID_PopUp_Items* RFID_PopUp_Pack)
{
  stringstream tmp;
  /* Attempt to add element to database */
  bool passfail=database.add_element( RFID_PopUp_Pack->final_data , "Sponge");
  if (passfail == true)
    {
      tmp<<"Element "<< RFID_PopUp_Pack->final_data <<" with description "<<"Sponge"<<" added to database.";
	
      /* Output new stuff to main window */
      PrintDatabase2( RFID_PopUp_Pack->Buffer1 );
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }
  else
    {
      tmp<<"That tag was already in the database.";
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }  
}
void Scissors_Button_Callback(GtkWidget *widget, RFID_PopUp_Items* RFID_PopUp_Pack)
{
  stringstream tmp;
  /* Attempt to add element to database */
  bool passfail=database.add_element( RFID_PopUp_Pack->final_data , "Scissors");
  if (passfail == true)
    {
      tmp<<"Element "<< RFID_PopUp_Pack->final_data <<" with description "<<"Scissors"<<" added to database.";
	
      /* Output new stuff to main window */
      PrintDatabase2( RFID_PopUp_Pack->Buffer1 );
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }
  else
    {
      tmp<<"That tag was already in the database.";
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }  
}
void Forceps_Button_Callback(GtkWidget *widget, RFID_PopUp_Items* RFID_PopUp_Pack)
{
  stringstream tmp;
  /* Attempt to add element to database */
  bool passfail=database.add_element( RFID_PopUp_Pack->final_data , "Forceps");
  if (passfail == true)
    {
      tmp<<"Element "<< RFID_PopUp_Pack->final_data <<" with description "<<"Forceps"<<" added to database.";
	
      /* Output new stuff to main window */
      PrintDatabase2( RFID_PopUp_Pack->Buffer1 );
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }
  else
    {
      tmp<<"That tag was already in the database.";
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }  
}
void Retractor_Button_Callback(GtkWidget *widget, RFID_PopUp_Items* RFID_PopUp_Pack)
{
  stringstream tmp;
  /* Attempt to add element to database */
  bool passfail=database.add_element( RFID_PopUp_Pack->final_data , "Retractor");
  if (passfail == true)
    {
      tmp<<"Element "<< RFID_PopUp_Pack->final_data <<" with description "<<"Retractor"<<" added to database.";
	
      /* Output new stuff to main window */
      PrintDatabase2( RFID_PopUp_Pack->Buffer1 );
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }
  else
    {
      tmp<<"That tag was already in the database.";
      PrintToTextView(RFID_PopUp_Pack->Buffer2, tmp.str());
    }  
}





gpointer BluetoothThread(Bluetooth_Thread_Items* Bluetooth_Pack)
{
	/* Set up bluetooth communication terminal */
	int bluetooth_filestream = open("/dev/rfcomm0", O_RDWR | O_NOCTTY | O_NDELAY);
	//int bluetooth_filestream = open("/dev/rfcomm0", O_RDWR | O_NOCTTY | O_NDELAY);
        if (bluetooth_filestream == -1)
        {
                printf("Error - Unable to open Bluetooth Connection\n");
        }
        struct termios options;
        tcgetattr(bluetooth_filestream, &options);
        options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
        options.c_iflag = IGNPAR;
        options.c_oflag = 0;
        options.c_lflag = 0;
        //tcflush(bluetooth_filestream, TCIFLUSH);
	//tcflush(bluetooth_filestream, TCOFLUSH);
        tcsetattr(bluetooth_filestream, TCSANOW, &options);

	/* Local variables */
	int rx_length = 0;
	char rx_buffer[2], tx_buffer[2];
	char Mssg_temp;
	char *Mssg_Point_temp;
	stringstream tmp_ss;
	int brightness = 90;
	string temp_string;
	char const *charbuf;

	/* Set Tx buffer end characters */
	tx_buffer[1] = '\r'; tx_buffer[2] = '\n';

	while (true)
	{
		/* See if there is any user input from the interface */
		if (g_async_queue_length(Bluetooth_Pack->Queue) > 0)
		{
			/* Pop Message */
			Mssg_Point_temp = (char*) g_async_queue_try_pop(Bluetooth_Pack->Queue);
			Mssg_temp = *Mssg_Point_temp;
			//cout <<"Received this in bluetooth queue: "<<Mssg_temp<<endl;
			if (Mssg_temp == increase_Mssg)
			{
				/* Increase Brightness */
				if (!(brightness == 100)) brightness += 5;

				/* Transmit bluetooth message */
				tx_buffer[0] = retrieve_char(&brightness);
				write(bluetooth_filestream, "Test: Increasing Brightness. Brightness character:\n\r", 52);
				write(bluetooth_filestream, tx_buffer, 3);

				/* Output to Interface Element */
				tmp_ss<<brightness;
				temp_string = tmp_ss.str();
				tmp_ss.str(std::string() ); tmp_ss.clear();  //clear stringstream & errors
				charbuf = (char*) temp_string.c_str();
				gdk_threads_enter();
				gtk_entry_set_text(GTK_ENTRY(Bluetooth_Pack->Brightness_Entry), charbuf);
				gdk_threads_leave();
				
			}
			else if (Mssg_temp == decrease_Mssg)
			{
				/* Decrease Brightness */
				if (!(brightness == 0)) brightness -= 5;

				/* Transmit bluetooth message */
				tx_buffer[0] = retrieve_char(&brightness);
				write(bluetooth_filestream, "Test: Decreasing Brightness. Brightness character:\n\r", 52);
				write(bluetooth_filestream, tx_buffer, 3);

				/* Output to Interface Element */
				tmp_ss<<brightness;
				temp_string = tmp_ss.str();
				tmp_ss.str(std::string() ); tmp_ss.clear();  //clear stringstream & errors
				charbuf = (char*) temp_string.c_str();
				gdk_threads_enter();
				gtk_entry_set_text(GTK_ENTRY(Bluetooth_Pack->Brightness_Entry), charbuf);
				gdk_threads_leave();
				
				
			}
			else if (Mssg_temp == Lamp_On_Mssg)
			{
				/* Transmit bluetooth message */
				tx_buffer[0] = 'x';
				write(bluetooth_filestream, "Command: Turn Lamp On\n\r", 23);
				write(bluetooth_filestream, tx_buffer, 3);
			}
			else if (Mssg_temp == Lamp_Off_Mssg)
			{
				/* Transmit bluetooth message */
				tx_buffer[0] = 'y';
				write(bluetooth_filestream, "Command: Turn Lamp Off\n\r", 24);
				write(bluetooth_filestream, tx_buffer, 3);
			}
 
		}
		/* Check for messages over bluetooth */
		rx_length = read(bluetooth_filestream, (void*) rx_buffer, 3);
		if (rx_length > 0)
		{
			cout<<"Char received: "<<rx_buffer[0]<<endl;
			if (rx_buffer[0] == 'X')
			{
				gdk_threads_enter();
				gtk_entry_set_text(GTK_ENTRY(Bluetooth_Pack->Status_Entry), "Lamp is On");
				write(bluetooth_filestream, "Acknowledge Lamp On Status\n\r", 28);
				gdk_threads_leave();
			}
			else if (rx_buffer[0] == 'Y')
			{
				gdk_threads_enter();
				gtk_entry_set_text(GTK_ENTRY(Bluetooth_Pack->Status_Entry), "Lamp is Off");
				write(bluetooth_filestream, "Acknowledge Lamp Off Status\n\r", 29);
				gdk_threads_leave();
			}

		}
		sleep(1.5);
	}

	return (NULL);
}

/* Callbacks for main menu buttons involving bluetooth operations */
void Increase_Brightness_CB (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &increase_Mssg);
}

void Decrease_Brightness_CB (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &decrease_Mssg);
}

void Lamp_On_CB (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &Lamp_On_Mssg);
}

void Lamp_Off_CB (GtkWidget *widget, GAsyncQueue *Queue)
{
	/* Push Message into queue */
	g_async_queue_push(Queue, (gpointer) &Lamp_Off_Mssg);
}

/* Function to convert brightness levels to the appropriate bluetooth message */
char retrieve_char(int *brightness)
{
	if (*brightness == 100) return 'A';
	else if (*brightness == 95) return 'B';
	else if (*brightness == 90) return 'C';
	else if (*brightness == 85) return 'D';
	else if (*brightness == 80) return 'E';
	else if (*brightness == 75) return 'F';
	else if (*brightness == 70) return 'G';
	else if (*brightness == 65) return 'H';
	else if (*brightness == 60) return 'I';
	else if (*brightness == 55) return 'J';
	else if (*brightness == 50) return 'K';
	else if (*brightness == 45) return 'L';
	else if (*brightness == 40) return 'M';
	else if (*brightness == 35) return 'N';
	else if (*brightness == 30) return 'O';
	else if (*brightness == 25) return 'P';
	else if (*brightness == 20) return 'Q';
	else if (*brightness == 15) return 'R';
	else if (*brightness == 10) return 'S';
	else if (*brightness == 05) return 'T';
	else if (*brightness == 00) return 'U';
	else return 'C';
}
