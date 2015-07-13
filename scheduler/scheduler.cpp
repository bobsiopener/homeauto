#include "scheduler.h"
#include "../utils.h"

scheduler::scheduler() {
  logfile = stdout;
  verbose = 0;
  keepGoing = true;
  running = false;
  threaded = true;
}
scheduler::scheduler(int verboseLevel) {
  logfile = stdout;
  verbose = verboseLevel;
  keepGoing = true;
  running = false;
  threaded = true;
}
string scheduler::enable() {
  writeLog(logfile, "Enabling Scheduler\n");
  keepGoing = true;
  response = "ACK";
  return (response);
}
string scheduler::disable() {
  writeLog(logfile, "Disabling Scheduler\n");
  keepGoing = false;
  response = "Disabling Scheduler\n";
  return (response);
}
void scheduler::setThreaded(bool newThreaded) { threaded = newThreaded; }
bool scheduler::isRunning() { return (running); }
string scheduler::setVerbose(int verboseLevel) {
  writeLog(logfile, "set verbose to level %i\n", verboseLevel);
  verbose = verboseLevel;
  response = "ACK";
  return (response);
}
scheduler::~scheduler() { pclose(logfile); }
void scheduler::setConfig(config *configFile) { settings = configFile; }
void scheduler::handleSchedule(plm *interface) {
  if (verbose > 0)
    writeLog(logfile, "Handling schedule...\n");
  running = true;
  eventMap::iterator thisEvent;
  bool removed;
  while (keepGoing) {
    if (!threaded)
      keepGoing = false;
    removed = false;
    for (thisEvent = allEvents.begin();
         thisEvent != allEvents.end() && !removed; thisEvent++) {
      currentEvent = thisEvent->second;
      removed = checkSchedule(interface);
    }
    sleep(30); // sleep 30 seconds, it's ok if it runs twice
  }
  if (!threaded)
    keepGoing = true;
  running = false;
  if (verbose > 0) {
    writeLog(logfile, "Exiting Scheduler\n");
    fprintf(stdout, "Exiting Scheduler\n");
  }
}
string scheduler::print() {
  eventMap::iterator thisEvent;
  response = "";
  writeLog(logfile, "Scheduler Print\n");
  for (thisEvent = allEvents.begin(); thisEvent != allEvents.end();
       thisEvent++) {
    currentEvent = thisEvent->second;
    response.append(currentEvent->print());
  }
  return (response);
}
string scheduler::getXML() {
  eventMap::iterator thisEvent;
  response = "<schedule>\n";
  writeLog(logfile, "Scheduler get XML\n");
  for (thisEvent = allEvents.begin(); thisEvent != allEvents.end();
       thisEvent++) {
    currentEvent = thisEvent->second;
    response.append(currentEvent->getXML());
  }
  response.append("</schedule>\n");
  return (response);
}
void scheduler::setLogFile(char *logFileName) {
  logfile = fopen(logFileName, "w");
  if (logfile == NULL) {
    fprintf(stderr, "Error opening log file: %s using stdout for logfile\n",
            logFileName);
    logfile = stdout;
  }
  setbuf(logfile, NULL);
}
void scheduler::read(char *filename) {
  /* format for config follow is:
   * deviceName binaryDays hour min command
   */
  ifstream In;
  string currentLine, lineDays, lineHour, lineMin, lineName;
  string command, value;
  int day, hour, min;
  In.open(filename);
  while (!In.eof()) {
    getline(In, currentLine);
    if (!isBlank(currentLine) && !startsWith(currentLine, "#")) {
      lineName = chopWord(currentLine);
      lineDays = chopWord(currentLine);
      lineHour = chopWord(currentLine);
      lineMin = chopWord(currentLine);
      command = chopWord(currentLine);
      value = chopWord(currentLine);
      // translate
      day = atoi(lineDays.c_str());
      if (lineHour == "sunset" || lineHour == "sunrise") {
        hour = 0;
      } else {
        hour = atoi(lineHour.c_str());
      }
      min = atoi(lineMin.c_str());
      writeLog(logfile, "adding %s %s %i %i %i %s\n", command.c_str(),
               value.c_str(), day, hour, min, lineName.c_str());
      currentEvent = addEvent(command, value, day, hour, min, lineName, false);
      if (lineHour == "sunset") {
        currentEvent->onSunset();
      } else if (lineHour == "sunrise") {
        currentEvent->onSunrise();
      }
      currentEvent->setWeather(theWeather);
    }
  }
}
string scheduler::scheduleEvent(const string &line) {
  /* format for config follow is:
   * deviceName binaryDays hour min command
   */
  string currentLine, lineDays, lineHour, lineMin, lineName;
  string command, value;
  int day, hour, min;
  currentLine = line;
  if (!isBlank(currentLine)) {
    lineName = chopWord(currentLine);
    lineDays = chopWord(currentLine);
    lineHour = chopWord(currentLine);
    lineMin = chopWord(currentLine);
    command = chopWord(currentLine);
    value = chopWord(currentLine);
  }
  // translate
  day = atoi(lineDays.c_str());
  if (lineHour == "sunset" || lineHour == "sunrise") {
    hour = 0;
  } else {
    hour = atoi(lineHour.c_str());
  }
  min = atoi(lineMin.c_str());
  currentEvent = addEvent(command, value, day, hour, min, lineName, false);
  if (lineHour == "sunset") {
    currentEvent->onSunset();
  } else if (lineHour == "sunrise") {
    currentEvent->onSunrise();
  }
  currentEvent->setWeather(theWeather);
  return ("Success");
}

