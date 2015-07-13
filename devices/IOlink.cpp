#include "IOlink.h"
#include <string.h>
#include <stdio.h>

IOLink::IOLink(plm* dev,const string& configName,const string& configAddress, int numHops, const string& myRoom, const string& myLoad, const string& myDescription) {
	hops = numHops;
	address = configAddress;
	name = configName;
	interface = dev;
	state = "off";
	logfile = stdout;
	type = "IO Link";
	room = myRoom;
	load = (myLoad == "yes") ? true : false;
	description = myDescription;
}

void IOLink::on() {
	char *command;
	command = new char[30];
	sprintf(command, "0262%s%.2X11FF",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void IOLink::execute(const string& command, const string& value) {
	if(command.find("on") == 0) {
		on();
	} else if(command == "off") {	
		off();
	} else if(command == "update") {
		status();
	} else if(command == "ping") {
		ping();
	} else if(command == "status") {
	} else {
		fprintf(stderr, "Unknown Command: %s\n", command.c_str());
	}
}	
