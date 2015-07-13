CXXFLAGS:=-Wall -g -fsanitize=address -lreadline -lncurses -pthread -lboost_system -lboost_thread -lpthread
CLIENTOBJECTS:=networkClient.o cmdline.o utils.o
LIBS:=/usr/local/lib/libhistory.a /usr/local/lib/libreadline.a /usr/lib/libncurses.a
HomeAutoClient: HomeAutoClient.cpp
	clang++ HomeAutoClient.cpp -pthread -lboost_system -lboost_thread -lpthread /usr/local/lib/libhistory.a /usr/local/lib/libreadline.a /usr/lib/libncurses.a -o HomeAutoClient
clean:
	rm *.o
	
