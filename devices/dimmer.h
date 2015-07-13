#ifndef DIMMER_H_
#define DIMMER_H_
#include "../plm.h"
#include "device.h"
class config;
#include "../config.h"


class dimmer:public device{
	public:
		dimmer(plm*,const string&,const string&,int,const string&,const string&,const string&);
		void execute(const string&, const string&);
	protected:
		void on(int level=100);
		void bright();
		void dim();
		string help(const string&);
		void setOnLevel(int);
		void setOnRamp(int);
		void setOffRamp(int);

};

#endif
