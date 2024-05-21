#include <libintl.h>
#include "rank.h"
#include "player.h"
#include "council.h"
#include "archspace.h"
#include "game.h"
#include "race.h"

TZone gRankZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CRank),
	0,
	0,
	NULL,
	"Zone CRank"
};

CRank::CRank()
{
	mID = 0;
	mPower = 0;
	mNumber = 0;
}

CRank::~CRank()
{
}

bool
CRank::change_power(int aPower)
{
	if (aPower == 0) return true;

	if (aPower > 0)
	{
		if ((long long int)aPower >
			(long long int)MAX_POWER - (long long int)mPower)
		{
			mPower = MAX_POWER;
		}
		else
		{
			mPower += aPower;
		}
	}
	else
	{
		if (mPower + aPower > MAX_POWER)
		{
			mPower = MAX_POWER;
		}
		else if (mPower < -aPower)
		{
			mPower = 0;
		}
		else
		{
			mPower += aPower;
		}
	}

	return true;
}

bool
CRank::initialize_from_player(CPlayer *aPlayer, int aType)
{
	if (!aPlayer) return false;
	if (aType < PLAYER_OVERALL || aType > PLAYER_TECH) return false;

	switch (aType)
	{
		case PLAYER_OVERALL :
		{
			mID = aPlayer->get_game_id();
			mPower = aPlayer->get_power();
		}
		break;

		case PLAYER_FLEET :
		{
			mID = aPlayer->get_game_id();
			CFleetList *
				FleetList = aPlayer->get_fleet_list();
			mPower = FleetList->get_power();
			mNumber = FleetList->length();
		}
		break;

		case PLAYER_PLANET :
		{
			mID = aPlayer->get_game_id();
			CPlanetList *
				PlanetList = aPlayer->get_planet_list();
			mPower = PlanetList->get_power();
			mNumber = PlanetList->length();
		}
		break;

		case PLAYER_TECH :
		{
			mID = aPlayer->get_game_id();
			CKnownTechList *
				KnownTechList = aPlayer->get_tech_list();
			mPower = KnownTechList->get_power();
			mNumber = KnownTechList->length();
		}

		case PLAYER_RATING :
		{
			mID = aPlayer->get_game_id();
			mNumber = aPlayer->get_power();
		}
		break;

		default :
			return false;
	}

	return true;
}

bool
CRank::initialize_from_council(CCouncil *aCouncil, int aType)
{
	if (!aCouncil) return false;
	if (aType < COUNCIL_OVERALL || aType > COUNCIL_DIPLOMATIC) return false;

	switch (aType)
	{
		case COUNCIL_OVERALL :
		{
			mID = aCouncil->get_id();

			mPower = 0;

			CPlayerList *
				Members = aCouncil->get_members();
			for (int i=0 ; i<Members->length() ; i++)
			{
				CPlayer *
					Member = (CPlayer *)Members->get(i);
				mPower += Member->get_power();
			}
		}
		break;

		case COUNCIL_FLEET :
		{
			mID = aCouncil->get_id();

			mPower = 0;
			mNumber = 0;

			CPlayerList *
				Members = aCouncil->get_members();
			for (int i=0 ; i<Members->length() ; i++)
			{
				CPlayer *
					Member = (CPlayer *)Members->get(i);
				CFleetList *
					FleetList = Member->get_fleet_list();
				mPower += FleetList->get_power();
				mNumber += FleetList->length();
			}
		}
		break;

		case COUNCIL_PLANET :
		{
			mID = aCouncil->get_id();

			mPower = 0;
			mNumber = 0;

			CPlayerList *
				Members = aCouncil->get_members();
			for (int i=0 ; i<Members->length() ; i++)
			{
				CPlayer *
					Member = (CPlayer *)Members->get(i);
				CPlanetList *
					PlanetList = Member->get_planet_list();
				mPower += PlanetList->get_power();
				mNumber += PlanetList->length();
			}
		}
		break;

		case COUNCIL_DIPLOMATIC :
		{
			mID = aCouncil->get_id();

			mPower = 0;
			mNumber = 0;

			CPlayerList *
				Members = aCouncil->get_members();
			for (int i=0 ; i<Members->length() ; i++)
			{
				CPlayer *
					Member = (CPlayer *)Members->get(i);
				CKnownTechList *
					KnownTechList = Member->get_tech_list();
				mPower += KnownTechList->get_power();
				mNumber += KnownTechList->length();
			}
		}
		break;

		default :
			return false;
	}

	return true;
}

