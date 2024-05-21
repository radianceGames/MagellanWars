#include "race.h"
#include <libintl.h>

TZone gRaceZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CRace),
	0,
	0,
	NULL,
	"Zone CRace"
};

extern char *AbilityName[];

CRace::CInnateTechList::~CInnateTechList()
{
	remove_all();
}

bool
CRace::CInnateTechList::free_item(TSomething aItem)
{
	(void)aItem;

	return true;
}

CRace::CRace()
{
	mID = mSociety = -1;
	mTemperature = 300;
	mGravity = 1.0;
	mMoraleModifier = mBerserkModifier = mSurvivalModifier = 0;
	mInitialEmpireRelation = 50;
}

CRace::~CRace()
{
}

const char *
CRace::get_society_name()
{
	switch(mSociety)
	{
		case SOCIETY_TOTALISM:
			return GETTEXT("Totalism");
		case SOCIETY_CLASSISM:
			return GETTEXT("Classism");
		case SOCIETY_PERSONALISM:
			return GETTEXT("Personalism");
		case SOCIETY_UNKNOWN:
			return GETTEXT("Unknown");
	}
	return NULL;
}

bool
CRace::set_society_by_name(char *aSociety)
{
	if (!strcmp(aSociety, "Totalism"))
		mSociety = SOCIETY_TOTALISM;
	else if (!strcmp(aSociety, "Classism"))
		mSociety = SOCIETY_CLASSISM;
	else if (!strcmp(aSociety, "Personalism"))
		mSociety = SOCIETY_PERSONALISM;
	else if (!strcmp(aSociety, "Unknown"))
		mSociety = SOCIETY_UNKNOWN;
	else {
		mSociety = SOCIETY_CLASSISM;
		return false;
	}
	return true;
}

void
CRace::set_society(int aSociety)
{
	if (aSociety<SOCIETY_BEGIN || aSociety>=SOCIETY_MAX )
	{
		mSociety = SOCIETY_CLASSISM;
		return;
	}

	mSociety = aSociety;
	return;
}

void
CRace::set_ability(int aAbility)
{
	if (aAbility<ABILITY_BEGIN || aAbility>=ABILITY_MAX) return;

	mAbility += aAbility;
	return;
}

bool
CRace::set_ability(char *aAbility)
{
	int
		Index = ability_name_to_index(aAbility);

	if (Index<ABILITY_BEGIN || Index>=ABILITY_MAX) return false;

	mAbility += Index;
	return true;
}

const char *
CRace::get_atmosphere()
{
	static char Atmosphere[CPlanet::GAS_MAX+1];

	Atmosphere[CPlanet::GAS_H2] = mAtmosphere[CPlanet::GAS_H2] + '0';
	Atmosphere[CPlanet::GAS_Cl2] = mAtmosphere[CPlanet::GAS_Cl2] + '0';
	Atmosphere[CPlanet::GAS_CO2] = mAtmosphere[CPlanet::GAS_CO2] + '0';
	Atmosphere[CPlanet::GAS_O2] = mAtmosphere[CPlanet::GAS_O2] + '0';
	Atmosphere[CPlanet::GAS_N2] = mAtmosphere[CPlanet::GAS_N2] + '0';
	Atmosphere[CPlanet::GAS_CH4] = mAtmosphere[CPlanet::GAS_CH4] + '0';
	Atmosphere[CPlanet::GAS_H2O] = mAtmosphere[CPlanet::GAS_H2O] + '0';

	Atmosphere[CPlanet::GAS_MAX] = 0;

	return Atmosphere;
}

bool
CRace::set_atmosphere(const char *aAtmosphere)
{
	if (strlen(aAtmosphere) < CPlanet::GAS_MAX) return false;

	mAtmosphere[CPlanet::GAS_H2] = aAtmosphere[CPlanet::GAS_H2] - '0';
	mAtmosphere[CPlanet::GAS_Cl2] = aAtmosphere[CPlanet::GAS_Cl2] - '0';
	mAtmosphere[CPlanet::GAS_CO2] = aAtmosphere[CPlanet::GAS_CO2] - '0';
	mAtmosphere[CPlanet::GAS_O2] = aAtmosphere[CPlanet::GAS_O2] - '0';
	mAtmosphere[CPlanet::GAS_N2] = aAtmosphere[CPlanet::GAS_N2] - '0';
	mAtmosphere[CPlanet::GAS_CH4] = aAtmosphere[CPlanet::GAS_CH4] - '0';
	mAtmosphere[CPlanet::GAS_H2O] = aAtmosphere[CPlanet::GAS_H2O] - '0';

	return true;
}

