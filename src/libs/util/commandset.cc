#include "util.h"

TZone gCommandSetZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCommandSet),
	0,
	0,
	NULL,   
	"Zone CCommandSet"
};

int
	CCommandSet::mMasks[8] = 
	{
		0x0001,
		0x0002,
		0x0004,
		0x0008,
		0x0010,
		0x0020,
		0x0040,
		0x0080
	};

CCommandSet::CCommandSet()
{
	memset(mCommands, 0, sizeof(mCommands));
}

CCommandSet::CCommandSet(const CCommandSet& aCommandSet)
{
	memcpy(mCommands, aCommandSet.mCommands, sizeof(mCommands));
}

void
CCommandSet::clear()
{
	memset(mCommands, 0, sizeof(mCommands));
}

bool
CCommandSet::has(int aCommand)
{
	if (aCommand < 0 || aCommand >= 128*8) 
	{
		system_log("Out of range CCommandSet::has()");
		return false;
	}
	return bool((mCommands[location(aCommand)] & mask(aCommand)) != 0);
}

void
CCommandSet::disable_command(int aCommand)
{
	if (aCommand < 0 || aCommand >= 128*8)
	{
		system_log("Out of range CCommandSet::disable_command()");
		return ;
	}
	mCommands[location(aCommand)] &= ~mask(aCommand);
}

void 
CCommandSet::enable_command(int aCommand)
{
	if (aCommand < 0 || aCommand >= 128*8)
	{
		system_log("Out of range CCommandSet::disable_command()");
		return ;
	}
	mCommands[location(aCommand)] |= mask(aCommand);
}

void
CCommandSet::disable_command(const CCommandSet& aCommandSet)
{
	for(int i=0; i<128; i++)
		mCommands[i] &= ~mask(aCommandSet.mCommands[i]);
}

void
CCommandSet::enable_command(const CCommandSet& aCommandSet)
{
	for(int i=0; i<128; i++)
		mCommands[i] |=  mask(aCommandSet.mCommands[i]);
}

CCommandSet&
CCommandSet::operator &= (const CCommandSet& aCommandSet)
{
	for(int i=0; i<128; i++)
		mCommands[i] &= aCommandSet.mCommands[i];
	return *this;
}

CCommandSet&
CCommandSet::operator |= (const CCommandSet& aCommandSet)
{
	for(int i=0; i<128; i++)
		mCommands[i] |= aCommandSet.mCommands[i];
	return *this;
}

CCommandSet
operator&(const CCommandSet& aCommandSet1, 
		const CCommandSet& aCommandSet2)
{
	CCommandSet 
		Temp(aCommandSet1);
	
	Temp &= aCommandSet2;

	return Temp;
}

CCommandSet
operator|(const CCommandSet& aCommandSet1, 
		const CCommandSet& aCommandSet2)
{
	CCommandSet 
		Temp(aCommandSet1);
	
	Temp |= aCommandSet2;

	return Temp;
}

bool
CCommandSet::is_empty()
{
	for(int i=0; i<128; i++)
		if (mCommands[i] != 0)
			return false;
	return true;
}

int
operator==(const CCommandSet& aCommandSet1, 
		const CCommandSet& aCommandSet2)
{
	for(int i=0; i<128; i++)
		if (aCommandSet1.mCommands[i] != aCommandSet2.mCommands[i])
			return 0;
	return 1;
}

const char* 
CCommandSet::get_string(int aSize) const
{
	static char
		HexTable[17] = "0123456789ABCDEF";
	static char
		CommandString[128*2 + 1];
	
	CommandString[0] = 0;
	if (aSize < 0) 
		aSize = 128*8 - 1;

	aSize++;

	int 
		Size = (aSize/8) + ((aSize % 8) ? 1:0);

	for(int i=0; i<Size; i++)
	{
		CommandString[i*2] = HexTable[mCommands[i]/16];
		CommandString[i*2+1] = HexTable[mCommands[i]%16];
	}
	CommandString[Size*2] = 0; 
	return (const char*)CommandString;
}

bool
CCommandSet::set_string(const char* aString)
{
	static char
		HexTable[17] = "0123456789ABCDEF";

	int 
		Length = strlen(aString);
	if (Length % 2) return false;
	if (Length > 128*2) return false;

	memset(mCommands, 0, sizeof(mCommands));

	for(int i=0; i<Length/2; i++)
	{
		char * 
			High = strchr(HexTable, aString[i*2]);
		if (!High) 
			return false;

		char *
			Low = strchr(HexTable, aString[i*2+1]);
		if (!Low) 
			return false;

		mCommands[i] = (High-HexTable)*16+(Low-HexTable);
	}
	return true;
}
