# Versioning system
VERSION ?= 0.00-0000
REVISION ?= devbuild 

CPP=$(CROSS_COMPILE)g++

OBJS = psd.o trig_params.o  sampling_params.o psd_params.o proj_misc.o
#OBJS = psd.o trig_params.o psd_params.o trig_params.o sampling_params.o
SRCS = $(subst .o,.c, $(OBJS)))

TARGET = psd

CFLAGS  = -g -Wall -Werror
CFLAGS +=  -std=c++0x
CFLAGS += -I../../api/include
CFLAGS += -I/opt/redpitaya/include
CFLAGS += -DVERSION=$(VERSION) -DREVISION=$(REVISION) 
LDFLAGS = -L/opt/redpitaya/lib
LDLAGS +=  -lm -lpthread
LDFLAGS += -ljsoncpp
LDLIBS = -lm -lpthread -l:librp.so
#CFLAGS +=  -librp

%.o : %.cpp
	$(CPP) -c $(CFLAGS) $< -o $@

#%.o: %.cpp
#	$(CC) -c $(CFLAGS) $< -o $@
#	$(CPP) -c -o  $@  $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CPP) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET) *.o
 
