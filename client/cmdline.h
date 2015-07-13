#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <string>
#include "networkClient.h"

class cmdline{
	public:
		cmdline(networkClient*);
		void getCommands();
		void linkCommand();
		void stop();
	private:
		std::string deviceName;
		std::string commandStr;
		std::string subCommandStr;
		std::string commandLine;
		std::string action;
		std::string value;
		bool continu;
		networkClient *execute;
};

#endif
