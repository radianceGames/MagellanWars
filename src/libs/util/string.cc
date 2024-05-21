#include "common.h"
#include "util.h"
#include <cstdarg>
#include <cstdio>

#define __DEBUG__

#ifdef MEMORY_DEBUG
#define BYTE_128			128-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_256			256-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_1K 			1024-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_2K 			1024*2-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_4K 			1024*4-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_8K				1024*8-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_16K 			1024*16-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_32K			1024*32-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_64K			1024*64-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_128K			1024*128-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_256K			1024*256-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_512K 			1024*512-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_2M 			1024*1024*2-sizeof(TChunk)-sizeof(TEnd)
#else
#define BYTE_64				64-sizeof(TChunk)
#define BYTE_128			128-sizeof(TChunk)
#define BYTE_256			256-sizeof(TChunk)
#define BYTE_1K 			1024-sizeof(TChunk)
#define BYTE_2K 			1024*2-sizeof(TChunk)
#define BYTE_4K 			1024*4-sizeof(TChunk)
#define BYTE_8K				1024*8-sizeof(TChunk)
#define BYTE_16K 			1024*16-sizeof(TChunk)
#define BYTE_32K			1024*32-sizeof(TChunk)
#define BYTE_64K			1024*64-sizeof(TChunk)
#define BYTE_128K			1024*128-sizeof(TChunk)
#define BYTE_256K			1024*256-sizeof(TChunk)
#define BYTE_512K 			1024*512-sizeof(TChunk)
#define BYTE_2M 			1024*1024*2-sizeof(TChunk)
#endif

extern TZone gGeneralZone;

TZone gStringZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CString),
	0,
	0,
	NULL,
	"Zone CString",
};

#if !defined(MEMORY_DEBUG)
TZone gInnerString64Zone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_64,
	0,
	0,
	NULL,
	"Zone 64Bytes String",
};
#endif

TZone gInnerString128Zone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_128,
	0,
	0,
	NULL,
	"Zone 128Bytes String",
};

TZone gInnerString256Zone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_256,
	0,
	0,
	NULL,
	"Zone 256Bytes String",
};

TZone gInnerString1KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_1K,
	0,
	0,
	NULL,
	"Zone 1024Bytes String",
};

TZone gInnerString2KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_2K,
	0,
	0,
	NULL,
	"Zone 2KBytes String",
};

TZone gInnerString4KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_4K,
	0,
	0,
	NULL,
	"Zone 4KBytes String",
};

TZone gInnerString8KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_8K,
	0,
	0,
	NULL,
	"Zone 8KBytes String",
};

TZone gInnerString16KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_16K,
	0,
	0,
	NULL,
	"Zone 16KBytes String",
};

TZone gInnerString32KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_32K,
	0,
	0,
	NULL,
	"Zone 32KBytes String",
};

TZone gInnerString64KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_64K,
	0,
	0,
	NULL,
	"Zone 64KBytes String",
};

TZone gInnerString128KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_128K,
	0,
	0,
	NULL,
	"Zone 128KBytes String",
};

TZone gInnerString256KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_256K,
	0,
	0,
	NULL,
	"Zone 256KBytes String",
};

TZone gInnerString512KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_512K,
	0,
	0,
	NULL,
	"Zone 512KBytes CString",
};

TZone gInnerString2MZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_2M,
	0,
	0,
	NULL,
	"Zone 2MBytes CString",
};

