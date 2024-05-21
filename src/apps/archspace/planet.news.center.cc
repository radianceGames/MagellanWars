#include "util.h"
#include "news.h"
#include "planet.h"
#include <libintl.h>
#include <cstdlib>

CPlanetNewsCenter::CPlanetNewsCenter()
{
	mPlanet = NULL;

	mPopulation = 0;
	mFactory = 0;
	mMilitaryBase = 0;
	mResearchLab = 0;
	mAtmosphere[0] = '\0';
	mTemperature = 0;
	mResource = 0;
	mGravity = 0;
}

CPlanetNewsCenter::~CPlanetNewsCenter()
{
}

void
CPlanetNewsCenter::init(CPlanet *aPlanet)
{
	mPlanet = aPlanet;

	mPopulation = aPlanet->get_population();

	CResource &
		Building = aPlanet->get_building();

	mFactory = Building.get(BUILDING_FACTORY);
	mMilitaryBase = Building.get(BUILDING_MILITARY_BASE);
	mResearchLab = Building.get(BUILDING_RESEARCH_LAB);

	mAtmosphere[0] = '\0';

	mTemperature = aPlanet->get_temperature();
	mResource = aPlanet->get_resource();
	mGravity = aPlanet->get_gravity();
}

const char *
CPlanetNewsCenter::get_query()
{
	if (mStoreFlag.is_empty()) return "";
	
	static CString Query;

	Query = " ";

#define STORE(x, y, z) if (mStoreFlag.has(x)) Query.format(y, z)

	STORE(STORE_POPULATION, ", news_population = %d", mPopulation);
	STORE(STORE_FACTORY, ", news_factory = %d", mFactory);
	STORE(STORE_MILITARY_BASE, ", news_military_base = %d", mMilitaryBase);
	STORE(STORE_RESEARCH_LAB, ", news_research_lab = %d", mResearchLab);

	mStoreFlag.clear();

	return (char*)Query;
}

const char *
CPlanetNewsCenter::generate()
{
	static CString Buffer;

//	SLOG("PlanetNewCenter mPlanet:%x", (int)mPlanet);

	Buffer.clear();

	int Population = mPlanet->get_population() - mPopulation;

	CString Old, New, Diff;

	if (Population)
	{

		Old = dec2unit(mPopulation);
		New = dec2unit(mPlanet->get_population());

		Buffer += "<TR>\n"
				"<TD CLASS=\"maintext\" COLSPAN=2>\n"
				"<CENTER>\n";
	
		if (Population < 0)
		{
			Diff = dec2unit(-Population);
			Buffer.format(
				GETTEXT("Population has been decreased by %1$s k.(from %2$s k to %3$s k)"),
					(char*)Diff, (char*)Old, (char*)New);
			Buffer += "<BR>\n";
		}
		else 
		{
			Diff = dec2unit(Population);
			Buffer.format(
				GETTEXT("Population has been increased by %1$s k.(from %2$s k to %3$s k)"),
					(char*)Diff, (char*)Old, (char*)New);
			Buffer += "<BR>\n";
		}

		Buffer += "</CENTER>\n"
				"</TD>\n"
				"</TR>\n";

		set_population(mPlanet->get_population());
	}

	int Current = mPlanet->mBuilding.get(BUILDING_FACTORY);
	int Factory = Current - mFactory;

	if (Factory)
	{
		Diff = dec2unit(abs(Factory));
		New = dec2unit(Current);

		Buffer += "<TR>\n"
				"<TD CLASS=\"maintext\" COLSPAN=2>\n"
				"<CENTER>\n";

		if (Factory < 0)
		{
			if (Factory == -1)
			{
				Buffer.format(GETTEXT("1 factory has been destroyed.(%1$s unit%2$s now)"),
								(char *)New, plural(Current));
			}
			else
			{
				Buffer.format(GETTEXT("%1$s factories have been destroyed.(%2$s unit%3$s now)"),
								(char *)Diff, (char *)New, plural(Current));
			}
		}
		else 
		{
			if (Factory == 1)
			{
				Buffer.format(GETTEXT("1 factory has been built.(%1$s unit%2$s now)"),
								(char *)New, plural(Current));
			}
			else
			{
				Buffer.format(GETTEXT("%1$s factories have been built.(%2$s unit%3$s now)"),
								(char *)Diff, (char *)New, plural(Current));
			}
		}
		Buffer += "<BR>\n";

		Buffer += "</CENTER>\n"
				"</TD>\n"
				"</TR>\n";

		set_factory(Current);
	}

	Current = mPlanet->mBuilding.get(BUILDING_MILITARY_BASE);
	int MilitaryBase = Current - mMilitaryBase;

	if (MilitaryBase)
	{
		Diff = dec2unit(abs(MilitaryBase));
		New = dec2unit(Current);

		Buffer += "<TR>\n"
				"<TD CLASS=\"maintext\" COLSPAN=2>\n"
				"<CENTER>\n";

		if (MilitaryBase < 0)
		{
			if (MilitaryBase == -1)
			{
				Buffer.format(GETTEXT("1 military base has been destroyed.(%1$s unit%2$s now)"),
								(char *)New, plural(Current));
			}
			else
			{
				Buffer.format(GETTEXT("%1$s military bases have been destroyed.(%2$s unit%3$s now)"),
								(char *)Diff, (char *)New, plural(Current));
			}
		}
		else 
		{
			if (MilitaryBase == 1)
			{
				Buffer.format(GETTEXT("1 military base has been built.(%1$s unit%2$s now)"),
								(char *)New, plural(Current));
			}
			else
			{
				Buffer.format(GETTEXT("%1$s military bases have been built.(%2$s unit%3$s now)"),
								(char *)Diff, (char *)New, plural(Current));
			}
		}
		Buffer += "<BR>\n";

		Buffer += "</CENTER>\n"
				"</TD>\n"
				"</TR>\n";

		set_military_base(Current);
	}

	Current = mPlanet->mBuilding.get(BUILDING_RESEARCH_LAB);
	int ResearchLab = Current - mResearchLab;

	if (ResearchLab)
	{
		Diff = dec2unit(abs(ResearchLab));
		New = dec2unit(Current);

		Buffer += "<TR>\n"
				"<TD CLASS=\"maintext\" COLSPAN=2>\n"
				"<CENTER>\n";

		if (ResearchLab < 0)
		{
			if (ResearchLab == -1)
			{
				Buffer.format(GETTEXT("1 research lab. has been destroyed.(%1$s unit%2$s now)"),
								(char *)New, plural(Current));
			}
			else
			{
				Buffer.format(GETTEXT("%1$s research labs. have been destroyed.(%2$s unit%3$s now)"),
								(char *)Diff, (char *)New, plural(Current));
			}
		}
		else 
		{
			if (ResearchLab == 1)
			{
				Buffer.format(GETTEXT("1 research lab. has been built.(%1$s unit%2$s now)"),
								(char *)New, plural(Current));
			}
			else
			{
				Buffer.format(GETTEXT("%1$s research labs. have been built.(%2$s unit%3$s now)"),
								(char *)Diff, (char *)New, plural(Current));
			}
		}
		Buffer += "<BR>\n";

		Buffer += "</CENTER>\n"
				"</TD>\n"
				"</TR>\n";

		set_research_lab(Current);
	}

	return (char*)Buffer;
}
