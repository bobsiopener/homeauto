#include "plm.h"
#include "utils.h"

plm::plm() {
	logfile = stdout;
	verbose = 0;
	baudrate = BAUDRATE;
	hwflowcontrol = false;
	translation = false;
	linebits = CS8;
	lineparity = 0;
	linestopbits = 0;
	lineflush = false;
	waitTime = NULL;
	pthread_mutex_init(&IO_request,NULL);
	pthread_mutex_init(&ttyIO_mutex,NULL);
}
plm::~plm() {
  pthread_mutex_destroy(&IO_request);
  pthread_mutex_destroy(&ttyIO_mutex);
}
void plm::setLogFile(char *logFileName) {
	logfile = fopen(logFileName,"w");
	if(logfile == NULL) {
		fprintf(stderr,"Error opening log file: %s using stdout for logfile\n",logFileName);
		logfile = stdout;
	}
	setbuf(logfile,NULL);
}
void plm::setTimeOut(int time) {
	waitTime = new struct timespec;
	msec = time;
	waitTime->tv_sec = ((msec - (msec % 1000)) / 1000);
	waitTime->tv_nsec = (msec % 1000) * 1000000;
}
void plm::setHops(int numHops) {
   hops = numHops;
}
void plm::setVerbose(int value) {
	verbose = value;
}
bool plm::openInterface(char* interface) {
    if((serial = open(interface, O_RDWR | O_NOCTTY)) == -1) {
		fprintf(stderr, "Error: can't open device %s\n", interface);
		return(false);
	} 
	ttyUSB = fdopen(serial, "r");
	return(true);
}
bool plm::initInterface() {
	struct termios term_attr;
	tcgetattr(serial,&term_attr);
	term_attr.c_cflag = baudrate | hwflowcontrol | linebits | linestopbits | lineparity | CLOCAL | CREAD;
	term_attr.c_iflag = IGNBRK | IGNPAR;
	term_attr.c_oflag = ONLRET;
	term_attr.c_lflag = 0;
	term_attr.c_cc[VMIN] = 0;
	term_attr.c_cc[VTIME] = 1;
	tcflush(serial, TCIFLUSH);
	tcsetattr(serial,TCSANOW,&term_attr);
	return(true);
}
/*
void plm::send(char* command) {
	int length;
	char* fullCmd;
	fullCmd = new char[256];
	sprintf(fullCmd, "plmsend -d /dev/ttyUSB0 %s", command);
	fprintf(stdout, " %s\n", fullCmd);
	FILE *p = popen(fullCmd, "r");
    char results[100];
    if(p != NULL) { 
		while(fgets(results, sizeof(results),p) != NULL);
    }
}
  */

void plm::send(char* command) {
	int length;
	char *cp,*response;
	unsigned char c,r = 0x00;
	pthread_mutex_lock(&IO_request);
	pthread_mutex_lock(&ttyIO_mutex);
	pthread_mutex_unlock(&IO_request);
	if (verbose>5) fprintf(logfile,"Locked Mutex for write\n");
	while(r != 0x06) {
		cp = command;
		length = strlen(command);
		response = new char[length + 64];
		strcpy(response,"");
		for (int i=0; i<length; i+=2) {
			c = hextoc(*(cp+i),*(cp+i+1));
			write(serial,&c,1);
			if (verbose>2) fprintf(logfile,"%.2X : %c,%c ",c,*(cp+i),*(cp+i+1));
			//Wrote a byte to the PLM, now we read back the byte
			read(serial,&r,1);
			if (verbose>2) fprintf(logfile,"read: %.2X\n",r);
			//gather the response from the interface
			sprintf(response,"%s%.2X", response,r);
			//I think there could be some error checking to make sure
			//the initial command and the response match

		}
		if (verbose>2) fprintf(logfile,"reading message back from PLM\n");
		while ((r != 0x06) && (r != 0x15)) {
			read(serial,&r,1);
			sprintf(response,"%s%.2X", response,r);
			if (verbose>2) fprintf(logfile,"read: %.2X\n",r);
		}
		//0x15 is negative acknowledge
		if(r == 0x15) {
			fprintf(logfile,"Recieved NACK, waiting for a second\n");
			usleep(1000000);	
		}		
	}
	if (verbose>2) fprintf(logfile,"Recieved ACK\n");
	pthread_mutex_unlock(&ttyIO_mutex);
	if (verbose>5) fprintf(logfile,"unlocked Mutex for write\n");
	if (verbose>1) fprintf(logfile,"response: %s\n",response);
	delete[] response;
}

string plm::waitForEvent(bool* keepGoing) {
	// Wait for some data to be available at the serial port.
	int nfds,ready;
	int counter=0;
	char converted[3];
    string results="";
    unsigned char next_byte;
    bool notFound = true;
	nfds = serial;
	nfds++;
	fd_set readfds;
	//char c,buff[128],tbuff[128];
	while( notFound && *keepGoing)
    {
		FD_ZERO(&readfds);
		FD_SET(serial,&readfds);
		//Using the request mutex should force the threads to take turns
		pthread_mutex_lock(&IO_request);
		pthread_mutex_lock(&ttyIO_mutex);
		pthread_mutex_unlock(&IO_request);
		if (verbose>=10) fprintf(logfile,"Locked Mutex for read\n");
		ready = pselect(nfds,&readfds,NULL,NULL,waitTime,NULL);
		if (FD_ISSET(serial,&readfds) && ready > 0) {
			read(serial,&next_byte,1);
			if (verbose>3) fprintf(logfile,"buffer is %.2X\n",next_byte);
			//HERE I RECEIVE THE FIRST ANSWER
			//Toss out everything that doesn't start with 02
			if((counter == 0 && next_byte == 2) || counter > 0){
				counter++;
				if(counter>=11) {
					sprintf(converted, "%.2X\n",(uc)next_byte);
					results.append(converted);
					counter = 0;
					notFound = false;
					if (verbose>=10) fprintf(logfile,"unlocked Mutex for read\n");
					pthread_mutex_unlock(&ttyIO_mutex);
					if (verbose>3) fprintf(logfile,"buffer filled\n\n");
					return(results.c_str());
				} else {
					sprintf(converted, "%.2X",(uc)next_byte);
					results.append(converted);
				}
			} 
		}
		if (verbose>=10) fprintf(logfile,"unocked Mutex for read\n");
		pthread_mutex_unlock(&ttyIO_mutex);
	}
	if (verbose>=10) fprintf(logfile,"Read: %s\n", results.c_str());
	return(results);
}



