#if !defined(__ARCHSPACE_TRIGGERS_H__)
#define __ARCHSPACE_TRIGGERS_H__

#include "runtime.h"
#include "game.h"

#define INTERVAL_GET_KEY			60*15		// second
#define INTERVAL_DEBUG				60		// second
#define INTERVAL_PUSH				5		// second
#define INTERVAL_COUNCIL			96		// turns
#define INTERVAL_OPTIMIZE			24*60*60	// second
#define INTERVAL_ACTION				60		// second
#define INTERVAL_RELATION_TIMEOUT		60*60		// second
#define INTERVAL_RANK				60*60		// second
//#define INTERVAL_EVENT          30 // turns
//#define INTERVAL_CRONTAB		60
#define INTERVAL_CRONTAB        15
#define INTERVAL_EMPIRE_COUNTERATTACK_MAGISTRATE		30*60	// second
#define INTERVAL_EMPIRE_COUNTERATTACK_EMPIRE_PLANET		30*60	// second
#define INTERVAL_EMPIRE_DB_SAVE                      5*60
/**
*/
class CTriggerGetKey: public CTrigger
{
	public:
		virtual ~CTriggerGetKey() {}

		virtual const char *name() { return "get key"; }
	protected:
		virtual time_t interval() { return INTERVAL_GET_KEY; }
		virtual bool handler();
};


/**
*/
class CTriggerDebug: public CTrigger
{
	public:
		virtual ~CTriggerDebug() {}

		virtual const char *name() { return "debug"; }
	protected:
		virtual time_t interval() { return INTERVAL_DEBUG; }
		virtual bool handler();

};

/**
*/
class CTriggerAction: public CTrigger
{
	public:
		virtual ~CTriggerAction() {}

		virtual const char *name() { return "action"; }
	protected:
		virtual time_t interval() { return INTERVAL_ACTION; }
		virtual bool handler();
};

/**
*/
class CTriggerOptimize: public CTrigger
{
	public:
		virtual ~CTriggerOptimize() {}

		virtual const char *name() { return "optimize"; }
	protected:
		// test 60 sec
		// original 24 hour
		virtual time_t interval() { return INTERVAL_OPTIMIZE; }
		virtual bool handler();
};

/**
*/
class CTriggerRelationTimeout: public CTrigger
{
	public:
		virtual ~CTriggerRelationTimeout() {}

		virtual const char *name() { return "relation timeout"; }
	protected:
		// test 60 sec
		// original 1 hour
		virtual time_t interval() { return INTERVAL_RELATION_TIMEOUT; }
		virtual bool handler();
};

class CTriggerRank: public CTrigger
{
	public:
		virtual ~CTriggerRank() {}

		virtual const char *name() { return "rank"; }
	protected:
		virtual time_t interval() { return INTERVAL_RANK; }
		virtual bool handler();
};

// start telecard 2001/02/15
#define INTERVAL_BLACKMARKET_EXPIRE	60	// 1 minute

class CTriggerBlackMarketItem: public CTrigger
{
	public:
		virtual ~CTriggerBlackMarketItem() {}
		virtual const char *name() { return "blackmarket_item"; }
	protected:
		virtual time_t interval() { return CBlackMarket::mBlackMarketItemRegen; }
		virtual bool handler();
};

class CTriggerBlackMarketExpire: public CTrigger
{
	public:
		virtual ~CTriggerBlackMarketExpire() {}
		virtual const char *name() { return "blackmarket_expire"; }
	protected:
		virtual time_t interval() { return INTERVAL_BLACKMARKET_EXPIRE; }
		virtual bool handler();
};

// end telecard 2001/02/15

class CCronTab
{
	public:
		CCronTab();
		virtual ~CCronTab();

		time_t get_last_activated_time() { return mLastActivatedTime; }
		virtual int get_run_second() { return 0; }
		virtual int get_run_minute() { return 0; }
		virtual int get_run_hour() { return 0; }
		virtual int get_run_day() { return 0; }
		virtual int get_run_week() { return 0; }

		virtual const char *name()  = 0;

		void run();
		void load_last_activated_time();

	private:
		virtual void handler() = 0;

