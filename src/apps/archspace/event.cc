#include "common.h"
#include "util.h"
#include "event.h"
#include "script.h"
#include "define.h"
#include <cstdlib>
#include "player.h"
#include "race.h"
#include "archspace.h"
#include <cstdio>
#include "game.h"

TZone gPlayerEventZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEvent),
	0,
	0,
	NULL,
	"Zone CEvent"
};

TZone gEventInstanceZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEventInstance),
	0,
	0,
	NULL,
	"Zone CEventInstance"
};

TZone gEventEffectZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEventEffect),
	0,
	0,
	NULL,
	"Zone CEventEffect"
};

char *
CEvent::mTypeName[] = {
	"",
	"System",
	"Council",
	"Cluster",
	"Magistrate",
	"Empire",
	"Galactic",
	"Answer",
	"Racial",
	"Major",
	"Human Only",
	"Targoid Only",
	"Buckaneer Only",
	"Tecanoid Only",
	"Evintos Only",
	"Agerus Only",
	"Bosalian Only",
	"Xeloss Only",
	"Xerusian Only",
	"Xesperados Only",
	"Immune to Human",
	"Immune to Targoid",
	"Immune to Buckaneer",
	"Immune to Tecanoid",
	"Immune to Evintos",
	"Immune to Agerus",
	"Immune to Bosalian",
	"Immune to Xeloss",
	"Immune to Xerusian",
	"Immune to Xesperados",
	""
};

