#include "news.h"
#include <cstdio>
#include "player.h"
#include <libintl.h>
#include <cctype>
#include "archspace.h"
#include "game.h"

CNewsCenter::CNewsCenter()
{
	mTurn = -1;
	mProduction = 0;
	mResearch = 0;
	mPopulation = 0;
}

CNewsCenter::~CNewsCenter()
{
}

const char*
CNewsCenter::get_query()
{
	if (mStoreFlag.is_empty()) return " ";

	static CString Query;

	Query.clear();

#define STORE(x, y, z) if (mStoreFlag.has(x)) Query.format(y, z)

	STORE(STORE_TURN, ", news_turn = %d", mTurn);
	STORE(STORE_PRODUCTION, ", news_production = %d", mProduction);
	STORE(STORE_RESEARCH, ", news_research = %d", mResearch);
	STORE(STORE_POPULATION, ", news_population = %d", mPopulation);

	save_news();

	mStoreFlag.clear();

	return (char*)Query;
}

void
CNewsCenter::save_news()
{
	FILE
		*fp;
	CString
		FileNameBase,
		NewsFile;

	FileNameBase = mOwner->get_news_file_name();

	NewsFile.clear();
	NewsFile.format( "%s.ability", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open news file %s for save", (char*)NewsFile );
	} else {
		fputs( mAbility.get_string(ABILITY_MAX), fp );
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.tech", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open news file %s for save", (char*)NewsFile );
	} else {
		if( mKnownTechNews.get_query() )
			fputs( mKnownTechNews.get_query(), fp );
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.planet", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open news file %s for save", (char*)NewsFile );
	} else {
		if( mPlanetNews.get_query() )
			fputs( mPlanetNews.get_query(), fp );
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.project", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open news file %s for save", (char*)NewsFile );
	} else {
		if( mPurchasedProjectNews.get_query() )
			fputs( mPurchasedProjectNews.get_query(), fp );
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.admiral", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open news file %s for save", (char*)NewsFile );
	} else {
		if( mAdmiralNews.get_query() )
			fputs( mAdmiralNews.get_query(), fp );
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.time", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open news file %s for save", (char*)NewsFile );
	}
	else
	{
		if (mTimeNews.length() == 0)
		{
			fputs(" ", fp);
		}
		else
		{
			for (int i=0 ; i<mTimeNews.length() ; i++)
			{
				CQuery *
					Query = (CQuery *)mTimeNews.CSortedList::get(i);

				if (Query->get_name() == NULL || Query->get_value() == NULL) continue;

				fprintf(fp, "%s", Query->get_name());

				CString
					Temp;
				Temp.format(" %d", strlen(Query->get_value()));
				fprintf(fp, "%s", (char *)Temp);

				fprintf(fp, "%s", " ");
				fprintf(fp, "%s", Query->get_value());
			}
		}
	}

	fclose(fp);
}

void
CNewsCenter::known_tech_news(CKnownTech* aKnownTech)
{
	mStoreFlag += STORE_TECH;
	mKnownTechNews.known_tech_news(aKnownTech->get_name_with_level());
}

void
CNewsCenter::new_planet_news(CPlanet* aPlanet)
{
	mStoreFlag += STORE_PLANET;
	mPlanetNews.new_planet_news(aPlanet->get_name());
}

void
CNewsCenter::purchased_project_news(CPurchasedProject* aProject)
{
	mStoreFlag += STORE_PROJECT;
	mPurchasedProjectNews.purchased_project_news(aProject->get_name());
}

void
CNewsCenter::discovered_project_news(CProject* aProject)
{
	//mDiscoveredProjectNews.discovered_project_news(aProject->get_name());
}

void
CNewsCenter::admiral_news(CAdmiral* aAdmiral)
{
	mStoreFlag += STORE_ADMIRAL;
	mAdmiralNews.admiral_news(aAdmiral->get_name());
}

const char*
CNewsCenter::get_current_time()
{
	static char TimeStr[40];
	time_t Time;
	struct tm *TimePtr;

	time(&Time);
	TimePtr = localtime(&Time);
	strftime(TimeStr, 40, "%Y%m%d%H%M", TimePtr);

	return TimeStr;
}

