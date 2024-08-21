CC = gcc
CFLAGS = -g -Wall -pedantic

APPFILES = app.c
SLAVEFILES = slave.c
VIEWFILES = view.c

APPOBJ = $(APPFILES:.c=.o)
SLAVEOBJ = $(SLAVEFILES:.c=.o)
VIEWOBJ = $(VIEWFILES:.c=.o)

APPBINARY = app
SLAVEBINARY = slave
VIEWBINARY = view

.PHONY: all clean app slave view

all: 
	make app
	make slave
	make view

app: $(APPBINARY)

slave: $(SLAVEBINARY)

view: $(VIEWBINARY)

$(APPBINARY): $(APPOBJ)
	$(CC) $(CFLAGS) -o $(APPBINARY) $(APPOBJ)

$(SLAVEBINARY): $(SLAVEOBJ)
	$(CC) $(CFLAGS) -o $(SLAVEBINARY) $(SLAVEOBJ)

$(VIEWBINARY): $(VIEWOBJ)
	$(CC) $(CFLAGS) -o $(VIEWBINARY) $(VIEWOBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(APPOBJ) $(SLAVEOBJ) $(VIEWOBJ) $(APPBINARY) $(SLAVEBINARY) $(VIEWBINARY)
