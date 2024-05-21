#include "common.h"
#include "util.h"

#ifdef MEMORY_DEBUG
#include <cstdlib>
#include <cstdio>
#define BYTE_128	128-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_256	256-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_1K		1024-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_2K		1024*2-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_4K		1024*4-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_8K		1024*8-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_16K	1024*16-sizeof(TChunk)-sizeof(TEnd)
#define BYTE_1M		1024*1024-sizeof(TChunk)-sizeof(TEnd)
#else
#define BYTE_128	128-sizeof(TChunk)
#define BYTE_256	256-sizeof(TChunk)
#define BYTE_1K		1024-sizeof(TChunk)
#define BYTE_2K		1024*2-sizeof(TChunk)
#define BYTE_4K		1024*4-sizeof(TChunk)
#define BYTE_8K		1024*8-sizeof(TChunk)
#define BYTE_16K	1024*16-sizeof(TChunk)
#define BYTE_1M		1024*1024-sizeof(TChunk)
#endif


extern TZone gGeneralZone;
/*
TZone gInnerList64Zone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	64-sizeof(TChunk)-sizeof(TEnd),
	0,
	0,
	NULL,   
	"Zone 64Bytes List"
};
*/
TZone gInnerList128Zone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_128,
	0,
	0,
	NULL,   
	"Zone 128Bytes List"
};

TZone gInnerList256Zone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_256,
	0,
	0,
	NULL,   
	"Zone 256Bytes List"
};

TZone gInnerList1KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_1K,
	0,
	0,
	NULL,   
	"Zone 1KBytes List"
};

TZone gInnerList2KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_2K,
	0,
	0,
	NULL,   
	"Zone 2KBytes List"
};

TZone gInnerList4KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_4K,
	0,
	0,
	NULL,   
	"Zone 4KBytes List"
};

TZone gInnerList8KZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	BYTE_8K,
	0,
	0,
	NULL,   
	"Zone 8KBytes List"
};

TZone gInnerList16KZone =
{
	PTH_MUTEX_INIT,	
	recycle_allocation,
	recycle_free,
	BYTE_16K,
	0,
	0,
	NULL,   
	"Zone 16KBytes List"
};

TZone gInnerList1MZone =
{
	PTH_MUTEX_INIT,	
	recycle_allocation,
	recycle_free,
	BYTE_1M,
	0,
	0,
	NULL,   
	"Zone 1MKBytes List"
};



static size_t 
get_inner_list_size(size_t aSize)
{
	if (aSize == 0) return 0;
//	else if (aSize <= 64) return 64;
	else if (aSize <= BYTE_128) return BYTE_128;
	else if (aSize <= BYTE_256) return BYTE_256;
	else if (aSize <= BYTE_1K) return BYTE_1K;
	else if (aSize <= BYTE_2K) return BYTE_2K;
	else if (aSize <= BYTE_4K) return BYTE_4K;
	else if (aSize <= BYTE_8K) return BYTE_8K;
	else if (aSize <= BYTE_16K) return BYTE_16K;
	else if (aSize <= BYTE_1M) return BYTE_1M;
	return aSize;
}

static TZone*
get_inner_list_zone(size_t aSize)
{
	if (aSize <= 0) return NULL;
//	if (aSize <= 64) return &gInnerList64Zone;
	if (aSize <= BYTE_128) return &gInnerList128Zone;
	if (aSize <= BYTE_256) return &gInnerList256Zone;
	if (aSize <= BYTE_1K) return &gInnerList1KZone;
	if (aSize <= BYTE_2K) return &gInnerList2KZone;
	if (aSize <= BYTE_4K) return &gInnerList4KZone;
	if (aSize <= BYTE_8K) return &gInnerList8KZone;
	if (aSize <= BYTE_16K) return &gInnerList16KZone;
	if (aSize <= BYTE_1M) return &gInnerList1MZone;
	return &gGeneralZone;
}

static void* 
allocation(size_t aSize)
{
	size_t Size = get_inner_list_size(aSize);
	TZone *Zone = get_inner_list_zone(Size);
	void *Ptr = (Zone->malloc)(Zone, Size);
//	system_log("list allocation %d req:%d  %x", Size, aSize, (int)Ptr);
	return Ptr;	
}

static void
recycle(void *aObject, size_t aSize)
{
	size_t Size = get_inner_list_size(aSize);
	TZone *Zone = get_inner_list_zone(Size);

//	system_log("list free %d req:%d  %x", Size, aSize, (int)aObject);

	(Zone->free)(aObject);
}

