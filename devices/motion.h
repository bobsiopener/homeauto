#ifndef MOTION_H_
#define MOTION_H_
#include "../plm.h"
#include "device.h"
class config;
#include "../config.h"


class motion:public device{
	public:
		motion(plm*,const string&,const string&,int,const string&,const string&,const string&);
		void execute(const string&, const string&);
		string getAction(const string&);
	protected:
		string help(const string&);
};

#endif
