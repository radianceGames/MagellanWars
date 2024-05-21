#include "../pages.h"
#include "../archspace.h"
#include <cstdlib>
#include "../banner.h"
#include "../game.h"

bool
CBotPage::get_conversion()
{
	if (!mConnection) return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country);
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country);

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("CHAR_SET", GAME->get_charset());
	ITEM("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	return true;
}
void
CBotPage::message_page(const char *aMessage)
{
	ITEM("MESSAGE", aMessage)

	output("message.html");
}

bool
CBotPage::handle(CConnection &aConnection)
{
	mConnection = &aConnection;
	if (strstr(allow_method(), aConnection.get_method()) == NULL)
	{
		message_page("Forbidden access");
		return false;
	}

	mConversion.remove_all();
	get_conversion();
	mQuery.remove_all();
	mQuery.set(QUERY_STRING, '&');
	return true;
}
