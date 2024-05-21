#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageClustersCouncil::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("CLUSTER_ID", ClusterIDString);
	int
		ClusterID = as_atoi(ClusterIDString);
	CCluster *
		Cluster = UNIVERSE->get_by_id(ClusterID);
	if (!Cluster)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("That cluster doesn't exist."));
		return output("info/council_search_error.html");
	}

	if (Cluster->get_council_count() == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("That cluster doesn't have any councils."));
		return output("info/council_search_error.html");
	}

	ITEM("COUNCIL_LIST_INFO",
			(char *)format(GETTEXT("Councils that belong to the cluster %1$s"),
							Cluster->get_nick()));

	ITEM("SELECT_COUNCIL_MESSAGE",
			GETTEXT("Select from the list to see the detailed council information."));

	ITEM("COUNCIL_LIST", Cluster->info_council_list_html());

//	system_log("end page handler %s", get_name());

	return output("info/clusters_council.html");
}