CRankTable::CRankTable()
{
	mRankType = -1;
	mUpdateTime = 0;
}

CRankTable::~CRankTable()
{
	remove_all();
}

bool
CRankTable::free_item(TSomething aItem)
{
	CRank *
		Rank = (CRank *)aItem;

	if (Rank == NULL) return false;

	delete Rank;

	return true;
}

int
CRankTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CRank
		*Rank1 = (CRank *)aItem1,
		*Rank2 = (CRank *)aItem2;

	if (Rank1->get_power() > Rank2->get_power()) return -1;
	if (Rank1->get_power() < Rank2->get_power()) return 1;
	return 0;
}

int
CRankTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CRank *
		Rank = (CRank *)aItem;

	if (Rank->get_power() > (int)aKey) return 1;
	if (Rank->get_power() < (int)aKey) return -1;
	return 0;
}

int
CRankTable::add_rank(CRank *aRank)
{
	if (!aRank) return -1;

	for (int i=0 ; i<length() ; i++)
	{
		CRank *
			Rank = (CRank *)get(i);

		if (Rank->get_id() == aRank->get_id()) return -1;
	}

	insert_sorted(aRank);

	return aRank->get_id();
}

bool
CRankTable::remove_rank(int aID)
{
	for (int i=length()-1 ; i>=0 ; i--)
	{
		CRank *
			Rank = (CRank *)get(i);
		if (Rank->get_id() == aID)
		{
			CSortedList::remove(i);
			return true;
		}
	}

	return false;
}

CRank *
CRankTable::get_by_id(int aID)
{
	for (int i=0 ; i<length() ; i++)
	{
		CRank *
			Rank = (CRank *)get(i);

		if (Rank->get_id() == aID) return Rank;
	}

	return NULL;
}

int
CRankTable::get_rank_by_id(int aID)
{
	for (int i=0 ; i<length() ; i++)
	{
		CRank *
			Rank = (CRank *)get(i);

		if (Rank->get_id() == aID) return (i+1);
	}

	return -1;
}

void
CRankTable::set_rank_type(int aType)
{
	if (aType < CRank::PLAYER_OVERALL ||
		aType > CRank::COUNCIL_DIPLOMATIC) return;

	mRankType = aType;
}

int
CRankTable::get_top_number_of_techs()
{
	CRank *aRank = (CRank *)get(0);
	
	return aRank->get_number();
}

