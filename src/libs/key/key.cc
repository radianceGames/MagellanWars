#include "key.h"

TZone gKeyServerZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CKeyServer),
	0,
	0,
	NULL,   
	"Zone CKeyServer"
};
CKeyServer::CKeyServer()
{
}

CKeyServer::~CKeyServer()
{
}

int
CKeyServer::read()
{
	int 
		NRead;
	NRead = ::pth_read(get(), mBuffer, MAX_KEY_RECEIVE_BUFFER);

	return NRead;
}

int
CKeyServer::write()
{
	int
		NWrite;
	NWrite = ::pth_write(get(), "~", 1);

	return NWrite;
}

const char* 
CKeyServer::get_buffer()
{
	return (char*)mBuffer;
}
