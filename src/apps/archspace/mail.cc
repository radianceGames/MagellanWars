CMail::CMail():CStore()
{
	mSender = -1;
	mReceiver = -1;
	mTime = 0;
	mTitle = "";
	mContent = "";
}

CMail::~CMail()
{
}

void
CMail::initialize(int aSender, int aReceiver, 
		const char *aTitle, const char *aContent)
{
	mSender = aSender;
	mReceiver = aReceiver;
	mTitle = aTitle;
	mContent = aContent;
}

CString&
CMail::query()
{
	static CString
		Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query = "INSERT INTO mail "
					"(sender, receiver, time, title, content) VALUES";
			Query.format("(%d, %d, %d, \"%s\", \"%s\")", 
						mSender, mReceiver, mTime, 
						(char*)mTitle, (char*)mContent);
			break;
		case QUERY_UPDATE:
			Query = "UPDATE mail SET remove = 'true' WHERE ";
			Query.format("sender = %d AND receiver = %d AND time = %d "
						"AND title = \"%s\" AND content = \"%s\"",
							mSender, mReceiver, mTime, 
							(char*)mTitle, (char*)mContent);
			break;
		case QUERY_DELETE:
			Query = "DELETE FROM mail WHERE ";
			Query.format("sender = %d AND receiver = %d AND time = %d "
						"AND title = \"%s\" AND content = \"%s\"",
							mSender, mReceiver, mTime, 
							(char*)mTitle, (char*)mContent);
			break;
	}

	mStoreFlag.clear();

	return Query;
}
