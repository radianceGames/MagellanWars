#include "news.h"
#include "define.h"
#include <libintl.h>

CKnownTechNews::~CKnownTechNews()
{
}

void
CKnownTechNews::known_tech_news(const char *aName)
{
	if (mNews.length())
		mNews.format(", %s", aName);
	else 
		mNews = aName;
}

const char*
CKnownTechNews::filter(const char* aNews)
{
	if (!aNews) return NULL;

	static CString
		News;
	News.clear();

	if (strchr(aNews, ','))
		News.format(GETTEXT("New Tech : %1$s have been discovered."),
					aNews);
	else 
		News.format(GETTEXT("New Tech : %1$s has been discovered."),
					aNews);

	return (char*)News;
}

