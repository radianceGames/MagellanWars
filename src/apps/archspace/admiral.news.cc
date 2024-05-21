#include "news.h"
#include "define.h"
#include <libintl.h>

CAdmiralNews::~CAdmiralNews()
{
}

void
CAdmiralNews::admiral_news(const char *aName)
{
	if (mNews.length())
		mNews.format(", %s", aName);
	else 
		mNews = aName;
}

const char*
CAdmiralNews::filter(const char* aNews)
{
	if (!aNews) return NULL;

	static CString
		News;
	News.clear();

	if (strchr(aNews, ','))
		News = format(GETTEXT("Admiral: %1$s are available."), aNews);
	else 
		News = format(GETTEXT("Admiral: %1$s is available."), aNews);

	return (char*)News;
}
