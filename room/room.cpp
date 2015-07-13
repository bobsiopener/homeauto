#include "room.h"

room::room(const string nameOfRoom) {
	name = nameOfRoom;
}

void room::addCommands(const string allCommands) {
	ifstream In;
	string currentLine,name,value,type;
	In.open(fileName);
	while(!In.eof()) {
		getline(In, currentLine);
		if(!isBlank(currentLine)) {
			if(!startsWith(currentLine,"#")) {
				name = chopWord(currentLine);
				value = chopWord(currentLine);
				type = chopWord(currentLine);
				if(name == "TIMEOUT") {
					interface->setTimeOut(atoi(value.c_str()));
				}
			}
		}
	}
	In.close();
}

void room::entered() {
	people++;
}

void room:left() {
	people = people - 1;
	if(people <0) {
		fprintf(stderr, "Error Room %s has negative people", name.c_str());
	}
}
