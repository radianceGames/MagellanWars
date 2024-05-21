#include "tech.h"
#include "define.h"
#include "archspace.h"
#include "game.h"

const int mPowerMult = 10;

TZone gKnownTechListZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CKnownTechList),
	0,
	0,
	NULL,
	"Zone CKnownTechList"
};

CKnownTechList::CKnownTechList()
{
	mQueryType = QUERY_INSERT;
	mOwner = 0;
	mPower = 0;
}

CKnownTechList::~CKnownTechList()
{
	remove_all();
}

bool
CKnownTechList::free_item(TSomething aItem)
{
	CKnownTech
		*Tech = (CKnownTech *)aItem;

	if( Tech == NULL ) return false;

	delete Tech;

	return true;
}

int
CKnownTechList::compare(TSomething aItem1, TSomething aItem2) const
{
	CKnownTech
		*KnownTech1 = (CKnownTech*)aItem1,
		*KnownTech2 = (CKnownTech*)aItem2;

	if (KnownTech1->get_id() > KnownTech2->get_id()) return 1;
	if (KnownTech1->get_id() < KnownTech2->get_id()) return -1;
	return 0;
}

int
CKnownTechList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CKnownTech
		*KnownTech = (CKnownTech*)aItem;

	if (KnownTech->get_id() > (int)aKey) return 1;
	if (KnownTech->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CKnownTechList::initialize(MYSQL_ROW aRow)
{
    SLOG("known.tech.list.cc : CKnownTechList::initialize : called!");
	enum
	{
		FIELD_OWNER = 0,
		FIELD_INFO,
		FIELD_LIFE,
		FIELD_MATTER,
		FIELD_SOCIAL,
		FIELD_UPGRADE,
		FIELD_SCHEMATICS,
		FIELD_AMATTER
	};

	mOwner = as_atoi(aRow[FIELD_OWNER]);

	CCommandSet
		Info,
		Life,
		Matter,
		Social,
		Upgrade,
		Schematics,
        AMatter;

	Info.set_string(aRow[FIELD_INFO]);
	Life.set_string(aRow[FIELD_LIFE]);
	Matter.set_string(aRow[FIELD_MATTER]);
	Social.set_string(aRow[FIELD_SOCIAL]);
    Upgrade.set_string(aRow[FIELD_UPGRADE]);
    Schematics.set_string(aRow[FIELD_SCHEMATICS]);
    AMatter.set_string(aRow[FIELD_AMATTER]);

	for (int i=0 ; i<TECH_TABLE->length() ; i++)
	{
		CTech *
			Tech = (CTech *)TECH_TABLE->get(i);

		bool
			AddTech = false;
		switch (Tech->get_type())
		{
			case CTech::TYPE_INFORMATION :
				if (Info.has(Tech->get_id() % 100)) AddTech = true;
				break;

			case CTech::TYPE_LIFE :
				if (Life.has(Tech->get_id() % 100)) AddTech = true;
				break;

			case CTech::TYPE_MATTER_ENERGY :
				if (Matter.has(Tech->get_id() % 100)) AddTech = true;
				break;

			case CTech::TYPE_SOCIAL :
				if (Social.has(Tech->get_id() % 100)) AddTech = true;
				break;

			default :
				break;
		}

		if (AddTech)
		{
			CKnownTech *
				KnownTech = new CKnownTech(Tech);
			KnownTech->set_owner(mOwner);
			KnownTech->set_attribute(Tech->get_attribute());

			add_known_tech(KnownTech);
		}
	}

	mQueryType = QUERY_UPDATE;
	return true;
}

bool
CKnownTechList::remove_known_tech(int aTechID)
{
	int 
		Index;

	Index = find_sorted_key((void*)aTechID);
	if (Index < 0) return false;

	CKnownTech *
		Tech = (CKnownTech *)get(Index);

	switch (Tech->get_type())
	{
		case CTech::TYPE_INFORMATION :
			mInfo -= Tech->get_id() % 100;
			mStoreFlag += STORE_INFO;
			break;

		case CTech::TYPE_LIFE :
			mLife -= Tech->get_id() % 100;
			mStoreFlag += STORE_LIFE;
			break;

		case CTech::TYPE_MATTER_ENERGY :
			mMatter -= Tech->get_id() % 100;
			mStoreFlag += STORE_MATTER;
			break;

		case CTech::TYPE_SOCIAL :
			mSocial -= Tech->get_id() % 100;
			mStoreFlag += STORE_SOCIAL;
			break;

		default :
			break;
	}

	mPower -= (Tech->get_level() * mPowerMult);

	return CSortedList::remove(Index);
}

int 
CKnownTechList::add_known_tech(CKnownTech *aTech)
{
	if (!aTech) return -1;

	if (find_sorted_key((TConstSomething)aTech->get_id()) >= 0) 
		return -1;
	insert_sorted(aTech);

	switch (aTech->get_type())
	{
		case CTech::TYPE_INFORMATION :
			mInfo += aTech->get_id() % 100;
			mStoreFlag += STORE_INFO;
			break;

		case CTech::TYPE_LIFE :
			mLife += aTech->get_id() % 100;
			mStoreFlag += STORE_LIFE;
			break;

		case CTech::TYPE_MATTER_ENERGY :
			mMatter += aTech->get_id() % 100;
			mStoreFlag += STORE_MATTER;
			break;

		case CTech::TYPE_SOCIAL :
			mSocial += aTech->get_id() % 100;
			mStoreFlag += STORE_SOCIAL;
			break;

		default :
			break;
	}

	mPower += (aTech->get_level() * mPowerMult);

	return aTech->get_id();
}

CKnownTech*
CKnownTechList::get_by_id(int aTechID)
{
	if (!length()) return NULL;

	int 
		Index;

	Index = find_sorted_key((void*)aTechID);

	if (Index < 0) return NULL;

	return (CKnownTech*)get(Index);
}

bool
CKnownTechList::refresh_power()
{
	mPower = 0;
	for(int i=0 ; i<length() ; i++)
	{
		CKnownTech *
			Tech = (CKnownTech *)get(i);

		int
			AvailablePower = MAX_POWER - mPower;
		if (Tech->get_power() > AvailablePower)
		{
			mPower = MAX_POWER;
			break;
		}
		else
		{
			mPower += Tech->get_power();
		}
	}

	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(mOwner);
	if (Owner == NULL)
	{
		SLOG("ERROR : Owner is NULL in CKnownTechList::refresh_power(), mOwner = %d", mOwner);
	}
	else if (Owner->get_game_id() != EMPIRE_GAME_ID)
	{
		Owner->refresh_power();
	}

	return true;
}

int
CKnownTechList::count_by_category( int aType )
{
	int
		Known = 0;

	for( int i = 0; i < length(); i++ ){
		CKnownTech
			*Tech = (CKnownTech*)get(i);

		if( Tech->get_type() == aType ) Known++;
	}

	return Known;
}

CString &
CKnownTechList::query()
{
	static CString 
		Query;
	Query.clear();

	switch (type())
	{
		case QUERY_INSERT :
/*		Query.format("INSERT INTO tech (owner, info, life, matter, social)"
								" VALUES (%d, '%s', '%s', '%s', '%s')",
							mOwner,
							(char *)add_slashes(mInfo.get_string(64)),
							(char *)add_slashes(mLife.get_string(64)),
							(char *)add_slashes(mMatter.get_string(64)),
							(char *)add_slashes(mSocial.get_string(64)));         */
			Query.format("INSERT INTO tech (owner, info, life, matter, social, upgrade, schematics, amatter)"
								" VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
							mOwner,
							(char *)add_slashes(mInfo.get_string(64)),
							(char *)add_slashes(mLife.get_string(64)),
							(char *)add_slashes(mMatter.get_string(64)),
							(char *)add_slashes(mSocial.get_string(64)));
			break;

		case QUERY_UPDATE :
			Query.format("UPDATE tech SET owner = %d",
					mOwner);

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_INFO, ", info = '%s'",
					(char *)add_slashes(mInfo.get_string(64)));
			STORE(STORE_LIFE, ", life = '%s'",
					(char *)add_slashes(mLife.get_string(64)));
			STORE(STORE_MATTER, ", matter = '%s'",
					(char *)add_slashes(mMatter.get_string(64)));
			STORE(STORE_SOCIAL, ", social = '%s'",
					(char *)add_slashes(mSocial.get_string(64)));

			Query.format(" WHERE owner = %d", mOwner);

			mStoreFlag.clear();

			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM tech WHERE owner = %d", mOwner);
			break;

		default :
			break;
	}

	return Query;
}

