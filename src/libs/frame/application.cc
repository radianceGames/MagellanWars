#include "frame.h"
#include <cstdlib>

CApplication*
	gApplication = NULL;

CApplication::CApplication()
{
}

CApplication::~CApplication()
{
}

bool
CApplication::initialize(int Argc, char **Argv)
{
	int loaded = 0;

	set_signalset();
	set_signalfunction();

	for(int i=1; i<Argc; i++)
	{
		if (strcmp(Argv[i], "-c") == 0)
		{
			if (i+1 < Argc) {
				if (!mConfiguration.load(Argv[i+1]))
				{
					system_log("Cannot read configuration file %s \n", 
							Argv[i+1]);	
					return false;
				}
				loaded = 1;
			}
		}
	}

	if( loaded == 0 ) {
		if( !mConfiguration.load( "/etc/archspace/archspace.config" ) ) {
			system_log("Cannot read configuration file /etc/archspace/archspace.config\n" );
			return false;
		}
	}
	return true;
}

void
CApplication::signal_ignore(int aSignalNumber)
{
	system_log("ignore signal:%s", aSignalNumber);
}

void
CApplication::signal_exits(int aSignalNumber)
{
	CString
		Command;
	Command.clear();

	system_log("Signal down server by %d", aSignalNumber);
	Command.format("/usr/bin/gcore -c ascore.%d archspace %d", getpid(), getpid());
	system((char *)Command);
	exit(1);
}
