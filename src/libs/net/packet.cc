#include "common.h"
#include "util.h"
#include "net.h"

TZone gPacketZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPacket), 
	0,
	0,
	NULL,
	"Zone CPacket"
};

CPacket::CPacket(): CNode()
{
	mSent = 0;
}

CPacket::CPacket(CPacket& aPacket): CNode()
{
	memcpy(&mPacket, &aPacket.mPacket, sizeof(TMessage));
	mSent = aPacket.mSent;
}

CPacket::~CPacket()
{
}


CPacket*
CPacket::clone()
{
	CPacket
		*Packet = new CPacket(*this);

	return Packet;
}

bool
CPacket::set(void *aData, int aSize)
{
	if (aSize > (signed)sizeof(TMessage)) return false;
	memcpy(&mPacket, aData, aSize);
	mSent = 0;

	mPacket.size = aSize;
	return true;
}

int
CPacket::sent(int aSent)
{
	if (aSent > 0 && aSent > mSent)
		mSent = aSent;

	return mSent;
}

