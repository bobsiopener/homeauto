#include "responder.h"
#include "network.h"
#include "utils.h"

responder::responder() {
  logfile = stdout;
  verbose = 0;
  keepGoing = true;
  running = false;
  threaded = true;
}
responder::responder(int verboseLevel) {
  logfile = stdout;
  verbose = verboseLevel;
  keepGoing = true;
  running = false;
  threaded = true;
}
responder::~responder() { pclose(logfile); }
void responder::updateAllDevices() {
  writeLog(logfile, "Updating state of all devices\n");
  settings->updateAllDevices();
  writeLog(logfile, "Done with update\n");
}
void responder::setNetwork(network *passedNet) { networkInterface = passedNet; }
void responder::setTerminal(cmdline *passedTerm) { terminal = passedTerm; }
void responder::setThreaded(bool newThreaded) { threaded = newThreaded; }
string responder::setVerbose(int verboseLevel) {
  writeLog(logfile, "set verbose to level %i\n", verboseLevel);
  verbose = verboseLevel;
  response = "ACK";
  return (response);
}
void responder::setInterface(plm *insteon) { interface = insteon; }
void responder::handlePlmEvents() {
  // This routine is the parent loop, it waits for events, then calls
  // findDevice,
  // which figures out who sent the event and what it was.
  // Then execResponses performs any actions in response
  if (verbose > 0)
    writeLog(logfile, "Handleing PLM Events...\n");
  running = true;
  while (keepGoing) {
    if (!threaded)
      keepGoing = false;
    event = interface->waitForEvent(&keepGoing);
    if (event != "") {
      findDevice();
      execResponses();
    }
  }
  if (!threaded)
    keepGoing = true;
  running = false;
  if (verbose > 0) {
    writeLog(logfile, "Exiting PLM Responder\n");
    writeLog(stdout, "Exiting PLM Responder\n");
  }
}
void responder::handleTripEvents() {
  // This routine is the parent loop, it waits for events, then calls
  // findDevice,
  // which figures out who sent the event and what it was.
  // Then execResponses performs any actions in response
  if (verbose > 0)
    writeLog(logfile, "Handleing Trip Events...\n");
  running = true;
  while (keepGoing) {
    if (!threaded)
      keepGoing = false;
    // event = trip->waitForEvent(&keepGoing);
    if (event != "") {
      findDevice();
      execResponses();
    }
  }
  if (!threaded)
    keepGoing = true;
  running = false;
  if (verbose > 0) {
    writeLog(logfile, "Exiting Trip Responder\n");
    writeLog(stdout, "Exiting Trip Responder\n");
  }
}
void responder::setScheduler(scheduler *schedule) { timer = schedule; }
void responder::setConfig(config *configFile) { settings = configFile; }
void responder::findDevice() {
  // figure which device sent the command and what that command was
  string address;
  address = event.substr(4, 6);
  currentActor = settings->getDevice(address);
  if (currentActor != NULL) {
    action = currentActor->getAction(event);
    deviceName = currentActor->getName();
  } else {
    deviceName = "";
    writeLog(logfile, "Count not find Device: %s Event:%s\n", address.c_str(),
             event.c_str());
  }
}
void responder::execResponses() {
  string message;
  message = "<device><name>";
  message.append(deviceName);
  message.append("</name><action>");
  message.append(action);
  message.append("</action></device>\n");
  writeLog(logfile, "notifying client:%s\n", message.c_str());
  networkInterface->send(message);
  terminal->printXML(message);
  // This routine handles all the actions that should happen in response to a
  // detected action
  // I'm going to use the links here to update the status of the master switch
  if (deviceName == "KitchenFridge") {
    if (verbose >= 2)
      writeLog(logfile, "Exec response for KitchenFridge\n");
    currentResponder = settings->getDevice("KitchenHallway");
    if (currentResponder != NULL) {
      currentResponder->setState(action);
    } else {
      writeLog(logfile, "ERROR: count not find device: LivingRoomSeating\n");
    }
  } /* else if(deviceName == "LivingRoomMotion") {
          if(verbose >= 2) writeLog(logfile,"Exec response for
  LivingRoom\n");
          if(action.find("on0") == 0) {
                  currentResponder =
  settings->getDevice("LivingRoomSeating");
                  if(currentResponder != NULL) {
                          if(theWeather->isDark())
  currentResponder->on(70);
                  } else {
                          writeLog(logfile, "ERROR: count not find device:
  LivingRoomSeating\n");
                  }
                  timer->delayedAction("LivingRoomSeating","off", 20);
          }
  } else if(deviceName == "KitchenMotion") {
          if(verbose >= 2) writeLog(logfile,"Exec response for
  KitchenMotion\n");
          if(action.find("on0") == 0) {
                  currentResponder =
  settings->getDevice("KitchenHallway");
                  if(currentResponder != NULL) {
                          if(theWeather->isDark())
  currentResponder->on(70);
                  } else {
                          writeLog(logfile, "ERROR: count not find device:
  KitchenHallway\n");
                  }
                  timer->delayedAction("KitchenHallway","off", 5);
          }
  } else if(deviceName == "KitchenMotion2") {
          if(verbose >= 2) writeLog(logfile,"Exec response for
  KitchenMotion2\n");
          if(action.find("on0") == 0) {
                  currentResponder =
  settings->getDevice("KitchenHallway");
                  if(currentResponder != NULL) {
                          if(theWeather->isDark())
  currentResponder->on(70);
                  } else {
                          writeLog(logfile, "ERROR: count not find device:
  KitchenHallway\n");
                  }
                  timer->delayedAction("KitchenHallway","off", 5);
          }
  } else if(deviceName == "GarageMotion") {
          if(verbose >= 1) writeLog(logfile,"Exec response for
  GarageMotion\n");
          if(action.find("on0") == 0) {
                  currentResponder = settings->getDevice("Garage6Button");
                  if(currentResponder != NULL) {
                          if(theWeather->isDark())
  currentResponder->on(100);
                  } else {
                          writeLog(logfile, "ERROR: count not find device:
  Garage6Button\n");
                  }
                  timer->delayedAction("Garage6Button","off", 1);
          }
  } else if(deviceName == "KitchenStairsMotion") {
          if(verbose >= 1) writeLog(logfile,"Exec response for
  KitchenStairs\n");
          if(action.find("on0") == 0) {
                  currentResponder =
  settings->getDevice("KitchenHallway");
                  if(currentResponder != NULL) {
                          if(theWeather->isDark())
  currentResponder->on(70);
                  } else {
                          writeLog(logfile, "ERROR: count not find device:
  KitchenHallway\n");
                  }
                  timer->delayedAction("KitchenHallway","off", 5);
          }
  }
  */
}
void responder::setWeather(weather *weatherPtr) { theWeather = weatherPtr; }
void responder::setLogFile(char *logFileName) {
  logfile = fopen(logFileName, "w");
  if (logfile == NULL) {
    writeLog(stderr, "Error opening log file: %s using stdout for logfile\n",
             logFileName);
    logfile = stdout;
  }
  setbuf(logfile, NULL);
}
string responder::enable() {
  // will turn the enable on for the loop that handles responses
  writeLog(logfile, "Enabling Responder\n");
  keepGoing = true;
  response = "ACK";
  return (response);
}
string responder::disable() {
  writeLog(logfile, "Disabling responder\n");
  keepGoing = false;
  response = "Disabling responder\n";
  return (response);
}
bool responder::isRunning() { return (running); }
string responder::status() {
  // Print out the status of running and enabled
  if (running) {
    // writeLog(stdout,"Running: yes ");
    response = "responder Running\n";
  } else {
    // writeLog(stdout,"Running: no ");
    response = "responder notRunning\n";
  }
  if (keepGoing) {
    // writeLog(stdout,"Enabled: yes\n");
    response.append("responder enabled\n");
  } else {
    // writeLog(stdout,"Enabled: no\n");
    response.append("responder disabled\n");
  }
  return (response);
}