static TZone*
get_inner_string_zone(size_t aMaxSize)
{
#if !defined(MEMORY_DEBUG)
	if (aMaxSize == BYTE_64) return &gInnerString64Zone;	else
#endif
	if (aMaxSize == BYTE_128) return &gInnerString128Zone;
	else if (aMaxSize == BYTE_256) return &gInnerString256Zone;
	else if (aMaxSize == BYTE_1K) return &gInnerString1KZone;
	else if (aMaxSize == BYTE_2K) return &gInnerString2KZone;
	else if (aMaxSize == BYTE_4K) return &gInnerString4KZone;
	else if (aMaxSize == BYTE_8K) return &gInnerString8KZone;
	else if (aMaxSize == BYTE_16K) return &gInnerString16KZone;
	else if (aMaxSize == BYTE_32K) return &gInnerString32KZone;
	else if (aMaxSize == BYTE_64K) return &gInnerString64KZone;
	else if (aMaxSize == BYTE_128K) return &gInnerString128KZone;
	else if (aMaxSize == BYTE_256K) return &gInnerString256KZone;
	else if (aMaxSize == BYTE_512K) return &gInnerString512KZone;
	else if (aMaxSize == BYTE_2M) return &gInnerString2MZone;
	else {
		system_log("Chunk : Could not find matching zone (%d)", aMaxSize);
		return &gGeneralZone;
	}
}

static size_t
get_inner_string_size(size_t aMaxSize)
{
	if (aMaxSize == 0) return 0;
#if !defined(MEMORY_DEBUG)
	else if (aMaxSize <= BYTE_64) return BYTE_64;
#endif
	else if (aMaxSize <= BYTE_128) return BYTE_128;
	else if (aMaxSize <= BYTE_256) return BYTE_256;
	else if (aMaxSize <= BYTE_1K) return BYTE_1K;
	else if (aMaxSize <= BYTE_2K) return BYTE_2K;
	else if (aMaxSize <= BYTE_4K) return BYTE_4K;
	else if (aMaxSize <= BYTE_8K) return BYTE_8K;
	else if (aMaxSize <= BYTE_16K) return BYTE_16K;
	else if (aMaxSize <= BYTE_32K) return BYTE_32K;
	else if (aMaxSize <= BYTE_64K) return BYTE_64K;
	else if (aMaxSize <= BYTE_128K) return BYTE_128K;
	else if (aMaxSize <= BYTE_256K) return BYTE_256K;
	else if (aMaxSize <= BYTE_512K) return BYTE_512K;
	else if (aMaxSize <= BYTE_2M) return BYTE_2M;

	system_log("Chunk : Could not find matching size (%d)", aMaxSize);

	return aMaxSize;
}

unsigned int CString::mClassCount = 0;
unsigned int CString::mMemoryUsage = 0;

void
CString::print_debug()
{
	system_log("CString debug    CLASS:%d, MEMORY:%d", mClassCount, mMemoryUsage);
}

CString::CString()
{
	mData = NULL;
	mSize = 0;
	mMaxSize = 0;
	mClassCount++;
}

CString::CString(size_t aLength)
{
	mData = NULL;
	mSize = 0;
	mMaxSize = 0;
	set_size(aLength);
	mClassCount++;
}

CString::CString(const CString& aCopy)
{
	mData = NULL;
	mSize = 0;
	mMaxSize = 0;
	if (aCopy.length())
		append(aCopy);
	mClassCount++;
}

CString::CString(const char* aCopy)
{
	mData = NULL;
	mSize = 0;
	mMaxSize = 0;
	append(aCopy);
	mClassCount++;
}

CString::~CString()
{
	mClassCount--;
	free();
}

