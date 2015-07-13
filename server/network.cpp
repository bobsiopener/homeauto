#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "network.h"
/* For sockaddr_in */
#include <netinet/in.h>
#include <arpa/inet.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>
#include <sstream>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "network.h"

#define MAX_LINE 16384
using namespace std;

struct event_interp {
  struct event_base *base;
  network *interface;
  std::string address;
};

void readcb(struct bufferevent *bev, void *ctx) {
  struct event_interp *interfaceStruct = (event_interp *)ctx;
  network *interface = interfaceStruct->interface;
  string address = interfaceStruct->address;
  string message;
  struct evbuffer *input,*output;
  char *line;
  size_t n;
  input = bufferevent_get_input(bev);
  while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
    interface->run(line);
    message = line;
    writeLog(interfaceStruct->interface->logfile, "%s: recieved: '%s'\n", address.c_str(), line);
    if (message.find("close") != string::npos) {
      output = bufferevent_get_output(bev);
      message = "bye\n";
      evbuffer_add(output, message.c_str(), message.length());
    }
    free(line);
  }
  char buf[1024];
  while (evbuffer_get_length(input)) {
    evbuffer_remove(input, buf, sizeof(buf));
    message = buf;
    writeLog(interfaceStruct->interface->logfile, "%s: recieved: '%s'\n", address.c_str(), buf);
    if (message.find("close") != string::npos) {
      output = bufferevent_get_output(bev);
      message = "bye\n";
      evbuffer_add(output, message.c_str(), message.length());
    } else {
      interface->run(buf);
    }
  }
}

void errorcb(struct bufferevent *bev, short error, void *ctx) {
  struct event_interp *interface = (event_interp *)ctx;
  string address = interface->address;
  if (error & BEV_EVENT_CONNECTED) {
    writeLog(interface->interface->logfile, "Connected to %s okay\n",
             address.c_str());
  } else {
    if (error & BEV_EVENT_EOF) {
      writeLog(interface->interface->logfile, "connection to %s closed\n",
               address.c_str());
      /* connection has been closed, do any clean up here */
      /* ... */
    } else if (error & BEV_EVENT_ERROR) {
      writeLog(interface->interface->logfile, "%s: error\n", address.c_str());
      /* check errno to see what error occurred */
      /* ... */
    } else if (error & BEV_EVENT_TIMEOUT) {
      writeLog(interface->interface->logfile, "%s: timeout\n", address.c_str());
      /* must be a timeout event handle, handle it */
      /* ... */
    }
    interface->interface->remove(address);
    bufferevent_free(bev);
  }
}

void do_accept(evutil_socket_t listener, short event, void *arg) {
  //writeLog(logfile, "inside do accept\n");
  struct event_interp *interface = (event_interp *)arg;
  struct event_base *base = interface->base;
  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int fd = accept(listener, (struct sockaddr *)&ss, &slen);
  if (fd < 0) {
    perror("accept");
  } else if (fd > FD_SETSIZE) {
    close(fd);
  } else {
    struct bufferevent *bev;
    evutil_make_socket_nonblocking(fd);
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    // socket to connection map
    string key;
    struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
    key = inet_ntoa(sin->sin_addr);
		writeLog(interface->interface->logfile,"Connection from %s\n",key.c_str());
    std::ostringstream s;
    s << fd;
    key.append(s.str());
    interface->address = key;
    interface->interface->add(key, bev);
    bufferevent_setcb(bev, readcb, NULL, errorcb, (void *)interface);
    bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    struct evbuffer *output;
    output = bufferevent_get_output(bev);
    evbuffer_add(output, "Connected to HomeAutoServer", 27);
  }
}

void network::send(const string &message) {
  struct evbuffer *output;
  struct bufferevent *bev;
  string sendMessage = message;
	//writeLog(logfile,"Sending: '%s'",message.c_str());
  sendMessage.append("\n");
  for (connectionMap::iterator it = connections.begin();
       it != connections.end(); ++it) {
    bev = it->second;
    output = bufferevent_get_output(bev);
    evbuffer_add(output, sendMessage.c_str(), sendMessage.length());
  }
}
void network::remove(const string &key) {
  //writeLog(logfile, "removing %s\n", key.c_str());
  connections.erase(key);
}
void network::add(const string &key, struct bufferevent *newEvent) {
  //writeLog(logfile, "adding %s\n", key.c_str());
  connections[key] = newEvent;
}

network::network(command *theExecutor) {
  execute = theExecutor;
  port = 2020;
  logfile = stdout;
}
network::network(command *theExecutor, int passedPort) {
  execute = theExecutor;
  logfile = stdout;
  port = passedPort;
}
network::~network() {}
void network::setLogFile(char *logFileName) {
  logfile = fopen(logFileName, "w");
  if (logfile == NULL) {
    writeLog(stderr, "Error opening log file: %s using stdout for logfile\n",
             logFileName);
    logfile = stdout;
  }
  setbuf(logfile, NULL);
}
void network::stop() {
  //create a connection to the server in here to clear the accept
  send("shutting down now\n");
  event_base_loopbreak(base);
  int connector;
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(port);
  connector = socket(AF_INET, SOCK_STREAM, 0);
  connect(connector,(struct sockaddr *) &sin,sizeof(sin));
  close(connector);
}
void network::getCommands() {
  evutil_socket_t listener;
  struct sockaddr_in sin;
  //  struct event_base *base;
  struct event_interp *interp = new event_interp;
  struct event *listener_event;
	base = event_base_new();
  interp->base = base;
  if (!interp->base)
    return; /*XXXerr*/
  interp->interface = this;

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(port);

  listener = socket(AF_INET, SOCK_STREAM, 0);
  evutil_make_socket_nonblocking(listener);

  int one = 1;
  setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("bind");
    return;
  }

  if (listen(listener, 16) < 0) {
    perror("listen");
    return;
  }

  listener_event = event_new(base, listener, EV_READ | EV_PERSIST,
                             do_accept, (void *)interp);
  /*XXX check it */
  event_add(listener_event, NULL);

  event_base_dispatch(base);
}

void network::run(char *buffer) {
  string commandLine, deviceName, commandStr, subCommandStr, value;
  commandLine = (string)buffer;
  deviceName = chopWord(commandLine);
  commandStr = chopWord(commandLine);
  subCommandStr = chopWord(commandLine);
  value = chopWord(commandLine);
  removeSpaces(deviceName);
  removeSpaces(commandStr);
  removeSpaces(subCommandStr);
  removeSpaces(value);
  if (deviceName != "close") {
    execute->run(deviceName, commandStr, subCommandStr, value);
  }
}
