CXXFLAGS:=-Wall -g 
LINKFLAGS:=-pthread -levent -lpthread 
OBJECTS:=utils.o plm.o responder.o device.o motion.o IOlink.o switch.o dimmer.o thermostat.o config.o timedEvent.o scheduler.o cmdline.o weather.o xml.o command.o network.o 6button.o
HomeAutoServer: $(OBJECTS) HomeAutoServer.cpp
	clang++ $(CXXFLAGS) $(OBJECTS) HomeAutoServer.cpp -o HomeAutoServer $(LINKFLAGS)
NoThread:
	clang++ $(CXXFLAGS) $(OBJECTS) HomeAutoServer.cpp -fsanitize=address -o HomeAutoServer_nothread $(LINKFLAGS)
CheckAddress:   
	clang++ $(CXXFLAGS) $(OBJECTS) HomeAutoServer.cpp -fsanitize=address -o HomeAutoServer $(LINKFLAGS)
CheckThreads: 
	clang++ $(CXXFLAGS) $(OBJECTS) HomeAutoServer.cpp -fsanitize=thread -o HomeAutoServer $(LINKFLAGS)
CheckUndef:  
	clang++ $(CXXFLAGS) $(OBJECTS) HomeAutoServer.cpp -fsanitize=undefined -o HomeAutoServer $(LINKFLAGS)
HomeAutoServer_nothread: $(OBJECTS) HomeAutoServer_nothread.cpp
	clang++ $(CXXFLAGS) $(OBJECTS) HomeAutoServer_nothread.cpp -o HomeAutoServer_nothread $(LINKFLAGS)
plm.o: plm.cpp utils.o
	clang++ $(CXXFLAGS) -c plm.cpp
weather.o: ./weather/weather.cpp
	clang++ $(CXXFLAGS) -c ./weather/weather.cpp
xml.o: ./weather/xml.cpp
	clang++ $(CXXFLAGS) -c ./weather/xml.cpp
responder.o: responder.cpp scheduler.o plm.o config.o device.o
	clang++ $(CXXFLAGS) -c responder.cpp
device.o: ./devices/device.cpp plm.o
	clang++ $(CXXFLAGS) -c ./devices/device.cpp
motion.o: ./devices/motion.cpp plm.o
	clang++ $(CXXFLAGS) -c ./devices/motion.cpp
IOlink.o: ./devices/IOlink.cpp plm.o
	clang++ $(CXXFLAGS) -c ./devices/IOlink.cpp
switch.o: ./devices/switch.cpp plm.o
	clang++ $(CXXFLAGS) -c ./devices/switch.cpp
dimmer.o: ./devices/dimmer.cpp plm.o
	clang++ $(CXXFLAGS) -c ./devices/dimmer.cpp
6button.o: ./devices/6button.cpp plm.o
	clang++ $(CXXFLAGS) -c ./devices/6button.cpp
thermostat.o: ./devices/thermostat.cpp plm.o
	clang++ $(CXXFLAGS) -c ./devices/thermostat.cpp
config.o: config.cpp utils.o thermostat.o
	clang++ $(CXXFLAGS) -c config.cpp
timedEvent.o: ./scheduler/timedEvent.cpp plm.o
	clang++ $(CXXFLAGS) -c ./scheduler/timedEvent.cpp
scheduler.o: ./scheduler/scheduler.cpp timedEvent.o config.o utils.o
	clang++ $(CXXFLAGS) -c ./scheduler/scheduler.cpp
utils.o: utils.cpp
	clang++ $(CXXFLAGS) -c utils.cpp
cmdline.o: cmdline.cpp plm.o config.o
	clang++ $(CXXFLAGS) -c cmdline.cpp
command.o: command.cpp utils.o
	clang++ $(CXXFLAGS) -c command.cpp
network.o: network.cpp utils.o
	clang++ $(CXXFLAGS) -c network.cpp
clean:
	rm *.o
	
