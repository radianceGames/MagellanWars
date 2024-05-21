#include "universe.h"

CClusterList::CClusterList()
{
}

CClusterList::~CClusterList()
{
	remove_all();
}

bool
CClusterList::free_item(void *aItem)
{
	(void)aItem;

	return true;
}

CCluster *
CClusterList::get_by_id(int aID)
{
	int
		Index = find_sorted_key((TConstSomething)aID);
	if (Index < 0) return NULL;
	return (CCluster*)get(Index);
}

int
CClusterList::compare(TSomething aItem1, TSomething aItem2) const
{
	CCluster
		*Cluster1 = (CCluster*)aItem1,
		*Cluster2 = (CCluster*)aItem2;

	if (Cluster1->get_id() > Cluster2->get_id()) return 1;
	if (Cluster1->get_id() < Cluster2->get_id()) return -1;

	return 0;
}

int
CClusterList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CCluster
		*Cluster = (CCluster*)aItem;

	if (Cluster->get_id() > (int)aKey) return 1;
	if (Cluster->get_id() < (int)aKey) return -1;

	return 0;
}

int
CClusterList::add_cluster(CCluster *aCluster)
{
	assert(aCluster);

	int
		Index = find_sorted_key((TConstSomething)aCluster->get_id());
	if (Index >= 0) return -1;

	insert_sorted(aCluster);
	return aCluster->get_id();
}

bool 
CClusterList::remove_cluster(int aCluster)
{
	int
		Index = find_sorted_key((TConstSomething)aCluster);

	if (Index < 0) return false;

	return CSortedList::remove(Index);
}
