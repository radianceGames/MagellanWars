#include "planet.h"
#include "player.h"
#include "archspace.h"

TZone gPlanetListZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPlanetList),
	0,
	0,
	NULL,
	"Zone CPlanetList"
};
void
CPlanetList::get_info()
{
	SLOG("PlanetList Root:%x", (int)mRoot);
}

CPlanetList&
CPlanetList::operator= (CPlanetList& PlanetList)
{
	SLOG("Warning: Planet List not allowed copy");
	return *this;
}

CPlanetList::CPlanetList(): CList(10, 10)
{
	mPower = 0;
}

CPlanetList::~CPlanetList()
{
	remove_all();
}

bool
CPlanetList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CPlanetList::add_planet(CPlanet* aPlanet)
{
	assert(aPlanet);

	at(aPlanet->get_order(), aPlanet);

	refresh_power();

	return aPlanet->get_id();
}

bool
CPlanetList::remove_planet(int aPlanetID)
{
	if (!length())
		return false;

	CPlanet *
		Planet = get_by_id(aPlanetID);
	if (!Planet) return false;

	refresh_power();

	int
		Order = Planet->get_order();

	CList::remove(Order);

	for(int i=Order ; i<length(); i++)
	{
		Planet = (CPlanet*)get(i);
		Planet->set_order(i);
	}

	return true;
}

bool
CPlanetList::remove_without_free_planet(int aPlanetID)
{
	if (!length())
		return false;

	CPlanet *
		Planet = get_by_id(aPlanetID);
	if (!Planet) return false;

	refresh_power();

	int
		Order = Planet->get_order();

	CList::remove_without_free(Order);

	for(int i=Order ; i<length(); i++)
	{
		Planet = (CPlanet*)get(i);
		Planet->set_order(i);
	}

	return true;
}

CPlanet*
CPlanetList::get_by_order(int aOrder)
{
	if (aOrder > length() || aOrder < 0) return NULL;

	return (CPlanet*)get(aOrder);
}

CPlanet*
CPlanetList::get_by_id(int aPlanetID)
{
	CPlanet *Planet;

	for(int i=0; i<length(); i++)
	{
		Planet = (CPlanet*)get(i);
		if (Planet->get_id() == aPlanetID)
			return Planet;
	}

	return NULL;
}

const char *
CPlanetList::generate_news()
{
	static CString buf;

	buf.clear();

	for(int i=0; i<length(); i++)
	{
		CPlanet
			*Planet = (CPlanet *)get(i);

		buf += Planet->news();
	}

	return (char *)buf;
}

const char *
CPlanetList::html_select()
{
	static CString
		Buf;

	Buf = "<SELECT NAME=PLANET>\n";

	for( int i = 0; i < length(); i++ ){
		CPlanet
			*Planet = (CPlanet*)get(i);

		Buf.format( "<OPTION VALUE = %d>%s</OPTION>\n", Planet->get_id(), Planet->get_name() );

	}

	Buf += "</SELECT>\n";

	return (char*)Buf;
}

int
CPlanetList::total_population()
{
	int
		Total = 0;

	for(int i = 0; i<length(); i++)
	{
		CPlanet *
			Planet = (CPlanet *)get(i);

		if (MAX_GENERAL_INT - Total < Planet->get_population())
		{
			return MAX_GENERAL_INT;
		}
		Total += Planet->get_population();
	}

	return Total;
}

int CPlanetList::count_planet_with_attribute( int aAttribute ) {
	int Count = 0;

	for( int i = 0; i < length(); i++ ){
		CPlanet *Planet = (CPlanet*)get(i);

		if( Planet->has_attribute(aAttribute)) Count++;
	}

	return Count;
}

int
CPlanetList::count_planet_from_cluster( int aClusterID )
{
	int
		Count = 0;

	for( int i = 0; i < length(); i++ ){
		CPlanet
			*Planet = (CPlanet*)get(i);

		if( Planet->get_cluster_id() == aClusterID ) Count++;
	}

	return Count;
}

