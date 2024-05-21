#if !defined(__ARCHSPACE_RELAY_PROTOCOL_H__)
#define __ARCHSPACE_RELAY_PROTOCOL_H__

/*
	Relay -> Game			0xA000
	Game -> Relay			0xB000
	Inner Comm				0xC000
*/

#define INTER_PROTOCOL_TIMEOUT			20

#define MT_GAME_SERVER_INFO_REQ			0xC001
#define MT_GAME_SERVER_INFO_ACK			0xC002

#define MT_CREATE_NEW_PLAYER_REQ		0xA001
#define MT_CREATE_NEW_PLAYER_ACK		0xA002

#define MT_COMMIT_SUICIDE_REQ			0xB001
#define MT_COMMIT_SUICIDE_ACK			0xB002

#include "util.h"

class CAcknowlegde
{
	public:
		CAcknowlegde() { mPortalID = -1; }
		virtual ~CAcknowlegde() {}

		int get_portal_id() { return mPortalID; }

		virtual int type() { return -1; }
	protected:
		int
			mPortalID;
};

class CCreateNewPlayerAck: public CAcknowlegde
{
	public:
		CCreateNewPlayerAck():CAcknowlegde()
		{
			mName = "";
			mGameID = -1;
			mRace = -1;
			mServerID = -1;
		}
		virtual ~CCreateNewPlayerAck() {}

		virtual int type() { return MT_CREATE_NEW_PLAYER_ACK; } 

		void initialize(int aPortalID, const char *aName, 
				int aGameID, int aRace, int aServerID)
		{
			mPortalID = aPortalID;
			mName = aName;
			mGameID = aGameID;
			mRace = aRace;
			mServerID = aServerID;
		}

		const char *get_name() { return (char*)mName; }
		int get_game_id() { return mGameID; }
		int get_race() { return mRace; }
		int get_server() { return mServerID; }
	protected:
		CString
			mName;
		int 
			mGameID;
		int
			mRace;
		int 
			mServerID;
};

#endif