char *
CRankTable::top_ten_info_html(int aID)
{
	static CString
		Info;
	Info.clear();

	if (mRankType >= CRank::PLAYER_OVERALL && mRankType <= CRank::PLAYER_TECH)
	{
		Info = "<TABLE BORDER=0 CELLPADDING=1 CELLSPACING=1 WIDTH=605>\n";
		Info += "<TR>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Rank"));
		Info += "</TD>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Name"));
		Info += "</TD>\n";

		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Council"));
		Info += "</TD>\n";

		if (mRankType == CRank::PLAYER_FLEET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Fleets"));
			Info += "</TD>\n";
		} if (mRankType == CRank::PLAYER_PLANET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Planets"));
			Info += "</TD>\n";
		} if (mRankType == CRank::PLAYER_TECH)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Techs"));
			Info += "</TD>\n";
		}
		if (mRankType == CRank::PLAYER_RATING)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Rating"));
			Info += "</TD>\n";
		}

		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Power"));
		Info += "</TD>\n";

		Info += "</TR>\n";

		bool
			BGColor = true;

		for (int i=0 ; i<10 && i < length() ; i++)
		{
			CRank *
				Rank = (CRank *)get(i);
			CPlayer *
				Player = PLAYER_TABLE->get_by_game_id(Rank->get_id());

			if (BGColor)
			{
				Info += "<TR BGCOLOR=\"#000000\">\n";
			} else
			{
				Info += "<TR BGCOLOR=\"#0D0D0D\">\n";
			}

			if (Player->get_game_id() == aID)
			{
				Info.format("<TD CLASS=\"white\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info += "<TD CLASS=\"white\">";
				Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank_highlight\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Player->get_race()), Player->get_game_id(), Player->get_nick());
				Info += "</TD>\n";

				CCouncil *
					Council = Player->get_council();

				Info.format("<TD NOWRAP CLASS=\"white\">&nbsp;<A CLASS=\"rank_highlight\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
							Council->get_id(), Council->get_nick());

				if (mRankType == CRank::PLAYER_FLEET ||
					mRankType == CRank::PLAYER_PLANET ||
					mRankType == CRank::PLAYER_TECH ||
					mRankType == CRank::PLAYER_RATING)
				{
					Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			} else
			{
				Info.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info += "<TD CLASS=\"tabletxt\">";
				Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Player->get_race()), Player->get_game_id(), Player->get_nick());
				Info += "</TD>\n";

				CCouncil *
					Council = Player->get_council();
				Info.format("<TD NOWRAP CLASS=\"tabletxt\">&nbsp;<A CLASS=\"rank\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
							Council->get_id(), Council->get_nick());

				if (mRankType == CRank::PLAYER_FLEET ||
					mRankType == CRank::PLAYER_PLANET ||
					mRankType == CRank::PLAYER_TECH ||
					mRankType == CRank::PLAYER_RATING)
				{
					Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			}

			Info += "</TR>\n";

			BGColor = !BGColor;
		}

		Info += "</TABLE>\n";
	} else if (mRankType >= CRank::COUNCIL_OVERALL &&
				mRankType <= CRank::COUNCIL_DIPLOMATIC)
	{
		Info = "<TABLE BORDER=0 CELLPADDING=1 CELLSPACING=1 WIDTH=605>\n";
		Info += "<TR>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Rank"));
		Info += "</TD>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Name"));
		Info += "</TD>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Speaker"));
		Info += "</TD>\n";

		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Members"));
		Info += "</TD>\n";

		if (mRankType == CRank::COUNCIL_FLEET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Fleets"));
			Info += "</TD>\n";
		}
		if (mRankType == CRank::COUNCIL_PLANET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Planets"));
			Info += "</TD>\n";
		}

		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Power"));
		Info += "</TD>\n";

		Info += "</TR>\n";

		bool
			BGColor = true;

		for (int i=0 ; i<10 && i < length(); i++)
		{
			CRank *
				Rank = (CRank *)get(i);
			CCouncil *
				Council = COUNCIL_TABLE->get_by_id(Rank->get_id());

			if (BGColor)
			{
				Info += "<TR BGCOLOR=\"#000000\">\n";
			} else
			{
				Info += "<TR BGCOLOR=\"#0D0D0D\">\n";
			}

			if (Council->get_id() == aID)
			{
				Info.format("<TD CLASS=\"white\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info.format("<TD CLASS=\"white\">&nbsp;<A CLASS=\"rank_highlight\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
						Council->get_id(), Council->get_nick());

				CPlayer *
					Speaker = Council->get_speaker();
				if (Speaker != NULL)
				{
					Info += "<TD CLASS=\"white\">";
					Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank_highlight\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Speaker->get_race()), Speaker->get_game_id(), Speaker->get_nick());
					Info += "</TD>\n";
				}
				else
				{
					Info.format("<TD CLASS=\"white\">&nbsp;%s</TD>\n", GETTEXT("NONE"));
				}

				CPlayerList *
					MemberList = Council->get_members();
				Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">&nbsp;%s</TD>\n",
							dec2unit(MemberList->length()));

				if (mRankType == CRank::COUNCIL_FLEET ||
					mRankType == CRank::COUNCIL_PLANET)
				{
					Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			} else
			{
				Info.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info.format("<TD CLASS=\"tabletxt\">&nbsp;<A CLASS=\"rank\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
							Council->get_id(), Council->get_nick());

				CPlayer *
					Speaker = Council->get_speaker();
				if (Speaker != NULL)
				{
					Info += "<TD CLASS=\"tabletxt\">";
					Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Speaker->get_race()), Speaker->get_game_id(), Speaker->get_nick());
					Info += "</TD>\n";
				}
				else
				{
					Info.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n", GETTEXT("NONE"));
				}

				CPlayerList *
					MemberList = Council->get_members();
				Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">&nbsp;%s</TD>\n",
							dec2unit(MemberList->length()));

				if (mRankType == CRank::COUNCIL_FLEET ||
					mRankType == CRank::COUNCIL_PLANET)
				{
					Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			}

			Info += "</TR>\n";

			BGColor = !BGColor;
		}

		Info += "</TABLE>\n";
	} else return NULL;

	return (char *)Info;
}

