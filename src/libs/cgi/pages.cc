#include "cgi.h"
#include <cstdlib>

CPageStation::CPageStation()
{
}

CPageStation::~CPageStation()
{
	remove_all();
	if (CPage::mHTMLStation)
		delete CPage::mHTMLStation;
}

bool
CPageStation::free_item(TSomething aItem)
{
	CPage
		*Page = (CPage*)aItem;

	if (!Page) return false;

//	system_log("delete page %s", Page->get_name());
	delete Page;

	return true;
}

int
CPageStation::compare(TSomething aItem1, TSomething aItem2) const
{
	CPage
		*Page1 = (CPage*)aItem1,
		*Page2 = (CPage*)aItem2;

	return strcmp(Page1->get_name(), Page2->get_name());	
}

int
CPageStation::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPage
		*Page = (CPage*)aItem;

	return strcmp(Page->get_name(), (char*)aKey);
}

bool
CPageStation::initialize(const char *aHTMLPath)
{
	system_log("initalize page station", aHTMLPath);

	if (!CPage::mHTMLStation)
	{
		CPage::mHTMLStation = new CHTMLStation();
		if (!CPage::mHTMLStation->load(aHTMLPath)) return false;
	}

	if (!registration()) return false;

	return true;
}

CPage*
CPageStation::find_page(const char *aName)
{
	int
		Res = find_sorted_key(aName);
	if (Res < 0) return NULL;
	return (CPage*)get(Res);
}

CString	* 
CPageStation::get_page(CConnection &aConnection)
{
	CPage
		*Page;
	
	Page = find_page(aConnection.get_uri());

	if (!Page)
	{
		Page = find_page("not_found.as");	
		if (!Page)
		{
			system_log("Could not found notfound page");
			exit(-1);
		}
		return Page->get_html(aConnection);
	}

	return Page->get_html(aConnection);
}
