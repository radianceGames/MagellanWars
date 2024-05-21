#include "banner.h"
#include "define.h"
#include <cstdio>
#include <cstring>

CBanner::CBanner()
{
}

CBanner::~CBanner()
{
}

bool
CBanner::set_country(char *aCountry)
{
	if (aCountry == NULL) return false;

	mCountry = aCountry;
	refresh_key();

	return true;
}

bool
CBanner::set_menu_name(char *aMenuName)
{
	if (aMenuName == NULL) return false;

	mMenuName = aMenuName;
	refresh_key();

	return true;
}

bool
CBanner::set_top_banner()
{
	mIsTopBanner = true;
	refresh_key();

	return true;
}

bool
CBanner::set_bottom_banner()
{
	mIsTopBanner = false;
	refresh_key();

	return true;
}

int
CBanner::get_number_of_banner_code()
{
	return mBannerCodeList.length();
}

char *
CBanner::get_banner_code_by_index(int aIndex)
{
	char *
		RandomBannerCode = (char *)mBannerCodeList.get(aIndex);

	if (RandomBannerCode == NULL) return " ";

	return RandomBannerCode;
}

bool
CBanner::add_banner_code(char *aBanner)
{
	if (aBanner == NULL) return false;

	mBannerCodeList.insert_sorted(aBanner);
	return true;
}

bool
CBanner::refresh_key()
{
	if (get_country() == NULL || get_menu_name() == NULL) return false;

	mKey.clear();
	mKey.format("%s %s", get_country(), get_menu_name());

	if (mIsTopBanner == true)
	{
		mKey += " TOP";
	}
	else
	{
		mKey += " BOTTOM";
	}

	return true;
}

CBannerCenter::CBannerCenter()
{
	mBottomBanner = false;
	mCurrentBannerCodeIndex = 0;
}

CBannerCenter::~CBannerCenter()
{
}

bool
CBannerCenter::free_item(TSomething aItem)
{
	CBanner *
		Banner = (CBanner *)aItem;
	if (Banner == NULL) return false;

	delete Banner;

	return true;
}

int
CBannerCenter::compare(TSomething aItem1, TSomething aItem2) const
{
	CBanner
		*Banner1 = (CBanner *)aItem1,
		*Banner2 = (CBanner *)aItem2;

	if (strcmp(Banner1->get_key(), Banner2->get_key()) > 0) return 1;
	if (strcmp(Banner1->get_key(), Banner2->get_key()) < 0) return -1;
	return 0;
}

int
CBannerCenter::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CBanner *
		Banner = (CBanner *)aItem;

	if (strcmp(Banner->get_key(), (char *)aKey) > 0) return 1;
	if (strcmp(Banner->get_key(), (char *)aKey) < 0) return -1;
	return 0;
}

bool
CBannerCenter::add_banner(CBanner *aBanner)
{
	if (aBanner == NULL) return false;

	int
		Index = find_sorted_key((void *)aBanner->get_key());
	if (Index >= 0) return false;

	insert_sorted(aBanner);

	return true;
}

bool
CBannerCenter::remove_banner(CBanner *aBanner)
{
	if (aBanner == NULL) return false;

	int
		Index = find_sorted_key((void *)aBanner->get_key());
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool
CBannerCenter::register_country(char *aCountry)
{
	if (aCountry == NULL) return false;

	int
		Index = mCountryList.find_sorted_key(aCountry);
	if (Index >= 0) return true;

	mCountryList.insert_sorted((void *)aCountry);
	return true;
}

bool
CBannerCenter::register_menu(char *aMenu)
{
	if (aMenu == NULL) return false;

	int
		Index = mMenuList.find_sorted_key(aMenu);
	if (Index >= 0) return true;

	mMenuList.insert_sorted((void *)aMenu);
	return true;
}

#define TOPBANNER "<script type=\"text/javascript\">google_ad_client = \"pub-9347639997854244\";google_ad_width = 468;google_ad_height = 60;google_ad_format = \"468x60_as\";google_ad_type = \"text_image\";google_ad_channel =\"\";google_page_url = document.location;google_color_border = \"333333\";google_color_bg = \"000000\";google_color_link = \"FFFFFF\";google_color_url = \"999999\";google_color_text = \"CCCCCC\";</script><script type=\"text/javascript\"src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\"></script>"
#define BOTTOMBANNER "<script type=\"text/javascript\">google_ad_client = \"pub-9347639997854244\";google_ad_width = 234;google_ad_height = 60;google_ad_format = \"234x60_as\";google_ad_type = \"text\";google_ad_channel =\"\";google_page_url = document.location;google_color_border = \"333333\";google_color_bg = \"000000\";google_color_link = \"FFFFFF\";google_color_url = \"999999\";google_color_text = \"CCCCCC\";</script><script type=\"text/javascript\" src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\"></script> <script type=\"text/javascript\">google_ad_client = \"pub-9347639997854244\";google_ad_width = 234;google_ad_height = 60;google_ad_format = \"234x60_as\";google_ad_type = \"text\";google_ad_channel =\"\";google_color_border = \"333333\";google_color_bg = \"000000\";google_color_link = \"FFFFFF\";google_page_url = document.location;google_color_url = \"999999\";google_color_text = \"CCCCCC\";</script><script type=\"text/javascript\" src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\"></script>"

//"<script type=\"text/javascript\"> google_ad_client = \"pub-9347639997854244\"; google_ad_width = 160; google_ad_height = 600; google_ad_format = \"160x600_as\"; google_ad_type = \"text_image\"; google_ad_channel =\"\"; google_page_url = document.location; </script> <script type=\"text/javascript\" src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\"> </script>

//<script type=\"text/javascript\">google_ad_client = \"pub-9347639997854244\";google_ad_width = 160;google_ad_height = 600;google_ad_format = \"160x600_as\";google_ad_type = \"text_image\";google_ad_channel =\"\";google_page_url = document.location;google_color_border = \"333333\";google_color_bg = \"000000\";google_color_link = \"FFFFFF\";google_color_url = \"999999\";google_color_text = \"CCCCCC\";</script><script type=\"text/javascript\"  src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\"></script>"


bool
CBannerCenter::load(char *aBannerPath, char *aTopBanner, char *aBottomBanner)
{
	CBanner *
			Banner = new CBanner();

	Banner->set_country("DEFAULT");
	Banner->set_menu_name("DEFAULT");
	Banner->set_top_banner();
	Banner->add_banner_code(TOPBANNER);
	add_banner(Banner);

	//delete Banner;
	Banner = new CBanner();
	Banner->set_country("DEFAULT");
	Banner->set_menu_name("DEFAULT");
	Banner->set_bottom_banner();
	Banner->add_banner_code(BOTTOMBANNER);
	add_banner(Banner);
}

char *
CBannerCenter::get_top_banner_by_country_menu(char *aCountry, char *aMenu)
{
	return TOPBANNER;
}

char *
CBannerCenter::get_bottom_banner_by_country_menu(char *aCountry, char *aMenu)
{
	return TOPBANNER;
}