void
CString::set_size(size_t aNewSize)
{
#ifdef MEMORY_DEBUG
	check_border("set_size() beginn");
#endif

	if (aNewSize == 0)
	{
		mMemoryUsage -= mMaxSize;
		if (mData)
		{
			TZone *Zone = get_inner_string_zone(mMaxSize);
			(Zone->free)(mData);
		}
		mData = NULL;
		mMaxSize = 0;
		mSize = 0;
		return;
	}

	aNewSize++;

	if (aNewSize <= mMaxSize)
	{
		mSize = aNewSize-1;
		mData[mSize] = 0;
		return;
	}

	if (mData == NULL)
	{
		aNewSize = get_inner_string_size(aNewSize);
		TZone *Zone = get_inner_string_zone(aNewSize);

		if (aNewSize > BYTE_2M) system_log("Chunk : max size %d -> %d (NewString)", mMaxSize, aNewSize);

		mData = (char*)(Zone->malloc)(Zone, aNewSize);

		assert(mData);
		mSize = 0;
		mData[0] = 0;
		mMaxSize = aNewSize;
		mMemoryUsage += mMaxSize;
#ifdef MEMORY_DEBUG
		check_border("set_size() case -1");
#endif
	} else {
		mMemoryUsage -= mMaxSize;

		aNewSize = get_inner_string_size(aNewSize);
		TZone *Zone = get_inner_string_zone(aNewSize);

#ifdef MEMORY_DEBUG
		check_border("set_size() case 0");
		if (aNewSize > BYTE_2M) system_log("Chunk : max size %d -> %d (%s)", mMaxSize, aNewSize, mData);
#endif

		char *Data = (char*)(Zone->malloc)(Zone, aNewSize);
		assert(Data);


		if (mSize < aNewSize)
		{
#ifdef MEMORY_DEBUG
			check_border("set_size() case 1-1");
#endif
			memcpy(Data, mData, mSize+1);
#ifdef MEMORY_DEBUG
			check_border("set_size() case 1-2");
#endif
		} else {
#ifdef MEMORY_DEBUG
			check_border("set_size() case 2-1");
#endif
			memcpy(Data, mData, aNewSize);
#ifdef MEMORY_DEBUG
			check_border("set_size() case 2-2");
#endif
		}

		Zone = get_inner_string_zone(mMaxSize);
		(Zone->free)(mData);

		mData = Data;

		mMaxSize = aNewSize;
		if (mSize >= aNewSize) mSize = aNewSize-1;
		mData[mSize] = 0;

#ifdef MEMORY_DEBUG
		check_border("set_size() case 3");
#endif
		mMemoryUsage += mMaxSize;
	}
}

void
CString::free()
{
	if (mData)
	{
#ifdef MEMORY_DEBUG
		check_border("free()");
#endif
		TZone *Zone = get_inner_string_zone(mMaxSize);
		(Zone->free)(mData);
		mMemoryUsage -= mMaxSize;
		mData = NULL;
		mSize = mMaxSize = 0;
	}
}

CString&
CString::insert(size_t aIndex, const CString& aString)
{
	if (mData == NULL)
	{
		append(aString);
		return *this;
	}

	size_t
		StringLength = aString.length();
	size_t
		NewSize = StringLength + length();

	if (NewSize >= mMaxSize)
		set_size(NewSize);

	memcpy(mData+aIndex+StringLength, mData+aIndex, mSize-aIndex);
	memcpy(mData+aIndex, aString.get_data(), StringLength);
	mData[mSize=NewSize] = 0;

	return *this;
}

CString&
CString::overwrite(size_t aIndex, const CString &aString)
{
	if (mData == NULL)
	{
		append(aString);
		return *this;
	}

	size_t
		StringLength = aString.length();
	size_t
		NewSize = aIndex+StringLength;

	if (NewSize >= mMaxSize)
		set_size(NewSize);

	memcpy(mData+aIndex, aString.get_data(), StringLength);
	mData[mSize=NewSize] = 0;

	return *this;
}

