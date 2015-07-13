#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#include "timedEvent.h"
#include "../config.h"
#include "../utils.h"
#include <string.h>
#include <tr1/unordered_map>
#include <math.h>
#include "../plm.h"
#include "../weather/weather.h"
#include <fstream>

using namespace std;

typedef tr1::unordered_map<string, timedEvent*> eventMap;
class scheduler
{
	public:
		scheduler();
		scheduler(int);
		~scheduler();
		void handleSchedule(plm*);
		void setConfig(config*);
		void setWeather(weather*);
		void read(char*);
		void setLogFile(char*);
		void updateTime();
		bool checkSchedule(plm*);
		string setVerbose(int);
		void delayedAction(const string&,const string&, const string&, int);
		string scheduleEvent(const string&);
		timedEvent* addEvent(string, string, int, int, int, string, bool);
		void removeEvent(timedEvent*);
		string enable();
		string disable();
		void setThreaded(bool);
		bool isRunning();
		string print();
		string getXML();
		string status();
		//int getHour();
	private:
		string response;
		int binaryDay(int);
		config *settings;
		eventMap allEvents;
		timedEvent *currentEvent;
		FILE *logfile;
		int verbose;
		bool keepGoing;
		bool running;
		bool threaded;
		weather *theWeather;
};
#endif
