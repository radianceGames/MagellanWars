#include "ctrl.model.h"
#include "util.h"
#include "define.h"
#include <libintl.h>

TZone gControlModelZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CControlModel),
	0,
	0,
	NULL,
	"Zone CControlModel"
};

char *AbilityName[] = {
	"",
	"No Breath",
	"Terraform Gravity",
	"No Spy",
	"Dead",
	"Complete Dead",
	"PSI",
	"Enhanced PSI",
	"Duke",
	"Marquis",
	"Earl",
	"Viscount",
	"Baron",
	"Rogue Duke",
	"Rogue Marquis",
	"Rogue Earl",
	"Rogue Viscount",
	"Rogue Baron",
	"Genetic Engineering Specialist",
	"Fragile Mind Structure",
	"Great Spawning Pool",
	"Fast Maneuver",
	"Stealth",
	"Scavenger",
	"Information Network Specialist",
	"Efficient Investment",
	"Downloadable Commander Experience",
	"Asteroid Management",
	"Diplomat",
	"Trained Mind",
	"Pacifist",
	"Fanatic Fleet",
	"High Morale",
	"Stealth Pirate",
	"Tactical Mastery",
	"Fanatical Recruiting",
	"Militaristic Dominance",
	"Advanced Battle Processing",
	"Hive Ship Yard",
	"Enhanced PSI Weaponry",
	"Natural Stealth",
	"Stealthed Ambush",
	"Temporal Mastery",
	"Catalysm"
};

CControlModel::CControlModel()
{
	clear();
}

void CControlModel::clear()
{
	mEnvironment = 0;
	mGrowth = 0;
	mResearch = 0;
	mProduction = 0;
	mMilitary = 0;
	mSpy = 0;
	mCommerce = 0;
	mEfficiency = 0;
	mGenius = 0;
	mDiplomacy = 0;
	mFacilityCost = 0;
}

CControlModel::CControlModel(CControlModel& aControlModel)
{
	mEnvironment = aControlModel.mEnvironment;
	mGrowth = aControlModel.mGrowth;
	mResearch = aControlModel.mResearch;
	mProduction = aControlModel.mProduction;
	mMilitary = aControlModel.mMilitary;
	mSpy = aControlModel.mSpy;
	mCommerce = aControlModel.mCommerce;
	mEfficiency = aControlModel.mEfficiency;
	mGenius = aControlModel.mGenius;
	mDiplomacy = aControlModel.mDiplomacy;
	mFacilityCost = aControlModel.mFacilityCost;
}

CControlModel::~CControlModel()
{
}

int
CControlModel::get_environment()
{
	if (mEnvironment > -1) return -1;
	if (mEnvironment < -10) return -10;

	return mEnvironment;
}

int
CControlModel::get_growth()
{
	if (mGrowth > 10) return 10;
	if (mGrowth < -5) return -5;

	return mGrowth;
}

int
CControlModel::get_production()
{
	if (mProduction < -3) return -3;

	return mProduction;
}

int
CControlModel::get_research()
{
	if (mResearch < -9) return -9;

	return mResearch;
}

int
CControlModel::get_military()
{
	if (mMilitary > 10) return 10;
	if (mMilitary < -6) return -6;

	return mMilitary;
}

int
CControlModel::get_uncapped_military()
{
	return mMilitary;
}

int
CControlModel::get_spy()
{
	return mSpy;
}

int
CControlModel::get_commerce()
{
	return mCommerce;
}

int
CControlModel::get_efficiency()
{
	return mEfficiency;
}

int
CControlModel::get_genius()
{
	return mGenius;
}

int
CControlModel::get_diplomacy()
{
	return mDiplomacy;
}

int
CControlModel::get_facility_cost()
{
	if (mFacilityCost > 5) return 5;
	return mFacilityCost;
}

int
CControlModel::get_value(int aEnvironment)
{
	switch (aEnvironment)
	{
		case CM_ENVIRONMENT:
			return mEnvironment;

		case CM_GROWTH:
			return mGrowth;

		case CM_RESEARCH:
			return mResearch;

		case CM_PRODUCTION:
			return mProduction;

		case CM_MILITARY:
			return mMilitary;

		case CM_SPY:
			return mSpy;

		case CM_COMMERCE:
			return mCommerce;

		case CM_EFFICIENCY:
			return mEfficiency;

		case CM_GENIUS:
			return mGenius;

		case CM_DIPLOMACY:
			return mDiplomacy;

		case CM_FACILITY_COST:
			return mFacilityCost;

		default:
			return -666;
	}
}

