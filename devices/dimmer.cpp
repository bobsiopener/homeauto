#include "dimmer.h"
#include <string.h>
#include <stdio.h>

dimmer::dimmer(plm *dev, const string &configName, const string &configAddress,
               int numHops, const string &myRoom, const string &myLoad,
               const string &myDescription) {
  hops = numHops;
  address = configAddress;
  name = configName;
  interface = dev;
  state = "off";
  logfile = stdout;
  type = "Dimmer";
  room = myRoom;
  load = (myLoad == "yes") ? true : false;
  description = myDescription;
}

void dimmer::on(int level) {
  char *command;
  command = new char[30];
  if (level > 100)
    level = 100;
  if (level < 0)
    level = 0;
  level = (level * 255) / 100;
  sprintf(command, "0262%s%.2X11%.2X", address.c_str(), hops, level);
  interface->send(command);
  delete[] command;
}

void dimmer::bright() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1500", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}
void dimmer::dim() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1600", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}

void dimmer::setOnLevel(int level) {
  // update On and Ramp
  char *baseCommand, *extCommand, *fullCommand;
  if (level > 100)
    level = 100;
  if (level < 0)
    level = 0;
  level = (level * 255) / 100;
  baseCommand = new char[17];
  sprintf(baseCommand, "0262%s%.2X2E00", address.c_str(), getFlags(true));
  extCommand = new char[29];
  sprintf(extCommand, "0106%.2X00000000000000000000B1", level);
  fullCommand = new char[45];
  sprintf(fullCommand, "%s%s", baseCommand, extCommand);
  interface->send(fullCommand);
  delete[] baseCommand;
  delete[] extCommand;
  delete[] fullCommand;
}
void dimmer::setOnRamp(int time) {
  // update On and Ramp
  char *baseCommand, *extCommand, *fullCommand;
  if (time > 5400)
    time = 5400;
  if (time < 0)
    time = 0;
  baseCommand = new char[17];
  sprintf(baseCommand, "0262%s%.2X2E00", address.c_str(), getFlags(true));
  extCommand = new char[29];
  sprintf(extCommand, "0105%.2X00000000000000000000B1", time);
  fullCommand = new char[45];
  sprintf(fullCommand, "%s%s", baseCommand, extCommand);
  interface->send(fullCommand);
  delete[] baseCommand;
  delete[] extCommand;
  delete[] fullCommand;
}
void dimmer::setOffRamp(int rampOff) {
  // no need to get any new data
  char *command;
  command = new char[30];
  if (rampOff > 100)
    rampOff = 100;
  if (rampOff < 0)
    rampOff = 0;
  rampOff = (rampOff * 15) / 100;
  sprintf(command, "0262%s%.2X2F0%.1X", address.c_str(), getFlags(), rampOff);
  interface->send(command);
  delete[] command;
}

void dimmer::execute(const string &command, const string &value) {
  if (command.find("on") == 0) {
    int level;
    if (value != "") {
      level = atoi(value.c_str());
      on(level);
    } else {
      on();
    }
  } else if (command == "off") {
    off();
  } else if (command == "bright") {
    bright();
  } else if (command == "dim") {
    dim();
  } else if (command == "update") {
    status();
  } else if (command == "ping") {
    ping();
  } else if (command == "status") {
  } else if (command == "setOnLevel") {
    int level;
    if (value != "") {
      level = atoi(value.c_str());
      setOnLevel(level);
    }
  } else if (command == "setOnRamp") {
    int level;
    if (value != "") {
      level = atoi(value.c_str());
      setOnRamp(level);
    }
  } else if (command == "setOnLevel") {
    int level;
    if (value != "") {
      level = atoi(value.c_str());
      setOffRamp(level);
    }
  } else {
    fprintf(stderr, "Unknown Command: %s\n", command.c_str());
  }
}

string dimmer::help(const string &command) { return (""); }
