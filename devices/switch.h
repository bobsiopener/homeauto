#ifndef SWITCH_H_
#define SWITCH_H_
#include "../plm.h"
#include "device.h"
class config;
#include "../config.h"

class binarySwitch:public device{
	public:
		binarySwitch(plm*,const string&,const string&,int,const string&,const string&,const string&);
		void execute(const string&, const string&);
	protected:
		void on();
		string help(const string&);
};

#endif