bool scheduler::checkSchedule(plm *interface) {
  // returns true if event was removed.
  bool removed = false;
  if (currentEvent != NULL) {
    if (currentEvent->checkTime()) {
      if (verbose >= 1) {
        time_t rawTime;
        struct tm *currentTime;
        rawTime = time(NULL);
        currentTime = localtime(&rawTime);
        writeLog(logfile, "Executing schedule for %s : %s @ %i:%.2i\n",
                 currentEvent->getName().c_str(),
                 currentEvent->getCommand().c_str(), currentTime->tm_hour,
                 currentTime->tm_min);
      }
      // removing the event before execution so that the timedEvent can recreate
      // itself
      if (currentEvent->removeAfter())
        removeEvent(currentEvent);
      currentEvent->execute(interface);
      if (currentEvent->removeAfter()) {
        delete currentEvent;
        removed = true;
      }
      // There was a problem when trying to write to the PLM too soon after
      // reading from it
      // I would like to add a sleep here for 1 sec to prevent writes too
      // quickly
      sleep(1);
    }
  } else {
    fprintf(stderr, "Scheduler: currentEvent is NULL in checkSchedule\n");
  }
  return (removed);
}
void scheduler::delayedAction(const string &name, const string &value,
                              const string &action, int delay) {
  // Add an event <delay> minutes from now
  int day, hour, min;
  time_t rawTime;
  struct tm *currentTime;
  rawTime = time(NULL);
  currentTime = localtime(&rawTime);
  day = (currentTime->tm_wday + 1);
  hour = currentTime->tm_hour;
  min = currentTime->tm_min;
  min = min + delay;
  if (min >= 60) {
    min = min % 60;
    hour++;
  }
  if (hour >= 24) {
    hour = hour % 24;
    day++;
  }
  if (day > 7) {
    day = day % 7;
  }
  day = binaryDay(day);
  addEvent(action, value, day, hour, min, name, true);
}
void scheduler::setWeather(weather *weatherPtr) { theWeather = weatherPtr; }
int scheduler::binaryDay(int day) {
  int binary;
  binary = (int)pow(10, day - 1);
  return (binary);
}
timedEvent *scheduler::addEvent(string command, string value, int day, int hour,
                                int min, string name, bool removeAfter) {
  timedEvent *theOneToAdd;
  string index;
  eventMap::iterator thisEvent;
  index = name;
  index.append(command);
  thisEvent = allEvents.find(index);
  // check if event exists, if it does just update the time
  if (thisEvent == allEvents.end()) {
    if (verbose >= 2)
      writeLog(logfile,
               "Adding schedule: %s: Day:%i Hour:%i Min:%i command: %s\n",
               name.c_str(), day, hour, min, command.c_str());
    theOneToAdd = new timedEvent(command, value, day, hour, min, name,
                                 removeAfter, settings);
    allEvents[index] = theOneToAdd;
  } else {
    if (verbose >= 2)
      writeLog(logfile,
               "updating schedule: %s: Day:%i Hour:%i Min:%i command: %s\n",
               name.c_str(), day, hour, min, command.c_str());
    theOneToAdd = thisEvent->second;
    theOneToAdd->updateTime(day, hour, min);
  }
  theOneToAdd->setWeather(theWeather);
  return (theOneToAdd);
}
void scheduler::removeEvent(timedEvent *removeMe) {
  if (removeMe != NULL) {
    string index;
    index = removeMe->getName();
    index.append(removeMe->getCommand());
    allEvents.erase(index);
  } else {
    fprintf(stderr, "Scheduler: removeEvent: Event is null\n");
  }
}
string scheduler::status() {
  if (running) {
    // fprintf(stdout,"Running: yes ");
    response = "scheduler Running\n";
  } else {
    // fprintf(stdout,"Running: no ");
    response = "scheduler notRunning\n";
  }
  if (keepGoing) {
    // fprintf(stdout,"Enabled: yes\n");
    response.append("scheduler enabled\n");
  } else {
    // fprintf(stdout,"Enabled: no\n");
    response.append("scheduler disabled\n");
  }
  return (response);
}
