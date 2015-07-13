#include "config.h"
#include "utils.h"
#include "./devices/dimmer.h"
#include "./devices/motion.h"
#include "./devices/switch.h"
#include "./devices/thermostat.h"
#include "./devices/IOlink.h"
#include <iostream>
#include <fstream>
using namespace std;

config::config(plm *plmInst) { interface = plmInst; }
void config::read(char *fileName) {
  logDir = "/home/bob/log/";
  ifstream In;
  string currentLine, name, value, type, room, load, description;
  In.open(fileName);
  while (!In.eof()) {
    getline(In, currentLine);
    if (!isBlank(currentLine)) {
      if (!startsWith(currentLine, "#")) {
        name = chopWord(currentLine, ":");
        value = chopWord(currentLine, ":");
        if (name == "TIMEOUT") {
          interface->setTimeOut(atoi(value.c_str()));
        } else if (name == "ONLEVELS") {
          brightLevel = value;
        } else if (name == "LOG") {
          logDir = value;
        } else if (name == "HOPS") {
          hops = atoi(value.c_str());
          // bits 2 and 3 are for hops remaining so we multiply by
          // 4 to shift the bits 2 to the left and then add back the original
          // hops
          // which is the same as multiplying by 5
          hops = hops * 5;
          interface->setHops(atoi(value.c_str()));
        } else {
          type = chopWord(currentLine, ":");
          room = chopWord(currentLine, ":");
          load = chopWord(currentLine, ":");
          description = chopWord(currentLine, ":");
          // new device
          if (type == "Dimmer") {
            currentDevice = new dimmer(interface, name, value, hops, room, load,
                                       description);
          } else if (type == "Switch") {
            currentDevice = new binarySwitch(interface, name, value, hops, room,
                                             load, description);
          } else if (type == "IOLink") {
            currentDevice = new IOLink(interface, name, value, hops, room, load,
                                       description);
          } else if (type == "Motion") {
            currentDevice = new motion(interface, name, value, hops, room, load,
                                       description);
          } else if (type == "Thermostat") {
            currentDevice = new thermostat(interface, name, value, hops, room,
                                           load, description);
          } else {
            currentDevice = new device(interface, name, value, hops, room, load,
                                       description);
          }
          currentDevice->setLogDir(logDir);
          allDevices[name] = currentDevice;
          allDevices[value] = currentDevice;
          allDevicesByName[name] = currentDevice;
          allDevicesByAddress[value] = currentDevice;
        }
      }
    }
  }
  In.close();
}
config::~config() {
	device *toRemove;
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    toRemove = thisDevice->second;
		delete toRemove;
  }
	allDevices.clear();
	allDevicesByName.clear();
	allDevicesByAddress.clear();
}
void config::printAllDevices() {
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    (thisDevice->second)->print();
  }
}
void config::updateAllDevices() {
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    (thisDevice->second)->updateState();
    usleep(500000);
  }
}

string config::getAndroidDetails() {
  string allDeviceStatus, Load;
  allDeviceStatus = "";
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    allDeviceStatus.append((thisDevice->second)->getDetails());
  }
  return (allDeviceStatus);
}
string config::getAllDeviceNames() {
  string allDeviceStatus;
  allDeviceStatus = "";
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    allDeviceStatus.append((thisDevice->second)->getName());
    allDeviceStatus.append("\n");
  }
  return (allDeviceStatus);
}
string config::getOnDeviceNames() {
  string allDeviceStatus;
  allDeviceStatus = "";
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    if ((thisDevice->second)->isOn()) {
      allDeviceStatus.append((thisDevice->second)->getName());
      allDeviceStatus.append("\n");
    }
  }
  return (allDeviceStatus);
}
string config::getAllDeviceStats() {
  string allDeviceStatus;
  allDeviceStatus = "";
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    allDeviceStatus.append((thisDevice->second)->getName());
    allDeviceStatus.append(" ");
    allDeviceStatus.append((thisDevice->second)->getAddress());
    allDeviceStatus.append(" ");
    allDeviceStatus.append((thisDevice->second)->getState());
    allDeviceStatus.append("\n");
  }
  return (allDeviceStatus);
}
string config::getOnDeviceStats() {
  string allDeviceStatus;
  allDeviceStatus = "";
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    if ((thisDevice->second)->isOn()) {
      allDeviceStatus.append((thisDevice->second)->getName());
      allDeviceStatus.append(" ");
      allDeviceStatus.append((thisDevice->second)->getAddress());
      allDeviceStatus.append(" ");
      allDeviceStatus.append((thisDevice->second)->getState());
      allDeviceStatus.append("\n");
    }
  }
  return (allDeviceStatus);
}
string config::getDeviceStats(const string &deviceName) {
  if (allDevices.find(deviceName) != allDevices.end()) {
    string DeviceStatus;
    currentDevice = allDevices[deviceName];
    DeviceStatus = currentDevice->getName();
    DeviceStatus.append(" ");
    DeviceStatus.append(currentDevice->getAddress());
    DeviceStatus.append(" ");
    DeviceStatus.append(currentDevice->getState());
    DeviceStatus.append("\n");
    return (DeviceStatus);
  } else {
    return ("Device Not Found\n");
  }
}
string config::getXML() {
  string allDeviceXML;
  allDeviceXML = "<devices>\n";
  deviceMap::iterator thisDevice;
  for (thisDevice = allDevicesByName.begin();
       thisDevice != allDevicesByName.end(); thisDevice++) {
    allDeviceXML.append((thisDevice->second)->getXML());
  }
  allDeviceXML.append("</devices>\n");
  return (allDeviceXML);
}
device *config::getDevice(string deviceName) {
  if (allDevices.find(deviceName) != allDevices.end()) {
    return (allDevices[deviceName]);
  } else {
    return (NULL);
  }
}
string config::getAction(const string &command) {
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
  } else if (actionCode == "13") {
    action = "off";
  } else if (actionCode == "06") {
    action = "ACK";
  } else {
    // actionCode = command.substr(18,4);
    action = "UNKNOWN";
  }
  return (action);
}
