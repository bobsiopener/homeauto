#include "weather.h"

weather::weather(scheduler* alarm) {
	char fileName[] = "/etc/homeauto/weather.conf";
	configFile = new char[strlen(fileName) + 1];
	strcpy(configFile,fileName);
	configure();
	//site = "http://api.wunderground.com/api/7036a22565158b29/astronomy/conditions/lang:EN/q/18954.xml";
	//API = "7036a22565158b29";
	sunset = new struct tm;
	sunrise = new struct tm;
	updated = new struct tm;
	timer = alarm;
	observedWeather = "";
	nightAdjuster = 40;
	morningAdjuster = 0;
}

weather::~weather() {
delete sunset;
delete sunrise;
}
void weather::configure() {
	ifstream In;
	string currentLine,name,value;
	In.open(configFile);
	while(!In.eof()) {
		getline(In, currentLine);
		if(!isBlank(currentLine)) {
			if(!startsWith(currentLine,"#")) {
				name = chopWord(currentLine, ":");
				value = chopWord(currentLine);
				if(name == "SITE") {
					site = value;
				}
				else if(name == "API") {
					API = value;
				}
				else {
					//weather condition
					weatherCondition[name] = value;
				}
			}
		}
	}
	In.close();
}
struct tm* weather::getSunSetTime() {
	return(sunset);
}
struct tm* weather::getSunRiseTime() {
	return(sunrise);
}
bool weather::isDark() {
	time_t rawTime;
	bool result;
	int weatherAdjuster = 0 ;
	struct tm *currentTime;
	rawTime = time(NULL);
	currentTime = localtime(&rawTime);
	if(weatherCondition[observedWeather].find("Dark") == 0) weatherAdjuster = 180;
	if(weatherCondition[observedWeather].find("Dark") == 0) {
		result = true;
	} else {
		result = false;
	}
	result = result || ((sunset->tm_hour * 60) + sunset->tm_min - weatherAdjuster) < ((currentTime->tm_hour * 60) + currentTime->tm_min);
	result = result || ((sunrise->tm_hour * 60) + sunrise->tm_min + weatherAdjuster) > ((currentTime->tm_hour * 60) + currentTime->tm_min);
	return(result);
}
bool weather::isLight() {
	time_t rawTime;
	bool result;
	int weatherAdjuster = 0 ;
	struct tm *currentTime;
	rawTime = time(NULL);
	currentTime = localtime(&rawTime);
	if(weatherCondition[observedWeather].find("Dark") == 0) weatherAdjuster = 180;
	if(weatherCondition[observedWeather].find("Dark") == 0) {
		result = false;
	} else {
		result = true;
	}
	result = result && ((sunset->tm_hour * 60) + sunset->tm_min - weatherAdjuster - nightAdjuster) > ((currentTime->tm_hour * 60) + currentTime->tm_min);
	result = result && ((sunrise->tm_hour * 60) + sunrise->tm_min + weatherAdjuster + morningAdjuster) < ((currentTime->tm_hour * 60) + currentTime->tm_min);
	return(result);
}
void weather::updateData() {
	string siteContents;
	siteContents = downloadSite();
	parseData(siteContents);
	time_t rawTime;
        rawTime = time(NULL);
        updated = localtime(&rawTime);
	//set to update weather in 15 minutes
	timer->delayedAction("weather","","update",15);
}
string weather::downloadSite() {
	string command, results;
	struct tm *now;
	command = "wget -q -O - ";
	command.append(site);
	time_t rawTime;
        rawTime = time(NULL);
        now = localtime(&rawTime);
	fprintf(stdout,"%i/%i %i:%i ",now->tm_mon,now->tm_mday,now->tm_hour,now->tm_min);
	fprintf(stdout,"Executing %s\n", command.c_str());
	results = shell(command);
	return(results);
}
void weather::parseData(string& source) {
	doc.load(source);
	sunset->tm_hour = atoi(doc.child("response")->child("moon_phase")->child("sunset")->child("hour")->value().c_str());
	sunset->tm_min = atoi(doc.child("response")->child("moon_phase")->child("sunset")->child("minute")->value().c_str());
	sunrise->tm_hour = atoi(doc.child("response")->child("moon_phase")->child("sunrise")->child("hour")->value().c_str());
	sunrise->tm_min = atoi(doc.child("response")->child("moon_phase")->child("sunrise")->child("minute")->value().c_str());
	temp = (float)atof(doc.child("response")->child("current_observation")->child("temp_f")->value().c_str());
	wind_mph = (float)atof(doc.child("response")->child("current_observation")->child("wind_mph")->value().c_str());
	observedWeather = doc.child("response")->child("current_observation")->child("weather")->value();
}
void weather::printData() {
	fprintf(stdout,"Sunset time: %i:%.2i\n",sunset->tm_hour,sunset->tm_min);
	fprintf(stdout,"Sunrise time: %i:%.2i\n",sunrise->tm_hour,sunrise->tm_min);
	if(isDark()) {
		fprintf(stdout,"It is dark\n");
	} else {
		fprintf(stdout,"It is not dark\n");
	}
	if(isLight()) {
		fprintf(stdout, "It is Light\n");
	} else {
		fprintf(stdout, "It is not light\n");
	}
	fprintf(stdout,"It is %.1fF degrees outside\n",temp);
	fprintf(stdout,"There is %.1fmph of wind outside\n",wind_mph);
	fprintf(stdout,"The weather outside is %s\n",observedWeather.c_str());
}
string weather::getData() {
	string response;
	char line[256];
	sprintf(line,"Weather:%i;%.2i",sunset->tm_hour,sunset->tm_min);
	response = line;
	sprintf(line,":%i;%.2i",sunrise->tm_hour,sunrise->tm_min);
	response.append(line);
	sprintf(line,":%.1fF",temp);
	response.append(line);
	sprintf(line,":%.1fmph",wind_mph);
	response.append(line);
	sprintf(line,":%s\n",observedWeather.c_str());
	response.append(line);
	return(response);
}
string weather::getXML() {
	string response;
	char line[256];
	response = "<weather>";
	sprintf(line,"<SunsetTime>%i:%i</SunsetTime>",sunset->tm_hour,sunset->tm_min);
	response.append(line);
	sprintf(line,"<SunriseTime>%i:%i</SunriseTime>",sunrise->tm_hour,sunrise->tm_min);
	response.append(line);
	sprintf(line,"<temp>%.1fF</temp>",temp);
	response.append(line);
	sprintf(line,"<wind>%.1fmph</wind>",wind_mph);
	response.append(line);
	sprintf(line,"<visable>%s</visable>",observedWeather.c_str());
	response.append(line);
	sprintf(line,"<updated>%i/%i %i:%i</updated>",updated->tm_mon,updated->tm_mday,updated->tm_hour,updated->tm_min);
        response.append(line);
	response.append("</weather>\n");
	return(response);
}