CString&
CString::append(const CString& aString, size_t aCount)
{
	if (!aString.get_data()) return *this;

	if (aCount == (size_t)-1)
		aCount = aString.length();

	if (!aCount)
		return *this;

	if( aCount > 1024*1024 ){
		system_log( "Chunk Wrong append-s %d/%d : %d : %s", aCount, aString.length(), aString.get_data(), aString.get_data() );
		system_log( "Chunk Wrong append %d : %s", mSize, (mData)?mData:"(null)" );
#ifdef MEMORY_DEBUG
		if( aString.get_data() ){
			TChunk
				*Chunk = (TChunk*)(aString.get_data()-sizeof(TChunk));
			system_log( "Chunk Wrong : %d : %s", Chunk->magic, Chunk->info );
		}
#endif
		return *this;
	}

	size_t
		NewSize = mSize + aCount;

/*	if (mMaxSize > BYTE_2M)
	{
		system_log("debug core dump");
		sprintf(buf, "/usr/bin/gcore -c debugcore.%d archspace %d", getpid(), getpid());
		system(buf);
		exit(1);
	}*/

#ifdef MEMORY_DEBUG
	char
		buf[100];
	sprintf( buf, "append-s case 0 - %d/%d/%d/%d", mSize, mMaxSize, aCount, aString.length() );
	check_border( buf );
#endif

	if (NewSize >= mMaxSize)
		set_size(NewSize);

#ifdef MEMORY_DEBUG
	sprintf( buf, "append-s case 1 - %d/%d/%d/%d", mSize, mMaxSize, aCount, aString.length() );
	check_border( buf );
#endif

#ifdef MEMORY_DEBUG
	sprintf( buf, "append-s case 2 - %d/%d/%d/%d", mSize, mMaxSize, aCount, aString.length() );
	check_border( buf );
#endif

	memcpy(mData + mSize, aString.get_data(), aCount);

#ifdef MEMORY_DEBUG
	sprintf( buf, "append-s case 3 - %d/%d/%d/%d", mSize, mMaxSize, aCount, aString.length() );
	check_border( buf );
#endif

	mData[mSize=NewSize] = 0;

#ifdef MEMORY_DEBUG
	sprintf( buf, "append-s case 4 - %d/%d/%d/%d", mSize, mMaxSize, aCount, aString.length() );
	check_border( buf );
#endif

	return *this;
}

CString&
CString::append(const char *aString, size_t aCount)
{
	if (!aString) return *this;

	if (aCount == (size_t)-1)
		aCount = strlen(aString);

	if (!aCount)
		return *this;

	if( aCount > 1024*1024 ){
		system_log( "Chunk Wrong append-c %d : %s", aCount, aString );
		return *this;
	}

	size_t
		NewSize = mSize + aCount;

/*	if (mMaxSize > BYTE_2M)
	{
		system_log("debug core dump");
		sprintf(buf, "/usr/bin/gcore -c debugcore.%d archspace %d", getpid(), getpid());
		system(buf);
		exit(1);
	}*/

#ifdef MEMORY_DEBUG
	char
		buf[100];
	sprintf( buf, "append-c case 0 - %d/%d %d/%d", mSize, mMaxSize, aCount, strlen(aString) );
	check_border( buf );
#endif

	if (NewSize >= mMaxSize)
		set_size(NewSize);

#ifdef MEMORY_DEBUG
	sprintf( buf, "append-c case 1 - %d/%d/%d/%d", mSize, mMaxSize, aCount, strlen(aString) );
	check_border( buf );
#endif

	memcpy(mData+mSize, aString, aCount);

#ifdef MEMORY_DEBUG
	sprintf( buf, "append-c case 2 - %d/%d/%d/%d", mSize, mMaxSize, aCount, strlen(aString) );
	check_border( buf );
#endif

	mData[mSize=NewSize] = 0;

#ifdef MEMORY_DEBUG
	sprintf( buf, "append-c case 3 - %d/%d/%d/%d", mSize, mMaxSize, aCount, strlen(aString) );
	check_border( buf );
#endif

	return *this;
}

CString::operator char*() const
{
	return get_data();
}

const CString&
CString::operator=(const char *aString)
{
	clear();
	if (!aString) return *this;
	append(aString);
	return *this;
}

const CString&
CString::operator=(const CString &aString)
{
	clear();
	if (!aString.get_data()) return *this;
	append((char*)aString);
	return *this;
}