bool
CEventScript::get(CEventTable *aEventTable)
{
	SLOG("start event script no:%d", mRoot.length());

	int i=0;
	TSomething Event;

	while((Event = get_array(i++)))
	{
		CEvent
			*TmpEvent = new CEvent;

		TmpEvent->set_name(get_name(Event));

		TSomething
			Something;

		Something = get_section("Number", Event);
		if (Something) {
			TmpEvent->set_id(atoi(get_data(Something)));
		} else {
			SLOG("WRONG EVENT FILE at %s", TmpEvent->get_name());
			return false;
		}

		Something = get_section("Description", Event);
		if (Something) {
			TmpEvent->set_description(get_data(Something));
		} else {
			SLOG("WRONG EVENT FILE at %s(%d)",
					TmpEvent->get_name(), TmpEvent->get_id() );
			return false;
		}

		Something = get_section("Image", Event);
		if (Something) {
			TmpEvent->set_image(get_data(Something));
		} else {
//			SLOG("WRONG EVENT FILE at %s(%d)",
//					TmpEvent->get_name(), TmpEvent->get_id() );
            TmpEvent->set_image("/image/as_game/blank.gif");
		}

		Something = get_section("Type", Event);
		if (Something) {
			TSomething
				Type;
			int
				i = 0;

			while( (Type = get_array(i, Something)) ){
				TmpEvent->set_type(get_data(Type));
				i++;
			}
		}

		Something = get_section("Duration", Event);
		if (Something) {
			TmpEvent->set_duration(atoi(get_data(Something)));
		} else {
			TSomething
				Min, Max;

			Min = get_section("DurationMin");
			Max = get_section("DurationMax");
			if( Min && Max ){
				TmpEvent->set_duration_min(atoi(get_data(Min)));
				TmpEvent->set_duration_max(atoi(get_data(Max)));
			}
		}

		Something = get_section("MinHonor", Event);
		if (Something)
        {
//		    SLOG("Something: %d", atoi(get_data(Something)));
			TmpEvent->set_honor_min(atoi(get_data(Something)));
			Something = get_section("MaxHonor", Event);
//			SLOG("Something 2: %d", atoi(get_data(Something)));
			if (Something)
				TmpEvent->set_honor_max(atoi(get_data(Something)));
			else {
			    SLOG("WARNING: HonorMin without HonorMax :: TmpEvent->get_id() = %d", TmpEvent->get_id());
			    TmpEvent->set_honor_min(-1);
			    TmpEvent->set_honor_max(-1);
            }

		} else {
			    TmpEvent->set_honor_min(-1);
			    TmpEvent->set_honor_max(-1);
		}
		Something = get_section("MinTurn", Event);
				if (Something)
		        {
		//		    SLOG("Something: %d", atoi(get_data(Something)));
					TmpEvent->set_turn_min(atoi(get_data(Something)));
					Something = get_section("MaxTurn", Event);
		//			SLOG("Something 2: %d", atoi(get_data(Something)));
					if (Something)
						TmpEvent->set_turn_max(atoi(get_data(Something)));
					else {
					    SLOG("WARNING: MinTurn without MaxTurn :: TmpEvent->get_id() = %d", TmpEvent->get_id());
					    TmpEvent->set_turn_min(-1);
					    TmpEvent->set_turn_max(-1);
		            }

				} else {
					    TmpEvent->set_turn_min(-1);
					    TmpEvent->set_turn_max(-1);
		}
//		SLOG("honor min:%d honor max:%d",TmpEvent->get_honor_min(),TmpEvent->get_honor_max());

		Something = get_section("EffectList", Event);
		if (Something)
		{
			TSomething
				Effect;
			int
				i = 0;

			while( (Effect = get_array(i, Something)) ){
				CEventEffect
					*TmpEffect = new CEventEffect();

				if( TmpEffect->set_type(get_name(Effect)) == false ){
					SLOG( "EVENT wrong effect %d %s - %s", TmpEvent->get_id(), TmpEvent->get_name(), get_name(Effect) );
					delete TmpEffect;
					i++;
					continue;
				}

				TSomething
					Part;
				Part = get_section("Target", Effect);
				if( Part == NULL )
					TmpEffect->set_target(-1);
				else
					TmpEffect->set_target(atoi(get_data(Part)));
				Part = get_section("Arg1", Effect);
				if( Part )
					TmpEffect->set_argument1(atoi(get_data(Part)));
				Part = get_section("Arg2", Effect);
				if( Part )
					TmpEffect->set_argument2(atoi(get_data(Part)));

				Part = get_section("Chance", Effect);
				if( Part )
					TmpEffect->set_chance(atoi(get_data(Part)));

				Part = get_section("Duration", Effect);
				if (Part == NULL)
				{
					SLOG("WARNING : No effect duration, TmpEvent->get_id() = %d, Effect's name = %s", TmpEvent->get_id(), get_name(Effect));
					//delete TmpEffect;
					//i++;
					//continue;
					TmpEffect->set_duration(TmpEvent->get_duration());
				}
				else
				{
					TmpEffect->set_duration(atoi(get_data(Part)));
				}

				Part = get_section("BeginOnly", Effect);
				if( Part && strcasecmp(get_data(Part),"Yes") == 0 )
					TmpEffect->add_attribute(CEventEffect::EE_BEGIN_ONLY);

				Part = get_section("Answer", Effect);
				if( Part ){
					if( strcasecmp(get_data(Part),"Yes") == 0 )
						TmpEffect->add_attribute(CEventEffect::EE_ANSWER_YES);
					else if( strcasecmp(get_data(Part),"No") == 0 )
						TmpEffect->add_attribute(CEventEffect::EE_ANSWER_NO);
					else
						SLOG( "EVENT wrong answer %d %s %s", TmpEvent->get_id(), TmpEvent->get_name(), get_data(Part) );
				}

				Part = get_section("Apply", Effect);
				if( Part)
				{
					if( TmpEffect->set_apply(get_data(Part)) == false )
						SLOG( "EVENT wrong apply %d %s %s", TmpEvent->get_id(), TmpEvent->get_name(), get_data(Part) );
				}

				TmpEffect->set_source( CPlayerEffect::ST_EVENT, TmpEvent->get_id() );
				TmpEvent->add_effect( TmpEffect );

				i++;
			}
		}

		if( aEventTable->add_event(TmpEvent) == 0 )
			SLOG( "EVENT duplicate key found : %s %d", TmpEvent->get_name(), TmpEvent->get_id() );
	}
	SLOG("end script");
	return true;
}

CEventEffect::CEventEffect()
{
	mDuration = mChance = 0;
}

CEventEffect::~CEventEffect()
{
}

void
CEventEffect::set_chance( int aChance )
{
	mChance = aChance;
	mAttribute += EE_CHANCE;
}

int
CEventEffect::get_chance()
{
	if( mAttribute.has(EE_CHANCE) ) return mChance;
	return 100;
}

bool
CEventEffectList::free_item(TSomething aItem)
{
	CEventEffect
		*Effect = (CEventEffect*)aItem;

	if( Effect == NULL ) return false;

	delete Effect;
	return true;
}

