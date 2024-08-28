CC = gcc
CFLAGS = -g -Wall -pedantic -Iincludes

APPFILES = ./src/app.c
SLAVEFILES = ./src/slave.c
VIEWFILES = ./src/view.c

APPOBJ = $(APPFILES:.c=.o)
SLAVEOBJ = $(SLAVEFILES:.c=.o)
VIEWOBJ = $(VIEWFILES:.c=.o)

APPBINARY = app
SLAVEBINARY = slave
VIEWBINARY = view

RESULT=result.txt

.PHONY: all clean app slave view cleanObj

all: $(APPBINARY) $(SLAVEBINARY) $(VIEWBINARY) cleanObj

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
	rm -f $(APPBINARY) $(SLAVEBINARY) $(VIEWBINARY) $(RESULT)
	make cleanObj

cleanObj:
	rm -f $(APPOBJ) $(SLAVEOBJ) $(VIEWOBJ)