void
CNewsCenter::time_news(const char* aNewsString)
{
	mStoreFlag += STORE_TIME_NEWS;

	const char *
		Time = get_current_time();
	CString
		News;
	News = mTimeNews.get_value(Time);

	if (News.length() > 0) News += "\n";

	News += aNewsString;

	mTimeNews.set_value(Time, (char *)News);
}

const char*
CNewsCenter::time_news()
{
	static CString News;
	struct tm Time;
	char Buf[50];

	News.clear();

	if (!mTimeNews.length()) return " ";

	for(int i=mTimeNews.length()-1; i>=0; i--)
	{
		News += "<TR VALIGN=TOP>\n";

		News += "<TD>\n";
		News += "<TABLE>\n";

		CQuery *
			Query = (CQuery *)mTimeNews.CSortedList::get(i);
		strptime(Query->get_name(), "%Y%m%d%H%M", (struct tm*)&Time);
		strftime(Buf, 50, "%Y/%m/%d/%H:%M", (struct tm*)&Time);

		News += "<TD WIDTH=96 CLASS=\"maintext\" VALIGN=TOP>\n";
		News.format("<FONT COLOR=\"#FFFFFF\" SIZE=2>%s</FONT>", Buf);
		News += "</TD>\n";

		CString Buffer = Query->get_value();
		Buffer.nl2br();
		News.format("<TD WIDTH=424 CLASS=\"maintext\">%s</TD>\n", (char *)Buffer);

		News += "</TABLE>\n";
		News += "</TD>\n";

		News += "</TR>\n";
	}

	mStoreFlag += STORE_TIME_NEWS;
	mTimeNews.remove_all();

	return (char*)News;
}

bool
CNewsCenter::set_owner(CPlayer *aOwner)
{
	if (aOwner == NULL) return false;

	mOwner = aOwner;
	return true;
}

