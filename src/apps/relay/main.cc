#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#include "debug.h"
#include "util.h"
#include "net.h"
#include "frame.h"
#include "runtime.h"
#include "database.h"

#include "relay.h"

bool
CRelayPageStation::registration()
{
	insert_sorted(new CPageNotFound());	
	insert_sorted(new CPageCreate());	
	insert_sorted(new CPageCreate2());	
	insert_sorted(new CPageLogin());	

	return true;
}

bool
CRelayDatabase::initialize(int aArgc, char** aArgv)
{
	bool
		Res;
	Res = CApplication::initialize(aArgc, aArgv);
	char
		*SystemLog =
			mConfiguration.get_string("Server",
					"SystemLogFile", "./systemlog");
	strcpy(SYSTEM_LOGFILE, SystemLog);

	system_log("start to initalize database");

	char
		*Host = mConfiguration.get_string("Database", "Host", NULL);
	char
		*Database = mConfiguration.get_string("Database",
												"Database", NULL);
	char
		*User = mConfiguration.get_string("Database", "User", NULL);
	char
		*Password = mConfiguration.get_string("Database",
												"Password", NULL);
	if (!Host || !Database || !User || !Password)
	{
		system_log("could not get information to open MySQL Server");
		return false;
	}

	system_log("Host:%s, User:%s, Password:%s, Database:%s",
			Host, User, Password, Database);

	if (!CMySQL::open(Host, User, Password, Database)) return false;
		system_log("open mysql connection");

	char
		*DBConnection = mConfiguration.get_string("Database",
					"Connection", NULL);
	if (!DBConnection)
	{
		system_log("Could not get DBConnection in database");
		return false;
	} 

	int
		Ret = 0;
	int
		Count = 0;

	while(Count++ < 30)
	{
		Ret = CFIFO::open(DBConnection, FIFO_RECEIVE);
		if (Ret < 0) sleep(1);
		else break;
	}
	if (Ret < 0) return false;

	system_log("Open a database connection %d %s", Ret, DBConnection);

	return Res;
};

int
main(int argc, char* argv[])
{
	// for deamon
	int
		Child;

	if ((Child = fork()) < 0) return -1;
	else if (Child != 0) exit(0);

	setsid();
	umask(0);

	if ((Child = fork()) < 0) return -1;

	if (Child == 0)
	{
		CRelayDatabase
			Database;
		if (!Database.initialize(argc, argv))
		{
			system_log("Cannot start database process");
			return -1;
		}
		Database.run();
		system_log("terminate database");
		return 0;
	} else {
		char
			DomainDir[255];
		strcpy(DomainDir, getenv("HOME"));
		strcat(DomainDir, "/locale");
		setlocale(LC_ALL, "");

		bindtextdomain("relay", DomainDir);
		textdomain("relay");

		CRelay
			Relay;
		if (!Relay.initialize(argc, argv))
		{
			system_log("Cannot start relay server!");
		} else 
		{
			Relay.run();
			Relay.wait_key_thread();
		}

		kill(Child, SIGTERM);
		int
			Status;
		wait(&Status);
		system_log("wait status %d", Status);
	}
	exit(EXIT_SUCCESS);
}

