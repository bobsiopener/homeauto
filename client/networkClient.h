#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <stdlib.h>
//#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>

class cmdline;

class networkClient
{
    public:
		networkClient(char*,char*);
		~networkClient();
		char* run(const std::string&);
		void setCmdLine(cmdline*);
		void close();
	private:
		ssize_t rio_readn(int fd, char *usrbuf, size_t n);
		bool isCreated();
		void createSocket();
		void connectSocket();

		int sockfd;
		int portno;
		int n;
		int verbose;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		cmdline *commandLine;
};

#endif // NETWORKCLIENT_H
