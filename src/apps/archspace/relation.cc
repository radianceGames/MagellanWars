#include <libintl.h>
#include "relation.h"
#include "define.h"


CRelation::CRelation()
{
	mID = 0;
	mRelation = RELATION_NONE;
	mTime = 0;
}

const char *
CRelation::get_relation_description()
{
	return get_relation_description(mRelation);
}

const char *
CRelation::get_relation_description(CRelation::ERelation aRelation)
{
	switch(aRelation)
	{
		case RELATION_NONE:
				return GETTEXT("None");
		case RELATION_SUBORDINARY:
				return GETTEXT("Subordinary");
		case RELATION_ALLY:
				return GETTEXT("Alliance");
		case RELATION_PEACE:
				return GETTEXT("Peace");
		case RELATION_TRUCE:
				return GETTEXT("Truce");
		case RELATION_WAR:
				return GETTEXT("War");
		case RELATION_TOTAL_WAR:
				return GETTEXT("Total War");
		case RELATION_BOUNTY:
				return GETTEXT("Bounty");
		case RELATION_HOSTILE:
				return "Hostile";
	}
	return GETTEXT("None");
}