static void*
reallocation(void *aData, size_t aOriginalSize, size_t aNewSize)
{
	if (aOriginalSize == aNewSize) return aData;

	if (get_inner_list_size(aOriginalSize) == 
			get_inner_list_size(aNewSize)) return aData;

	void *Data = allocation(aNewSize);
	if (aData && aOriginalSize > 0)
	{	
		memcpy(Data, aData, aOriginalSize);
		TZone *Zone = get_inner_list_zone(aOriginalSize);
		TZone *NewZone = get_inner_list_zone(aNewSize);
		if( aOriginalSize > aNewSize )
			system_log( "Chunk OrigSize %d %s > NewSize %d %s", aOriginalSize, Zone->name, aNewSize, NewZone->name );

/*	system_log("list reallocation:%d(%x)->%d(%x)\n", 
			get_inner_list_size(aOriginalSize), 
			aData,
			get_inner_list_size(aNewSize),
			Data);*/
	
		(Zone->free)(aData);
	}
	return Data;
}


CList::CList(int aLimit, int aThreshold)
{
	mRoot = (TSomething*)allocation(aLimit*sizeof(TSomething));
	mSize = aLimit*sizeof(TSomething);
	my_memset(mRoot, 0, aLimit*sizeof(TSomething));
	mLimit = aLimit;
	mCount = 0;
	mThreshold = aThreshold;
}

CList::~CList()
{
	if (mRoot) 
	{
		recycle(mRoot, mLimit*sizeof(TSomething));
		mRoot = NULL;
	}
}

void 
CList::remove_all()
{
	int Index;
	for(Index = mCount-1; Index>=0; Index--)
		if (!free_item(mRoot[Index]))
			break;
	set_length(Index+1);
	while(Index >= 0)
		remove(Index--);
}

void 
CList::remove_without_free_all()
{
	for (int i=mCount-1 ; i>=0 ; i--)
	{
		remove_without_free(i);
	}
}

void 
CList::set_length(int aLength)
{
	if (aLength < 0) return;

	mCount = aLength;
	if (aLength > mLimit)
	{
		aLength = ((aLength + mThreshold - 1) / mThreshold) 
						* mThreshold;

		if (aLength)
		{
			mRoot = (TSomething*)reallocation(mRoot, 
						mLimit*sizeof(TSomething), aLength*sizeof(TSomething));

// Nara
			TChunk
				*Chunk = (TChunk*)((char*)mRoot-sizeof(TChunk));
			if( Chunk->zone == NULL ){
				system_log( "Chunk Border Violation - broken Chunk" );
				mSize = aLength*sizeof(TSomething);
			} else {
				mSize = Chunk->zone->object_size;
			}
			if( mSize < aLength*sizeof(TSomething) ){
				if( Chunk->zone )
					system_log( "Chunk too small size - %d/%d %s", mSize, aLength*sizeof(TSomething), Chunk->zone->name );
			}
// Nara end
		} else {
			if (mRoot) recycle(mRoot, mLimit*sizeof(TSomething));
			mRoot = NULL;
			mSize = 0;
		}

		int 
			Clear = aLength-mCount;
		if (Clear > 0) my_memset(mRoot+mCount, 0, Clear);
		mLimit = aLength;
	}
}

int
CList::find(TSomething aWhich)
{
	// 순차적으로 동일한 포인터가 있는지 검색한다.
	for(int Index=0; Index < length(); Index++)
		if (mRoot[Index] == aWhich){
#ifdef MEMORY_DEBUG
			check_trouble( mRoot[Index], "find" );
#endif
			return Index;
		}
	return -1;
}

bool 
CList::remove(int aIndex)
{
	if (aIndex < mCount)
	{
		// 아이템의 메모리를 해지한다.
#ifdef MEMORY_DEBUG
		check_trouble( mRoot[aIndex], "remove" );
#endif
		if (!free_item(mRoot[aIndex]))
			return false;

		// 리스트의 크기를 줄이고 아래쪽의 포인터를 한칸씩 끌어올린다.
		int 
			NCount = mCount - 1;
		my_memmove(&mRoot[aIndex], &mRoot[aIndex+1], 
				(NCount-aIndex)*sizeof(TSomething));
		// 새 리스트의 크기를 설정한다.
		set_length(NCount);
		return true;
	} else 
		return false;
}

