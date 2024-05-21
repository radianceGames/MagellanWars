#include "net.h"
#include <fcntl.h>
#include <cerrno>

CSocket::CSocket()
{
	mSocket = -1;
}

CSocket::~CSocket()
{
	mInput.remove_all();
	mOutput.remove_all();
	close();
}

void 
CSocket::close()
{
	if (mSocket >= 0)
	{
//		system_log("close socket %d", mSocket);
		::close(mSocket);
		mSocket = -1;
	}
}

void
CSocket::nonblock(int aSocket)
{
	if (aSocket > 0)
	{
		if (fcntl(aSocket, F_SETFL, O_NDELAY) == -1)
		{
			// error
		}
	} else {
		if (fcntl(mSocket, F_SETFL, O_NDELAY) == -1)
		{
			// error
		}
	}
}

int
CSocket::read_packet(CPacket *aPacket)
{
	int
		Done = 0,
		Begin = 0,
		Count = 0;
	unsigned short int
		Size;
	unsigned char
		Buffer[MAX_PACKET_SIZE];

	Begin = ::pth_read(mSocket, Buffer, sizeof(unsigned short int));
	if (Begin < (signed)sizeof(unsigned short int)) 
		return -1;

//	memcpy(&Size, Buffer, sizeof(unsigned short int));
	Size = ((int) Buffer[0]) + ((int) Buffer[1]) * 256;
	if (Size > MAX_PACKET_SIZE) 
		return -1;

	while(Begin < Size && Count++ < 20)
	{
		Done = ::pth_read(mSocket, Buffer+Begin, Size-Begin);
		if (Done < 0) 
		{
			if (errno != EAGAIN) return -1;
			Done = 0;
			pth_nap((pth_time_t){0, 1});
		}
		Begin += Done;
	}
	if (!aPacket->set(Buffer, Begin)) return -1;
	return Begin;
}

int
CSocket::write_packet(CPacket *aPacket)
{
	int
		Done;
//	system_log("write %d bytes", aPacket->count());
	time_t
		now = time(0);

	do
	{
		Done = ::pth_write(mSocket,
			((char*)aPacket->get())+aPacket->sent(),
		aPacket->count()-aPacket->sent());
		if (Done < 0)
		{
			if (errno == EAGAIN)
				return -2;
			else
				return -1;
		} else if( Done == 0 ){
			return -3;
		} else
			aPacket->sent(aPacket->sent() + Done);

		if( time(0) > now+2 ){
			system_log( "write_packet [%d/%d] %s", aPacket->sent(), aPacket->count(), ((TMessage*)aPacket->get())->data+MESSAGE_HEADER_SIZE );
			return -3;
		}
	} while(aPacket->sent() < aPacket->count());
	return aPacket->sent();
}
