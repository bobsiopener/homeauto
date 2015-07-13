#ifndef TIMEDEVENT_H_
#define TIMEDEVENT_H_
#include <time.h>
#include <string.h>
#include <fstream>
#include "../config.h"
#include "../plm.h"
class weather;
class timedEvent
{
	public:
		timedEvent(string, string, int,int,int,string,bool removeAft, config*);
		void updateTime(int,int,int);
		bool checkTime();
		void execute(plm*);
		string getName();
		string getCommand();
		string getValue();
		bool removeAfter();
		void setWeather(weather*);
		void onSunset();
		void onSunrise();
		string print();
		string getXML();
	private:
		string name;
		string command;
		string value;
		//time_t lastExecuted;
		bool checkDay(int,int);
		int day; //day is a binary rep of the day where each bit is a day of week
		int hour;
		int min;
		weather *theWeather;
		bool removeAfterExec;
		bool runOnSunrise;
		bool runOnSunset;
		config *settings;
};
#endif
