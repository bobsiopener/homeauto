#include "thermostat.h"
#include <string.h>
#include <stdio.h>

thermostat::thermostat(plm* dev,const string& configName,const string& configAddress, int numHops, const string& myRoom, const string& myLoad, const string& myDescription) {
	hops = numHops;
	address = configAddress;
	name = configName;
	interface = dev;
	state = "off";
	logfile = stdout;
	type = "Thermostat";
	temp = 71;
	heat = 70;
	cool = 72;
	mode = "off";
	fan = "auto";
	room = myRoom;
	load = (myLoad == "yes") ? true : false;
	description = myDescription;
}
string thermostat::status() {
	string response;
	char line[256];
	sprintf(line, "temp:%i\nheat:%i\ncool:%i\nmode:%s\nfan:%s",temp,heat,cool,mode.c_str(),fan.c_str());
	response = line;
	return(response);
}
void thermostat::execute(const string& command, const string& value) {
	if(command.find("setHeatTemp") == 0) {
		int level;
		if(value != "") {
			level = atoi(value.c_str());
			setHeatTemp(level);
		} else {
			//Error this needs a value
			fprintf(stderr, "setHeatTemp needs a tempature\n");
		}
	} else if(command == "setCoolTemp") {	
		int level;
		if(value != "") {
			level = atoi(value.c_str());
			setCoolTemp(level);
		} else {
			//Error this needs a value
			fprintf(stderr, "setCoolTemp needs a tempature\n");
		}
	} else if(command == "setMode") {
		if(value.find("Heat") == 0) {
			setModeHeat();
		} else if(value.find("Cool") == 0) {
			setModeCool();
		} else if(value.find("Auto") == 0) {
			setModeAuto();
		} else if(value.find("Off") == 0) {
			setModeOff();
		}
	} else if(command == "setFan") {
		if(value.find("on") == 0) {
			setFanOn();
		} else if(value.find("off") == 0) {
			setFanAuto();
		}  else if(value.find("auto") == 0) {
			setFanAuto();
		}			
	} else if(command == "status") {
	} else {
		fprintf(stderr, "Unknown Command: %s\n", command.c_str());
	}
}
void thermostat::setCoolTemp(int level) {
	char *command;
	command = new char[50];
	level = (level*255)/50;
	//sprintf(command, "0262%s%.2i6C%.2X0000000000000000000000000000",address.c_str(),hops, level);
	sprintf(command, "0262%s%.2X6C%.2X",address.c_str(),hops, level);
	interface->send(command);
	delete[] command;
}

void thermostat::setHeatTemp(int level) {
	char *command;
	command = new char[50];
	level = (level*255)/50;
	//sprintf(command, "0262%s1A6D%.2X000000000000000000000000000D",address.c_str(), level);
	sprintf(command, "0262%s%.2X6D%.2X",address.c_str(),hops, level);
	interface->send(command);
	delete[] command;
}

void thermostat::setModeHeat() {
	char *command;
	command = new char[50];
	//sprintf(command, "0262%s1A6B040000000000000000000000000091",address.c_str());
	sprintf(command, "0262%s%.2X6B04",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void thermostat::setModeCool() {
	char *command;
	command = new char[50];
	//sprintf(command, "0262%s1A6B050000000000000000000000000090",address.c_str());
	sprintf(command, "0262%s%.2X6B05",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void thermostat::setModeAuto() {
	char *command;
	command = new char[50];
	//sprintf(command, "0262%s1A6B06000000000000000000000000008F",address.c_str());
	sprintf(command, "0262%s%.2X6B06",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void thermostat::setModeOff() {
	char *command;
	command = new char[50];
	//sprintf(command, "0262%s1A6B09000000000000000000000000008C",address.c_str());
	sprintf(command, "0262%s%.2X6B09",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void thermostat::setFanOn() {
	char *command;
	command = new char[50];
	//sprintf(command, "0262%s1A6B07000000000000000000000000008E",address.c_str());
	sprintf(command, "0262%s%.2X6B07",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void thermostat::setFanAuto() {
	char *command;
	command = new char[50];
	//sprintf(command, "0262%s1A6B08000000000000000000000000008D",address.c_str());
	sprintf(command, "0262%s%.2X6B04",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void thermostat::getMode() {
	char *command;
	command = new char[50];
	//sprintf(command, "0262%s0A6B02",address.c_str());
	sprintf(command, "0262%s%.2X6B02",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

void thermostat::getTemp() {
	char *command;
	command = new char[50];
	sprintf(command, "0262%s1A6A00",address.c_str());
	sprintf(command, "0262%s%.2X6A00",address.c_str(),hops);
	interface->send(command);
	delete[] command;
}

string thermostat::getAction(const string& command) {
	string actionCode,action,level;
	actionCode = command.substr(18,2);
	if(actionCode == "6A") {
		action = "Temp";
		level = command.substr(20,2);
		//level is in hex, convert to decimal
		char levelChar[4];
		char *pEnd;
		int decLevel;
		strcpy(levelChar,level.c_str());
		decLevel = (int)strtol(levelChar, &pEnd, 16);
		decLevel = (int)(((float)decLevel / 255 ) * 100 + 0.5);
		sprintf(levelChar,"%i",decLevel);
		action.append(levelChar);
		writeLog(logfile, "ON %s\n", level.c_str());
	} else if(actionCode == "6B") {
		action = "Mode";
		level = command.substr(20,2);
		writeLog(logfile, "Mode \n");
	} else if(actionCode == "06") {
		action = "ACK";
		writeLog(logfile, "ACK\n");
	} else if(actionCode == "11") {
		action = "HeatOn";
		writeLog(logfile, "HeatOn\n");
	}  else if(actionCode == "13") {
		action = "HeatOff";
		writeLog(logfile, "HeatOff\n");
	}  else {
		writeLog(logfile, "unknown command %s\n", command.c_str());
		//actionCode = command.substr(18,4);
		action = "UNKNOWN";
	}
	setState(action);
	return(action);
}

string thermostat::getXML() {
	string XML;
	XML = "";
	char line[256];
	sprintf(line, "<device name=%s>\n",name.c_str());
	sprintf(line, "%s<address>%s</address>\n" , line, address.c_str());
	sprintf(line, "%s<state>%s</state>\n</device>\n",line, state.c_str());
	XML.append(line);
	return(XML);
}

void thermostat::print() {
	writeLog(logfile, "%s %s %s\n", name.c_str(), address.c_str(), state.c_str());
}
