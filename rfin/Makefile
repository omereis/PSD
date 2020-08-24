# Versioning system
VERSION ?= 0.00-0000
REVISION ?= devbuild 

#CC=$(CROSS_COMPILE)gcc
CC=$(CROSS_COMPILE)g++

OBJS = cpp_rfin.o
#OBJS = rfin.o
#OBJS = rfin.o fpga.o
#OBJS = psd.o trig_params.o psd_params.o trig_params.o sampling_params.o
SRCS = $(subst .o,.c, $(OBJS)))

TARGET = rfin

CFLAGS  = -g -Wall -Werror
#CFLAGS +=  -std=gnu99
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

$(TARGET): $(OBJS)
	#gcc -o $(TARGET) $(OBJS)
	gcc -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET) *.o



