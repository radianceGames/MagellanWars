#include "net.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

CFIFO::CFIFO():CSocket()
{
}

CFIFO::~CFIFO()
{
}

int
CFIFO::open(const char *aFIFOName, int aMode)
{
	int 
		Res = -1;
	
	if (aMode == FIFO_RECEIVE)
	{
		unlink(aFIFOName);
	}
	
	if (access(aFIFOName, F_OK) != 0) 
	{
		Res = mkfifo(aFIFOName, 0777);
		if (Res != 0) 
			return -1;
	} 

	switch(aMode)
	{
		case FIFO_SEND:
			system_log("Open write FIFO(%s) O_WRONLY", aFIFOName);
			Res = ::open(aFIFOName, O_WRONLY);
			system_log("db.fifo Result: %d", Res);
			nonblock(Res);
			break;
		case FIFO_RECEIVE:
			system_log("Open read FIFO(%s) O_RDONLY", aFIFOName);
			Res = ::open(aFIFOName, O_RDONLY);
			break;
		default:
			system_log("Could not understand open FIFO mode");
			return -1;
	};

	if (Res > 0) set(Res);
	
	return Res;
}
