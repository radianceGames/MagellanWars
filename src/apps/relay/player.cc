#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


CPlayer::CPlayer():CStore()
{
	mID = -1;
	mServer = -1;
	mName = "NoName";
}

CPlayer::~CPlayer()
{
}

CString&
CPlayer::query()
{
	static CString
		Query;

	Query.clear();

	return Query;
}