bool
CPlanetList::refresh_power()
{
	int
		OwnerGameID = 0;

	mPower = 0;
	for(int i=0 ; i<length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)get(i);

		int
			AvailablePower = MAX_POWER - mPower;
		if (Planet->get_power() > AvailablePower)
		{
			mPower = MAX_POWER;
			break;
		}
		else
		{
			mPower += Planet->get_power();
		}

		OwnerGameID = Planet->get_owner_id();
	}

	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(OwnerGameID);
	if (Owner == NULL)
	{
		SLOG("ERROR : Owner is NULL in CPlanetList::refresh_power(), OwnerGameID = %d", OwnerGameID);
		if (length() > 0)
		{
		CPlanet *Planet = (CPlanet *)get(length()-1);
        if (Planet != NULL)
            SLOG("Last Planets ID: %d", Planet->get_id());
		}
	}
	else if (Owner->get_game_id() != EMPIRE_GAME_ID)
	{
		Owner->refresh_power();
	}

	return true;
}

char *
CPlanetList::siege_planet_border_planet_select_html()
{
	static CString
		Select;
	Select.clear();

	if (!length()) return NULL;

	Select = "<SELECT NAME=\"PLANET_ID\" onChange=siege() onkeypress=siege()>\n";

	for (int i = length()/2 ; i<length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)get(i);

		Select.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
						Planet->get_id(), Planet->get_name());
	}

	Select += "</SELECT>\n";

	return (char *)Select;
}

char *
CPlanetList::blockade_border_planet_select_html()
{
	static CString
		Select;
	Select.clear();

	if (!length()) return NULL;

	Select = "<SELECT NAME=\"PLANET_ID\" onChange=blockade() onkeypress=blockade()>\n";

	for (int i = length()/2 ; i<length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)get(i);

		Select.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
						Planet->get_id(), Planet->get_name());
	}

	Select += "</SELECT>\n";

	return (char *)Select;
}

char *
CPlanetList::raid_border_planet_select_html()
{
	static CString
		Select;
	Select.clear();

	if (!length()) return NULL;

	Select = "<SELECT NAME=\"PLANET_ID\" onChange=raid() onkeypress=raid()>\n";

	for (int i = length()/2 ; i<length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)get(i);

		Select.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
						Planet->get_id(), Planet->get_name());
	}

	Select += "</SELECT>\n";

	return (char *)Select;
}

char *
CPlanetList::whitebook_planet_list_html()
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"500\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";

	int
		Column = 0;

	for (int i=0 ; i<length() ; i++)
	{
		if (Column == 3)
		{
			List += "</TR>\n";
			List += "<TR>\n";
			Column = 0;
		}

		CPlanet *
			Planet = (CPlanet *)get(i);

		List += "<TD CLASS=\"tabletxt\" WIDTH=\"166\">";
		List += "&nbsp;";
		List.format("<A HREF=\"whitebook_control_model_planet.as?PLANET_ID=%d\">%s</A>",
					Planet->get_id(), Planet->get_name());
		List += "</TD>\n";

		Column++;
	}

	for (int i=Column ; i < 3 ; i++)
	{
		List += "<TD CLASS=\"tabletxt\" WIDTH=\"166\">&nbsp;</TD>\n";
		if (Column == 2) List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

bool
CPlanetList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)get(i);
		if (Planet->is_changed() == false) continue;

		Planet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Planet);
	}

	return true;
}

void
CPlanetList::add_destroy_planet(int ID)
{
	CPlanet
		*Planet = get_by_id(ID);

	Planet->set_destroy_timer(TURNS_PER_24HOURS * 3);
}

void
CPlanetList::cancel_destroy_planet(int ID)
{
	CPlanet
		*Planet = get_by_id(ID);

	Planet->set_destroy_timer(0);
}

