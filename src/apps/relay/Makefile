CC=g++
MSG=$(HOME)/bin/message
XGETTEXT=/usr/local/bin/xgettext
MSGFMT=/usr/local/bin/msgfmt
MAKE=/usr/bin/make

CFLAGS=-g -pipe -Wall -D_REENTRANT
LIBDIRS=-L/usr/local/lib -L/usr/lib -L$(HOME)/src/libs -L/usr/local/lib/mysql 
LIBS=-larchspace -lcrypt -lcipher -lkvm -static -lmysqlclient -lc_r -lintl
INCLUDES=-I/usr/include -I/usr/local/include -I../../libs/include \
		-I/usr/local/include/mysql -I/usr/local/include/pth
DEFINES=-DDEBUG

HEADERFILES=relay.h data.h pages.h script.h protocol.h
CCFILES=main.cc relay.cc relay.conn.cc web.conn.cc \
		user.cc user.table.cc script.cc race.cc \
		admin.cc admin.table.cc \
		page/notfound.cc page/common.cc page/create.cc\
		page/login.cc page/create2.cc page/admin.cc \
		server.cc server.list.cc banner.cc
MSGFILES=page/login.cc
CCOBJS=$(CCFILES:.cc=.o)

.cc.o: $(HEADERFILES)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) $(DEFINES)

all:relay install

relay:$(CCOBJS) ../../libs/libarchspace.a
	$(CC) -o relay $(CCOBJS) $(LIBDIRS) $(LIBS) 

clean:
	rm -f *.o 
	rm -f page/*.o

install:relay
	cp relay $(HOME)/global/bin

CLEAN:
	/usr/local/bin/mysql Relay < $(HOME)/src/apps/relay/DB/all.sql

INSTALL:
	$(HOME)/global/bin/globalctl stop
	/usr/local/bin/mysql Relay < $(HOME)/src/apps/relay/DB/all.sql
	sleep 2
	make
	$(HOME)/global/bin/globalctl start

RESTART:
	$(HOME)/global/bin/globalctl stop
	sleep 2
	make
	$(HOME)/global/bin/globalctl start



message: msg/relay.po

msg/relay.src: $(MSGFILES)
	$(MSG) -o msg/relay.src $(MSGFILES)

msg/relay.po: msg/relay.src
	@if [ -f msg/relay.po]; then \
		$(XGETTEXT) -a -j -d archspace \
			--language=C++ --output=msg/relay.po \
			--width=80 msg/relay.src; \
	else \
		$(XGETTEXT) -a -d archspace \
			--language=C++ --output=msg/relay.po \
			--width=80 msg/relay.src; \
	fi
	@cd msg && $(MAKE) || exit 1;
