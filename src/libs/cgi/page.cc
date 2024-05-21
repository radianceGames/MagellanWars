#include "cgi.h"

CHTMLStation*
	CPage::mHTMLStation = NULL;

CPage::CPage()
{
}

CPage::~CPage()
{
}

CString*
CPage::get_html(CConnection &aConnection)
{
	mOutput.clear();

	handle(aConnection);

	return &mOutput;
}
