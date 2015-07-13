/* Read in the /etc/insteon.conf and get the names and addresses
of all devices */
#ifndef CONFIG_H_
#define CONFIG_H_

class device;
#include "./devices/device.h"
#include <tr1/unordered_map>
#include <string.h>


typedef tr1::unordered_map<string, device*> deviceMap;
class config{
	public:
		config(plm*);
		~config();
		void read(char*);
		device* getDevice(string);
		string getAction(const string&);
		void printAllDevices();
		string getAllDeviceStats();
		string getAllDeviceNames();
		string getAndroidDetails();
		string getOnDeviceNames();
		string getOnDeviceStats();
		void updateAllDevices();
		string getDeviceStats(const string&);
		char* generateCommand(const string&, const string&);
		string getXML();
	private:
		plm* interface;
		int hops;
		string brightLevel;
		string logDir;
		device *currentDevice;
		deviceMap allDevices;
		deviceMap allDevicesByName;
		deviceMap allDevicesByAddress;
};
#endif