CEvent::CEvent()
{
	mID = 0;
	mDurationMin = mDurationMax = 0;
}

CEvent::~CEvent()
{
}

bool
CEvent::set_type( char *aType )
{
	int
		Type = get_type(aType);

	if( Type == EVENT_TYPE_MAX ) return false;
	set_type( Type );

	return true;
}

int
CEvent::get_type( char *aType )
{
	int i;

	for( i = 1; i < EVENT_TYPE_MAX; i++ ){
		if( strcasecmp( aType, mTypeName[i] ) == 0 ) break;
	}

	return i;
}

int
CEvent::get_duration()
{
	if( mDurationMin == mDurationMax ) return mDurationMin;

	int
		Diff = mDurationMax-mDurationMin;

	return mDurationMin-1+number(Diff);
}

bool
CEvent::possible_event(CPlayer *aPlayer)
{
	int
		ImmuneTo = 0;
	switch (aPlayer->get_race())
	{
		case CRace::RACE_HUMAN :
			ImmuneTo = EVENT_IMMUNE_TO_HUMAN;
			break;
		case CRace::RACE_TARGOID :
			ImmuneTo = EVENT_IMMUNE_TO_TARGOID;
			break;
		case CRace::RACE_BUCKANEER :
			ImmuneTo = EVENT_IMMUNE_TO_BUCKANEER;
			break;
		case CRace::RACE_TECANOID :
			ImmuneTo = EVENT_IMMUNE_TO_TECANOID;
			break;
		case CRace::RACE_EVINTOS :
			ImmuneTo = EVENT_IMMUNE_TO_EVINTOS;
			break;
		case CRace::RACE_AGERUS :
			ImmuneTo = EVENT_IMMUNE_TO_AGERUS;
			break;
		case CRace::RACE_BOSALIAN :
			ImmuneTo = EVENT_IMMUNE_TO_BOSALIAN;
			break;
		case CRace::RACE_XELOSS :
			ImmuneTo = EVENT_IMMUNE_TO_XELOSS;
			break;
		case CRace::RACE_XERUSIAN :
			ImmuneTo = EVENT_IMMUNE_TO_XERUSIAN;
			break;
		case CRace::RACE_XESPERADOS :
			ImmuneTo = EVENT_IMMUNE_TO_XESPERADOS;
			break;
	}

	if( has_type(ImmuneTo) ) return false;
	return true;
}

const char *
CEvent::get_type_str()
{
	static CString
		Buf;
	Buf.clear();

	for( int i = 0; i < EVENT_TYPE_MAX; i++ )
		if( has_type(i) ) Buf.format( "%s ", mTypeName[i] );

	return (char*)Buf;
}

bool CEvent::honor_in_range(int aHonor)
{
/* CString temp;
 temp.clear();
 if (mName == NULL)
    temp = "(null)";
 else
    temp = mName;
 SLOG("LOGGING: Event %s ID %d", (char *)temp, mID);*/
 if (mHonorMin == -1 || mHonorMax == -1)
 {
 //   SLOG("[OK] H:%d HMax: %d HMin: %d", aHonor, mHonorMax, mHonorMin);
    return true;
 }
 if (aHonor <= mHonorMax && aHonor >= mHonorMin)
 {
//   SLOG("[OK] H:%d HMax: %d HMin: %d", aHonor, mHonorMax, mHonorMin);
    return true;
 }
// SLOG("[!] H:%d HMax: %d HMin: %d", aHonor, mHonorMax, mHonorMin);
 return false;
}
bool CEvent::turn_in_range()
{
/* CString temp;
 temp.clear();
 if (mName == NULL)
    temp = "(null)";
 else
    temp = mName;
 SLOG("LOGGING: Event %s ID %d", (char *)temp, mID);*/
 if (mTurnMin == -1 || mTurnMax == -1)
 {
 //   SLOG("[OK] H:%d HMax: %d HMin: %d", aHonor, mHonorMax, mHonorMin);
    return true;
 }
 if ((CGame::get_game_time() / CGame::mSecondPerTurn) <= mTurnMax && (CGame::get_game_time() / CGame::mSecondPerTurn) >= mTurnMin)
 {
//   SLOG("[OK] H:%d HMax: %d HMin: %d", aHonor, mHonorMax, mHonorMin);
    return true;
 }
// SLOG("[!] H:%d HMax: %d HMin: %d", aHonor, mHonorMax, mHonorMin);
 return false;
}

