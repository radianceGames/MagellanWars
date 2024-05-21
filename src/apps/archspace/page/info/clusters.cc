#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageClusters::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	ITEM("SELECT_CLUSTER_MESSAGE",
			GETTEXT("Select from the list to see the councils belonging to that cluster."));

	ITEM("CLUSTER_LIST", UNIVERSE->info_cluster_list_html());

//	system_log("end page handler %s", get_name());

	return output( "info/clusters.html" );
}