const CString&
CString::operator+=(const char *aString)
{
	if (!aString) return *this;
	append(aString);
	return *this;
}

const CString&
CString::operator+=(const CString &aString)
{
	if (!aString.get_data()) return *this;
	append((char*)aString);
	return *this;
}

CString
operator+(const CString &aString1, const char *aString2)
{

	CString
		Ret;
	try
	{
	Ret.append((char*)aString1);
	Ret.append(aString2);
	}
	catch(...)
	{
	}

	return Ret;
}

CString
operator+(const char *aString1, const CString& aString2)
{
	CString
		Ret;
	Ret.append(aString1);
	Ret.append((char*)aString2);

	return Ret;
}

CString
operator+(const CString& aString1, const CString& aString2)
{
	CString
		Ret;
	Ret.append((char*)aString1);
	Ret.append((char*)aString2);

	return Ret;
}

CString&
CString::format(const char *aFormat, ...)
{
	static char Buffer[1024*1024];

	va_list Args;

	va_start(Args, aFormat);
	vsnprintf(Buffer, 1024*1024-1, aFormat, Args);
	va_end(Args);
	append(Buffer);

	return *this;
}

void
CString::resize()
{
	mSize = strlen(mData);
	if (mSize >= mMaxSize) mSize = mMaxSize-1;
}

void
CString::add_slashes()
{
	if (!length()) return;

	static char Buffer[1024*512];

	int
		Index = 0;

	for(int i=0; i<(signed)length(); i++)
	{
		if (mData[i] == '\"')
		{
			Buffer[Index++] = '\\';
			Buffer[Index++] = mData[i];
		} else if (mData[i] == '\'')
		{
			Buffer[Index++] = '\\';
			Buffer[Index++] = mData[i];
		} else if (mData[i] == '\\') {
			Buffer[Index++] = '\\';
			Buffer[Index++] = mData[i];
		} else
			Buffer[Index++] = mData[i];
	}
	Buffer[Index] = 0;

	clear();
	append(Buffer);
}

void
CString::mark_forward_slashes()
{
	if (!length()) return;

	static char Buffer[1024*512];

	int
		Index = 0;

	for(int i=0; i<(signed)length(); i++)
	{
		if (mData[i] == '/')
		{
			Buffer[Index++] = '\\';
			Buffer[Index++] = mData[i];
		}
		else
			Buffer[Index++] = mData[i];
	}
	Buffer[Index] = 0;

	clear();
	append(Buffer);
}

void
CString::strip_slashes()
{
	if (!length()) return;

	static char Buffer[1024*512];

	int
		Index = 0;

	for(int i=0; i<(signed)length(); i++)
	{
		if (mData[i] == '\\')
		{
			if (mData[i+1] == '\\')
			{
				Buffer[Index++] = '\\';
				i++;
			}
		} else
			Buffer[Index++] = mData[i];
	}
	Buffer[Index] = 0;

	clear();
	append(Buffer);
}

void
CString::strip_all_slashes()
{
	if (!length()) return;

	static char Buffer[1024*512];

	int
		Index = 0;

	for(int i=0; i<(signed)length(); i++)
	{
		if (mData[i] == '/')
		{
			if (mData[i+1] == '/')
			{
				Buffer[Index++] = ' ';
				i++;
			}
		}
		else if (mData[i] == '\\')
		{
			if (mData[i+1] == '\\')
			{
				Buffer[Index++] = ' ';
				i++;
			}
		}
        else
			Buffer[Index++] = mData[i];
	}
	Buffer[Index] = 0;

	clear();
	append(Buffer);
}

void
CString::asciiunprintablechars()
{
	if (!length()) return;
	static char Buffer[1024*512];
	int Index = 0;

	for(int i=0; i<(signed)length(); i++)
	{
		if (!(mData[i] == '\r')){
			Buffer[Index++] = mData[i];
		}

	}
	while(Buffer[Index-1] == ' ')
	{
		Buffer[Index-1] = 0;
		Index--;
	}
	Buffer[Index] = 0;

	clear();
	append(Buffer);

}

