#ifndef THERMOSTAT_H_
#define THERMOSTAT_H_
#include "../plm.h"
#include "device.h"
class config;
#include "../config.h"


class thermostat:public device{
	public:
		thermostat(plm*,const string&,const string&,int,const string&,const string&,const string&);
		void execute(const string&, const string&);
		string getAction(const string&);
		string getXML();
		void print();
	protected:
		string status();
		void setCoolTemp(int);
		void setHeatTemp(int);
		void setModeHeat();
		void setModeCool();
		void setModeAuto();
		void setModeOff();
		void setFanOn();
		void setFanAuto();
		void getMode();
		void getTemp();
		string help(const string&);
		int temp;
		int heat;
		int cool;
		string mode;
		string fan;
};

#endif
