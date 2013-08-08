#CC=arm-unknown-linux-gnu-gcc
CC=gcc

LIBS = -lpthread  -lyajl -ldl -lm -lcurl
FLAGS = -Wall -g -I../../include -L../../lib  -L/usr/local/lib -Wl,-rpath=../../lib
EXECS = spotter

all: $(EXECS)

spotter: spotter.c spotter_services.c ../../lib/*.c ../../lib/rbtree/*.c
		 $(CC) $(FLAGS) -o $@ $? $(LIBS)
		cp $(EXECS) ../build/
		cp bt_sensor.cfg ../build/

clean:
		rm -f $(EXECS)

plugins:
		$(CC) -I../../include -fPIC -g -c -lbluetooth -lm -lpthread -lssl inq.c reporter.c
		$(CC) -export-dynamic -shared -g -Wl,-soname,libbt_sensor.so.1 -o libbt_sensor.so.1.0.1 inq.o reporter.o -lc -lbluetooth -lm -lpthread -lssl
		cp *.so* ../../lib/spot_plugins/
