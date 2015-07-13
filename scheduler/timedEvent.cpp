#include "timedEvent.h"
#include "../weather/weather.h"

timedEvent::timedEvent(string passedCommand, string passedValue, int dayIn, int hourIn, int minIn, string nameIn, bool removeAft, config *passedSettings) {
	command = passedCommand;
	value = passedValue;
	name = nameIn;
	day = dayIn;
	hour = hourIn;
	min = minIn;
	removeAfterExec = removeAft;	
	runOnSunrise = false;
	runOnSunset = false;
	settings = passedSettings;
}
string timedEvent::print() {
	string response;
	char line[256];
	//fprintf(stdout, "%s: Days:%i Hour:%i Min:%i ",name.c_str(),day,hour,min);
	sprintf(line, "<event><name>%s</name><command>%s</command>",name.c_str(),command.c_str());
	response = line;
	sprintf(line, "<day>%.7i</day><time>%.2i:%.2i</time>",day,hour,min);
	response.append(line);
	if(removeAfterExec) {
		sprintf(line,"<removeAfter>true</removeAfter>");
	} else {
		sprintf(line,"<removeAfter>false</removeAfter>");
	}
        response.append(line);
        if(runOnSunrise) {
		sprintf(line,"<sunrise>true</sunrise>");
	} else {
		sprintf(line,"<sunrise>false</sunrise>");
	}
        response.append(line);
        if(runOnSunset) {
		sprintf(line,"<sunset>true</sunset></event>\n");
	} else {
		sprintf(line,"<sunset>false</sunset></event>\n");
	}
        response.append(line);
        return(response);
}
string timedEvent::getXML() {
	string response;
	char line[256];
	sprintf(line, "<name>%s</name>\n",name.c_str());
	sprintf(line, "%s<command>%s</command>\n" , line, command.c_str());
	sprintf(line, "%s<day>%i</day>\n<hour>%i</hour>\n<min>%i</min>\n",line, day,hour,min);
	response = line;
	if(removeAfterExec) {
		sprintf(line,"<remove>true</remove>\n");
	} else {
		sprintf(line,"<remove>false</remove>\n");
	}
	response.append(line);
	if(runOnSunrise) {
		sprintf(line,"<OnSunrise>true</OnSunrise>\n");
	} else {
		sprintf(line,"<OnSunrise>false</OnSunrise>\n");
	}
	response.append(line);
	if(runOnSunset) {
		sprintf(line,"<OnSunset>true</OnSunset>\n");
	} else {
		sprintf(line,"<OnSunset>false</OnSunset>\n");
	}
	response.append(line);
	return(response);
}
void timedEvent::updateTime(int dayIn, int hourIn, int minIn) {
	day = dayIn;
	hour = hourIn;
	min = minIn;
}
string timedEvent::getName() {
	return(name);
}
string timedEvent::getCommand() {
	return(command);
}
string timedEvent::getValue() {
	return(value);
}
void timedEvent::onSunrise() {
	runOnSunrise = true;
}
void timedEvent::onSunset() {
	runOnSunset = true;
}
void timedEvent::execute(plm* interface) {
	if(name == "weather") {
		if(command == "update") {
			theWeather->updateData();
		} else {
			fprintf(stderr, "Unknown weather command: %s\n", command.c_str());
		}
	}else {
		device *myDevice;
		myDevice = settings->getDevice(name);
		myDevice->execute(command, value);
	}
}
void timedEvent::setWeather(weather* newWeather) {
	theWeather = newWeather;
}
bool timedEvent::removeAfter() {
	return(removeAfterExec);
}
bool timedEvent::checkTime() {
	time_t rawTime;
	bool result;
	struct tm *currentTime,*sunTime;
	rawTime = time(NULL);
	int totalMins, totalCheck;
	currentTime = localtime(&rawTime);
	result = checkDay((currentTime->tm_wday + 1),day);
	totalMins = currentTime->tm_hour * 60 + currentTime->tm_min; 
	if(runOnSunset) {
		sunTime = theWeather->getSunSetTime();
		totalCheck = sunTime->tm_hour * 60 + sunTime->tm_min + min;
	} else if(runOnSunrise) {
		sunTime = theWeather->getSunRiseTime();
		totalCheck = sunTime->tm_hour * 60 + sunTime->tm_min + min;
	} else {
		totalCheck = hour * 60 + min;
	}
	result = result && (totalCheck == totalMins);
	return(result);
}
bool timedEvent::checkDay(int currentDay, int binaryDay) {
	//current day is 1-7 in days from sunday
	//binary day is a 7 digit binary form of the dates
	int checkDay;
	for(int i=1;i<=7;i++) {
		checkDay = i * (binaryDay % 10);
		binaryDay = binaryDay / 10;
		if(currentDay == checkDay) return(true);
	}
	return(false);
}
