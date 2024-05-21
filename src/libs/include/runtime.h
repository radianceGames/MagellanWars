#if !defined(__ARCHSPACE_LIB_RUNTIME_H__)
#define __ARCHSPACE_LIB_RUNTIME_H__

#include "common.h"
#include "util.h"

/**
*/
class CTrigger: public CBase
{
	public:
		CTrigger();
		virtual ~CTrigger();

		virtual bool shoot();
		virtual const char *name()=0;

		virtual bool handler() = 0;
	protected:
		virtual time_t interval() = 0;

		time_t mLastAction;	
};

/**
*/
class CTriggerStation: public CList
{
	public:
		CTriggerStation();
		virtual ~CTriggerStation();

		bool initialize();

		void fire();

		void fire( const char *aTriggerName );

		virtual const char *debug_info() { return "trigger station"; }
	protected:
		virtual bool free_item(TSomething aItem);

		virtual bool registration() = 0;
};

#endif
