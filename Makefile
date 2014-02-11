# Platform detection
SYS := $(shell gcc -dumpmachine)

# For raspberry pi
ifneq (, $(findstring arm-linux, $(SYS)))
	CC       = g++-4.7
	CC_FLAGS = -c -std=c++11 -DRASPBERRY
	LD_FLAGS = -lwiringPi
# For cygwin
else ifneq (, $(findstring cygwin, $(SYS)))
	CC       = g++
	CC_FLAGS = -c -Wall -Wno-switch -pedantic -std=c++11 -D__USE_W32_SOCKETS -D_WIN32_WINNT=0x0501 -DCYGWIN
	LD_FLAGS = -L/usr/local/lib -lws2_32 -lmswsock
endif

# Source settings
EXECUTABLE  = pibell-server
ACTIONS    += Audio.cpp
SOURCES    += main.cpp Client.cpp Listener.cpp Server.cpp
LD_FLAGS   += -lboost_system -lboost_filesystem

# Force always to rebuild main
 .PHONY: $(EXECUTABLE)

# System
OBJECTS = $(SOURCES:.cpp=.o)
all: $(BUILD_DIR) $(SOURCES) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $^ -o $@ $(LD_FLAGS)

.cpp.o:
	$(CC) $(CC_FLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
