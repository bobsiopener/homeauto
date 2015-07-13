#ifndef NETWORK_H_
#define NETWORK_H_
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <list>
#include <string.h>
#include "command.h"
#include "utils.h"

typedef tr1::unordered_map<string, struct bufferevent *> connectionMap;

class network {
public:
  network(command *);
  network(command *, int);
  ~network();
  void linkCommand();
  void getCommands();
  void run(char *);
  void stop();
  void send(const std::string &);
  void add(const std::string &, struct bufferevent *);
  void remove(const std::string &);
  struct event_base *base;
  void setLogFile(char *);
  FILE *logfile;

private:
  connectionMap connections;
  int port;
  command *execute;
};

#endif
