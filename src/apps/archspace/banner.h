#if !defined(__ARCHSPACE_BANNER_H__)
#define __ARCHSPACE_BANNER_H__

#include "util.h"
#include "cgi.h"

class CBanner
{
	public :
		CBanner();
		~CBanner();

		char *get_country() { return (char *)mCountry; }
		bool set_country(char *aCountry);

		char *get_menu_name() { return (char *)mMenuName; }
		bool set_menu_name(char *aMenuName);

		bool is_top_banner() { return mIsTopBanner; }
		bool set_top_banner();
		bool set_bottom_banner();

		int get_number_of_banner_code();

		char *get_banner_code_by_index(int aIndex);
		bool add_banner_code(char *aBanner);

		char *get_key() { return (char *)mKey; }
		bool refresh_key();

	private :
		CString
			mCountry,
			mMenuName;
		bool
			mIsTopBanner;
		CStringList
			mBannerCodeList;
		CString
			mKey;

};

class CBannerCenter : public CSortedList
{
	public:
		CBannerCenter();
		virtual ~CBannerCenter();

		bool add_banner(CBanner *aBanner);
		bool remove_banner(CBanner *aBanner);

		bool register_country(char *aCountry);
		bool register_menu(char *aMenu);

		bool load(char *aFilePath, char *aTopBanner, char *aBottomBanner);
		bool is_top_banner_available() { return mTopBanner; }
		bool is_bottom_banner_available() { return mBottomBanner; }

		char *get_top_banner_by_country_menu(char *aCountry = NULL, char *aMenu = NULL);
		char *get_bottom_banner_by_country_menu(char *aCountry = NULL, char *aMenu = NULL);

	protected :
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;

	private :
		bool
			mTopBanner,
			mBottomBanner;
		CStringList
			mCountryList,
			mMenuList;
		int
			mCurrentBannerCodeIndex;
};

#endif
