# sopti Makefile
# Pierre-Marc Fournier

CXXFLAGS=-g -Wall
SOURCES=$(wildcard *.cpp) 
HEADERS=$(wildcard *.hpp)
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))
TARGET=sopti

DIST_OTHER=data/update COPYING legende_cours_poly.txt

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
	
