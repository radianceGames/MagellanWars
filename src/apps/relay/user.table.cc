#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "data.h"

CUserTable::CPortalIndex::CPortalIndex(): CSortedList(512, 512)
{
}

CUserTable::CPortalIndex::~CPortalIndex()
{
	remove_all();
}

int
CUserTable::CPortalIndex::compare(TSomething aItem1, TSomething aItem2,
									int aMode) const
{
	(void)aMode;
	CUser
		*User1 = (CUser*)aItem1,
		*User2 = (CUser*)aItem2;
	
	if (User1->get_portal_id() > User2->get_portal_id()) return 1;
	if (User1->get_portal_id() < User2->get_portal_id()) return -1;
	return 0;
}

int
CUserTable::CPortalIndex::compare_key(TSomething aItem, 
				TConstSomething aKey, int aMode) const
{
	(void)aMode;
	CUser
		*User = (CUser*)aItem;
	
	if (User->get_portal_id() > (int)aKey) return 1;
	if (User->get_portal_id() < (int)aKey) return -1;
	return 0;
}


CUserTable::CNameIndex::CNameIndex(): CSortedList(512, 512)
{
}

CUserTable::CNameIndex::~CNameIndex()
{
	remove_all();
}

int
CUserTable::CNameIndex::compare(TSomething aItem1, TSomething aItem2,
									int aMode) const
{
	(void)aMode;
	CUser
		*User1 = (CUser*)aItem1,
		*User2 = (CUser*)aItem2;
	
	return strcmp(User1->get_name(), User2->get_name());
}

int
CUserTable::CNameIndex::compare_key(TSomething aItem, 
				TConstSomething aKey, int aMode) const
{
	(void)aMode;
	CUser
		*User = (CUser*)aItem;

	return strcmp(User->get_name(), (const char*)aKey);
}



CUserTable::CUserTable():CList(1024, 1024)
{
	mMaxGameID = 0;
}

CUserTable::~CUserTable()
{
	remove_all();
}

bool
CUserTable::free_item(TSomething aItem)
{
	CUser
		*User = (CUser*)aItem;

	if (User) delete User;

	return true;
}

CUser*
CUserTable::get_by_portal_id(int aPortalID)
{
	int 
		Index = aPortalID % MAX_HASHLIST;

	int 
		Pos = mPortalIndex[Index].find_sorted_key(
								(TSomething)aPortalID);
	if (Pos < 0) return NULL;

	return (CUser*)mPortalIndex[Index].get(Pos);
}

CUser*
CUserTable::get_by_name(const char *aName)
{
	int 
		Index = 0;
	
	unsigned char
		*Name = (unsigned char*)aName;

	for(int i=0; i<(int)strlen(aName); i++)
		Index += Name[i];

	CNameIndex
		*NameIndex = &mNameIndex[Index%MAX_HASHLIST];

	system_log("Check User %s[%d]", aName, Index%MAX_HASHLIST);
	for(int i=0; i<NameIndex->length(); i++)
	{
		CUser
			*User = (CUser*)NameIndex->get(i);
		system_log("[%d. %s]", i, User->get_name());
	}

	int 
		Pos = NameIndex->find_sorted_key((TSomething)aName);

	if (Pos < 0) return NULL;

	return (CUser*)NameIndex->get(Pos);
}

bool 
CUserTable::add(CUser& aUser)
{
	int
		GameID = aUser.get_game_id();
	int 
		PortalID = aUser.get_portal_id();

	if (GameID < length())
	{
		CUser
			*User = (CUser*)get(GameID);
		if (User)
			system_log("GameID-PortalID(%d-%d) is exists", 
					User->get_game_id(), User->get_portal_id()); 
		return false;
	}

	operator[](GameID) = &aUser;
	mPortalIndex[PortalID%MAX_HASHLIST].insert_sorted(&aUser);		

	int
		Index = 0;

	unsigned char 
		*Name = (unsigned char*)aUser.get_name();

	for(int i=0; i<(int)strlen((const char*)Name); i++)
		Index += Name[i];

	mNameIndex[Index%MAX_HASHLIST].insert_sorted(&aUser);
	
	return true;
}

bool 
CUserTable::remove(int aGameID)
{
	if (aGameID >= length()) 
		return false;

	CUser
		*User = (CUser*)get(aGameID);
	if (!User) return false;

	replace(aGameID, NULL);

	mUnusedIDList.insert_sorted((TSomething)aGameID);

	CPortalIndex
		*PortalIndex = &mPortalIndex[aGameID%MAX_HASHLIST];

	int 
		Pos = PortalIndex->find_sorted_key(
				(TSomething)User->get_portal_id());
	if (Pos >= 0)	
	{
		PortalIndex->remove(Pos);
	} else {
		system_log("could not delete portal index item GameID(%d)", 
				aGameID);
		return false;
	}

	int
		Index = 0;

	unsigned char 
		*Name = (unsigned char*)User->get_name();

	for(int i=0; i<(int)strlen((const char*)Name); i++)
		Index += Name[i];

	CNameIndex
		*NameIndex = &mNameIndex[Index%MAX_HASHLIST];
	
	Pos = NameIndex->find_sorted_key((TSomething)Name);

	if (Pos >= 0)
	{
		NameIndex->remove(Pos);
	} else {
		system_log("could not delete name index item GameID(%d)",
				aGameID);
		return false;
	}
	
	free_item(User);

	return true;
}

CUserTable&
CUserTable::operator+=(CUser& aUser)
{
	add(aUser);
	return *this;
}

CUserTable&
CUserTable::operator+=(CUser* aUser)
{
	if (aUser)
		add(*aUser);
	return *this;
}

CUserTable&
CUserTable::operator-=(CUser& aUser)
{
	remove(aUser.get_game_id());
	return *this;
}

CUserTable&
CUserTable::operator-=(CUser* aUser)
{
	if (aUser) 
		remove(aUser->get_game_id());
	return *this;
}

CUserTable&
CUserTable::operator-=(int aGameID)
{
	remove(aGameID);
	return *this;
}

int
CUserTable::get_unused_game_id()
{
	int 
		Ret;

	if (mUnusedIDList.length() > 0)
	{
		Ret = (int)mUnusedIDList.get(0);
		mUnusedIDList.remove(0);
	} else 
		Ret = ++mMaxGameID;

	return Ret;
}

bool
CUserTable::load(CMySQL &aMySQL)
{
	system_log("User loading");

	int MaxID = 0;
	int Count = 0;
	int Prev = 0;

	aMySQL.query("LOCK TABLE User READ");
	aMySQL.query("SELECT GameID, PortalID, Name, Race,"
				" CreateTime, LastLogin, Server FROM User");

	aMySQL.use_result();
	while(aMySQL.fetch_row())
	{
		if (!Count++) 
			Prev = atoi(aMySQL.row()[0]);
		int 
			GameID = atoi(aMySQL.row()[0]);
		for(int i=Prev+1; GameID-i>0; i++)
		{
			system_log("Unused id = %d", i);
			mUnusedIDList.insert_sorted((TSomething)i);
		}

		if (MaxID < GameID) 
			MaxID = GameID;

		Prev = GameID;

		CUser
			*User = new CUser(aMySQL.row());
		add(*User);

		system_log("%d. %s", GameID, aMySQL.row()[2]);
	}
	aMySQL.free_result();

	aMySQL.query("UNLOCK TABLES");

	system_log("%d users are loaded", Count);

	if (mMaxGameID < MaxID) mMaxGameID = MaxID;

	system_log("MaxGameID = %d", mMaxGameID);

	return true;
}
