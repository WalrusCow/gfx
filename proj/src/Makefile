SOURCES = $(wildcard *.cpp primitives/*.cpp materials/*.cpp lights/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
LDFLAGS = $(shell pkg-config --libs lua5.1) -llua5.1 -lpng -lpthread
CPPFLAGS = $(shell pkg-config --cflags lua5.1) -std=c++1y
CXXFLAGS = -I. $(CPPFLAGS) -W -Wall -g -std=c++1y
CXX = g++
MAIN = rt
RM = rm -f

all: $(MAIN)

depend: $(DEPENDS)

clean:
	$(RM) $(OBJECTS) $(MAIN)

$(MAIN): $(OBJECTS)
	@echo Creating $@...
	@$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) -o $@ -c $(CXXFLAGS) $<

depend: .depend

.depend: $(SRCS)
	touch ./.depend
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend

include .depend
