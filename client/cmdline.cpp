#include "cmdline.h"
#include "utils.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>



/* cmdline sends the commands from the command line to the interperter
 * this class is in charge of printing the response
 */
cmdline::cmdline(networkClient *theExecuter) {
	continu = true;
	execute = theExecuter;
}

void cmdline::getCommands() {
	char *response = NULL;
	char prompt[]="HomeAutoClient $ ";
	using_history ();
	while(continu) {
		//std::getline(std::cin,commandLine);
		rl_instream = stdin;
		commandLine = readline(prompt);
		add_history(commandLine.c_str());
		if(commandLine == "exit") {
			continu = false;
			execute->close();
		} else if(commandLine == "close") {
			execute->run(commandLine);
			execute->close();
		} else {
			response = execute->run(commandLine);
			if(response != NULL) fprintf(stdout, "response:%s\n", response);
			bzero(response,strlen(response));
			delete(response);
			response = NULL;
		}
	}
	fprintf(stdout, "Exiting Command Line\n");
}

void cmdline::linkCommand() {
	execute->setCmdLine(this);
}
void cmdline::stop() {
	continu = false;
}
