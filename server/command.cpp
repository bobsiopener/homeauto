#include "command.h"
#include "cmdline.h"
#include "network.h"
#include "responder.h"
#include "utils.h"

command::command(config *configFile, plm *interfaceOut, responder *handler,
                 scheduler *manager, weather *weatherPtr) {
  settings = configFile;
  interface = interfaceOut;
  continu = true;
  theResponder = handler;
  theScheduler = manager;
  theWeather = weatherPtr;
  commandLine = NULL;
  networkInterface = NULL;
  verbose = 0;
}
void command::setCmdLine(cmdline *passedCmd) { commandLine = passedCmd; }
void command::setNetwork(network *passedNet) { networkInterface = passedNet; }

void command::run(const string &deviceName, const string &command,
                  const string &subcommand, const string &value) {
  string response = "";
  device *foundDevice;
  // std::transform(data.begin(), data.end(), data.begin(), ::tolower);
  if (verbose >= 2)
    fprintf(stdout, "Device: %s Command: %s subCommand %s, value: %s\n",
            deviceName.c_str(), command.c_str(), subcommand.c_str(),
            value.c_str());
  // look for keywords here (exit, responder, scheduler)
  // Should add some error checking into recieved command
  if (deviceName == "shutdown") {
    done();
  } else if (deviceName == "exit") {
    done();
  } else if (deviceName == "responder") {
    if (command == "disable")
      theResponder->disable();
    if (command == "enable")
      theResponder->enable();
    if (command == "status")
      theResponder->status();
    if (command == "set") {
      if (subcommand == "verbose")
        theResponder->setVerbose(atoi(value.c_str()));
    }
  } else if (deviceName == "scheduler") {
    if (command == "disable")
      theScheduler->disable();
    if (command == "enable")
      theScheduler->enable();
    if (command == "print")
      response = theScheduler->print();
    if (command == "status")
      theScheduler->status();
    if (command == "add")
      theScheduler->scheduleEvent(subcommand);
    if (command == "set") {
      if (subcommand == "verbose")
        theScheduler->setVerbose(atoi(value.c_str()));
    }
  } else if (deviceName == "insteon") {
    if (command == "set") {
      if (subcommand == "verbose")
        interface->setVerbose(atoi(value.c_str()));
    } else if (command == "send") {
      int length = strlen(subcommand.c_str());
      char *sendVar;
      sendVar = new char[length + 1];
      strcpy(sendVar, subcommand.c_str());
      interface->send(sendVar);
    }
  } else if (deviceName == "weather") {
    if (command == "update") {
      theWeather->updateData();
    } else if (command == "print") {
      response = theWeather->getXML();
    } else if (command == "configure") {
      theWeather->configure();
    } else if (command == "get") {
      if (subcommand == "data")
        response = theWeather->getData();
    }
  } else if (deviceName == "print") {
    if (command == "devices") {
      settings->printAllDevices();
    }
  } else if (deviceName == "get") {
    if (command == "all" && subcommand == "device" && value == "names") {
      response = settings->getAllDeviceNames();
    } else if (command == "android" && subcommand == "device" &&
               value == "details") {
      response = settings->getAndroidDetails();
    } else if (command == "on" && subcommand == "device" && value == "names") {
      response = settings->getOnDeviceNames();
    } else if (command == "all" && subcommand == "device" && value == "stats") {
      response = settings->getAllDeviceStats();
    } else if (command == "devices" && subcommand == "xml") {
      response = settings->getXML();
    } else if (command == "weather") {
      if (subcommand == "xml") {
        response = theWeather->getXML();
      } else {
        response = theWeather->getData();
      }
    } else if (command == "schedule") {
      if (subcommand == "xml") {
        response = theScheduler->getXML();
      } else {
        response = theScheduler->print();
      }
    } else if (command == "categories") {
      response = getCategories();
    } else if (command == "XML") {
      response = getAllXML();
    } else if (command == "android") {
      if (subcommand == "categories") {
        response = getCategories();
      }
    }

  } else if (deviceName == "help") {
    response =
        "\nThere are 3 services you can query: weather, responder, and "
        "scheduler\nThey have the following "
        "commands:\nresponder:\n\tdisable\n\tenable\n\tstatus\n\tset verbose "
        "#\nscheduler:\n\tdisable\n\tenable\n\tstatus\n\tprint\n\tset verbose "
        "#\nweather:\n\tupdate\n\tprint\n\tconfigure\n\tget data\n";
  } else {
    foundDevice = settings->getDevice(deviceName);
    if (foundDevice != NULL) {
      foundDevice->execute(command, subcommand);
    } else {
      fprintf(stderr, "Command: %s not recognized\n", deviceName.c_str());
      response = deviceName;
      response.append(" not recognized\n");
    }
  }
  if (response != "") {
    networkInterface->send(response);
    commandLine->printXML(response);
    //goal here is to clear response, like a bzero
    response = "";
  }
}

string command::getAllXML() {
  string results;
  results = settings->getXML();
  results.append(theScheduler->getXML());
  results.append(theWeather->getXML());
  return (results);
}

string command::getCategories() {
  string results;
  results = "";
  if (settings != NULL) {
    results.append("devices:get android device details\n");
  }
  if (theScheduler != NULL) {
    results.append("scheduler:scheduler print\n");
  }
  if (theWeather != NULL) {
    results.append("weather:weather print\n");
  }
  /*if(networkInterface != NULL) {
          results.append("<network> </network>");
  }*/
  return (results);
}

string command::done() {
  string response;
  response = "";
  if (theResponder != NULL)
    response.append(theResponder->disable());
  if (theScheduler != NULL)
    response.append(theScheduler->disable());
  if (commandLine != NULL)
    response.append(commandLine->stop());
  if (networkInterface != NULL)
    networkInterface->stop();
  continu = false;
  return (response);
}
