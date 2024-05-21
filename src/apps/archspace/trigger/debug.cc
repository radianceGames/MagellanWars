//#include "util.h"
#include "common.h"
#include "../triggers.h"
#include "../archspace.h"
#include "../council.h"
#include "../battle.h"
#include "../admin.h"
#include "../preference.h"
#include "../portal.h"

extern TZone gPacketZone;

//#if !defined(MEMORY_DEBUG)
//extern TZone gInnerString64Zone;
//#endif

extern TZone gInnerString128Zone;
extern TZone gInnerString256Zone;
extern TZone gInnerString1KZone;
extern TZone gInnerString2KZone;
extern TZone gInnerString4KZone;
extern TZone gInnerString8KZone;
extern TZone gInnerString16KZone;
extern TZone gInnerString32KZone;
extern TZone gInnerString64KZone;
extern TZone gInnerString128KZone;
extern TZone gInnerString256KZone;
extern TZone gInnerString512KZone;
extern TZone gInnerString2MZone;
extern TZone gInnerList128Zone;
extern TZone gInnerList256Zone;
extern TZone gInnerList1KZone;
extern TZone gInnerList2KZone;
extern TZone gInnerList4KZone;
extern TZone gInnerList8KZone;
extern TZone gInnerList16KZone;
extern TZone gInnerList1MZone;


static int gTotal, gUsed, gPool;
static int gTotalDiff=0, gUsedDiff=0, gPoolDiff=0;



static void acc_zone(TZone *aZone)
{
	gTotal += aZone->object_size*aZone->object_total;
	gUsed += aZone->object_size*(aZone->object_total-aZone->object_for_recycle);
	gPool += aZone->object_size*aZone->object_for_recycle;
}

