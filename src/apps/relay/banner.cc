#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "banner.h"

CBanner::CBanner():CBase()
{
	mID = -1;
	mBanner = "";
}

CBanner::CBanner(int aID, const char* aBanner): CBase()
{
	mID = aID;
	mBanner = aBanner;
//	system_log("ID:%d, Type:%c, Banner:%s", aID, aType, aBanner);
}

CBanner::~CBanner()
{
}

CBannerList::CBannerList():CList(10, 10)
{
}

CBannerList::~CBannerList()
{
	remove_all();
}

bool
CBannerList::free_item(TSomething aItem)
{
	CBanner
		*Banner = (CBanner*)aItem;

	if (!Banner) return false;

	delete Banner;

	return true;
}

CBannerCenter::CBannerCenter(CApplication* aApplication):CMySQL()
{
	mApplication = aApplication;
}

CBannerCenter::~CBannerCenter()
{
}

const char*
CBannerCenter::default_banner()
{
	return 
		"<CENTER>\n"
		"<A HREF=\"http://ultra.archspace.co.kr/main.html\">\n"
		"<IMG SRC=\"http://image.archspace.co.kr/image/banner.gif\" ISMAP BORDER=0 ></A>\n"
		"</CENTER>\n";
}

bool
CBannerCenter::initialize()
{
	if (!mApplication) return false;

	char 
		*Host = mApplication->configuration().get_string(
						"Advertisement", "Host", NULL),
		*User = mApplication->configuration().get_string(
						"Advertisement", "User", NULL),
		*Password = mApplication->configuration().get_string(
						"Advertisement", "Password", NULL),
		*Database = mApplication->configuration().get_string(
						"Advertisement", "Database", NULL);

	if (!Host || !User || !Password || !Database) return false;

	system_log("initalize Advertisement");
	if (!CMySQL::open(Host, User, Password, Database)) return false;

	if (!load()) return false;
	
	CMySQL::close();

	return true;
}

bool 
CBannerCenter::load()
{
	system_log("load Advertisement Code");

	query("LOCK TABLES Ad READ");

	query("SELECT ad_id, ad_type, ad_text FROM Ad ORDER BY ad_type");
	use_result();
	while(fetch_row())
	{
		if (mRow[1][0] == 'P')
			mPBanner.push(new CBanner(atoi(mRow[0]), mRow[2]));
		else
			mSBanner.push(new CBanner(atoi(mRow[0]), mRow[2]));
	}
	free_result();

	query("UNLOCK TABLES");

	return true;
}

const char *
CBannerCenter::get_banner(int &aPType, int &aSType)
{
	char 
		Type;
	CBanner*
		Banner;
	CString
		Temp;

	system_log("P:%d, S:%d, NP:%d, NS:%d", aPType, aSType, 
			mPBanner.length(), mSBanner.length());
	Type = 'E';

	if (mPBanner.length() && aPType < 0)
	{
		Banner = (CBanner*)mPBanner[0];
		Temp = Banner->get_code();
		Type = 'P';
		aPType = 0;
	} else if (mPBanner.length() && aPType < mPBanner.length())
	{
		Banner = (CBanner*)mPBanner[aPType];
		Temp = Banner->get_code();
		Type = 'P';
		aPType++;
	} if (mSBanner.length() && aSType < 0)
	{
		Banner = (CBanner*)mSBanner[0];
		Temp = Banner->get_code();
		Type = 'S';
		aSType = 0;
	} else if (mSBanner.length() && aSType < mSBanner.length())
	{
		Banner = (CBanner*)mSBanner[aSType];
		Temp = Banner->get_code();
		Type = 'S';
		aSType++;
	} else {
		Temp = default_banner();
		Type = 'S';
		aSType = 0;
	}

	static CString
		Output;
	Output.clear();

	if (Type == 'P')
		Output.format("\n<IMG SRC=\"/Ad/scookie.phtml?ctxt=AdInfoP&cval=%d\" WIDTH=5 HEIGHT=5 ALT=\"\">\n", aPType); 		
	else if (Type == 'S')
		Output.format("\n<IMG SRC=\"/Ad/scookie.phtml?ctxt=AdInfoS&cval=%d\" WIDTH=5 HEIGHT=5 ALT=\"\">\n", aSType); 		
	else return default_banner();

	Output += Temp;
	return (char*)Output;
}
