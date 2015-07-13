#include <string.h>
#include "config.h"
#include "plm.h"
#include "./devices/device.h"
#include "./scheduler/scheduler.h"
#include "./weather/weather.h"

#ifndef COMMAND_H_
#define COMMAND_H_
class cmdline;
class network;
class responder;

class command {
public:
  command() {};
	virtual ~command() {};
  command(config *, plm *, responder *, scheduler *, weather *);
  virtual void run(const string &, const string &, const string &,
                     const string &);
  void setCmdLine(cmdline *);
  void setNetwork(network *);
  virtual string done();
  string getAllXML();

protected:
  bool continu;
  int verbose;
  config *settings;
  responder *theResponder;
  scheduler *theScheduler;
  weather *theWeather;
  plm *interface;
  cmdline *commandLine;
  network *networkInterface;
  string getCategories();
};

#endif
