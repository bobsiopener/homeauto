#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/asio.hpp>
#include <pthread.h>

bool continu = true;

void *listenThread(void *threadArg) {
  using boost::asio::ip::tcp;
  tcp::socket *s;
  s = (tcp::socket *)threadArg;
  char reply[8000];
  while (continu) {
    boost::system::error_code error;
    s->read_some(boost::asio::buffer(reply), error);
    fprintf(stdout, "%s", reply);
  }
  fprintf(stdout, "listen exit\n");
  pthread_exit((void *)0);
}

void *commandThread(void *threadArg) {
  using boost::asio::ip::tcp;
  tcp::socket *s;
  s = (tcp::socket *)threadArg;
	fprintf(stdout,"\n");
  char prompt[] = "HomeAutoClient $ ";
  std::string command;
  using_history();
  while (continu) {
    using namespace std; // For strlen.
    rl_instream = stdin;
    command = readline(prompt);
    add_history(command.c_str());
    if (command == "exit") {
      continu = false;
      command = "close";
    } else if (command == "shutdown") {
      continu = false;
    }
    command.append("\n");
    size_t request_length = command.length();
    using boost::asio::ip::tcp;
    boost::asio::write(*s,
                       boost::asio::buffer(command.c_str(), request_length));
    usleep(150000);
  }
  fprintf(stdout, "command exit\n");
  pthread_exit((void *)0);
}
int main() {
  char address[] = "192.168.10.185";
  // char address[] = "127.0.0.1";
  char port[] = "2020";
  pthread_t threads[5];
  boost::asio::io_service io_service;
  using boost::asio::ip::tcp;
  tcp::resolver resolver(io_service);
  tcp::resolver::query query(tcp::v4(), address, port);
  tcp::resolver::iterator iterator = resolver.resolve(query);

  tcp::socket *s = new tcp::socket(io_service);
  boost::asio::connect(*s, iterator);
  setbuf(stdout, NULL);

  pthread_create(&threads[0], NULL, listenThread, s);
  usleep(100000);
  pthread_create(&threads[1], NULL, commandThread, s);
  pthread_exit(NULL);
  fprintf(stdout, "Exiting Program\n");
  return (0);
}
