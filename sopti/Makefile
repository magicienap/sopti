# ireader Makefile
# Pierre-Marc Fournier
# July 23 2004

CXXFLAGS=-g -Wall
SOURCES=$(wildcard *.cpp) 
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))
TARGET=sopti

# Flags to use for a production executable
PROD_CXXFLAGS="-O3 -DNODEBUG"
PROD_TARGET=sopti_prod
PROD_RUNNING_DIR=~/sopti_run

all: $(TARGET)

clean:
	rm -f *.o $(TARGET) 

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET)	$(OBJS)

.PHONY: web

web: exec
	cp $(PROD_TARGET) $(PROD_RUNNING_DIR)/sopti

.PHONY: exec

prod:
	make clean
	make CXXFLAGS=$(PROD_CXXFLAGS) TARGET=$(PROD_TARGET) $(PROD_TARGET)
	
