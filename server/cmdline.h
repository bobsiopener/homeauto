#include <string.h>
#include "command.h"

#ifndef CMDLINE_H_
#define CMDLINE_H_

class cmdline{
	public:
		cmdline(command*);
		void getCommands();
		void linkCommand();
		string stop();
		void printXML(string);
	private:
		string deviceName;
		string commandStr;
		string subCommandStr;
		string commandLine;
		string action;
		string value;
		bool continu;
		command *execute;
};

#endif
