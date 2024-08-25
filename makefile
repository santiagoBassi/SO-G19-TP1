CC = gcc
CFLAGS = -g -Wall -pedantic -Iincludes -std=c99 -Werror

APPFILES = ./src/app.c
SLAVEFILES = ./src/slave.c
VIEWFILES = ./src/view.c

APPOBJ = $(APPFILES:.c=.o)
SLAVEOBJ = $(SLAVEFILES:.c=.o)
VIEWOBJ = $(VIEWFILES:.c=.o)

APPBINARY = app
SLAVEBINARY = slave
VIEWBINARY = view

.PHONY: all clean cleanObj

all:  $(SLAVEBINARY) $(VIEWBINARY) cleanObj #$(APPBINARY)

$(APPBINARY): $(APPOBJ)
	$(CC) $(CFLAGS) -o $(APPBINARY) $(APPOBJ)

$(SLAVEBINARY): $(SLAVEOBJ)
	$(CC) $(CFLAGS) -o $(SLAVEBINARY) $(SLAVEOBJ)

$(VIEWBINARY): $(VIEWOBJ)
	$(CC) $(CFLAGS) -o $(VIEWBINARY) $(VIEWOBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(APPBINARY) $(SLAVEBINARY) $(VIEWBINARY)
	make cleanObj

cleanObj:
	rm -f $(APPOBJ) $(SLAVEOBJ) $(VIEWOBJ)
