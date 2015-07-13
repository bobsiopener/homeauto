#ifndef IOLINK_H_
#define IOLINK_H_
#include "../plm.h"
#include "device.h"
class config;
#include "../config.h"


class IOLink:public device{
	public:
		IOLink(plm*,const string&,const string&,int,const string&,const string&,const string&);
		void execute(const string&, const string&);
	protected:
		void on();
		string help(const string&);

};

#endif
