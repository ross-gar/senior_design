all: RFID_data.o Funcs.o Main.o	
	g++ RFID_data.o Funcs.o Main.o -o project -Wall `pkg-config --cflags --libs gtk+-3.0`

Main.o: Main.cpp
	g++ Main.cpp -c -Wall `pkg-config --cflags --libs gtk+-3.0`

Funcs.o: Funcs.cpp
	g++ Funcs.cpp -c -Wall `pkg-config --cflags --libs gtk+-3.0`

RFID_data.o: RFID_data.cpp
	g++ RFID_data.cpp -c -Wall `pkg-config --cflags --libs gtk+-3.0`