CControlModel&
CControlModel::operator+=(const CControlModel &aControlModel)
{
	change_environment(aControlModel.mEnvironment);
	change_growth(aControlModel.mGrowth);
	change_research(aControlModel.mResearch);
	change_production(aControlModel.mProduction);
	change_military(aControlModel.mMilitary);
	change_spy(aControlModel.mSpy);
	change_commerce(aControlModel.mCommerce);
	change_efficiency(aControlModel.mEfficiency);
	change_genius(aControlModel.mGenius);
	change_diplomacy(aControlModel.mDiplomacy);
	change_facility_cost(aControlModel.mFacilityCost);

	return *this;
}

CControlModel&
CControlModel::operator-=(const CControlModel &aControlModel)
{
	change_environment(-aControlModel.mEnvironment);
	change_growth(-aControlModel.mGrowth);
	change_research(-aControlModel.mResearch);
	change_production(-aControlModel.mProduction);
	change_military(-aControlModel.mMilitary);
	change_spy(-aControlModel.mSpy);
	change_commerce(-aControlModel.mCommerce);
	change_efficiency(-aControlModel.mEfficiency);
	change_genius(-aControlModel.mGenius);
	change_diplomacy(-aControlModel.mDiplomacy);
	change_facility_cost(-aControlModel.mFacilityCost);

	return *this;
}

CControlModel&
CControlModel::operator=(const CControlModel &aControlModel)
{
	mEnvironment = aControlModel.mEnvironment;
	mGrowth = aControlModel.mGrowth;
	mResearch = aControlModel.mResearch;
	mProduction = aControlModel.mProduction;
	mMilitary = aControlModel.mMilitary;
	mSpy = aControlModel.mSpy;
	mCommerce = aControlModel.mCommerce;
	mEfficiency = aControlModel.mEfficiency;
	mGenius = aControlModel.mGenius;
	mDiplomacy = aControlModel.mDiplomacy;
	mFacilityCost = aControlModel.mFacilityCost;

	return *this;
}

bool
CControlModel::set(char *aType, int aValue)
{
	if (!strcasecmp("Environment", aType))
		set_environment(aValue);
	else if (!strcasecmp("Growth", aType))
		set_growth(aValue);
	else if (!strcasecmp("Research", aType))
		set_research(aValue);
	else if (!strcasecmp("Production", aType))
		set_production(aValue);
	else if (!strcasecmp("Military", aType))
		set_military(aValue);
	else if (!strcasecmp("Spy", aType))
		set_spy(aValue);
	else if (!strcasecmp("Commerce", aType))
		set_commerce(aValue);
	else if (!strcasecmp("Efficiency", aType))
		set_efficiency(aValue);
	else if (!strcasecmp("Genius", aType))
		set_genius(aValue);
	else if (!strcasecmp("Diplomacy", aType))
		set_diplomacy(aValue);
	else if (!strcasecmp("Facility Cost", aType))
		set_facility_cost(aValue);
	else return false;

	return true;
}

bool
CControlModel::get(char *aType, int &aValue)
{
	if (!strcasecmp("Environment", aType))
		aValue = get_environment();
	else if (!strcasecmp("Growth", aType))
		aValue = get_growth();
	else if (!strcasecmp("Research", aType))
		aValue = get_research();
	else if (!strcasecmp("Production", aType))
		aValue = get_production();
	else if (!strcasecmp("Military", aType))
		aValue = get_military();
	else if (!strcasecmp("Spy", aType))
		aValue = get_spy();
	else if (!strcasecmp("Commerce", aType))
		aValue = get_commerce();
	else if (!strcasecmp("Efficiency", aType))
		aValue = get_efficiency();
	else if (!strcasecmp("Genius", aType))
		aValue = get_genius();
	else if (!strcasecmp("Diplomacy", aType))
		aValue = get_diplomacy();
	else if (!strcasecmp("Facility Cost", aType))
		aValue = get_facility_cost();
	else return false;

	return true;
}

bool
CControlModel::change(char *aType, int aValue)
{
	if (!strcasecmp("Environment", aType))
		change_environment(aValue);
	else if (!strcasecmp("Growth", aType))
		change_growth(aValue);
	else if (!strcasecmp("Research", aType))
		change_research(aValue);
	else if (!strcasecmp("Production", aType))
		change_production(aValue);
	else if (!strcasecmp("Military", aType))
		change_military(aValue);
	else if (!strcasecmp("Spy", aType))
		change_spy(aValue);
	else if (!strcasecmp("Commerce", aType))
		change_commerce(aValue);
	else if (!strcasecmp("Efficiency", aType))
		change_efficiency(aValue);
	else if (!strcasecmp("Genius", aType))
		change_genius(aValue);
	else if (!strcasecmp("Diplomacy", aType))
		change_diplomacy(aValue);
	else if (!strcasecmp("Facility Cost", aType))
		change_facility_cost(aValue);
	else return false;

	return true;
}