bool
CRace::set_temperature(int aTemperature)
{
	if (aTemperature > MAX_TEMPERATURE)
	{
		mTemperature = MAX_TEMPERATURE;
		return false;
	}

	if (aTemperature < MIN_TEMPERATURE)
	{
		mTemperature = MIN_TEMPERATURE;
		return false;
	}

	mTemperature = aTemperature;
	return true;
}

bool
CRace::set_gravity(double aGravity)
{
	if (aGravity > MAX_GRAVITY)
	{
		mGravity = MAX_GRAVITY;
		return false;
	}

	if (aGravity < MIN_GRAVITY)
	{
		mGravity = MIN_GRAVITY;
		return false;
	}

	mGravity = aGravity;
	return true;
}

const char *
CRace::html_print_row()
{
	static CString
		Buffer;

	Buffer.clear();

	Buffer.format("Name : %s(%d)<BR>\n", get_name(), get_id());
	Buffer.format("Society : %s<BR>\n", get_society_name());

	Buffer += "Innate Tech : ";
	for(int i=0; i<mInnateTechList.length(); i++)
		Buffer.format("%d ", (int)mInnateTechList.get(i));

	Buffer += "<BR>\n";

	Buffer.format("Air : %c%c%c%c%c%c%c<BR>\n", 
			mAtmosphere[CPlanet::GAS_H2], 
			mAtmosphere[CPlanet::GAS_Cl2],
			mAtmosphere[CPlanet::GAS_CO2], 
			mAtmosphere[CPlanet::GAS_O2],
			mAtmosphere[CPlanet::GAS_N2], 
			mAtmosphere[CPlanet::GAS_CH4],
			mAtmosphere[CPlanet::GAS_H2O]);
	Buffer.format("Gravity : %fG<BR>\n", mGravity);
	Buffer.format("Temperature : %dK<BR>\n", mTemperature);

	Buffer += "Control Model :<BR>\n";
	Buffer += mControlModel.html_in_race();

	Buffer += "Ability : ";
	for(int i=ABILITY_BEGIN; i<ABILITY_MAX; i++)
		if (mAbility.has(i))
			Buffer.format("%s ", AbilityName[i]);

	Buffer += "<BR>\n";

	Buffer.format( "Description : <BR>\n%s", get_description() );

	return (const char*)Buffer;
}

char *
CRace::race_control_model_information_html()
{
	static CString
		Info;
	Info.clear();

	if (mControlModel.is_empty())
	{
		Info = "<TABLE WIDTH=\"500\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		Info += "<TR>\n";

		Info.format("<TD>%s</TD>\n", GETTEXT("Your race has no racial modifiers."));

		Info += "</TR>\n";
		Info += "</TABLE>\n";
	}

	Info = "<TABLE WIDTH=\"500\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Info += "<TR>\n";

	int
		Column = 0;
	for (int i=CControlModel::CM_ENVIRONMENT ; i<CControlModel::CM_MAX ; i++)
	{
		if (mControlModel.get_value(i) == 0) continue;

		if (Column == 2)
		{
			Info += "</TR>\n";
			Info += "<TR>\n";
			Column = 0;
		}

		Info += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Info.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>",
					CControlModel::get_cm_description(i));
		Info += "</TD>\n";

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
					mControlModel.get_value(i));

		Column++;
	}

	if (Column == 1)
	{
		Info += "<TD CLASS=\"tabletxt\" WIDTH=\"250\" COLSPAN=\"2\">&nbsp;</TD>\n";
	}

	Info += "</TR>\n";
	Info += "</TABLE>\n";

	return (char *)Info;
}

