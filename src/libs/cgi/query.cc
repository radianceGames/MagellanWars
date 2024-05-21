#include "common.h"
#include "util.h"
#include "cgi.h"

TZone gQueryZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CQuery),
	0,
	0,
	NULL,
	"Zone CQuery",
};

CQuery::CQuery()
{
	mName.clear();
	mValue.clear();
}

CQuery::~CQuery()
{
}

void
CQuery::set(const char *aName, const char *aValue)
{
	mName.clear();
	mValue.clear();
	mName.append(aName);
	mValue.append(aValue);
}

void 
CQuery::set_name(const char *aName)
{
	mName.clear();
	mName.append(aName);
}

void
CQuery::set_value(const char *aValue)
{
	mValue.clear();
	mValue.append(aValue);
}

void
CQuery::unescape_value()
{
	mValue.unescape();
}