		time_t
			mLastActivatedTime;
};

/**
*/
class CCronTabDatabasePush: public CCronTab
{
	public:
		virtual ~CCronTabDatabasePush() {}

		virtual const char *name() { return "database push"; }
	protected:
		virtual int get_run_second() { return INTERVAL_PUSH; }
		virtual void handler();
};

class CCronTabEvent: public CCronTab
{
	public:
		virtual ~CCronTabEvent() {};

		virtual const char *name(){ return "event"; }

		//virtual int get_run_day() { return 1; }
		virtual int get_run_second() { return 24 * CGame::mSecondPerTurn; }
	private:
		virtual void handler();
};

class CCronTabCommerce: public CCronTab
{
	public:
		virtual ~CCronTabCommerce() {};

		virtual const char *name(){ return "commerce"; }

		virtual int get_run_day() { return 1; }
//		virtual int get_run_second() { return 60; }
	private:
		virtual void handler();
};

class CCronTabEmpireDatabaseSave: public CCronTab
{
	public:
		virtual ~CCronTabEmpireDatabaseSave() {};

		virtual const char *name(){ return "empire_db_save"; }

		virtual int get_run_second() { return INTERVAL_EMPIRE_DB_SAVE; }
	private:
		virtual void handler();
};


class CCronTabEmpireFleetRegeneration: public CCronTab
{
	public:
		virtual ~CCronTabEmpireFleetRegeneration() {};

		virtual const char *name(){ return "empire_fleet_regeneration"; }

		virtual int get_run_second()
				{ return CEmpire::mFleetRegenCycleInTurn * CGame::mSecondPerTurn; }
	private:
		virtual void handler();
};

class CCronTabEmpireCounterattackMagistrate: public CCronTab
{
	public:
		virtual ~CCronTabEmpireCounterattackMagistrate() {};

		virtual const char *name(){ return "empire_counterattack_magistrate"; }

		virtual int get_run_second() { return INTERVAL_EMPIRE_COUNTERATTACK_MAGISTRATE; }
	private:
		virtual void handler();
};

class CCronTabEmpireCounterattackEmpirePlanet: public CCronTab
{
	public:
		virtual ~CCronTabEmpireCounterattackEmpirePlanet() {};

		virtual const char *name(){ return "empire_counterattack_empire_planet"; }

		virtual int get_run_second() { return INTERVAL_EMPIRE_COUNTERATTACK_EMPIRE_PLANET; }
	private:
		virtual void handler();
};

class CCronTabEmpireRelation: public CCronTab
{
	public:
		virtual ~CCronTabEmpireRelation() {};

		virtual const char *name(){ return "empire_relation"; }

		virtual int get_run_day() { return 1; }
	private:
		virtual void handler();
};

class CCronTabEmpireDemand: public CCronTab
{
	public:
		virtual ~CCronTabEmpireDemand() {};

		virtual const char *name(){ return "empire_demand"; }

		virtual int get_run_week() { return 1; }
	private:
		virtual void handler();
};

class CCronTabCouncil: public CCronTab
{
	public:
		virtual ~CCronTabCouncil() {};

		virtual const char *name(){ return "council"; }

		virtual int get_run_second()
			{ return (INTERVAL_COUNCIL * CGame::mSecondPerTurn); }
	private:
		virtual void handler();
};

class CCronTabDummy: public CCronTab
{
	public:
		virtual ~CCronTabDummy() {};

		virtual const char *name(){ return "dummy"; }

		virtual int get_run_minute() { return 1; }
	private:
		virtual void handler();
};

class CCronTabNewPlayer : public CCronTab
{
	public :
		virtual ~CCronTabNewPlayer() {};
		virtual const char *name() { return "new_player"; }
		virtual int get_run_second() { return 10; }
	private :
		virtual void handler();
};

class CTriggerRunCronTab: public CTrigger, public CList
{
	public:
		virtual ~CTriggerRunCronTab() {};
		virtual const char *name() { return "run_crontab"; }
	protected:
		virtual time_t interval() { return INTERVAL_CRONTAB; }
		virtual bool handler();
		virtual bool free_item(TSomething aItem);
};
#endif