bool
CTriggerDebug::handler()
{
	gTotal = gUsed = gPool = 0;

// log reduction
	CString::print_debug();
	// Library
	zone_info(&gPacketZone);
	zone_info(&gQueryZone);
	zone_info(&gQueryListZone);
	zone_info(&gMessageZone);
	zone_info(&gStringZone);
	zone_info(&gSQLZone);

	// Game
	zone_info(&gPlayerActionZone);
	zone_info(&gCouncilActionZone);
	zone_info(&gDetachmentPlayerPlayerZone);
	zone_info(&gDetachmentPlayerCouncilZone);
	zone_info(&gDetachmentCouncilCouncilZone);
	zone_info(&gAdmiralZone);
	zone_info(&gFleetEffectZone);
	zone_info(&gAdmissionZone);
	zone_info(&gCouncilZone);
	zone_info(&gControlModelZone);
	zone_info(&gDefensePlanZone);
	zone_info(&gDefenseFleetZone);
	zone_info(&gPlayerEffectZone);
	zone_info(&gEmpireAdmiralInfo);
	zone_info(&gEmpireFleetInfo);
	zone_info(&gEmpirePlanetInfo);
	zone_info(&gEmpireActionZone);
	zone_info(&gEventEffectZone);
	zone_info(&gEventInstanceZone);
	zone_info(&gFortressZone);
	zone_info(&gMagistrateZone);
	zone_info(&gDiplomaticMessageZone);
	zone_info(&gCouncilMessageZone);
	zone_info(&gPlanetZone);
	zone_info(&gPlayerZone);
	zone_info(&gProjectZone);
	zone_info(&gPurchasedProjectZone);
	zone_info(&gRankZone);
	zone_info(&gShipDesignZone);
	zone_info(&gShipToBuildZone);
	zone_info(&gShipBuildQZone);
	zone_info(&gDockedShipZone);
	zone_info(&gArchspaceConnectionZone);
	zone_info(&gKnownTechZone);
	zone_info(&gFleetZone);
	zone_info(&gBattleZone);
	zone_info(&gBattleRecordZone);
	zone_info(&gBattleFleetZone);
	zone_info(&gClusterZone);
	zone_info(&gCouncilActionZone);
	zone_info(&gPlayerRelationZone);
	zone_info(&gCouncilRelationZone);
	zone_info(&gShipSizeZone);
	zone_info(&gDamagedShipZone);
	zone_info(&gTargetFleetZone);
	zone_info(&gFormationPointZone);
	zone_info(&gPreferenceZone);
	zone_info(&gPortalUserZone);
	zone_info(&gPortalUserListZone);	
	zone_info(&gPlayerEventZone);
	zone_info(&gSQLZone);
//#if !defined(MEMORY_DEBUG)
//	zone_info(&gInnerString64Zone);
//#endif
	zone_info(&gInnerString128Zone);
	zone_info(&gInnerString256Zone);
	zone_info(&gInnerString1KZone);
	zone_info(&gInnerString2KZone);
	zone_info(&gInnerString4KZone);
	zone_info(&gInnerString8KZone);
	zone_info(&gInnerString16KZone);
	zone_info(&gInnerString32KZone);
	zone_info(&gInnerString64KZone);
	zone_info(&gInnerString128KZone);
	zone_info(&gInnerString256KZone);
	zone_info(&gInnerString512KZone);
	zone_info(&gInnerString2MZone);
	zone_info(&gInnerList128Zone);
	zone_info(&gInnerList256Zone);
	zone_info(&gInnerList1KZone);
	zone_info(&gInnerList2KZone);
	zone_info(&gInnerList4KZone);
	zone_info(&gInnerList8KZone);
	zone_info(&gInnerList16KZone);
	zone_info(&gInnerList1MZone);



	// Acc
	acc_zone(&gPacketZone);
	acc_zone(&gQueryZone);
	acc_zone(&gQueryListZone);
	acc_zone(&gMessageZone);
	acc_zone(&gStringZone);
	acc_zone(&gSQLZone);

	// Game
	acc_zone(&gPlayerActionZone);
	acc_zone(&gCouncilActionZone);
	acc_zone(&gDetachmentPlayerPlayerZone);
	acc_zone(&gDetachmentPlayerCouncilZone);
	acc_zone(&gDetachmentCouncilCouncilZone);
	acc_zone(&gAdmiralZone);
	acc_zone(&gFleetEffectZone);
	acc_zone(&gAdmissionZone);
	acc_zone(&gCouncilZone);
	acc_zone(&gControlModelZone);
	acc_zone(&gDefensePlanZone);
	acc_zone(&gDefenseFleetZone);
	acc_zone(&gPlayerEffectZone);
	acc_zone(&gEmpireAdmiralInfo);
	acc_zone(&gEmpireFleetInfo);
	acc_zone(&gEmpirePlanetInfo);
	acc_zone(&gEmpireActionZone);
	acc_zone(&gEventEffectZone);
	acc_zone(&gEventInstanceZone);
	acc_zone(&gFortressZone);
	acc_zone(&gMagistrateZone);
	acc_zone(&gDiplomaticMessageZone);
	acc_zone(&gCouncilMessageZone);
	acc_zone(&gPlanetZone);
	acc_zone(&gPlayerZone);
	acc_zone(&gProjectZone);
	acc_zone(&gPurchasedProjectZone);
	acc_zone(&gRankZone);
	acc_zone(&gShipDesignZone);
	acc_zone(&gShipToBuildZone);
	acc_zone(&gShipBuildQZone);
	acc_zone(&gDockedShipZone);
	acc_zone(&gArchspaceConnectionZone);
	acc_zone(&gKnownTechZone);
	acc_zone(&gFleetZone);
	acc_zone(&gBattleZone);
	acc_zone(&gBattleRecordZone);
	acc_zone(&gBattleFleetZone);
	acc_zone(&gClusterZone);
	acc_zone(&gCouncilActionZone);
	acc_zone(&gPlayerRelationZone);
	acc_zone(&gCouncilRelationZone);
	acc_zone(&gShipSizeZone);
	acc_zone(&gDamagedShipZone);
	acc_zone(&gTargetFleetZone);
	acc_zone(&gFormationPointZone);
	acc_zone(&gPlayerEventZone);
	acc_zone(&gPreferenceZone);
	acc_zone(&gPortalUserZone);
	acc_zone(&gPortalUserListZone);	
	acc_zone(&gSQLZone);	
	
//#if !defined(MEMORY_DEBUG)
//	acc_zone(&gInnerString64Zone);
//#endif
	acc_zone(&gInnerString128Zone);
	acc_zone(&gInnerString256Zone);
	acc_zone(&gInnerString1KZone);
	acc_zone(&gInnerString2KZone);
	acc_zone(&gInnerString4KZone);
	acc_zone(&gInnerString8KZone);
	acc_zone(&gInnerString16KZone);
	acc_zone(&gInnerString32KZone);
	acc_zone(&gInnerString64KZone);
	acc_zone(&gInnerString128KZone);
	acc_zone(&gInnerString256KZone);
	acc_zone(&gInnerString512KZone);
	acc_zone(&gInnerString2MZone);
	acc_zone(&gInnerList128Zone);
	acc_zone(&gInnerList256Zone);
	acc_zone(&gInnerList1KZone);
	acc_zone(&gInnerList2KZone);
	acc_zone(&gInnerList4KZone);
	acc_zone(&gInnerList8KZone);
	acc_zone(&gInnerList16KZone);
	acc_zone(&gInnerList1MZone);

	SLOG("TOTAL DYNAMIC MEMORY: %d, USED: %d, POOL: %d", gTotal, gUsed, gPool);
	gTotalDiff -= gTotal; gUsedDiff -= gUsed; gPoolDiff -= gPool;
	SLOG("DIFF TOTAL DYNAMIC MEMORY: %d, USED: %d, POOL: %d", -gTotalDiff, -gUsedDiff, -gPoolDiff);
	gTotalDiff = gTotal; gUsedDiff = gUsed; gPoolDiff = gPool;
	

/*	FILE *File = fopen("32Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString32KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("64Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString64KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("128Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString128KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("256Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString256KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}
	
	File = fopen("512Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString512KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("2Mstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString2MZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}


*/

//	mem_usage();
//#endif
/*
#ifdef MEMORY_DEBUG
	extern int chunk_id;
	extern THeaven heaven[];

	SLOG( "total Chunk %d", chunk_id );

	for( int i = 0; i < chunk_id; i++ ){
		if( heaven[i].end->magic != 100 ){
			system_log( "Chunk : End Violated : %s %s m%d e%d", heaven[i].chunk->info, heaven[i].zone->name, heaven[i].chunk->magic, heaven[i].end->magic );
		}
		if( heaven[i].chunk->magic != 100 && heaven[i].chunk->magic != 666 ){
			system_log( "Destroyed Chunk : %s %d %d m%d : %s %d ", heaven[i].chunk->info, heaven[i].chunk, i, heaven[i].chunk->magic, heaven[i].zone->name, heaven[i].zone->recycle);
			if (i==3311)
			{
				CShipSize* ShipSize = (CShipSize*)((char*)heaven[i].chunk+sizeof(TChunk));
				SLOG("Chunk Test Name: %s, ID:%d", ShipSize->get_name(), ShipSize->get_id());

			}
			int
				min_diff = 99999999,
				candi = 0;
			for( int j = 0; j < chunk_id; j++ ){
				int diff = (int)(heaven[i].chunk) - (int)(heaven[j].chunk);
				if( diff > 0 && diff < min_diff ){
					candi = j;
					min_diff = diff;
				}
			}
			system_log( "Candidate Chunk : %s %d %d %d m%d %s e%d", heaven[candi].chunk->info, min_diff, heaven[candi].chunk, candi, heaven[candi].chunk->magic, heaven[candi].zone->name, heaven[candi].end->magic );
		}
	}

#endif
*/
	return true;
}
