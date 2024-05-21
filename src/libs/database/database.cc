#include "database.h"
#include "store.h"
#include "cgi.h"

TZone gDatabaseZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDatabase),
	0,
	0,
	NULL,   
	"Zone CDatabase"
};

bool
	CDatabase::mServerLoop = true;

CDatabase::CDatabase():CApplication(), CFIFO(), CSortedList(100, 100),
	CMySQL()
{
}

CDatabase::~CDatabase()
{
	/*remove_all();
	system_log("destroy database");
	CMySQL::close();
	CFIFO::close();*/
}

void 
CDatabase::set_signalset()
{
	//sigemptyset(&mSignalSet);
	//sigaddset(&mSignalSet, SIGSEGV);
	//sigaddset(&mSignalSet, SIGTERM);
}

void 
CDatabase::set_signalfunction()
{
/*	signal(SIGALRM, signal_stop);
	signal(SIGTERM, signal_stop);
	signal(SIGPIPE, signal_ignore);
	signal(SIGBUS, signal_exits);
	signal(SIGINT, signal_ignore);
	signal(SIGHUP, signal_exits);
	signal(SIGQUIT, signal_exits);
	signal(SIGILL, signal_exits);
	signal(SIGTRAP, signal_exits);
	signal(SIGIOT, signal_exits);
	signal(SIGFPE, signal_ignore);
	signal(SIGALRM, signal_ignore);
	signal(SIGUSR1, signal_ignore);
	signal(SIGUSR2, signal_ignore);
	signal(SIGTSTP, signal_ignore);
	signal(SIGTTIN, signal_ignore);
	signal(SIGTTOU, signal_ignore);
	signal(SIGIO, signal_exits);
	signal(SIGXCPU, signal_exits);
	signal(SIGVTALRM, signal_exits);
	signal(SIGWINCH, signal_ignore);
	signal(SIGURG, signal_ignore);*/
}

void
CDatabase::signal_stop(int aSignalNumber)
{
//	system_log("database stop by signal(%d)", aSignalNumber);
//	CDatabase::mServerLoop = false;
}


bool
CDatabase::free_item(TSomething aItem)
{
/*	CSQL 
		*SQL = (CSQL*)aItem;

	delete SQL;
*/
	return true;
}

int
CDatabase::compare(TSomething aItem1, TSomething aItem2) const
{
/*	CSQL
		*SQL1 = (CSQL*)aItem1,
		*SQL2 = (CSQL*)aItem2;
	int 
		Ret = strcmp(SQL1->get_table(), SQL2->get_table());
	if (Ret) return Ret;

	if (SQL1->get_count() > SQL2->get_count()) return 1;
	if (SQL1->get_count() < SQL2->get_count()) return -1;*/
	return 0;
}    

int
CDatabase::compare_key(TSomething aItem, TConstSomething aKey) const
{
//	(void)aItem;
//	(void)aKey;
//	system_log("CDatabase could not use find_key()");
	return -1;
}

int 
CDatabase::read()
{
/*	static
		time_t t = time(0);
	static
		int count_run = 0, read_size = 0;

	if( t <= time(0)-60 ){
		system_log( "database read run %d packets %d bytes during %d seconds", count_run, read_size, time(0)-t );
		read_size = count_run = 0;
		t = time(0);
	}

	CPacket
		*Packet = new CPacket();

	if (!Packet) return -1;

	int 
		Count;

	if ((Count = read_packet(Packet)) > 0)
	{
		mInput.append(Packet);
		count_run++;
		read_size += Packet->count();
	} else {
		delete Packet;
		return -1;
	}

	return Count;*/
}

int 
CDatabase::write()
{
	return 0;
}

int
CDatabase::write_SQL()
{
/*	static
		time_t t = time(0);
	static
		int count_run = 0;

	if( t <= time(0)-60 ){
		system_log( "database write SQL run %d during %d seconds %d query remaining", count_run, time(0)-t, length() );
		count_run = 0;
		t = time(0);
	}

//	system_log("push SQL to MySQL server:%d", length());

	if (!length()) return 0;

	CString
		Query;
/*
	CString
		Table;
	Table = " ";

	Query = "LOCK TABLES ";

	for(int i=0; i<length(); i++)
	{
		CSQL
			*SQL = (CSQL*)CSortedList::get(i);
		if (strcmp((char*)Table, SQL->get_table())) 
		{
			Query.append(SQL->get_table());
			Query.append(" WRITE,");
			Table = SQL->get_table();
		}
	}
	Query[Query.length()-1] = ' ';
	CMySQL::query((char*)Query);
	system_log((char*)Query);
*/

	/*time_t
		now,
		old;
	while(CSortedList::length())
	{
		CSQL 
			*SQL = (CSQL*)CSortedList::get(0);

		old = time(0);
		if( CMySQL::query(SQL->get_query()) && CMySQL::error_no() == 2006 ){
			sleep(1);
			break;
		}
		now = time(0);

		// added by Nara for server optimizing
		if( now-old > 3 )
			system_log( "Too Late Response[%d sec] %d remaining: %s", now-old, length()-1, SQL->get_query() );
		count_run++;

		CSortedList::remove(0);
	}

//	CMySQL::query("UNLOCK TABLES");
//	system_log("UNLOCK TABLES");
*/
	return 0;
}

