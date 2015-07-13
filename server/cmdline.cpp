#include "cmdline.h"
#include "utils.h"
/* cmdline sends the commands from the command line to the interperter
 * this class is in charge of printing the response
 */
cmdline::cmdline(command *theExecuter) {
	continu = true;
	fprintf(stdout,"Starting the cmdLine reader\n");
	execute = theExecuter;
}

void cmdline::getCommands() {
	string response;
	while(continu) {
		std::getline(std::cin,commandLine);
		deviceName = chopWord(commandLine);
		commandStr = chopWord(commandLine);
		subCommandStr = chopWord(commandLine);
		value = chopWord(commandLine);
		removeSpaces(deviceName);
		removeSpaces(commandStr);
		removeSpaces(subCommandStr);
		removeSpaces(value);
		execute->run(deviceName,commandStr,subCommandStr,value);
	}
	fprintf(stdout, "Exiting Command Line\n");
}

void cmdline::linkCommand() {
	execute->setCmdLine(this);
}
string cmdline::stop() {
	continu = false;
        fprintf(stdout, "Stopping Command Line reader\n");
        return("Stopping CommandLine reader\n");
}

void cmdline::printXML(string response) {
	fprintf(stdout,"%s", response.c_str());
}
