#include "universe.h"
#include "define.h"
#include "archspace.h"
#include "event.h"
#include "script.h"

CClusterNameList *
CUniverse::mClusterNameList;

CUniverse::CUniverse()
{
	mMaxClusterID = 0;
}

CUniverse::~CUniverse()
{
	remove_all();
}

bool
CUniverse::free_item(void *aItem)
{
	assert(aItem);

	CCluster 
		*aCluster = (CCluster*)aItem;

	delete aCluster;

	return true;
}

CCluster*
CUniverse::get_by_id(int aID)
{
	int
		Index = find_sorted_key((TConstSomething)aID);
	if (Index < 0) return NULL;
	return (CCluster*)get(Index);
}

int
CUniverse::compare(TSomething aItem1, TSomething aItem2) const
{
	CCluster
		*Cluster1 = (CCluster*)aItem1,
		*Cluster2 = (CCluster*)aItem2;

	if (Cluster1->get_id() > Cluster2->get_id()) return 1;
	if (Cluster1->get_id() < Cluster2->get_id()) return -1;

	return 0;
}

int
CUniverse::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CCluster
		*Cluster = (CCluster*)aItem;

	if (Cluster->get_id() > (int)aKey) return 1;
	if (Cluster->get_id() < (int)aKey) return -1;

	return 0;
}

int 
CUniverse::add_cluster(CCluster *aCluster)
{
	assert(aCluster);

	int
		Index = find_sorted_key((TConstSomething)aCluster->get_id());

	if (Index >= 0) return -1;

	insert_sorted(aCluster);

	if (mMaxClusterID < aCluster->get_id())
		mMaxClusterID = aCluster->get_id();	

	return aCluster->get_id();
}

bool 
CUniverse::remove_cluster(int aID)
{
	int
		Index = find_sorted_key((TConstSomething)aID);

	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

const char* 
CUniverse::new_cluster_name()
{
	static const char *RomanNum[] = 
	{ 
		"I", "II", "III", "IV", "V", 
		"VI", "VII", "VIII", "IX", "X"
	};

	static CString Buffer;

	if (!mClusterNameList) return NULL;

	if (!mClusterNameList->length())
		return NULL;

	while(1)
	{
		Buffer = mClusterNameList->get_random_name();

		int Count = 0;

		for(int i = 0; i<length(); i++)
		{
			CCluster 
				*aCluster = (CCluster*)get(i);
			if (strncmp((char*)Buffer, aCluster->get_name(), 
								Buffer.length()) == 0) Count++;
		}

		if (Count < 10) 
		{
			if (!Count) break;
			Buffer.format(" %s", RomanNum[Count]);
			break;
		}
	}

	return (char*)Buffer;
}

CCluster*
CUniverse::new_cluster()
{
	CCluster *Cluster = new CCluster();

	Cluster->set_id(++mMaxClusterID);
	Cluster->set_name(new_cluster_name());
	Cluster->type(QUERY_INSERT);

	STORE_CENTER->store(*Cluster);

	add_cluster(Cluster);

	return Cluster;
}

bool 
CUniverse::load(CMySQL &aMySQL)
{
	SLOG("cluster loading");	

	//aMySQL.query("LOCK TABLE cluster READ");
	aMySQL.query("SELECT * FROM cluster");

	aMySQL.use_result();
	while (aMySQL.fetch_row())
	{
		CCluster *
			Cluster = new CCluster(aMySQL.row());
		Cluster->get_event_list()->load_cluster_event(Cluster);
		add_cluster(Cluster);
	}

	aMySQL.free_result();
	//aMySQL.query("UNLOCK TABLES");

	SLOG("%d cluster loaded", length());

	return true;
}

bool
CUniverse::load_name(const char *aFilename)
{
	SLOG("cluster name loading");

	CClusterNameScript Script;

	if (!Script.load(aFilename))
	{
		SLOG("Could not read cluster names script filename:%s",
					aFilename);
		return false;
	}

	mClusterNameList = new CClusterNameList();
	
	Script.get(mClusterNameList);

	SLOG("cluster name loaded %d", mClusterNameList->length());

	return true;
}

char *
CUniverse::info_cluster_list_html()
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"510\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";

	int
		Column = 3;

	for (int i=0 ; i<length() ; i++)
	{
		if (Column == 3) List += "<TR>\n";

		CCluster *
			Cluster = (CCluster *)get(i);

		List += "<TD CLASS=\"tabletxt\" WIDTH=\"170\">";
		List += "&nbsp;";
		List.format("<A HREF=\"clusters_council.as?CLUSTER_ID=%d\">%s</A>",
					Cluster->get_id(), Cluster->get_nick());
		List += "</TD>\n";

		if (Column == 3) Column = 1;
		else Column++;

		if (Column == 3) List += "</TR>\n";
	}

	for (int i=Column ; i < 3 ; i++)
	{
		List += "<TD CLASS=\"tabletxt\" WIDTH=\"170\">&nbsp;</TD>\n";
		if (Column == 2) List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