bool
CDatabase::analysis()
{
/*	static unsigned int
		LastCount = 0;
*//*	static
		time_t t = time(0);
	static
		int n_packet, n_query;

	if( t <= time(0)-60 ){
		system_log( "database analysis packet %d query %d during %d seconds %d input remaining", n_packet, n_query, time(0)-t, mInput.length() );
		n_packet = n_query = 0;
		t = time(0);
	}

	while(!mInput.is_empty())
	{
		CMessage
			Message;

		unsigned char
			DummyBlockCount;
		unsigned int
			CurrentCount = 0;

		CPacket
			*Packet = (CPacket*)mInput.first();
		n_packet++;

		if (Message.set_packet(Packet->get(), MESSAGE_RECEIVE) == false)
		{
			char *
				Data = (char *)Message.get_data();
			if (!Data)
			{
				system_log("ERROR : Removing a packet cause it has no data!");
				mInput.remove(Packet);
				continue;
			}

			Message.get_item(DummyBlockCount);
			Message.get_item(CurrentCount);

/*			if (LastCount == 0) LastCount = CurrentCount;
*//*		}

//		system_log("message type:%04x  packet size %d", Message.type(), Packet->count());

		if (Message.type() == MESSAGE_PUSH_REQUEST)
		{
			write_SQL();
			mInput.remove(Packet);
			continue;
		}

		Message.get_item(DummyBlockCount);
		Message.get_item(CurrentCount);
		if (Message.type() == MESSAGE_SQL_SEND)
		{
			char 
				*Data = (char*)Message.get_data();

			if (Data == NULL) return false;
			while (Data && ((Data[2] & 0x80) != 0x80))
			{
//				system_log("get_data %x", Data[2]);
				Packet = (CPacket *)mInput.next();
				if (Packet == NULL)
				{	
//					system_log("wait next packet");
					return true;
				}
//				system_log("Block Count %x", Data[2]);

				CMessage Msg;
				Msg.set_packet(Packet->get(), MESSAGE_RECEIVE);
//				system_log("packet to message %x", Msg.type());

				if (Msg.type() != MESSAGE_SQL_SEND) return false;
				Data = (char*)Msg.get_data();
//				system_log("Block Count %x", Data[2]);
			}
			if (!Data) return false;
			if ((Data[2] & 0x80) != 0x80) return false;

			Packet = (CPacket*)mInput.first();

			CSQL
				*SQL = new CSQL();
			CString 
				Query;
			char 
				*String;
			int 
				Length;

			char
				Buffer[STRING_DATA_BLOCK_SIZE+1];

			unsigned char
				BlockCount;
			unsigned int 
				Count;

			do {
				CMessage Message;
				Message.set_packet(Packet->get(), MESSAGE_RECEIVE);
//				system_log("merging message type:%x  packet size %d", Message.type(), Packet->count());
				Data = (char*)Message.get_data();
				Message.get_item(BlockCount);
/*				system_log("%02x %02x %02x %02x %02x %02x %02x %02x",
						Data[0], Data[1], Data[2], Data[3],
						Data[4], Data[5], Data[6], Data[7]);

				system_log("merging Block Count %x", BlockCount);*/
/*
				if (!BlockCount) return false;
				if ((BlockCount & 0x7F) == 1)
				{
					Message.get_item(Count);
					SQL->set_count(Count);

					Message.get_item(String, Length, true);
					memcpy(Buffer, String, Length);
					Buffer[Length] = 0;
					SQL->set_table(Buffer);

					Message.get_item(String, Length, true);
					Query.append(String, Length);
					CString
						DebugString;
					DebugString.clear();
					DebugString.append(String, Length);
				}
				else
				{
					unsigned int 
						SCount;
					Message.get_item(SCount);
					if (Count != SCount) 
					{
						delete SQL;
						return false;
					}

					Message.get_item(String, Length, true);
					Query.append(String, Length);
				}
				mInput.remove(Packet);
			Packet = (CPacket*)mInput.first();
			} while(!(BlockCount & 0x80));
			SQL->set_query(Query.get_data());

			n_query++;
			insert_sorted(SQL);
		}
		else
		{
			system_log("wrong message type:%x  packet size %d", Message.type(), Packet->count());
			mInput.remove(Packet);
			continue;
		}
	}*/
	return true;
}

int
CDatabase::run()
{
/*	struct timeval
		Timeout;

	int
		Res;

	Timeout.tv_usec = 0;
	Timeout.tv_sec = 1;

	fd_set 
		Checks,
		Reads,
		Exceptions;
	
	FD_ZERO(&Checks);

	FD_SET(mSocket, &Checks);

	while (mServerLoop)
	{
		Reads = Checks;
		Exceptions = Checks;

		signal_masking();
		Res = ::select(mSocket+1, 
				&Reads, (fd_set*)NULL, &Exceptions, &Timeout);
		signal_unmasking();

		if (Res < 0) return -1;

		if (FD_ISSET(mSocket, &Reads))
		{
			read();
		}

		if (FD_ISSET(mSocket, &Exceptions))
		{
			mServerLoop = false;
			system_log("ERROR : Exception FD has occured");
		}
	
		//system_log( "analysis %d input", mInput.length() );
		if (!analysis()) 
		{
			system_log("Could not understand a receive packet");
			break;
		}

		// added by Nara 2001. 6. 19. for server optimizing
		write_SQL();

/*	removed by Nara 2001. 6. 19. for server optimizing
		if (time(0) - PushForce > PUSH_FORCE_INTERVAL)
		{
			//start telcard 2001/05/15
			time_t t = time(0);
			int l = length();
			int m = mInput.length();
			//end telcard 2001/05/15
			write_SQL();
			PushForce = time(0);
			system_log("%d second elpased CDatabase::write_SQL() for %d input, %d queries.", time(0)-t, m, l);	//telcard 2001/05/15
		}
*/
/*
		if (getppid() == 1) mServerLoop = false;
	}
*/
//	write_SQL();
/*	CMySQL::close();
	CFIFO::close();
	kill(getppid(), SIGTERM);*/
	return 0;
}
