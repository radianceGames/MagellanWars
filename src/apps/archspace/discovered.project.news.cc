#include "news.h"
#include "define.h"
#include <libintl.h>

CDiscoveredProjectNews::~CDiscoveredProjectNews()
{
}

void
CDiscoveredProjectNews::discovered_project_news(const char *aName)
{
	if (mNews.length())
		mNews.format(", %s", aName);
	else 
		mNews = aName;
}

const char*
CDiscoveredProjectNews::filter(const char* aNews)
{
	if (!aNews) return NULL;

	static CString
		News;
	News.clear();

	if (strchr(aNews, ','))
		News = format(GETTEXT("Project : %1$s are available."), aNews);
	else 
		News = format(GETTEXT("Project : %1$s is available."), aNews);

	return (char*)News;
}
