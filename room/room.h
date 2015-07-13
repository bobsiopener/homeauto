#ifndef ROOM_H_
#define ROOM_H_

struct actions{
	string name;
	string action;
	actions *next;
}

class room{
	public:
		room(const string);
		void addCommands(const string);
		void entered();
		void left();
	private:
		int people;
		string name;
		actions *on;
		actions *off;
	
}
