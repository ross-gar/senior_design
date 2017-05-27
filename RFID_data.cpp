#include "RFID_data.h"
//#include <vector>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>

using namespace std;

/*Construct element which consists of an id number and a name
struct element 
{
unsigned long long int id_number;
string item_name; 
};*/

RFID_data::RFID_data()
{
//constructor for database
}

RFID_data::~RFID_data()
{
//destructor for RFID class
}

bool RFID_data::clear_database()
{
        database_elements.erase (database_elements.begin(),database_elements.end());  //erase every elm$ 
        if ( database_elements.size()==0 )
                return true; //return true if database sucesfully cleared
        else
                return false;  //return false if there was an error     
}
bool RFID_data::add_element(unsigned long long int id, string name)
{
        for (unsigned int i=0; i<database_elements.size(); i++)  //this for loop checks to see if $
        {
                if (database_elements.at(i).id_number==id)
                {
                return false;  //return false if number is already in database
                }
        }
	element a;
	a.id_number = id;
	a.item_name = name;
        database_elements.push_back(a);  //add element to the id number vector
        return true;  //return true if element added to database
}

bool RFID_data::remove_element(unsigned long long int id)
{
        for (unsigned int i=0; i<database_elements.size(); i++)
        {
                if (database_elements.at(i).id_number==id)
                {
                        database_elements.erase(database_elements.begin()+i);  //erase element f$
                        return true;  //return true if element is found and erased
                }
        }
        return false;  //return false if elememnt not found
}

bool RFID_data::remove_index(int id)
{
	if (id>=0)
	{
	database_elements.erase(database_elements.begin()+id);
	return true;
	}
	else
		return false;
}

element RFID_data::read_element(int num)
{
        return database_elements.at(num);  //reads out vector element at index num
}

int RFID_data::size()
{
        return database_elements.size();  //reads out the ID vector size (number of IDs)
}

bool RFID_data::check_id(unsigned long long int id)
{
	element test;
	for (unsigned int i=0; i<database_elements.size(); i++)
	{
		test = database_elements.at(i);
		if (test.id_number == id)
			return true;
	}
	return false;
}