# ireader Makefile
# Pierre-Marc Fournier
# July 23 2004

CXXFLAGS=-g -Wall
SOURCES=$(wildcard *.cpp) 
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))
TARGET=sopti

all: $(TARGET)

clean:
	rm -f *.o $(TARGET) 

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET)	$(OBJS)
	
