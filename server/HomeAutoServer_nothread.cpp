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
	pthread_exit((void*) 0);
}
void *startPlmEvents(void *threadArg) {
	cout<<"Starting Responder Plm"<<endl;
	responder *Events;
	Events = (responder*)threadArg;
	Events->setVerbose(10);
	char logfilename[] = "/home/bob/log/homeauto.insteon.log";
	Events->setLogFile(logfilename);
	Events->handlePlmEvents();
	pthread_exit((void*) 0);
}
void *updateDevices(void *threadArg) {
	cout<<"Starting Responder Plm"<<endl;
	responder *Events;
	Events = (responder*)threadArg;
	Events->updateAllDevices();
	pthread_exit((void*) 0);
}

void *startTripEvents(void *threadArg) {
	cout<<"Starting Responder Trip"<<endl;
	responder *Events;
	Events = (responder*)threadArg;
	Events->handleTripEvents();
	pthread_exit((void*) 0);
}
void *startCommand(void *threadArg) {
	cout<<"Starting Command Line Reader"<<endl;
	cmdline *terminal;
	terminal = new cmdline((command*)threadArg);
	terminal->linkCommand();
	terminal->getCommands();
	pthread_exit((void*) 0);
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
	char logfilenamePlm[] = "/home/bob/log/homeauto.plm.log";
	insteon->setLogFile(logfilenamePlm);
	char interfacename[] = "/dev/ttyUSB0";
	insteon->openInterface(interfacename);
	insteon->initInterface();


	//Create the responder task for handling incoming events
	responder *Events;
	Events = new responder;
	Events->setConfig(settings);
	Events->setInterface(insteon);	
	Events->setVerbose(10);
	char logfilenameEvent[] = "/home/bob/log/homeauto.insteon.log";
	Events->setLogFile(logfilenameEvent);
	
	//create the scheduler for handling time based events
	scheduler *Schedule;
	Schedule = new scheduler;
	Schedule->setVerbose(1);
	char logfilenameSch[] = "/home/bob/log/homeauto.scheduler.log";
	Schedule->setLogFile(logfilenameSch);
	
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
	
	//start the command interface
	command *interper;
	interper = new command(settings, insteon, Events, Schedule, theWeather);
	
	cmdline *terminal;
	terminal = new cmdline(interper);
	terminal->linkCommand();

	//initiate network
	network *networkInterface;
	networkInterface = new network(interper);
	interper->setNetwork(networkInterface);
	Events->setNetwork(networkInterface);
	char logfilenameNet[] = "/home/bob/log/homeauto.network.log";
	networkInterface->setLogFile(logfilenameNet);

	//pthread_create(&threads[0], NULL, startScheduling, Schedule);
	//usleep(50000);
	//pthread_create(&threads[1], NULL, startPlmEvents, Events);
	//usleep(50000);
	//pthread_create(&threads[2], NULL, startNetwork, connections);
	//pthread_create(&threads[2], NULL, updateDevices, Events);
	//Events->handlePlmEvents();
	//terminal->getCommands();
	networkInterface->getCommands();
	//schedule->handleSchedule(insteon);

	pthread_exit(NULL);
	cout<<"Exiting Program"<<endl;
	return(0);
}
