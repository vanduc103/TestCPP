CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		TestCpp.o

LIBS =

TARGET =	TestCpp

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
