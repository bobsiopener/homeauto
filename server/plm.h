#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#define uc unsigned char
#define BAUDRATE B19200
#define TTYLINENAME "/dev/ttyUSB0"
#ifndef PLM_H_
#define PLM_H_
using namespace std;

class plm {
	public:
		plm();
		~plm();
		void setLogFile(char*);
		void setTimeOut(int time);
		void setHops(int time);
		void setVerbose(int value);
		void send(char* command);
		void link(char* address);
		string waitForEvent(bool*);
		bool openInterface(char* interface = NULL);
		bool initInterface();
	private:
		int baudrate;
		int hwflowcontrol;
		int translation;
		int linebits;
		int lineparity;
		int linestopbits;
		int lineflush;
		int msec;
		struct timespec *waitTime;
		int hops;
		int verbose;
		bool notFound;
		int serial;
		FILE *ttyUSB;
		FILE *logfile;
		pthread_mutex_t ttyIO_mutex;
		pthread_mutex_t IO_request;
};
#endif