CEventTable::CEventTable()
{
}

CEventTable::~CEventTable()
{
	remove_all();
}

bool
CEventTable::free_item(TSomething aItem)
{
	CEvent
		*Event = (CEvent*)aItem;

	if( Event == NULL ) return false;

	delete Event;

	return true;
}

int
CEventTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CEvent
		*Event1 = (CEvent *)aItem1,
		*Event2 = (CEvent *)aItem2;

	if (Event1->get_id() > Event2->get_id()) return 1;
	if (Event1->get_id() < Event2->get_id()) return -1;
	return 0;
}

int
CEventTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CEvent
		*Event = (CEvent *)aItem;

	if (Event->get_id() > (int)aKey) return 1;
	if (Event->get_id() < (int)aKey) return -1;
	return 0;
}

int
CEventTable::add_event(CEvent *aEvent)
{
	if( aEvent == NULL ) return 0;

	if( find_sorted_key((TConstSomething)aEvent->get_id()) >= 0 )
		return 0;
	insert_sorted(aEvent);

	return aEvent->get_id();
}

CEvent *
CEventTable::get_by_id(int aID)
{
	if (length() == 0) return NULL;

	int
		Index = find_sorted_key((void *)aID);
	if (Index < 0) return NULL;

	return (CEvent *)get(Index);
}

CEvent *
CEventTable::get_random_by_type( int aType )
{
	int
		Count = 0;
	for (int i=0 ; i<length() ; i++)
	{
		CEvent *
			Event = (CEvent *)get(i);

		if (Event->has_type(aType) == true) Count++;
	}

	Count = number(Count);
	for (int i=0 ; i<length() ; i++)
	{
		CEvent *
			Event = (CEvent *)get(i);

		if (Event->has_type(aType) == true)
		{
			Count--;
			if (Count <= 0) return Event;
		}
	}

	return NULL;
}

CEvent *
CEventTable::get_random_by_honor_and_type (int aHonor, int aType)
{
	int
		Count = 0;
	for (int i=0 ; i<length() ; i++)
	{
		CEvent *
			Event = (CEvent *)get(i);

		if (Event->has_type(aType) == true && Event->honor_in_range(aHonor) == true && Event->turn_in_range( ))
            Count++;
	}

	Count = number(Count);
	for (int i=0 ; i<length() ; i++)
	{
		CEvent *
			Event = (CEvent *)get(i);

		if (Event->has_type(aType) == true && Event->honor_in_range(aHonor) == true && Event->turn_in_range())
		{
			Count--;
			if (Count <= 0) return Event;
		}
	}

	return NULL;
}

CEventInstance::CEventInstance()
{
	mID = mOwner = 0;
	mTime = mLife = 0;
	mEvent = NULL;
	mAnswered = false;
}

CEventInstance::CEventInstance(CEvent *aEvent, int aTime, int aLife)
{
	mID = mOwner = 0;
	mTime = aTime;
	mEvent = aEvent;
	mAnswered = false;
	if (aLife == -1)
	{
		mLife = CGame::get_game_time() + aEvent->get_duration()*CGame::mSecondPerTurn;
	}
	else
	{
		mLife = aLife;
	}
}

CEventInstance::CEventInstance(CEvent *aEvent, MYSQL_ROW aRow)
{
	mEvent = aEvent;
	mID = atoi(aRow[STORE_ID]);
	mOwner = atoi(aRow[STORE_OWNER]);
	mLife = atoi(aRow[STORE_LIFE]);
	mTime = atoi(aRow[STORE_TIME]) + CGameStatus::get_down_time();
	mStoreFlag += STORE_TIME;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
	mAnswered = (bool)atoi(aRow[STORE_ANSWERED]);
}

CEventInstance::~CEventInstance()
{
	mEvent = NULL;
}