char *
CRankTable::around_me_info_html(int aID)
{
	static CString
		Info;
	Info.clear();

	int
		MyRank = get_rank_by_id(aID);
	if (MyRank == -1) return NULL;

	if (mRankType >= CRank::PLAYER_OVERALL && mRankType <= CRank::PLAYER_RATING)
	{
		Info = "<TABLE BORDER=0 CELLPADDING=1 CELLSPACING=1 WIDTH=605>\n";
		Info += "<TR>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Rank"));
		Info += "</TD>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Name"));
		Info += "</TD>\n";

		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Council"));
		Info += "</TD>\n";

		if (mRankType == CRank::PLAYER_FLEET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Fleets"));
			Info += "</TD>\n";
		}
		if (mRankType == CRank::PLAYER_PLANET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Planets"));
			Info += "</TD>\n";
		}
		if (mRankType == CRank::PLAYER_TECH)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Techs"));
			Info += "</TD>\n";
		}
		if (mRankType == CRank::PLAYER_RATING)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Rating"));
			Info += "</TD>\n";
		}

		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Power"));
		Info += "</TD>\n";

		Info += "</TR>\n";

		bool
			BGColor = true;

		for (int i = MyRank-15-1 ; i <= MyRank+15-1 ; i++)
		{
			if (i < 0) continue;
			if (i >= length()) break;

			CRank *
				Rank = (CRank *)get(i);
			CPlayer *
				Player = PLAYER_TABLE->get_by_game_id(Rank->get_id());

			if (BGColor)
			{
				Info += "<TR BGCOLOR=\"#000000\">\n";
			} else
			{
				Info += "<TR BGCOLOR=\"#0D0D0D\">\n";
			}

			if (Player->get_game_id() == aID)
			{
				Info.format("<TD CLASS=\"white\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info += "<TD CLASS=\"white\">";
				Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank_highlight\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Player->get_race()), Player->get_game_id(), Player->get_nick());
				Info += "</TD>\n";

				CCouncil *
					Council = Player->get_council();

				Info.format("<TD NOWRAP CLASS=\"white\">&nbsp;<A CLASS=\"rank_highlight\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
							Council->get_id(), Council->get_nick());

				if (mRankType == CRank::PLAYER_FLEET ||
					mRankType == CRank::PLAYER_PLANET ||
					mRankType == CRank::PLAYER_TECH ||
                    mRankType == CRank::PLAYER_RATING)
				{
					Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			} else
			{
				Info.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info += "<TD CLASS=\"tabletxt\">";
				Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Player->get_race()), Player->get_game_id(), Player->get_nick());
				Info += "</TD>\n";

				CCouncil *
					Council = Player->get_council();
				
				Info.format("<TD NOWRAP CLASS=\"tabletxt\">&nbsp;<A CLASS=\"rank\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
							Council->get_id(), Council->get_nick());

				if (mRankType == CRank::PLAYER_FLEET ||
					mRankType == CRank::PLAYER_PLANET ||
					mRankType == CRank::PLAYER_TECH ||
					mRankType == CRank::PLAYER_RATING)
				{
					Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			}

			Info += "</TR>\n";

			BGColor = !BGColor;
		}

		Info += "</TABLE>\n";
	}
	else if (mRankType >= CRank::COUNCIL_OVERALL &&
			mRankType <= CRank::COUNCIL_DIPLOMATIC)
	{
		Info = "<TABLE BORDER=0 CELLPADDING=1 CELLSPACING=1 WIDTH=605>\n";
		Info += "<TR>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Rank"));
		Info += "</TD>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Name"));
		Info += "</TD>\n";

		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Speaker"));
		Info += "</TD>\n";
 
		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Members"));
		Info += "</TD>\n";

		if (mRankType == CRank::COUNCIL_FLEET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Fleets"));
			Info += "</TD>\n";
		}
		if (mRankType == CRank::COUNCIL_PLANET)
		{
			Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
			Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Planets"));
			Info += "</TD>\n";
		}

		Info += "<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"CENTER\" BGCOLOR=\"#171717\">";
		Info.format("<FONT SIZE=\"2\">%s</FONT>", GETTEXT("Power"));
		Info += "</TD>\n";

		Info += "</TR>\n";

		bool
			BGColor = true;

		for (int i = MyRank-15-1 ; i <= MyRank+15-1 ; i++)
		{
			if (i < 0) continue;
			if (i >= length()) break;

			CRank *
				Rank = (CRank *)get(i);
			CCouncil *
				Council = COUNCIL_TABLE->get_by_id(Rank->get_id());

			if( Council == NULL ) continue;

			if (BGColor)
			{
				Info += "<TR BGCOLOR=\"#000000\">\n";
			} else
			{
				Info += "<TR BGCOLOR=\"#0D0D0D\">\n";
			}

			if (Council->get_id() == aID)
			{
				Info.format("<TD CLASS=\"white\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info.format("<TD CLASS=\"white\">&nbsp;<A CLASS=\"rank_highlight\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
							Council->get_id(), Council->get_nick());

				CPlayer *
					Speaker = Council->get_speaker();
				if (Speaker != NULL)
				{
					Info += "<TD CLASS=\"white\">";
					Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank_highlight\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Speaker->get_race()), Speaker->get_game_id(), Speaker->get_nick());
					Info += "</TD>\n";
				}
				else
				{
					Info.format("<TD CLASS=\"white\">&nbsp;%s</TD>\n", GETTEXT("NONE"));
				}

				CPlayerList *
					MemberList = Council->get_members();
				Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">&nbsp;%s</TD>\n",
							dec2unit(MemberList->length()));

				if (mRankType == CRank::COUNCIL_FLEET ||
					mRankType == CRank::COUNCIL_PLANET)
				{
					Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"white\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			} else
			{
				Info.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD>\n", i+1);

				Info.format("<TD CLASS=\"tabletxt\">&nbsp;<A CLASS=\"rank\" HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A></TD>\n",
							Council->get_id(), Council->get_nick());

				CPlayer *
					Speaker = Council->get_speaker();
				if (Speaker != NULL)
				{
					Info += "<TD CLASS=\"tabletxt\">";
					Info.format("&nbsp;<IMG SRC=\"%s\"><A CLASS=\"rank\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>",
							get_race_symbol_url(Speaker->get_race()), Speaker->get_game_id(), Speaker->get_nick());
					Info += "</TD>\n";
				}
				else
				{
					Info.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n", GETTEXT("NONE"));
				}

				CPlayerList *
					MemberList = Council->get_members();
				Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">&nbsp;%s</TD>\n",
							dec2unit(MemberList->length()));

				if (mRankType == CRank::COUNCIL_FLEET ||
					mRankType == CRank::COUNCIL_PLANET)
				{
					Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s&nbsp;</TD>\n",
								dec2unit(Rank->get_number()));
				}

				Info.format("<TD NOWRAP CLASS=\"tabletxt\" ALIGN=\"RIGHT\">%s</TD>\n",
							dec2unit(Rank->get_power()));
			}

			Info += "</TR>\n";

			BGColor = !BGColor;
		}

		Info += "</TABLE>\n";
	} else return NULL;

	return (char *)Info;
}

