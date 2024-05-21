#include "news.h"

CNews::CNews()
{
	mViewed = false;
}

CNews::~CNews()
{
}

void
CNews::clear()
{
	mNews.clear();
	mViewed = false;
}

const char *
CNews::get_query()
{
	return (char*)mNews;
}

void
CNews::set_query(const char *aQuery)
{
	if (!aQuery) return;

	mNews = aQuery;
}

const char*
CNews::get()
{
	static CString
		News;
	News.clear();

	News = filter((char *)mNews);

	mViewed = true;
	//mNews.clear();

	return (char *)News;
}

