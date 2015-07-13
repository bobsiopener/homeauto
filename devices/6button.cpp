#include "6button.h"
#include <string.h>
#include <stdio.h>

sixButton::sixButton(plm *dev, const string &configName,
                     const string &configAddress, int numHops,
                     const string &myRoom, const string &myLoad,
                     const string &myDescription) {
  hops = numHops;
  address = configAddress;
  name = configName;
  interface = dev;
  state = "off";
  logfile = stdout;
  type = "Six Button";
  room = myRoom;
  load = (myLoad == "yes") ? true : false;
  description = myDescription;
  A = false;
  B = false;
  C = false;
  D = false;
}
void sixButton::handleGroup(int group, const string &command) {
  bool turn;
  turn = command.compare("on");
  if (group == 1)
    A = turn;
  else if (group == 2)
    B = turn;
  else if (group == 3)
    C = turn;
  else if (group == 4)
    D = turn;
}

string sixButton::getState() {
  string fullState = state;
  if (A)
    fullState.append(";A-on");
  else
    fullState.append(";A-off");
  if (B)
    fullState.append(";B-on");
  else
    fullState.append(";B-off");
  if (C)
    fullState.append(";C-on");
  else
    fullState.append(";C-off");
  if (D)
    fullState.append(";D-on");
  else
    fullState.append(";D-off");
  return (state);
}

void sixButton::on() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X11FF", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}
void sixButton::onA() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X11FF", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}
void sixButton::onB() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X11FF", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}
void sixButton::onC() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X11FF", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}
void sixButton::onD() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X11FF", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}
void sixButton::offA() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1300", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}
void sixButton::offB() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1300", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}

void sixButton::offC() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1300", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}

void sixButton::offD() {
  char *command;
  command = new char[30];
  sprintf(command, "0262%s%.2X1300", address.c_str(), hops);
  interface->send(command);
  delete[] command;
}

void sixButton::execute(const string &command, const string &value) {
  if (command.find("on") == 0) {
    on();
  } else if (command == "off") {
    off();
  } else if (command == "update") {
    status();
  } else if (command == "ping") {
    ping();
  } else if (command == "status") {
  } else {
    fprintf(stderr, "Unknown Command: %s\n", command.c_str());
  }
}
