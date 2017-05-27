#ifndef __RFID_data_H_INCLUDED //gaurd class definition
#define __RFID_data_H_INCLUDED

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//Construct element which consists of an id number and a name
struct element 
{
unsigned long long int id_number;
string item_name; 
};

class RFID_data  //the RFID tag numbers are constructed in a class called RFID_data
{
public:
RFID_data(); //constructor for RFID database
~RFID_data();  //destructor for RFID database
bool clear_database();  //operation to clear database
bool add_element(unsigned long long int id, string name);  //operation to add ID to database
bool remove_element(unsigned long long int id);  //operation to remove ID from database
bool remove_index(int id); //operation to remove element from database by index number on interface 
element read_element(int num);  //operation to read ID number from the database
int size();  //reads out number of elements in vector
bool check_id(unsigned long long int id); //checks if an element is in the database

protected:
vector<element> database_elements;  //creates a vector of elements
};

#endif
