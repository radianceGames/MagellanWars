#include "cgi.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

CHTMLStation::CHTMLStation():CSortedList(100, 50)
{
}

CHTMLStation::~CHTMLStation()
{
	remove_all();
}

bool
CHTMLStation::load(const char *aPath)
{
	system_log("loading html station");

	if (access(aPath, R_OK+X_OK)) return false;

	return load_subdirectory(aPath, "");

	return true;
}

bool
CHTMLStation::load_subdirectory(const char *aPath, const char *aSubPath)
{
	char
		Path[256];
	struct stat
		sbuf;

	sprintf(Path, "%s/%s", aPath, aSubPath);
//	system_log("search path %s", Path);

	if (access(Path, R_OK+X_OK))
	{
		system_log("Could not access %s", Path);
		return false;
	}
	
	DIR
		*Directory;
	struct dirent
		*Entry;

	Directory = opendir(Path);
	if (!Directory) return false;

	while((Entry = readdir(Directory)))
	{
		if (Entry->d_name[0] == '.') continue;

		sprintf( Path, "%s/%s/%s", aPath, aSubPath, Entry->d_name );
		if( stat( Path, &sbuf ) == -1 ) continue;

		if ((sbuf.st_mode & S_IFMT) == S_IFDIR )
		{
			if (!strcmp(Entry->d_name, "CVS")) continue;
			char 
				NewSubPath[256];
			if (strlen(aSubPath))
				sprintf(NewSubPath, "%s/%s", aSubPath, Entry->d_name);
			else 
				strcpy(NewSubPath, Entry->d_name);
			if (!load_subdirectory(aPath, NewSubPath)) return false;
		} else {
			char 
				ItemName[256];
			if (strlen(aSubPath))
				sprintf(ItemName, "%s/%s", aSubPath, Entry->d_name);
			else 
				strcpy(ItemName, Entry->d_name);
			CHTML
				*HTML = new CHTML();
			if (!HTML->load(aPath, ItemName))
			{
				delete HTML;
				return false;
			}
			insert_sorted(HTML);	
//			system_log("insert name [%x]%s", 
//					HTML->get_name(), HTML->get_name());
		}
	}
	closedir(Directory);

	return true;
}

bool
CHTMLStation::get_html(CString &aOutput, const char *aName, 
		CQueryList *aConversions)
{
	CHTML
		*HTML;

	HTML = find_html(aName);

	if (!HTML) return false;

//	system_log("CHTMLStation::get_html()");

	if (!HTML->get(aOutput, aConversions)) return false;

//	system_log("out CHTMLStation::get_html()");

	return true;
}

bool 
CHTMLStation::get_html(CPage& aPage, const char *aName, 
		CQueryList *aConversions) 
{
	return get_html(aPage.mOutput, aName, aConversions);
}

CHTML*
CHTMLStation::find_html(const char *aName)
{
	int 
		Res = find_sorted_key(aName);
	if (Res < 0) return NULL;

	return (CHTML*)get(Res);
}

bool 
CHTMLStation::free_item(TSomething aItem)
{
	CHTML
		*HTML = (CHTML*)aItem;

//	system_log("free html %s", HTML->get_name());
	if (!HTML) return false;

	delete HTML;

	return true;
}

int
CHTMLStation::compare(TSomething aItem1, TSomething aItem2) const
{
	CHTML
		*HTML1 = (CHTML*)aItem1,
		*HTML2 = (CHTML*)aItem2;

	return strcmp(HTML1->get_name(), HTML2->get_name());	
}

int
CHTMLStation::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CHTML
		*HTML = (CHTML*)aItem;

	return strcmp(HTML->get_name(), (char*)aKey);
}
