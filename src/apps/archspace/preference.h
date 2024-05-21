#ifndef __ARCHSPACE_PREFERENCE_H__
#define __ARCHSPACE_PREFERENCE_H__

extern TZone gPreferenceZone;

class CPreference: public CStore, public CCommandSet
{
public:
	enum EPreference
	{
		PR_JAVA = 0,
		PR_JAVASCRIPT,
		PR_ACCEPT_TRUCE,
		PR_ACCEPT_PACT,
		PR_ACCEPT_ALLY,
		PR_END 
	};

	enum ECommanderStat
	{
		LEVEL = 0,
		EXP,
		FLEET_COMMANDING,
		EFFICIENCY,
		// attack abilities
		//SIEGE_PLANET,
		//BLOCKADE,
		//RAID,
		//PRIVATEER,
		// defense abilities
		//SIEGE_REPELLING,
		//BREAK_BLOCKADE,
		//PREVENT_RAID,
		OFFENSE,
		DEFENSE,
		// effectiveness abilities
		MANEUVER,
		DETECTION,
		//INTERPRETATION,
		ARMADA_CLASS,
		ABILITY,
		CS_END
	};

public:
	CPreference(int aGameID);
	CPreference(MYSQL_ROW aRow);
	virtual ~CPreference();

	virtual const char *table() { return "player_pref"; }
	virtual CString& query();

public:
	int mPlayerID;
	unsigned int mCommanderView;

public:
	char *handle(int aPreference);
	char *handle(EPreference aPreference);      
	void setPlayerID(int aID);
	int  getPlayerID() { return mPlayerID; }
	const char *HTMLTable();
	const char *HTMLSelectOptions();
	char *EPreferencetoString(EPreference aPreference);
	bool hasCommanderStat(unsigned int aCommanderStat);
	void setCommanderView(unsigned int aCommanderView);
	const char *commander_view_html();

protected:
	enum EStoreFlag
	{
		STORE_PLAYER_ID = 0,
		STORE_JAVA_CHOICE,
		STORE_ACCEPT_TRUCE,
		STORE_ACCEPT_PACT,
		STORE_ACCEPT_ALLY,
		STORE_COMMANDER_VIEW
	};      
	RECYCLE(gPreferenceZone);
};
#endif
