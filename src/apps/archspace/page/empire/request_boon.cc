#include "../../pages.h"
#include <libintl.h>
#include "../../archspace.h"

bool
CPageEmpireRequestBoon::handler(CPlayer *aPlayer)
{
	ITEM( "DESCRIPTION", (char*)(*mRequestBoonDescription) );
	ITEM( "EMPIRE_RELATION", aPlayer->get_degree_name_of_empire_relation() );
	ITEM( "COURT_RANK", aPlayer->get_court_rank_name() );

	ITEM("CONFIRM_MESSAGE", GETTEXT("Will you request boon to the empire?"));

	return output("empire/request_boon.html");
}