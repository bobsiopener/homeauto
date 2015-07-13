#ifndef SWITCH_H_
#define SWITCH_H_
#include "../plm.h"
#include "device.h"
class config;
#include "../config.h"

class sixButton:public device{
	public:
		sixButton(plm*,const string&,const string&,int,const string&,const string&,const string&);
		void execute(const string&, const string&);
		void handleGroup(int, const string&);
		string getState();
	protected:
		void on();
		void onA();
		void onB();
		void onC();
		void onD();
		void offA();
		void offB();
		void offC();
		void offD();
		bool A;
		bool B;
		bool C;
		bool D;
		string help(const string&);
};

#endif