char *
CRankTable::get_race_symbol_url(int aRace)
{
	static CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("%s/image/as_game/info/", (char *)CGame::mImageServerURL);

	switch (aRace)
	{
		case CRace::RACE_HUMAN :
			ImageURL += "symbol_human.gif";
			return (char *)ImageURL;

		case CRace::RACE_TARGOID :
			ImageURL += "symbol_targoid.gif";
			return (char *)ImageURL;

		case CRace::RACE_BUCKANEER :
			ImageURL += "symbol_buckaneer.gif";
			return (char *)ImageURL;

		case CRace::RACE_TECANOID :
			ImageURL += "symbol_tecanoid.gif";
			return (char *)ImageURL;

		case CRace::RACE_EVINTOS :
			ImageURL += "symbol_evintos.gif";
			return (char *)ImageURL;

		case CRace::RACE_AGERUS :
			ImageURL += "symbol_agerus.gif";
			return (char *)ImageURL;

		case CRace::RACE_BOSALIAN :
			ImageURL += "symbol_bosalian.gif";
			return (char *)ImageURL;

		case CRace::RACE_XELOSS :
			ImageURL += "symbol_xeloss.gif";
			return (char *)ImageURL;

		case CRace::RACE_XERUSIAN :
			ImageURL += "symbol_xerusian.gif";
			return (char *)ImageURL;

		case CRace::RACE_XESPERADOS :
			ImageURL += "symbol_xesperados.gif";
			return (char *)ImageURL;

		default :
			return NULL;
	}
}

