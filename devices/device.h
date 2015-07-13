#ifndef DEVICE_H_
#define DEVICE_H_
#include "../plm.h"
#include "../utils.h"
class config;
#include "../config.h"


class device{
	public:
		device(){};
		device(plm*,const string&,const string&,int, const string&, const string&, const string&);
		virtual ~device() {};
		virtual void execute(const string&, const string&);
		virtual void handleGroup(int, const string&);
		virtual string getAction(const string&);
		virtual string getXML();
		virtual string getDetails();
		virtual void print();
		string getName();
		string getAddress();
		virtual string getState();
		string getType();
		string getRoom();
		bool   getLoad();
		string getDescription();
		void setState(const string&);
		void setConfig(config*);
		void updateState();
		void setHops(int); 
		void setLogDir(const string&);
		void enableResponses();
		void disableResponses();
		bool enabled();
		bool isOn();
		bool isOff();
	protected:
		void sendCommand(const string&);
		void ping();
		string name;
		string address;
		string state;
		string type;
		string room;
		int getFlags(bool extended = false);
		bool load;
		string description;
		void off();
		void setRampRate(int);
		void setLED(int);
		void setOn(int);
		void status();
		bool enable;
		plm* interface;
		config *settings;
		int hops;
		FILE *logfile;
};

#endif
