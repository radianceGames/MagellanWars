#include "message.h"
#include "define.h"
#include "archspace.h"

CDiplomaticMessageBox::CDiplomaticMessageBox():
		CSortedList(10, 10)
{
}

CDiplomaticMessageBox::~CDiplomaticMessageBox()
{
	remove_all();
}

int
CDiplomaticMessageBox::compare(TSomething aItem1, TSomething aItem2) const
{
	CDiplomaticMessage
		*Message1 = (CDiplomaticMessage*)aItem1,
		*Message2 = (CDiplomaticMessage*)aItem2;

	if (Message1->get_id() > Message2->get_id()) return 1;
	if (Message1->get_id() < Message2->get_id()) return -1;
	return 0;
}

int 
CDiplomaticMessageBox::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CDiplomaticMessage
		*Message = (CDiplomaticMessage*)aItem;

	if (Message->get_id() > (int)aKey) return 1;
	if (Message->get_id() < (int)aKey) return -1;

	return 0;
}

bool 
CDiplomaticMessageBox::add_diplomatic_message(
		CDiplomaticMessage *aMessage)
{
	if (find_sorted_key((TSomething)aMessage->get_id()) >= 0)
		return false;

	insert_sorted(aMessage);

	return true;
}

bool
CDiplomaticMessageBox::remove_diplomatic_message(int aMessageID)
{
	int Index = find_sorted_key((TSomething)aMessageID);

	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CDiplomaticMessage *
CDiplomaticMessageBox::get_by_id(int aMessageID)
{
	int Index = find_sorted_key((TSomething)aMessageID);

	if (Index < 0) return NULL;

	return (CDiplomaticMessage *)get(Index);
}

bool
CDiplomaticMessageBox::free_item(TSomething aItem)
{
	CDiplomaticMessage *Message = (CDiplomaticMessage*)aItem;

	assert(Message);

	delete Message;

	return true;
}

int
CDiplomaticMessageBox::count_unread_message()
{
	if (!length()) return 0;

	int
		Count = 0;
	for(int i=0 ; i<length() ; i++)
	{
		CDiplomaticMessage *
			Message = (CDiplomaticMessage *)get(i);

		if (Message->get_status() == CDiplomaticMessage::STATUS_UNREAD) Count++;
	}

	return Count;
}