const char*
CControlModel::html_in_race()
{
	static CString
		Buffer;
	Buffer.clear();

	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_ENVIRONMENT),
					mEnvironment);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_GROWTH),
					mGrowth);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_RESEARCH),
					mResearch);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_PRODUCTION),
					mProduction);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_MILITARY),
					mMilitary);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_SPY),
					mSpy);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_COMMERCE),
					mCommerce);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_EFFICIENCY),
					mEfficiency);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_GENIUS),
					mGenius);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_DIPLOMACY),
					mDiplomacy);
	Buffer.format("%s = %d<BR>\n",
					get_cm_description(CM_FACILITY_COST),
					mFacilityCost);

	return (char*)Buffer;
}

const char*
CControlModel::debug_info()
{
	static CString
		DebugString;

	DebugString.clear();
	DebugString.format(
				"Env:%d Grwth:%d Rsch:%d Prod:%d Mil:%d "
				"Spy:%d Cmmrc:%d Eff:%d Gen:%d Dpl:%d FCst:%d",
			mEnvironment, mGrowth, mResearch, mProduction,
			mMilitary, mSpy, mCommerce, mEfficiency, mGenius,
			mDiplomacy, mFacilityCost);

	return (char*)DebugString;
}

bool
CControlModel::is_empty()
{
	for (int i=CM_ENVIRONMENT ; i<CM_MAX ; i++)
	{
		if (get_value(i)) return false;
	}

	return true;
}

const char*
CControlModel::get_environment_description(int aEnvironment)
{
	if (aEnvironment >= -1)
		return GETTEXT("Suitable");

	switch(aEnvironment)
	{
		case -2: return GETTEXT("Uncomfortable");
		case -3: return GETTEXT("Inhospitable");
		case -4: return GETTEXT("Unfriendly");
		case -5: return GETTEXT("Forbidding");
		case -6: return GETTEXT("Grim");
		case -7: return GETTEXT("Brutal");
		case -8: return GETTEXT("Merciless");
		case -9: return GETTEXT("Hostile");
	};

	return GETTEXT("Desolate");
}

char *
CControlModel::get_cm_description(int aSection)
{
	switch (aSection)
	{
		case CM_ENVIRONMENT:
			return GETTEXT("Environment");

		case CM_GROWTH:
			return GETTEXT("Growth");

		case CM_RESEARCH:
			return GETTEXT("Research");

		case CM_PRODUCTION:
			return GETTEXT("Production");

		case CM_MILITARY:
			return GETTEXT("Military");

		case CM_SPY:
			return GETTEXT("Spy");

		case CM_COMMERCE:
			return GETTEXT("Commerce");

		case CM_EFFICIENCY:
			return GETTEXT("Efficiency");

		case CM_GENIUS:
			return GETTEXT("Genius");

		case CM_DIPLOMACY:
			return GETTEXT("Diplomacy");

		case CM_FACILITY_COST:
			return GETTEXT("Facility Cost");

		default:
			return NULL;
	}
}

char *
CControlModel::get_effects_string()
{
	static CString
		Effect;
	Effect.clear();

	bool
		AnyEffect = false;
	for (int i=CM_ENVIRONMENT ; i<CM_MAX ; i++)
	{
		int
			CM = get_value(i);
		if (CM > 0)
		{
			if (AnyEffect) Effect += ", ";
			Effect.format("%s %s", get_cm_description(i), integer_with_sign(CM));
			AnyEffect = true;
		}
	}

	if (!AnyEffect)
	{
		return NULL;
	} else
	{
		return (char *)Effect;
	}
}

char *
CControlModel::get_effects_string_for_html()
{
	static CString
		Effect;
	Effect.clear();

	bool
		AnyEffect = false;
	int
		Count = 0;

	for (int i=CM_ENVIRONMENT ; i<CM_MAX ; i++)
	{
		int
			CM = get_value(i);
		if (CM)
		{
			if (AnyEffect) Effect += ", ";
			if (Count == 2)
			{
				Effect += "<BR>\n";
				Count = 0;
			}

			if (CM>0) Effect.format("%s+%d", get_cm_description(i), CM);
			else Effect.format("%s%d", get_cm_description(i), CM);

			AnyEffect = true;
			Count++;
		}
	}

	if (!AnyEffect)
	{
		return NULL;
	} else
	{
		return (char *)Effect;
	}
}

