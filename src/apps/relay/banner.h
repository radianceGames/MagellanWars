#if !defined(__ARCHSPACE_BANNER_H__)
#define __ARCHSPACE_BANNER_H__

#include "frame.h"
#include "util.h"

class CBanner: public CBase
{
	public:
		CBanner();
		CBanner(int aID, const char *aBanner);
		virtual ~CBanner();

		int get_id() { return mID; }
		const char* get_code() { return (char*)mBanner; }
	protected:
		int
			mID;
		CString 
			mBanner;
};

class CBannerList: public CList
{
	public:
		CBannerList();
		virtual ~CBannerList(); 
	protected:
		virtual bool free_item(TSomething aItem);
};

class CBannerCenter: public CMySQL
{
	public:
		CBannerCenter(CApplication *aApplication);
		virtual ~CBannerCenter();

		bool initialize();

		const char* default_banner();
		const char* get_banner(int &aPType, int &aSType);
	protected:
		bool load();

		CApplication
			*mApplication;
		CBannerList
			mPBanner,
			mSBanner;
};

#endif
