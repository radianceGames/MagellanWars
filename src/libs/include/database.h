#if !defined(__ARCHSPACE_DATABASE_H__)
#define __ARCHSPACE_DATABASE_H__

#include "frame.h"
#include "net.h"

extern TZone gDatabaseZone;

#define PUSH_FORCE_INTERVAL		180

/**
*/
class CDatabase: public CApplication, public CFIFO, 
		public CSortedList, protected CMySQL
{
	public:
		CDatabase();
		virtual ~CDatabase();

		int run();
	protected:

		CCollection 
			mInput;

		static bool
			mServerLoop;

		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;

		virtual void set_signalset();
		virtual void set_signalfunction();
		static void signal_stop(int aSignalNumber);

		virtual int read();
		virtual int write();

		bool analysis();

		int write_SQL();

	RECYCLE(gDatabaseZone);
};

#endif
