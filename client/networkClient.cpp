#include "networkClient.h"
#include "cmdline.h"

networkClient::networkClient(char* address, char* port)
{
    portno = atoi(port);

    server = gethostbyname(address);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    }
    
    memset((char *) &serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //memcpy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
    memcpy((void *)&serv_addr.sin_addr, server->h_addr_list[0], server->h_length); 
    serv_addr.sin_port = htons(portno);
}

networkClient::~networkClient()
{
    //dtor
}


ssize_t networkClient::rio_readn(int fd, char *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;
    fprintf(stdout,"starting read\n");
    while (nleft > 0) {
		if ((nread = read(fd, bufp, nleft)) < 0) {
			fprintf(stdout,"read: %s\n", bufp);
			if (errno == EINTR) /* interrupted by sig handler return */
			nread = 0;      /* and call read() again */
			else
			return -1;      /* errno set by read() */
		}
		else if (nread == 0)
			break;              /* EOF */
		nleft -= nread;
		bufp += nread;
    }
    return (n - nleft);         /* return >= 0 */
}
void networkClient::setCmdLine(cmdline* passedCmd) {
	commandLine = passedCmd;
}
bool networkClient::isCreated(){
	return(sockfd < 0);
}
void networkClient::createSocket() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) fprintf(stderr,"ERROR opening socket");
}
void networkClient::close() {
	shutdown(sockfd,SHUT_RDWR);
}
void networkClient::connectSocket() {
	int established;
	established = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
	if(established < 0 && errno != EINTR) fprintf(stderr,"Error connecting to socket\n");
}
char* networkClient::run(const std::string& wholeMessage) {
	std::string response;
	if(!isCreated()) createSocket();
	connectSocket();

	//Send this command to the network server
	char *buffer;
	buffer = new char[8192];
	n = write(sockfd,wholeMessage.c_str(),wholeMessage.length());
	fprintf(stdout,"Message wrote\n");
	if (n < 0) fprintf(stderr,"ERROR writing to socket");
	n = rio_readn(sockfd,buffer,8191);
	fprintf(stdout,"Message recv\n");
	if (n < 0) fprintf(stderr,"ERROR reading from socket");
	
	return(buffer);
}
