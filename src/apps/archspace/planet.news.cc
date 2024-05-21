#include "news.h"
#include "define.h"
#include <libintl.h>

CPlanetNews::~CPlanetNews()
{
}

void
CPlanetNews::new_planet_news(const char *aName)
{
	if (mNews.length())
		mNews.format(", %s", aName);
	else 
		mNews = aName;
}

const char*
CPlanetNews::filter(const char* aNews)
{
	if (!aNews) return NULL;

	static CString
		News;
	News.clear();

	if (strchr(aNews, ','))
		News = format(GETTEXT("New Planet : %1$s have been discovered."),
				aNews);
	else 
		News = format(GETTEXT("New Planet : %1$s has been discovered."), 
				aNews);

	return (char*)News;
}
