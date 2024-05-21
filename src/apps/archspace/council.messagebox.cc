#include "message.h"
#include "define.h"
#include "archspace.h"

CCouncilMessageBox::CCouncilMessageBox():
		CSortedList(10, 10)
{
}

CCouncilMessageBox::~CCouncilMessageBox()
{
	remove_all();
}

int
CCouncilMessageBox::compare(TSomething aItem1, TSomething aItem2) const
{
	CCouncilMessage
		*Message1 = (CCouncilMessage*)aItem1,
		*Message2 = (CCouncilMessage*)aItem2;

	if (Message1->get_id() > Message2->get_id()) return 1;
	if (Message1->get_id() < Message2->get_id()) return -1;
	return 0;
}

int 
CCouncilMessageBox::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CCouncilMessage
		*Message = (CCouncilMessage*)aItem;

	if (Message->get_id() > (int)aKey) return 1;
	if (Message->get_id() < (int)aKey) return -1;

	return 0;
}

bool 
CCouncilMessageBox::add_council_message(
		CCouncilMessage *aMessage)
{
	if (find_sorted_key((TSomething)aMessage->get_id()) >= 0)
		return false;

	insert_sorted(aMessage);

	return true;
}

bool
CCouncilMessageBox::remove_council_message(int aMessageID)
{
	int Index = find_sorted_key((TSomething)aMessageID);

	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CCouncilMessage *
CCouncilMessageBox::get_by_id(int aMessageID)
{
	int Index = find_sorted_key((TSomething)aMessageID);

	if (Index < 0) return NULL;

	return (CCouncilMessage*)get(Index);
}

bool
CCouncilMessageBox::free_item(TSomething aItem)
{
	CCouncilMessage *Message = (CCouncilMessage*)aItem;

	assert(Message);

	delete Message;

	return true;
}

int
CCouncilMessageBox::count_unread_message()
{
	if (!length()) return 0;

	int Count = 0;

	for(int i=0; i<length(); i++)
	{
		CCouncilMessage *
			Message = (CCouncilMessage *)get(i);

		if(Message->get_status() == CCouncilMessage::STATUS_UNREAD) Count++;
	}

	return Count;
}
