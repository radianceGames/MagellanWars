#include "project.h"

CPurchasedProjectList::CPurchasedProjectList()
{
}

CPurchasedProjectList::~CPurchasedProjectList()
{
	remove_all();
}

bool
CPurchasedProjectList::free_item(TSomething aItem)
{
	CPurchasedProject
		*Project = (CPurchasedProject *)aItem;

	if( Project == NULL ) return false;

	delete Project;

	return true;
}

int
CPurchasedProjectList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPurchasedProject
		*KnownProject1 = (CPurchasedProject*)aItem1,
		*KnownProject2 = (CPurchasedProject*)aItem2;

	if (KnownProject1->get_id() > KnownProject2->get_id()) return 1;
	if (KnownProject1->get_id() < KnownProject2->get_id()) return -1;
	return 0;
}

int
CPurchasedProjectList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPurchasedProject
		*KnownProject = (CPurchasedProject*)aItem;

	if (KnownProject->get_id() > (int)aKey) return 1;
	if (KnownProject->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CPurchasedProjectList::remove_project(int aProjectID)
{
	int 
		Index = find_sorted_key((void*)aProjectID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

int 
CPurchasedProjectList::add_project(CPurchasedProject* aProject)
{
	if (!aProject) return -1;

	if (find_sorted_key((TConstSomething)aProject->get_id()) >= 0) 
		return -1;
	insert_sorted(aProject);

	return aProject->get_id();
}

CPurchasedProject*
CPurchasedProjectList::get_by_id(int aProjectID)
{
	if (!length()) return NULL;

	int 
		Index;

	Index = find_sorted_key((void*)aProjectID);

	if (Index < 0) return NULL;

	return (CPurchasedProject*)get(Index);
}

