#define BSD_COMP
#include "net.h"
#include <cerrno>
#include <cstdlib>
#include <sys/ioctl.h>

int
	CLoopServer::mLoopPerSecond = 500;

CLoopServer::CLoopServer(int aLoopPerSecond, int aMaxConnection):
		CServer(aMaxConnection)
{
	mLoopPerSecond = aLoopPerSecond;
	memset(&mLastTime, 0, sizeof(struct timeval));
	memset(&mRemainTime, 0, sizeof(struct timeval));
}

CLoopServer::~CLoopServer()
{
	remove_all();
}

struct timeval
timediff(struct timeval *aT1, struct timeval *aT2)
{
	struct timeval
		Result, 
		Temp;

	Temp = *aT1;
	Result.tv_usec = Temp.tv_usec - aT2->tv_usec;

	while(Result.tv_usec < 0)
	{
		Result.tv_usec += 1000000;
		--(Temp.tv_sec);
	}

	if ((Result.tv_sec = Temp.tv_sec - aT2->tv_sec) < 0)
	{
		Result.tv_usec = 0;
		Result.tv_sec = 0;
	}

	return Result;
}    

void
CLoopServer::get_remain_time()
{
	struct timeval
		TempTime,
		Now,
		SpentTime;
	long
		LoopTime;

	if (mLastTime.tv_sec == 0)
		gettimeofday(&mLastTime, (struct timezone*)0);

	LoopTime = (long)(1000000/mLoopPerSecond);
	TempTime.tv_sec = 0;
	TempTime.tv_usec = LoopTime;
	gettimeofday(&Now, (struct timezone *)0);
	SpentTime = timediff(&Now, &mLastTime);
	mRemainTime = timediff(&TempTime, &SpentTime);
/* too much log by Nara
	system_log("Last %d:%d, Now %d:%d, Spent %d:%d,"
				" Temp %d:%d, Remain %d:%d", 
			mLastTime.tv_sec, mLastTime.tv_usec,
			Now.tv_sec, Now.tv_usec,
			SpentTime.tv_sec, SpentTime.tv_usec,
			TempTime.tv_sec, TempTime.tv_usec,
			mRemainTime.tv_sec, mRemainTime.tv_usec); */

	mLastTime.tv_usec += LoopTime;
	while(mLastTime.tv_usec >= 1000000)
	{
		mLastTime.tv_usec -= 1000000;
		mLastTime.tv_sec++;
	}
}

void
CLoopServer::wait_cycle()
{
	if (::pth_select(0, (fd_set*)0, (fd_set*)0, (fd_set*)0,
			(struct timeval*)&mRemainTime) < 0)
	{
		system_log("Could not wait cycle %s\n%d %d", strerror(errno),
		mRemainTime.tv_sec, mRemainTime.tv_usec);
		exit(-1);
	}
// too much log - by Nara 
//	system_log("wait_cycle %d %d", 
//			mRemainTime.tv_sec, mRemainTime.tv_usec);
}   

void
CLoopServer::handle_exception()
{
	for(int i=length()-1; i>=0; i--)
	{
		CSocket
			*Socket = (CSocket*)CList::get(i);
		if (FD_ISSET(Socket->get(), &mExceptions))
		{
			system_log("handle_exception, %d", Socket->get());
			FD_CLR(Socket->get(), &mReads);
			FD_CLR(Socket->get(), &mWrites);
			remove(i);
			continue;
		}
	}
}

void
CLoopServer::handle_input()
{
	for(int i=length()-1; i>=0; i--)
	{
		CSocket
			*Socket = (CSocket*)CList::get(i);
		if (FD_ISSET(Socket->get(), &mReads))
		{
			int
				NRead;
			ioctl(Socket->get(), FIONREAD, &NRead);
			if (NRead == 0)
			{
//				system_log("%d.close by client %d", i, Socket->get());
				remove(i);
			} else {
//				system_log("handle input: %d[%d]", Socket->get(), NRead);
				if (Socket->read() < 0)
				{
					system_log("%d,read error %d", i, Socket->get());
					remove(i);
				}
			}
		}
	}
}

void
CLoopServer::handle_output()
{
		for(int i=length()-1; i>=0; i--)
		{
			CSocket
				*Socket = (CSocket*)CList::get(i);
			if (FD_ISSET(Socket->get(), &mWrites))
			{
				if (Socket->write() < 0)
				{
					remove(i);
					continue;
				}
			}
		}
}
