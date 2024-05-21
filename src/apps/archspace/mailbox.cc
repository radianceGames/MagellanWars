CMailBox::CMailBox():CSortedList(10, 10)
{
}

CMailBox::~CMailBox()
{
	remove_all();
}

bool
CMailBox::free_item(TSomething aItem)
{
	CMail 
		*Mail = (CMail*)aItem;

	if (!Mail) return false;

	delete Mail;

	return true;
}

int
CMailBox::compare(TSomething aItem1, TSomething aItem2) const
{
	CMail
		*Mail1 = (CMail*)aItem1,
		*Mail2 = (CMail*)aItem2;

	if (Mail1->get_time() > Mail2->get_time()) return 1;
	if (Mail1->get_time() < Mail2->get_time()) return -1;
	if (Mail1->get_receiver() < Mail2->get_receiver()) return 1;
	if (Mail1->get_receiver() > Mail2->get_receiver()) return -1;
	return strcmp(Mail1->get_title(), Mail2->get_title());
}

// do not use find key
int 
CMailBox::compare_key(TSomething aItem, TConstSomething aKey) const
{
	(void)aItem;
	(void)aKey;

	exit(-2);

	return -1;
}

