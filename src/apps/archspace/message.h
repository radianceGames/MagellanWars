#if !defined(__ARCHSPACE_MESSAGE_H__)
#define __ARCHSPACE_MESSAGE_H__

#include "store.h"
#include "relation.h"

class CPlayer;

//------------------------------------------------ CDiplomaticMessage

extern TZone gDiplomaticMessageZone;
/**
*/
class CDiplomaticMessage: public CStore
{
	public:
		enum EType
		{
			TYPE_NORMAL = 0,
			TYPE_SUGGEST_TRUCE,
			TYPE_DEMAND_TRUCE,
			TYPE_DECLARE_WAR,
			TYPE_SUGGEST_PACT,
			TYPE_BREAK_PACT,
			TYPE_SUGGEST_ALLY,
			TYPE_BREAK_ALLY,
			TYPE_REPLY_SUGGEST_TRUCE,
			TYPE_REPLY_DEMAND_TRUCE,
			TYPE_REPLY_SUGGEST_PACT,
			TYPE_REPLY_SUGGEST_ALLY,
			TYPE_REPLY,
			TYPE_DECLARE_HOSTILITY
		};

		enum EStatus
		{
			STATUS_UNREAD = 0,
			STATUS_READ,
			STATUS_ANSWERED,
			STATUS_WELL
		};

	private:
		int mID;

		EType mType;

		int
			mSenderID,
			mReceiverID;
		time_t
			mTime;
		EStatus
			mStatus;
		CString
			mContent;
		bool
			mIsAnswered;

		static int mMaxID;

	public:
		CDiplomaticMessage();
		CDiplomaticMessage(MYSQL_ROW aRow);
		virtual ~CDiplomaticMessage();

		virtual const char *table() { return "diplomatic_message"; }
		virtual CString& query();

		void initialize(EType aType,
				CPlayer *aSender, CPlayer *aReceiver,
				const char *aContent);

	public:
		int get_id();
		CPlayer* get_sender();
		CPlayer* get_receiver();
		time_t get_time();
		char *get_content();
		EType get_type();
		const char* get_time_string();
		bool check_need_required();
		static bool check_need_required(int aMessageType);

		const char* get_type_description();
		static const char* get_type_description(EType aType);
		const char* get_status_description();
		static const char* get_status_description(EStatus aStatus);
		EStatus get_status();

		void set_status(EStatus aStatus);

	public:
		static const char*
				message_select_html(CRelation::ERelation aRelation);
		const char *record_html(bool aIsForAdmin = false);
		const char *get_reply_html();

		void save();
		void erase();
		const char *get_file_name();

	RECYCLE(gDiplomaticMessageZone);
};

//------------------------------------- CDiplomaticMessageReceiveBox
/**
*/
class CDiplomaticMessageBox: public CSortedList
{
	public:
		CDiplomaticMessageBox();
		virtual ~CDiplomaticMessageBox();

		bool add_diplomatic_message(CDiplomaticMessage* aMessage);
		bool remove_diplomatic_message(int aMessageID);

		CDiplomaticMessage *get_by_id(int aMessageID);

		int count_unread_message();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "diplomatic message"; }
};

//------------------------------------------------ CMessage

extern TZone gCouncilMessageZone;
/**
*/
class CCouncilMessage: public CStore
{
	public:
		enum EType
		{
			TYPE_NORMAL = 0,
			TYPE_SUGGEST_TRUCE,
			TYPE_DEMAND_TRUCE,
			TYPE_DEMAND_SUBMISSION,
			TYPE_DECLARE_TOTAL_WAR,
			TYPE_DECLARE_WAR,
			TYPE_SUGGEST_PACT,
			TYPE_BREAK_PACT,
			TYPE_SUGGEST_ALLY,
			TYPE_BREAK_ALLY,
			TYPE_COUNCIL_FUSION_OFFER,
			TYPE_SET_FREE_SUBORDINARY,
			TYPE_DECLARE_INDEPENDENCE,
			TYPE_REPLY_SUGGEST_TRUCE,
			TYPE_REPLY_DEMAND_TRUCE,
			TYPE_REPLY_SUGGEST_PACT,
			TYPE_REPLY_SUGGEST_ALLY,
			TYPE_REPLY_DEMAND_SUBMISSION,
			TYPE_REPLY_COUNCIL_FUSION_OFFER,
			TYPE_REPLY
		};

		enum EStatus
		{
			STATUS_UNREAD = 0,
			STATUS_READ,
			STATUS_ANSWERED,
			STATUS_WELL
		};

	private:
		int mID;

		EType mType;

		int
			mSenderID,
			mReceiverID;
		time_t
			mTime;
		EStatus
			mStatus;
		CString
			mContent;
		bool
			mIsAnswered;

		static int mMaxID;

	public:
		CCouncilMessage();
		CCouncilMessage(MYSQL_ROW aRow);
		virtual ~CCouncilMessage();

		virtual const char *table() { return "council_message"; }
		virtual CString& query();

		void initialize(EType aType,
				CCouncil *aSender, CCouncil *aReceiver,
				const char *aContent);

	public:
		int get_id();
		CCouncil* get_sender();
		CCouncil* get_receiver();
		time_t get_time();
		char *get_content();
		EType get_type();
		const char* get_time_string();
		bool check_need_required();
		static bool check_need_required(int aMessageType);

		const char* get_type_description();
		static const char* get_type_description(EType aType);
		const char* get_status_description();
		static const char* get_status_description(EStatus aStatus);
		EStatus get_status();

		void set_status(EStatus aStatus);

	public:
		static const char*
				message_select_html(CCouncilRelation *aRelation,
									CCouncil*aCouncil);
		const char *record_html(bool aIsForAdmin = false);
		const char *get_reply_html();

		void save();
		void erase();
		const char *get_file_name();

	RECYCLE(gCouncilMessageZone);
};

//------------------------------------- CCouncilMessageReceiveBox
/**
*/
class CCouncilMessageBox: public CSortedList
{
	public:
		CCouncilMessageBox();
		virtual ~CCouncilMessageBox();

		bool add_council_message(CCouncilMessage* aMessage);
		bool remove_council_message(int aMessageID);

		CCouncilMessage *get_by_id(int aMessageID);

		int count_unread_message();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "council message"; }

};



#endif