const char*
CNewsCenter::generate()
{
	static CString Buffer;

	Buffer.clear();

	// turn
	int Turn = mOwner->mTurn - mTurn;
	if (!Turn) return NULL;
	Buffer = "<TR>\n";
	Buffer += "<TD CLASS=\"speakernews\" COLSPAN=2>\n";
	Buffer += "<CENTER>\n";
	Buffer += GETTEXT("Summary");
	Buffer += "<BR>\n";

	CString
		TurnString,
		OwnerTurnString;
	TurnString = dec2unit(mTurn);
	OwnerTurnString = dec2unit(mOwner->mTurn);

	Buffer.format(GETTEXT("From %1$s to %2$s (in turns)"),
					(char *)TurnString, (char *)OwnerTurnString);
	Buffer += "</CENTER>\n";
	Buffer += "</TD>\n";
	Buffer += "</TR>\n";

	set_turn(mOwner->mTurn);

	// production
	if ((mOwner->mProduction - mProduction) != 0)
	{
		Buffer += "<TR>\n";
		Buffer += "<TD CLASS=\"maintext\" COLSPAN=2>\n";
		Buffer += "<CENTER>\n";
		Buffer.format(GETTEXT("PP Change : %1$s"),
					dec2unit(mOwner->mProduction - mProduction));
		Buffer += "<BR>\n";
		Buffer += "</CENTER>\n";
		Buffer += "</TD>\n";
		Buffer += "</TR>\n";
		set_production(mOwner->mProduction);
	}

	// research
	if ((mOwner->mResearch - mResearch) != 0)
	{
		Buffer += "<TR>\n";
		Buffer += "<TD CLASS=\"maintext\" COLSPAN=2>\n";
		Buffer += "<CENTER>\n";
		Buffer.format(GETTEXT("RP Change : %1$s"),
					dec2unit(mOwner->mResearch - mResearch));
		Buffer += "<BR>\n";
		Buffer += "</CENTER>\n";
		Buffer += "</TD>\n";
		Buffer += "</TR>\n";
		set_research(mOwner->mResearch);
	}

	int Population = mOwner->calc_population();
	if ((Population - mPopulation) != 0)
	{
		Buffer += "<TR>\n";
		Buffer += "<TD CLASS=\"maintext\" COLSPAN=2>\n";
		Buffer += "<CENTER>\n";
		Buffer.format(GETTEXT("Population Change : %1$s"),
					dec2unit(Population - mPopulation));
		Buffer += "</CENTER>\n";
		Buffer += "<BR>\n";
		Buffer += "</TD>\n";
		Buffer += "</TR>\n";
		set_population(Population);
	}

	// tech
	if (!mKnownTechNews.empty())
	{
		Buffer += "<TR>\n";
		Buffer += "<TD COLSPAN=2 CLASS=\"maintext\" WIDTH=520>\n";
		Buffer += "<CENTER>\n";
		Buffer += mKnownTechNews.get();
		Buffer += "</CENTER>\n";
		Buffer += "</TD>\n";
		Buffer += "</TR>\n";

		mStoreFlag += STORE_TECH;
		//mKnownTechNews.clear();
	}

	// planet
	if (!mPlanetNews.empty())
	{
		Buffer += "<TR>\n";
		Buffer += "<TD COLSPAN=2 CLASS=\"maintext\" WIDTH=520>\n";
		Buffer += "<CENTER>\n";
		Buffer += mPlanetNews.get();
		Buffer += "</CENTER>\n";
		Buffer += "</TD>\n";
		Buffer += "</TR>\n";

		mStoreFlag += STORE_PLANET;
		//mPlanetNews.clear();
	}

	// project
	if (!mPurchasedProjectNews.empty())
	{
		Buffer += "<TR>\n";
		Buffer += "<TD COLSPAN=2 CLASS=\"maintext\" WIDTH=520>\n";
		Buffer += "<CENTER>\n";
		Buffer += mPurchasedProjectNews.get();
		Buffer += "</CENTER>\n";
		Buffer += "</TD>\n";
		Buffer += "</TR>\n";

		mStoreFlag += STORE_PROJECT;
		//mPurchasedProjectNews.clear();
	}

	// admiral
	if (!mAdmiralNews.empty())
	{
		Buffer += "<TR>\n";
		Buffer += "<TD COLSPAN=2 CLASS=\"maintext\" WIDTH=520>\n";
		Buffer += "<CENTER>\n";
		Buffer += mAdmiralNews.get();
		Buffer += "</CENTER>\n";
		Buffer += "</TD>\n";
		Buffer += "</TR>\n";

		mStoreFlag += STORE_ADMIRAL;
		//mAdmiralNews.clear();
	}

	Buffer += time_news();

	return (char*)Buffer;
}
void
CNewsCenter::clear_news()
{

	mStoreFlag += STORE_TECH;
	mStoreFlag += STORE_PLANET;
	mStoreFlag += STORE_PROJECT;
	mStoreFlag += STORE_ADMIRAL;
	mKnownTechNews.clear();
	mPlanetNews.clear();
	mPurchasedProjectNews.clear();
	mAdmiralNews.clear();

}
bool
CNewsCenter::isViewed()
{
	return (mKnownTechNews.mViewed || mPlanetNews.mViewed || mPurchasedProjectNews.mViewed || mAdmiralNews.mViewed);
}
void
CNewsCenter::set_time_news(FILE *aNewsFile)
{
	if (aNewsFile == NULL) return;

	char
		Buffer[1024 + 1];

	while (fgets(Buffer, 13, aNewsFile))
	{
		CString
			NewsTime,
			NewsString;

		if (!strcmp(Buffer, " ")) return;

		int
			i;
		for (i=0 ; i<12 ; i++)
		{
			if (!isdigit(Buffer[i])) break;
		}
		if (i<12) break;

		NewsTime = Buffer;

		int
			Length,
			Done = 0;
		fscanf(aNewsFile, " %d", &Length);

		fread(Buffer, 1, 1, aNewsFile);

		while (Done < Length)
		{
			memset(Buffer, '\0', sizeof(Buffer));

			int
				Temp;
			if (Length - Done > 1024)
			{
				Temp = fread(Buffer, 1024, 1, aNewsFile);
			}
			else
			{
				Temp = fread(Buffer, Length - Done, 1, aNewsFile);
			}

			Done += strlen(Buffer);
			NewsString += Buffer;
			if (strlen(Buffer)<1) break;
		}
		mTimeNews.set_value((char *)NewsTime, (char *)NewsString);
	}
}