CString &
CEventInstance::query()
{
	static CString
		Query;
	Query.clear();

	switch (type())
	{
		case QUERY_INSERT :
			Query.format( "INSERT INTO player_event ( id, owner, event, life, time, answered ) VALUES ( %d, %d, %d, %d, %d, %d )", mID, mOwner, get_event_id(), mLife, mTime, (int)mAnswered );
			break;
		case QUERY_DELETE :
			Query.format( "DELETE FROM player_event WHERE owner = %d AND id = %d", mOwner, mID );
			break;
		case QUERY_UPDATE :
			Query.format( "UPDATE player_event SET answered = %d WHERE owner = %d AND id = %d", (int)mAnswered, mOwner, mID );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
CEventInstance::set_id(int aID)
{
	mID = aID;
	mStoreFlag += STORE_ID;

	return true;
}

bool
CEventInstance::set_owner(int aOwner)
{
	mOwner = aOwner;
	mStoreFlag += STORE_OWNER;

	return true;
}

void
CEventInstance::save_new_event()
{
	type(QUERY_INSERT);
	STORE_CENTER->store(*this);
}

bool
CEventInstance::set_answered()
{
	mAnswered = true;
	mStoreFlag += STORE_ANSWERED;

	return true;
}

const char *
CEventInstance::get_time_str()
{
	static char str[100];
	time_t Time;
	struct tm *result;

	time(&Time);
	result = localtime(&Time);
	strftime(str, 50, "%Y/%m/%d/%H:%M", result);

	return str;
}

const char *
CEventInstance::html( char *aType )
{
	static CString
		Buf;
	Buf.clear();

	Buf += "<TR>\n";
	Buf += "<TD CLASS=\"maintext\">\n";

	Buf += "<TABLE>\n";
	Buf += "<TR>\n";

	Buf += "<TD WIDTH=\"96\" CLASS=\"maintext\" VALIGN=\"TOP\">";
	Buf.format("<FONT COLOR=\"#FFFFFF\" SIZE=\"2\">%s</FONT>", get_time_str());
	Buf += "</TD>\n";

	Buf += "<TD WIDTH=\"424\" CLASS=\"maintext\">\n";
	Buf.format("<A HREF=\"show_event.as?EVENT_ID=%d&EVENT_TYPE=%s\">%s</A>\n",
				get_id(), aType, mEvent->get_name());
	Buf += "</TD>\n";

	Buf += "</TR>\n";
	Buf += "</TABLE>\n";

	Buf += "</TD>\n";
	Buf += "</TR>\n";

	return (char*)Buf;
}

CEventInstanceList::CEventInstanceList()
{
}

CEventInstanceList::~CEventInstanceList()
{
	remove_all();
}

bool
CEventInstanceList::free_item(TSomething aItem)
{
	CEventInstance
		*Event = (CEventInstance*)aItem;

	if( Event == NULL ) return false;

	delete Event;

	return true;
}

int
CEventInstanceList::compare(TSomething aItem1, TSomething aItem2) const
{
	CEventInstance
		*Event1 = (CEventInstance *)aItem1,
		*Event2 = (CEventInstance *)aItem2;

	if (Event1->get_id() > Event2->get_id()) return 1;
	if (Event1->get_id() < Event2->get_id()) return -1;
	return 0;
}

int
CEventInstanceList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CEventInstance
		*Event= (CEventInstance *)aItem;

	if (Event->get_id() > (int)aKey) return 1;
	if (Event->get_id() < (int)aKey) return -1;
	return 0;
}

int
CEventInstanceList::add_event_instance(CEventInstance *aEvent)
{
	if (aEvent == NULL) return 0;

	if (find_sorted_key((TConstSomething)aEvent->get_id()) >= 0)
		return 0;
	insert_sorted(aEvent);

	return aEvent->get_id();
}

int
CEventInstanceList::get_new_id()
{
	int
		ID = 1;

	while(find_sorted_key((void*)ID) >= 0) ID++;

	return ID;
}

bool
CEventInstanceList::remove_event_instance(CEventInstance *aEvent)
{
	if (aEvent == NULL) return false;

	int
		Index = find_sorted_key((void*)(aEvent->get_id()));
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool
CEventInstanceList::remove_event_instance(int aID)
{
	int
		Index = find_sorted_key((void *)aID);
	if(Index < 0) return false;

	return CSortedList::remove(Index);
}

void
CEventInstanceList::save_galactic_event()
{
	FILE *fp;
	char buf[100];

	sprintf(buf, "%s/galactic", ARCHSPACE->configuration().get_string("Game", "EventDir"));
	fp = fopen(buf, "w");
	if (fp == NULL)
	{
		SLOG("Event : cannot open event file %s", buf);
		return;
	}

	for(int i=0 ; i<length() ; i++)
	{
		CEventInstance *
			Event = (CEventInstance *)get(i);
		fprintf(fp, "%d %d %d %d\n", Event->get_id(), Event->get_event_id(), Event->get_time(), Event->get_life());

	}

	fclose(fp);
}

void
CEventInstanceList::load_galactic_event()
{
	FILE *fp;
	char buf[100];

	sprintf( buf, "%s/galactic", ARCHSPACE->configuration().get_string( "Game", "EventDir" ) );
	fp = fopen( buf, "r" );
	if (fp == NULL) return;

	while(fgets(buf,99,fp))
	{
		int id, t, l, r, ev;

		r = sscanf( buf, "%d %d %d %d", &id, &ev, &t, &l );
		if( r < 4 ){
			SLOG( "galactic event loading %d prieces %d %d %d %d", r, id, ev, t, l );
			continue;
		}

		CEvent
			*Event = EVENT_TABLE->get_by_id(ev);
		if( Event == NULL ){
			SLOG( "Wrong Galactic Event loading %d %d %d %d", id, ev, t, l );
			continue;
		}

		CEventInstance
			*Instance = new CEventInstance( Event, t, l );
		Instance->set_id(id);
		add_event_instance(Instance);
	}

	fclose(fp);
}

void
CEventInstanceList::save_cluster_event( CCluster *aCluster )
{
	FILE *fp;
	char buf[100];

	sprintf( buf, "%s/%d", ARCHSPACE->configuration().get_string( "Game", "EventDir" ), aCluster->get_id() );
	fp = fopen( buf, "w" );
	if( fp == NULL ){
		SLOG( "Event : cannot open event file %s", buf );
		return;
	}

	for( int i = 0; i < length(); i++ ){
		CEventInstance
			*Event = (CEventInstance*)get(i);
		fprintf( fp, "%d %d %d %d\n", Event->get_id(), Event->get_event_id(), Event->get_time(), Event->get_life() );

	}

	fclose(fp);
}

void
CEventInstanceList::load_cluster_event( CCluster *aCluster )
{
	FILE *fp;
	char buf[100];

	sprintf( buf, "%s/%d", ARCHSPACE->configuration().get_string( "Game", "EventDir" ), aCluster->get_id() );
	fp = fopen( buf, "r" );
	if( fp == NULL ){
		return;
	}

	while(fgets(buf,99,fp)){
		int ev, id, t, l, r;

		r = sscanf( buf, "%d %d %d %d", &id, &ev, &t, &l );
		if( r < 4 ){
			SLOG( "cluster %s event loading %d prieces %d %d %d %d", aCluster->get_name(), r, id, ev, t, l );
			continue;
		}

		CEvent
			*Event = EVENT_TABLE->get_by_id(ev);
		if( Event == NULL ){
			SLOG( "Wrong cluster %s Event loading %d %d %d %d", aCluster->get_name(), id, ev, t, l );
			continue;
		}

		CEventInstance
			*Instance = new CEventInstance( Event, t, l );
		Instance->set_id(id);
		add_event_instance(Instance);
	}

	fclose(fp);
}

void
CEventInstanceList::expire_out_due_event()
{
	for (int i = length()-1; i >= 0; i--)
	{
		CEventInstance *
			Event = (CEventInstance*)get(i);

		if (Event->get_life() < CGame::get_game_time())
		{
			remove_event_instance(Event);
		}
	}
}

const char *
CEventInstanceList::html( char *aType )
{
	static CString
		Buf;
	Buf.clear();

	if (length() == 0) return NULL;

	Buf += "<TR><TD class=maintext ALIGN=center>\n";
	Buf += "<TABLE width=520 border=0 cellspacing=0 cellpadding=0>\n";

	for( int i = 0; i < length(); i++ ){
		CEventInstance
			*Instance = (CEventInstance*)get(i);

		Buf += Instance->html( aType );
	}

	Buf += "</TABLE>\n";
	Buf += "</TR></TD>\n";

	return (char*)Buf;
}

CEventInstance*
CEventInstanceList::get_by_id(int aID)
{
	if( length() == 0 ) return NULL;

	int
		Index = find_sorted_key((void *)aID);
	if( Index < 0 ) return NULL;

	return (CEventInstance*)get(Index);
}