void
CString::htmlspecialchars()
{
	if (!length()) return;
	asciiunprintablechars();

	static char Buffer[1024*512];

	int
		Count = 0;

	for(int i=0; i<(signed)length(); i++)
		if (strchr("<>&\"", mData[i])) Count++;

	int
		Index = 0;

	for(int i=0; i<(signed)length(); i++)
	{
		switch(mData[i])
		{
			case '&':
				Buffer[Index++] = '&';
				Buffer[Index++] = 'a';
				Buffer[Index++] = 'm';
				Buffer[Index++] = 'p';
				Buffer[Index++] = ';';
				break;
			case '<':
				Buffer[Index++] = '&';
				Buffer[Index++] = 'l';
				Buffer[Index++] = 't';
				Buffer[Index++] = ';';
				break;
			case '>':
				Buffer[Index++] = '&';
				Buffer[Index++] = 'g';
				Buffer[Index++] = 't';
				Buffer[Index++] = ';';
				break;
			case '\"':
				Buffer[Index++] = '&';
				Buffer[Index++] = 'q';
				Buffer[Index++] = 'u';
				Buffer[Index++] = 'o';
				Buffer[Index++] = 't';
				Buffer[Index++] = ';';
				break;
			default:
				Buffer[Index++] = mData[i];
		}
	}

	Buffer[Index] = 0;

	clear();
	append(Buffer);
}

void
CString::htmlentities()
{
	if (!length()) return;

	static char Buffer[1024*512];

	int
		Index = 0;
	int
		i = 0;

	while(i<(signed)length())
	{
		if (mData[i] == '&')
		{
			if (!strncasecmp(&mData[i], "&amp;", 5))
			{
				Buffer[Index++] = '&';
				i += 5;
			} else if (!strncasecmp(&mData[i], "&lt;", 4))
			{
				Buffer[Index++] = '<';
				i += 4;
			} else if (!strncasecmp(&mData[i], "&gt;", 4))
			{
				Buffer[Index++] = '>';
				i += 4;
			} else if (!strncasecmp(&mData[i], "&quot;", 6))
			{
				Buffer[Index++] = '\"';
				i += 6;
			} else Buffer[Index++] = mData[i++];
		} else Buffer[Index++] = mData[i++];
	}

	Buffer[Index] = 0;

	clear();
	append(Buffer);
}

void
CString::nl2br()
{
	if (!length()) return;

	static char Buffer[1024*512];

	int
		Count = 0;

	for(int i=0; i<(signed)length(); i++)
		if (mData[i] == '\n') Count++;

	int
		Index = 0;

	for(int i=0; i<(signed)length(); i++)
	{
		if (mData[i] == '\n')
		{
			Buffer[Index++] = '<';
			Buffer[Index++] = 'B';
			Buffer[Index++] = 'R';
			Buffer[Index++] = '>';
		} else
			Buffer[Index++] = mData[i];
	}

	Buffer[Index] = 0;

	clear();
	append(Buffer);
}

CString& add_slashes(CString &aString)
{
	return add_slashes((char*)aString);
}

CString& strip_slashes(CString &aString)
{
	return strip_slashes((char*)aString);
}



CString& htmlspecialchars(CString& aString)
{
	aString.htmlspecialchars();
	return aString;
}

CString& htmlentities(CString& aString)
{
	aString.htmlentities();
	return aString;
}

CString& nl2br(CString& aString)
{
	aString.nl2br();
	return aString;
}

CString& add_slashes(const char *aString)
{
	static CString
		Buffer;
	Buffer.clear();
	Buffer = aString;
	Buffer.add_slashes();
	return Buffer;
}

CString& mark_forward_slashes(const char *aString)
{
	static CString
		Buffer;
	Buffer.clear();
	Buffer = aString;
	Buffer.mark_forward_slashes();
	return Buffer;
}

