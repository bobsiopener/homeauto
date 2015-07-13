#ifndef RESPONDER_H_
#define RESPONDER_H_
#include "plm.h"
#include "config.h"
#include "./devices/device.h"
#include "./scheduler/scheduler.h"
#include "cmdline.h"
class network;

class responder
{
	public:
		responder();
		responder(int);
		~responder();
		void handlePlmEvents();
		void handleTripEvents();
		void setInterface(plm*);
		void setConfig(config*);
		void setWeather(weather*);
		void setLogFile(char*);
		void setThreaded(bool);
		void updateAllDevices();
		void setNetwork(network*);
		void setTerminal(cmdline*);
		string setVerbose(int);
		void setScheduler(scheduler*);
		string enable();
		string disable();
		bool isRunning();
		string status();
	private:
		void findDevice();
		plm *interface;
		scheduler *timer;
		string event;
		string action;
		string deviceName;
		string response;
		void execResponses();
		config *settings;
		device *currentActor;
		device *currentResponder;
		FILE *logfile;
		int verbose;
		bool keepGoing;
		bool running;
		bool threaded;
		weather *theWeather;
		network *networkInterface;
		cmdline *terminal;
};

#endif
