#include "common.h"
#include "util.h"
#include "database.h"
#include "store.h"
#include <string>
using namespace std;

TZone gSQLZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CSQL),
	0,
	0,
	NULL,   
	"Zone CSQL",
};

CSQL::CSQL()
{
    Next = NULL;
    mQuery = "";
}

CSQL::~CSQL()
{
}

void 
CSQL::set_query(const char *aQuery)
{
	mQuery = aQuery;
}