CString& strip_slashes(const char* aString)
{
	static CString
		Buffer;
	Buffer.clear();
	Buffer = aString;
	Buffer.strip_slashes();
	return Buffer;
}

CString& htmlspecialchars(const char* aString)
{
	static CString
		Buffer;
	Buffer.clear();
	Buffer = aString;
	Buffer.htmlspecialchars();
	return Buffer;
}

CString& htmlentities(const char* aString)
{
	static CString
		Buffer;
	Buffer.clear();
	Buffer = aString;
	Buffer.htmlentities();
	return Buffer;
}

CString& nl2br(const char* aString)
{
	static CString
		Buffer;
	Buffer.clear();
	Buffer = aString;
	Buffer.nl2br();
	return Buffer;
}

CString& urlencode(const char* aString)
{
	static CString
		Buffer;
	Buffer.clear();
	if (!aString) return Buffer;
	Buffer = aString;
	Buffer.urlencode();
	return Buffer;
}



CString&
format(const char *aFormat, ...)
{
	static char Buffer[1024*1024];
	static CString Ret;

	va_list Args;

	va_start(Args, aFormat);
	vsnprintf(Buffer, 1024*1024-1, aFormat, Args);
	va_end(Args);

	Ret = Buffer;

	return Ret;
}

void
CString::unescape()
{
	if (!mData) return;

	int
		i, j;

	char
		*Str = mData;

	for(i=0, j=0; Str[j]; i++, j++)
	{
		if ((Str[i] = Str[j]) == '%')
		{
			Str[i] = hex2dec(Str+j+1);
			j += 2;
		} else if (Str[j] == '+')
		{
			Str[i] = ' ';
		}
	}
	Str[i] = 0;
	resize();
}

static int my_isalnum(char c)
{
	if (c >= '0' && c <= '9') return 1;
	if (c >= 'A' && c <= 'Z') return 1;
	if (c >= 'a' && c <= 'z') return 1;

	return 0;
}

static char* char_to_hex(unsigned char c)
{
	static char hex[3];
	int t;

	t = c/16;
	if (t<10) hex[0] = '0'+t;
	else hex[0] = 'A'+t-10;

	t = c%16;
	if (t<10) hex[1] = '0'+t;
	else hex[1] = 'A'+t-10;

	return hex;
}

void
CString::urlencode()
{
	char Buffer[1024*512];

	if (!mData) return;

	char *Src = mData;
	char *Dest = Buffer;
	char *Hex;

	while(*Src)
	{
		if (my_isalnum(*Src) || *Src == '-' || *Src == '_' || *Src == '.')
			*Dest++ = *Src;
		else if (*Src == ' ') *Dest++ = '+';
		else {
			Hex = char_to_hex((unsigned char)*Src);
			*Dest++ = '%';
			*Dest++ = Hex[0];
			*Dest++ = Hex[1];
		}
		Src++;
	}
	*Dest = 0;

	clear();
	append(Buffer);
}

#ifdef MEMORY_DEBUG
bool
CString::check_border(char *msg) const
{
	if (!mData) return true;

	TChunk
		*Chunk = (TChunk*)((char*)mData - sizeof(TChunk));

	TEnd
		*End = (TEnd*)((char*)mData + mMaxSize);


	if (Chunk->magic != 100)
	{
		system_log("Chunk String: Chunk Violated %s %d %s", msg, Chunk->magic, mData);
		return false;
	}

	if (Chunk->zone->object_size != mMaxSize)
	{
		system_log("Chunk String: Zone size violated %s %d != %d %s %s", msg,
					mMaxSize, Chunk->zone->object_size, Chunk->zone->name, mData);
		return false;
	}

	if (End && End->magic != 100)
	{
		system_log("Chunk String : End Violated %s e%d", msg, End->magic);
		return false;
	}

	return true;
}
#endif
