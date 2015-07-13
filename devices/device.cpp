#include "device.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

device::device(plm *dev, const string &configName, const string &configAddress,
               int numHops, const string &myRoom, const string &myLoad,
               const string &myDescription) {
  hops = numHops;
  address = configAddress;
  name = configName;
  interface = dev;
  state = "off";
  logfile = stdout;
  type = "Generic";
  room = myRoom;
  load = (myLoad == "yes") ? true : false;
  description = myDescription;
}
void device::setLogDir(const string &logFileDir) {
  string logFileName;
  logFileName = logFileDir;
  logFileName.append("homeauto.");
  logFileName.append(name);
  logFileName.append(".log");
  logfile = fopen(logFileName.c_str(), "a");
  if (logfile == NULL) {
    fprintf(stderr, "Error opening log file: %s using stdout for logfile\n",
            logFileName.c_str());
    logfile = stdout;
  }
  setbuf(logfile, NULL);
}

void device::print() {
  fprintf(stdout, "%s %s %s\n", name.c_str(), address.c_str(), state.c_str());
}
string device::getDetails() {
  string details = "<device><name>";
  details.append(name);
  details.append("</name><state>");
  details.append(state);
  details.append("</state><type>");
  details.append(type);
  details.append("</type><room>");
  details.append(room);
  details.append("</room><load>");
  details.append(load ? "true" : "false");
  details.append("</load><description>");
  details.append(description);
  details.append("</description></device>\n");
  return (details);
}
void device::setState(const string &newState) {
  if (newState != "ACK" && newState != "UNKNOWN") {
    state = newState;
  }
}

void device::updateState() { status(); }
bool device::enabled() { return (enable); }
void device::enableResponses() { enable = true; }
void device::disableResponses() { enable = false; }

void device::ping() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1000", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}

void device::status() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1900", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}

void device::off() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1300", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}

int device::getFlags(bool extended) {
  int flags;
  // extended messages have flag 0x10, so we add 16 to hops because this is
  // saved as a decimal
  flags = extended ? hops + 16 : hops;
  return (flags);
}

string device::getName() { return (name); }
string device::getRoom() { return (room); }
string device::getDescription() { return (description); }
bool device::getLoad() { return (load); }
string device::getType() { return (type); }
string device::getAddress() { return (address); }
string device::getState() { return (state); }
string device::getXML() {
  string XML;
  XML = "";
  char line[256];
  sprintf(line, "<device name=%s>\n", name.c_str());
  sprintf(line, "%s<address>%s</address>\n", line, address.c_str());
  sprintf(line, "%s<state>%s</state>\n</device>\n", line, state.c_str());
  XML.append(line);
  return (XML);
}
bool device::isOn() { return (state.find("on") == 0); }
bool device::isOff() { return (state.find("off") == 0); }
void device::setHops(int numHops) { hops = numHops; }
void device::execute(const string &command, const string &value) {
  fprintf(stderr, "Command not defined: %s\n", command.c_str());
}
void device::handleGroup(int group, const string &command) {
  writeLog(logfile, "!!Please override the handleGroup function Group %i, %s\n",
           group, command.c_str());
}
string device::getAction(const string &command) {
  string actionCode, action, level;
  actionCode = command.substr(18, 2);
  level = command.substr(20, 2);
  // level is in hex, convert to decimal
  char levelChar[4];
  char *pEnd;
  int decLevel;
  strcpy(levelChar, level.c_str());
  decLevel = (int)strtol(levelChar, &pEnd, 16);
  decLevel = (int)(((float)decLevel / 255) * 100 + 0.5);

  if (actionCode == "11") {
    action = "on";
    // The devices also send a group number in the second command field, so if
    // the number is very low, thats what it is
    if (decLevel < 9) {
      handleGroup(decLevel, "on");
    } else {
      sprintf(levelChar, "%i", decLevel);
      action.append(levelChar);
      writeLog(logfile, "ON %i\n", decLevel);
      setState(action);
    }
  } else if (actionCode == "12") {
    action = "on";
    action.append("100");
    writeLog(logfile, "double tap ON %i\n", decLevel);
    setState(action);
  } else if (actionCode == "13") {
    if (decLevel > 0) {
      handleGroup(decLevel, "off");
    } else {
      action = "off";
      setState(action);
      writeLog(logfile, "OFF (%i)\n", decLevel);
    }
  } else if (actionCode == "14") {
    action = "off";
    setState(action);
    writeLog(logfile, "double tap OFF (%i)\n", decLevel);
  } else if (actionCode == "06") {
    action = "ACK";
    setState(action);
    writeLog(logfile, "ACK (%i)\n", decLevel);
  } else if (actionCode == "00") {
    action = "Update";
    level = command.substr(20, 2);
    // level is in hex, convert to decimal
    char levelChar[4];
    char *pEnd;
    int decLevel;
    strcpy(levelChar, level.c_str());
    decLevel = (int)strtol(levelChar, &pEnd, 16);
    decLevel = (int)(((float)decLevel / 255) * 100 + 0.5);
    sprintf(levelChar, "%i", decLevel);
    if (decLevel > 0) {
      action = "on";
      action.append(levelChar);
      writeLog(logfile, "ON %i (update)\n", decLevel);
      setState(action);
    } else {
      action = "off";
      setState(action);
      writeLog(logfile, "OFF (update)\n");
    }
  } else {
    writeLog(logfile, "unknown command %s\n", command.c_str());
    // actionCode = command.substr(18,4);
    action = "UNKNOWN(";
    action.append(actionCode);
    action.append(level);
    action.append(")");
    setState(action);
  }
  return (action);
}