bool 
CList::remove_without_free(int aIndex)
{
	if (aIndex < mCount)
	{
		int 
			NCount = mCount - 1;
		my_memmove(&mRoot[aIndex], &mRoot[aIndex+1], 
				(NCount-aIndex)*sizeof(TSomething));
		set_length(NCount);
		return true;
	} else 
		return false;
}


bool
CList::replace(int aIndex, TSomething aWhat)
{
#ifdef MEMORY_DEBUG
	check_trouble( aWhat, "replace" );
#endif
	if (aIndex < mCount)
	{
		// 현재 들어있는 아이템의 메모리를 해지하고 삭제한다.
		if (!free_item(mRoot[aIndex]))
			return false;
		// 주어진 아이템을 위치에 설정해 넣는다.
		mRoot[aIndex] = aWhat;
		return true;
	} else 
		return false;
}

bool 
CList::insert(int aIndex, TSomething aItem)
{
#ifdef MEMORY_DEBUG
	check_trouble( aItem, "insert" );
#endif
	if (aIndex <= mCount)
	{
		// 리스트의 총 길이를 늘린다.
		set_length(mCount+1);
		// 아이템 리스트를 한칸씩 밀어서 현재 아이템이 들어갈 공간을
		// 확보한다.
		my_memmove(&mRoot[aIndex+1], &mRoot[aIndex], 
				(mCount - aIndex -1)*sizeof(TSomething));
		// 아이템을 끼워넣는다.
		mRoot[aIndex] = aItem;
		return true;
	} else {
		return false;
	}
}

#ifdef MEMORY_DEBUG
void
CList::check_border( void *aPoint, char *msg ) const
{
	TChunk
		*Chunk = (TChunk*)( (char*)mRoot - sizeof(TChunk) );
	TEnd
		*End = NULL;

	if( Chunk->zone )
		End = (TEnd*)( (char*)mRoot + Chunk->zone->object_size );

	strcpy( Chunk->info, debug_info() );
	if( mRoot == NULL && aPoint ){
		system_log( "Chunk : operation on null root %s", debug_info() );
		return;
	}
	if( End && End->magic != 100 ){
		system_log( "Chunk : End Violated %s e%d", debug_info(), End->magic );
//		if( End->magic > 1000 )
//			mTrouble = (TSomething*)(End->magic);
	}

	if( Chunk->zone == NULL ){
		system_log( "Illegal Chunk : %s %d m%d %s", msg, Chunk, Chunk->magic, debug_info() );
	} else if( Chunk->magic != 100 ){
		system_log( "Chunk : wrong_magic %s %d %s %s", msg, Chunk->magic, Chunk->zone->name, debug_info() );
	} else if( (char*)aPoint < (char*)mRoot || ((char*)mRoot)+mSize < (char*)aPoint ){
		system_log( "Chunk Border Violation - %s %d/%d %d/%d %d %s %s", msg, aPoint, Chunk, mSize, Chunk->zone->object_size, Chunk->magic, Chunk->zone->name, debug_info() );
	}
}

void
CList::check_trouble( TSomething aCandi, char *msg ) const
{
/*
	if( mTrouble != NULL && aCandi == mTrouble ){
		TChunk
			*Chunk = (TChunk*)( (char*)aCandi - sizeof(TChunk) );
		TEnd
			*End = NULL;

		if( Chunk->zone )
			End = (TEnd*)( (char*)mRoot + Chunk->zone->object_size );

		if( End )
			system_log( "Chunk %s Null Zone/End %s", msg, Chunk->info );
		else
			system_log( "Chunk %s : %s : %s : m%d : e%d", msg, Chunk->info, Chunk->zone->name, Chunk->magic, End->magic );
	}
*/
}
#endif

void
CList::my_memmove( void *dst, void *src, size_t len )
{
#ifdef MEMORY_DEBUG
	check_border( src, "mmove1" );
	check_border( (char*)src+len, "mmove2" );
	check_border( dst, "mmove3" );
	check_border( (char*)dst+len, "mmove4" );
#endif
	memmove( dst, src, len );
}

void
CList::my_memset( void *b, int c, size_t len )
{
#ifdef MEMORY_DEBUG
	char buf[100];

	check_border( b, "mset1" );
	sprintf( buf, "mset2-%d", len );
	check_border( (char*)b+len-1, buf );
#endif
	memset( b, c, len );
}
