#if !defined(__ARCHSPACE_RELAY_PAGES_H__)
#define __ARCHSPACE_RELAY_PAGES_H__

#include "gettext.h"
#include "cgi.h"

class CPageNotFound: public CPage
{
	public:
		CPageNotFound():CPage() {}
		virtual ~CPageNotFound() {}

		virtual const char *get_name();
		virtual bool handle(CConnection &aConnection);
};

class CPageCommon: public CPage
{
	public: 
		CPageCommon():CPage() {}
		virtual ~CPageCommon() {}

		inline bool output(const char *aWebFile, 
				CQueryList& aConversion);
		void message_page(CConnection &aConnection, 
				const char *aMessage);
	protected:
		CQueryList& get_conversion(CConnection &aConnection);
};

inline bool
CPageCommon::output(const char *aWebFile, CQueryList& aConversion)
{
	if (!mHTMLStation->get_html(*this, aWebFile, &aConversion))
		return true;
	return false;

}

class CPageCreate: public CPageCommon
{
	public:
		virtual ~CPageCreate() {}

		virtual const char *get_name() { return "/archspace/create.as"; }
		virtual bool handle(CConnection &aConnection);

		const char *race_html(CConnection &aConnection);
};


class CPageCreate2: public CPageCommon
{
	public:
		virtual ~CPageCreate2() {}

		virtual const char *get_name() { return "/archspace/create2.as"; }
		virtual bool handle(CConnection &aConnection);
};

class CPageLogin: public CPageCommon
{
	public:
		CPageLogin():CPageCommon() {}
		virtual ~CPageLogin() {}

		virtual const char *get_name() { return "/archspace/login.as"; }
		virtual bool handle(CConnection &aConnection);
};

class CPageAdmin: public CPageCommon
{
	public:
		CPageAdmin():CPageCommon() {}
		virtual ~CPageAdmin() {}

		virtual const char *get_name() { return "/archspace/admin.as"; }

		virtual bool handle(CConnection &aConnection); 
		virtual bool admin_handle(CConnection &aConnection,
							const char* aAuthority) = 0;
};

class CPageAddAdmin: public CPageAdmin
{
	public:
		CPageAddAdmin():CPageAdmin() {}
		virtual ~CPageAddAdmin() {}

		virtual const char *get_name() { return "/archspace/add_admin.as"; }

		virtual bool admin_handle(CConnection &aConnection,
							const char* aAuthority) = 0;
};

#endif
