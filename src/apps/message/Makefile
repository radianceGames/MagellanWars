CC=g++
CFLAGS=-g -pipe -Wall
LIBDIRS=-L/usr/local/lib -L/usr/lib 
LIBS=-lm -lstdc++
INCLUDES=-I/usr/include -I/usr/local/include  
DEFINES=-DDEBUG
LEX=flex

HEADERFILES=
CCFILES=message.cc lex.cc

CCOBJS=$(CCFILES:.cc=.o)

all:lex.cc message install 

lex.cc:parse.l
	$(LEX) -olex.cc -P_MSG parse.l

.cc.o: $(HEADERFILES)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) $(DEFINES)

message:$(CCOBJS) 
	$(CC) -o message $(CCOBJS) $(LIBDIRS) $(LIBS) 


clean:
	rm -f $(CCOBJS) 

install:message
	cp message /usr/sbin
