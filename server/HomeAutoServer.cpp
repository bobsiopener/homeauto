#include "plm.h"
#include "config.h"
#include "responder.h"
#include "./devices/device.h"
#include "./scheduler/scheduler.h"
#include "cmdline.h"
#include "command.h"
#include "network.h"
#include "./weather/weather.h"
#include <pthread.h>

plm *insteon;

void *startScheduling(void *threadArg) {
	cout<<"Starting schedule"<<endl;
	scheduler *schedule;
	schedule = (scheduler*)threadArg;
	schedule->setVerbose(1);
	char logfilename[] = "/home/bob/log/homeauto.scheduler.log";
	schedule->setLogFile(logfilename);
	schedule->handleSchedule(insteon);
	return( 0);
}
void *startPlmEvents(void *threadArg) {
	cout<<"Starting Responder Plm"<<endl;
	responder *Events;
	Events = (responder*)threadArg;
	Events->setVerbose(10);
	char logfilename[] = "/home/bob/log/homeauto.insteon.log";
	Events->setLogFile(logfilename);
	Events->handlePlmEvents();
	return(0);
}
void *startTripEvents(void *threadArg) {
	cout<<"Starting Responder Trip"<<endl;
	responder *Events;
	Events = (responder*)threadArg;
	Events->handleTripEvents();
	return(0);
}
void *startCommand(void *threadArg) {
	cout<<"Starting Command Line thread"<<endl;
	cmdline *terminal;
	terminal = (cmdline*)threadArg;
	//read from the terminal and send it to the executer
	terminal->getCommands();
	return(0);
}

void *startNetwork(void *threadArg) {
  cout << "Starting Network Reader" << endl;
  network *socketReader;
  socketReader = (network *)threadArg;
  char logfilename[] = "/home/bob/log/homeauto.network.log";
  socketReader->setLogFile(logfilename);
  socketReader->getCommands();
  return (0);
}
int main() {
	pthread_t threads[5];
	insteon = new plm;
	config *settings;
	
	//read in the devices and other settings from the config file
	settings = new config(insteon);
	char devicesfilename[] = "/etc/homeauto/insteon.conf";
	settings->read(devicesfilename);
	//setup the PLM for talking with all the devices
	insteon->setVerbose(0);
	char logfilename[] = "/home/bob/log/homeauto.plm.log";
	insteon->setLogFile(logfilename);
	char interfacename[] = "/dev/ttyUSB0";
	insteon->openInterface(interfacename);
	insteon->initInterface();
	
	//Create the responder task for handling incoming events
	responder *Events;
	Events = new responder;
	Events->setConfig(settings);
	Events->setInterface(insteon);	
	
	//create the scheduler for handling time based events
	scheduler *Schedule;
	Schedule = new scheduler;
	
	//create Weather Module and load it into the scheduler and events
	weather *theWeather;
	theWeather = new weather(Schedule);
	Schedule->setWeather(theWeather);
	theWeather->updateData();
	Events->setWeather(theWeather);
	
	//set the Schedule config and load the static schedules
	Schedule->setConfig(settings);
	char schedulefilename[] = "/etc/homeauto/schedule.conf";
	Schedule->read(schedulefilename);
	Events->setScheduler(Schedule);
	
	//start the command interperter
	command *interper;
	interper = new command(settings, insteon, Events, Schedule, theWeather);
	
	//initiate network
	network *connections;
	connections = new network(interper, 2020);
	interper->setNetwork(connections);
	Events->setNetwork(connections);

	//initiate the terminal interface
	cmdline *terminal;
	terminal = new cmdline(interper);
	//tell the executer that this is the terminal interface
	terminal->linkCommand();
	Events->setTerminal(terminal);

	//create the thread for handling the schedule
	pthread_create(&threads[0], NULL, startScheduling, Schedule);
	usleep(50000);
	//create the thread for handling the events
	pthread_create(&threads[1], NULL, startPlmEvents, Events);
	usleep(50000);
	//create the thread for handling the network
	pthread_create(&threads[2], NULL, startNetwork, connections);
	usleep(50000);
	//create the interface with the command line
	pthread_create(&threads[3], NULL, startCommand, terminal);
	
	//Update all device details
	Events->updateAllDevices();
	fprintf(stdout,"All Devices updated after bootup\n");
	//settings->printAllDevices();

	pthread_exit(NULL);
	delete connections;
	delete interper;
	delete theWeather;
	delete Schedule;
	delete Events;
	delete settings;
	delete insteon;
	cout<<"Exiting Program"<<endl;
	return(0);
}
