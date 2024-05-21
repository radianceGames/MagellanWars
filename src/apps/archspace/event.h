#if !defined(__ARCHSPACE_EVENT_H__)
#define __ARCHSPACE_EVENT_H__

#include "effect.h"

class CCluster;

extern TZone gPlayerEventZone;
extern TZone gEventInstanceZone;
extern TZone gEventEffectZone;

class CEventEffect : public CPlayerEffect
{
	public :
		CEventEffect();
		virtual ~CEventEffect();

		enum {
			EE_ANSWER_YES = 1,
			EE_ANSWER_NO,
			EE_CHANCE,
			EE_DIFFERENT_DURATION,
			EE_BEGIN_ONLY
		};

	private :
		CCommandSet
			mAttribute;
		int
			mDuration,
			mChance;

	public :
		void set_chance( int aChance );
		int get_chance();

		bool has_attribute( int aAttr ) { return mAttribute.has(aAttr); }
		void add_attribute( int aAttr ) { mAttribute += aAttr; }

		void set_duration( int aDuration ) { mDuration = aDuration; mAttribute += EE_DIFFERENT_DURATION; }
		int get_duration() { return mDuration; }

		RECYCLE(gEventEffectZone);
};

class CEventEffectList : public CList
{
	public :
		CEventEffectList() {} ;
		virtual ~CEventEffectList() {} ;
	protected :
		virtual bool free_item(TSomething aItem);
		virtual const char *debug_info() const { return "event effect list"; }
};

class CEvent
{
	public :
		enum {
			EVENT_SYSTEM = 1,
			EVENT_COUNCIL,
			EVENT_CLUSTER,
			EVENT_MAGISTRATE,
			EVENT_EMPIRE,
			EVENT_GALACTIC,
			EVENT_ANSWER,
			EVENT_RACIAL,
			EVENT_MAJOR,
			EVENT_HUMAN_ONLY,
			EVENT_TARGOID_ONLY,
			EVENT_BUCKANEER_ONLY,
			EVENT_TECANOID_ONLY,
			EVENT_EVINTOS_ONLY,
			EVENT_AGERUS_ONLY,
			EVENT_BOSALIAN_ONLY,
			EVENT_XELOSS_ONLY,
			EVENT_XERUSIAN_ONLY,
			EVENT_XESPERADOS_ONLY,
			EVENT_IMMUNE_TO_HUMAN,
			EVENT_IMMUNE_TO_TARGOID,
			EVENT_IMMUNE_TO_BUCKANEER,
			EVENT_IMMUNE_TO_TECANOID,
			EVENT_IMMUNE_TO_EVINTOS,
			EVENT_IMMUNE_TO_AGERUS,
			EVENT_IMMUNE_TO_BOSALIAN,
			EVENT_IMMUNE_TO_XELOSS,
			EVENT_IMMUNE_TO_XERUSIAN,
			EVENT_IMMUNE_TO_XESPERADOS,
			EVENT_TYPE_MAX
		};

	private:
		int
			mID;
		CString
			mName,
			mDescription,
			mImage;
		CCommandSet
			mType;
		int
			mDurationMin,
			mDurationMax;
		int mHonorMin,
            mHonorMax;
        int mTurnMin,
            mTurnMax;
		CEventEffectList
			mEffectList;

		static char *mTypeName[];
	public:
		CEvent();
		~CEvent();

		int get_id() { return mID; }
		void set_id(int aID) { mID = aID; }

		const char *get_name() { return (char*)mName; }
		void set_name( char *aName ) { mName = aName; }

		const char *get_description() { return (char*)mDescription; }
		void set_description(char *aDesc) { mDescription = aDesc; }

		const char *get_image() { return (char*)mImage; }
		void set_image(char *aImage) { mImage = aImage; }

		bool has_type(int aType) { return mType.has(aType); }
		void set_type(int aType) { mType += aType; }
		bool set_type(char *aType);
		int get_type(char *aType);

		void set_duration(int aDuration) { mDurationMin = mDurationMax = aDuration; }
		void set_duration_min(int aDuration) { mDurationMin = aDuration; }
		void set_duration_max(int aDuration) { mDurationMax = aDuration; }
		int get_duration();
		int get_duration_min() { return mDurationMin; }
		int get_duration_max() { return mDurationMax; }

        void set_honor_min(int aHonorMin) { mHonorMin = aHonorMin; }
        void set_honor_max(int aHonorMax) { mHonorMax = aHonorMax; }
        void set_turn_min(int aTurnMin) { mTurnMin = aTurnMin; }
        void set_turn_max(int aTurnMax) { mTurnMax = aTurnMax; }
        int  get_honor_min() { return mHonorMin; }
        int  get_honor_max() { return mHonorMax; }
        int  get_turn_min() { return mTurnMin; }
        int  get_turn_max() { return mTurnMax; }
        bool  honor_in_range(int aHonor);
        bool  turn_in_range();

		void add_effect(CEventEffect *aEffect) { mEffectList.push(aEffect); }

		CEventEffectList *get_effect_list() { return &mEffectList; }

		bool possible_event(CPlayer *aPlayer);

		const char *get_type_str();

		RECYCLE(gPlayerEventZone);
};

class CEventTable: public CSortedList
{
	public:
		CEventTable();
		virtual ~CEventTable();

		int add_event(CEvent *aEvent);

		CEvent *get_by_id(int aID);

		CEvent *get_random_by_type( int aType );
		CEvent *get_random_by_honor_and_type (int aHonor, int aType);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;

		virtual const char *debug_info() const { return "event table"; }
};

class CEventInstance: public CStore
{
	public :
		enum {
			STORE_ID,
			STORE_OWNER,
			STORE_EVENT,
			STORE_LIFE,
			STORE_TIME,
			STORE_ANSWERED
		};

	private :
		int
			mID,
			mOwner;

		CEvent
			*mEvent;

		bool
			mAnswered;

		time_t
			mLife,
			mTime;
	public :
		CEventInstance();
		CEventInstance( CEvent *aEvent, MYSQL_ROW aRow);
		CEventInstance( CEvent *aEvent, int aTime, int aLife = -1 );
		virtual ~CEventInstance();

		virtual const char *table() { return "player_event"; }
		virtual CString &query();

		int get_id() { return mID; }
		bool set_id(int aID);

		int get_owner() { return mOwner; }
		bool set_owner(int aOwner);

		int get_event_id() { return mEvent->get_id(); }

		int get_time() { return mTime; }
		int get_life() { return mLife; }

		bool has_type( int aType ) { return mEvent->has_type(aType); }

		const char *get_name() { return mEvent->get_name(); }
		const char *get_description() { return mEvent->get_description(); }
		const char *get_type_str() { return mEvent->get_type_str(); }
		const char *get_image() { return mEvent->get_image(); }

		void save_new_event();

		bool set_answered();
		bool is_answered() { return mAnswered; }

		const char *get_time_str();
		const char *html( char *aType );

		CEvent *get_event() { return mEvent; }

		RECYCLE(gEventInstanceZone);
};

class CEventInstanceList: public CSortedList
{
	public:
		CEventInstanceList();
		virtual ~CEventInstanceList();

		int add_event_instance(CEventInstance *aEvent);
		bool remove_event_instance(CEventInstance *aEvent);
		bool remove_event_instance(int aID);

		int get_new_id();
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "event instance list"; }
	public:

		void save_galactic_event();
		void load_galactic_event();

		void save_cluster_event(CCluster *aCluster);
		void load_cluster_event(CCluster *aCluster);

		void expire_out_due_event();

		const char *html( char *aType );

		CEventInstance *get_by_id(int aID);
};

#endif
