
# Created by ZigFisher

# build helloworld executable when user executes "make"
rc: rc.o RCSwitch.o WProgram.o
	$(CXX) $(LDFLAGS) -std=c++0x -pthread rc.o RCSwitch.o WProgram.o -o rc

rc.o: rc.cpp
	$(CXX) $(CXXFLAGS) -std=c++0x -pthread  -c rc.cpp

RCSwitch.o: RCSwitch.cpp
	$(CXX) $(CXXFLAGS) -std=c++0x -pthread  -c RCSwitch.cpp

WProgram.o: WProgram.cpp
	$(CXX) $(CXXFLAGS) -std=c++0x -pthread  -c WProgram.cpp

# remove object files and executable when user executes "make clean"
clean:
	rm *.o rc