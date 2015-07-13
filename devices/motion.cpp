#include "motion.h"
#include <string.h>
#include <stdio.h>

motion::motion(plm *dev, const string &configName, const string &configAddress,
               int numHops, const string &myRoom, const string &myLoad,
               const string &myDescription) {
  hops = numHops;
  address = configAddress;
  name = configName;
  interface = dev;
  state = "off";
  logfile = stdout;
  type = "Motion";
  room = myRoom;
  load = (myLoad == "yes") ? true : false;
  description = myDescription;
}

string motion::getAction(const string &command) {
  string actionCode, action, level;
  actionCode = command.substr(18, 2);
  if (actionCode == "11") {
    action = "on";
    level = command.substr(20, 2);
    // level is in hex, convert to decimal
    char levelChar[4];
    char *pEnd;
    int decLevel;
    strcpy(levelChar, level.c_str());
    decLevel = (int)strtol(levelChar, &pEnd, 16);
    decLevel = (int)(((float)decLevel / 255) * 100 + 0.5);
    sprintf(levelChar, "%i", decLevel);
    action.append(levelChar);
    writeLog(logfile, "ON %s\n", level.c_str());
  } else if (actionCode == "13") {
    action = "off";
    writeLog(logfile, "OFF\n");
  } else if (actionCode == "06") {
    action = "ACK";
    writeLog(logfile, "ACK\n");
  } else {
    writeLog(logfile, "unknown command %s\n", command.c_str());
    // actionCode = command.substr(18,4);
    action = "UNKNOWN";
  }
  setState(action);
  return (action);
}

void motion::execute(const string &command, const string &value) {
  if (command == "status") {
  }
  fprintf(stderr, "Unknown Command: %s\n", command.c_str());
}
