#include "news.h"
#include "define.h"
#include <libintl.h>

CPurchasedProjectNews::~CPurchasedProjectNews()
{
}

void
CPurchasedProjectNews::purchased_project_news(const char *aName)
{
	if (mNews.length())
		mNews.format(", %s", aName);
	else 
		mNews = aName;
}

const char*
CPurchasedProjectNews::filter(const char* aNews)
{
	if (!aNews) return NULL;

	static CString
		News;
	News.clear();

	if (strchr(aNews, ','))
		//News = format(GETTEXT("Project : %1$s are available."), aNews);
		News = format(GETTEXT("Project : %1$s were completed."), aNews);
	else 
		//News = format(GETTEXT("Project : %1$s is available."), aNews);
		News = format(GETTEXT("Project : %1$s was completed."), aNews);

	return (char*)News;
}
