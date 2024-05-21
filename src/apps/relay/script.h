#if !defined(__ARCHSPACE_RELAY_SCRIPT_H__)
#define __ARCHSPACE_RELAY_SCRIPT_H__

#include "util.h"
#include "cgi.h"
#include "data.h"

class CRaceScript: public CScript
{
	public:
		virtual ~CRaceScript() {}

		bool get(CRaceList &aRaceList);	
};

#endif
