#include <string>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fstream>
#include "../utils.h"
#include "xml.h"
#include <stdlib.h>
#include "../scheduler/scheduler.h"

#ifndef WEATHER_H_
#define WEATHER_H_
using namespace std;
class scheduler;
class weather{
	public:
		weather(scheduler*);
		~weather();
		struct tm* getSunSetTime();
		struct tm* getSunRiseTime();
		string getData();
		string getXML();
		bool isDark();
		bool isLight();
		void updateData();
		void printData();
		void configure();
	private:
		string site;
		char* configFile;
		string API;
		string downloadSite();
		xml doc;
		void parseData(string&);
		struct tm *sunset;
		struct tm *sunrise;
		struct tm *updated;
		float temp;
		float wind_mph;
		int nightAdjuster;
		int morningAdjuster;
		string observedWeather;
		stringMap weatherCondition;
		scheduler *timer;
};

#endif
