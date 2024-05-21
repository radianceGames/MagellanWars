#include "common.h"
#include "util.h"
#include <cstdlib>
#include <cstdio>

TZone gGeneralZone = 
{
	PTH_MUTEX_INIT,	
	memory_allocation,
	memory_free,
	1,
	0,
	0,
	NULL,
	"GENERAL"
};

#ifdef MEMORY_DEBUG
THeaven heaven[2000000];

int chunk_id;
#endif


void *
memory_allocation(TZone *aZone, size_t aSize)
{
	system_log("Memory Allocation");
	(void)aZone;
	return ::malloc(aSize);
}

void 
memory_free(void *aObject)
{
	system_log("Memory Free");
	::free(aObject);
}

void *
recycle_allocation(TZone *aZone, size_t aSize)
{
	(void)aSize;

	TChunk *Chunk = NULL;

	if (aZone->object_size != aSize)
	{
		system_log("%s is requested unmached size", aZone->name);
		//exit(-2);
	}

	pth_mutex_acquire(&aZone->mutex, FALSE, NULL);

	if (aZone->recycle)
	{
		Chunk = (TChunk*)aZone->recycle;
		aZone->recycle = Chunk->next;
		Chunk->next = NULL;
		Chunk->zone = aZone;

#ifdef MEMORY_DEBUG
		TEnd *End;
		End = (TEnd*)((char*)Chunk+aSize+sizeof(TChunk));
		End->magic = 100;
		End->zone = aZone; 

		if( Chunk->magic != 666 ){
			system_log( "Chunk magic failure %d", Chunk->magic );
		}

		Chunk->magic = 100;
		strcpy( Chunk->info, "recycle" );
#endif

		aZone->object_for_recycle--;

		pth_mutex_release(&aZone->mutex);

		return ((char*)Chunk)+sizeof(TChunk);
	} else {
		int
#ifdef MEMORY_DEBUG
			Size = aSize+sizeof(TChunk)+sizeof(TEnd);
#else
			Size = aSize+sizeof(TChunk);
#endif
		for (int i=0; !Chunk && i < 3; i++)
		{
		  Chunk = (TChunk*)malloc(Size);
  		}	   
  		
        if (!Chunk)
  		{
  		   system_log("Could not allocate %d for zone %s of size %d in recycle_allocationed",Size,aZone->name, aSize);
		   return NULL;
	    }	   
  		
		Chunk->next = NULL;
		Chunk->zone = aZone;

#ifdef MEMORY_DEBUG
		Chunk->magic = 100;
		strcpy( Chunk->info, "new" );

		TEnd *End;
		End = (TEnd*)((char*)Chunk+aSize+sizeof(TChunk));
		End->magic = 100;
		End->heaven = chunk_id;
		End->zone = aZone;

		heaven[chunk_id].chunk = Chunk;
		heaven[chunk_id].zone = aZone;
		heaven[chunk_id].end = End;

		chunk_id++;
#endif

		aZone->object_total++;

		pth_mutex_release(&aZone->mutex);

		return ((char*)Chunk)+sizeof(TChunk);
	}
}

void
recycle_free(void *aObject)
{
	TChunk *Chunk;

	Chunk = ((TChunk*)(((char*)aObject)-sizeof(TChunk)));

#ifdef MEMORY_DEBUG
	TEnd *End = NULL;
	if( Chunk->zone )
		End = (TEnd*)((char*)Chunk+Chunk->zone->object_size+sizeof(TChunk));

	if( Chunk->magic != 100 ){
		system_log( "Chunk illegal magic! %d m%d", Chunk, Chunk->magic );
		for( int i = 0; i < chunk_id; i++ ){
			if( heaven[i].chunk == Chunk ){
				system_log( "Destroyed Chunk : %s %d %d m%d : %s %d : e%d", Chunk->info, Chunk, i, Chunk->magic, heaven[i].zone->name, heaven[i].zone->recycle, heaven[i].end->magic );
				int
					min_diff = 99999999,
					candi = 0;
				for( int j = 0; j < chunk_id; j++ ){
					int diff = (int)(Chunk) - (int)(heaven[j].chunk);
					if( diff > 0 && diff < min_diff ){
						candi = j;
						min_diff = diff;
					}
				}
				system_log( "Candidate Chunk : %s, %d %d %d m%d %s : e%d", heaven[candi].chunk->info, min_diff, heaven[candi].chunk, candi, heaven[candi].chunk->magic, heaven[candi].zone->name, heaven[i].end->magic );
				return;
			}
		}
		system_log( "Chunk is not found." );
		return;
	}
	if( End && End->magic != 100 ){
		system_log( "Chunk End Violated : %s m%d e%d", Chunk->info, Chunk->magic, End->magic );
	}

	Chunk->magic = 666;
#endif

	pth_mutex_acquire(&Chunk->zone->mutex, FALSE, NULL);

	Chunk->zone->object_for_recycle++;

	Chunk->next = (TChunk*)Chunk->zone->recycle;
	Chunk->zone->recycle = Chunk;

	pth_mutex_release(&Chunk->zone->mutex);
}

void
zone_info(TZone *aZone)
{
	system_log("%s: SIZE:%d, TOTAL:%d, USED:%d, POOL:%d, USED_BYTES:%d, POOLED_BYTES:%d", 
			aZone->name, 
			aZone->object_size,
			aZone->object_total, 
			aZone->object_total-aZone->object_for_recycle,
			aZone->object_for_recycle,
			(aZone->object_total-aZone->object_for_recycle)*aZone->object_size,
			aZone->object_for_recycle*aZone->object_size);
}

void
zone_print(TZone *aZone)
{
	printf("%s:	SIZE:%d, TOTAL:%d, USED:%d, POOL:%d, USED_BYTES:%d, POOLED_BYTES:%d\n", 
			aZone->name, 
			aZone->object_size,
			aZone->object_total, 
			aZone->object_total-aZone->object_for_recycle,
			aZone->object_for_recycle,
			aZone->object_total*aZone->object_size,
			aZone->object_for_recycle*aZone->object_size);
}
