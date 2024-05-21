#include <libintl.h>
#include "../../pages.h"
#include "../../race.h"
#include "../../battle.h"

bool
CPageDefensePlanSpecialNew::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	CCommandSet
		Race,
		AttackType;

	int
		PowerMin = 50,
		PowerMax = 200;

	QUERY("RACE_CHECK", RaceCheckString);

	if (RaceCheckString)
	{
		if (!strcasecmp(RaceCheckString, "ON"))
		{
			QUERY("HUMAN_CHECK", HumanCheckString);
			if (HumanCheckString)
			{
				if (!strcasecmp(HumanCheckString, "ON")) Race += CRace::RACE_HUMAN;
			}

			QUERY("TARGOID_CHECK", TargoidCheckString);
			if (TargoidCheckString)
			{
				if (!strcasecmp(TargoidCheckString, "ON")) Race += CRace::RACE_TARGOID;
			}

			QUERY("BUCKANEER_CHECK", BuckaneerCheckString);
			if (BuckaneerCheckString)
			{
				if (!strcasecmp(BuckaneerCheckString, "ON")) Race += CRace::RACE_BUCKANEER;
			}

			QUERY("TECANOID_CHECK", TecanoidCheckString);
			if (TecanoidCheckString)
			{
				if (!strcasecmp(TecanoidCheckString, "ON")) Race += CRace::RACE_TECANOID;
			}

			QUERY("EVINTOS_CHECK", EvintosCheckString);
			if (EvintosCheckString)
			{
				if (!strcasecmp(EvintosCheckString, "ON")) Race += CRace::RACE_EVINTOS;
			}

			QUERY("AGERUS_CHECK", AgerusCheckString);
			if (AgerusCheckString)
			{
				if (!strcasecmp(AgerusCheckString, "ON")) Race += CRace::RACE_AGERUS;
			}

			QUERY("BOSALIAN_CHECK", BosalianCheckString);
			if (BosalianCheckString)
			{
				if (!strcasecmp(BosalianCheckString, "ON")) Race += CRace::RACE_BOSALIAN;
			}

			QUERY("XELOSS_CHECK", XelossCheckString);
			if (XelossCheckString)
			{
				if (!strcasecmp(XelossCheckString, "ON")) Race += CRace::RACE_XELOSS;
			}

			QUERY("XERUSIAN_CHECK", XerusianCheckString);
			if (XerusianCheckString)
			{
				if (!strcasecmp(XerusianCheckString, "ON")) Race += CRace::RACE_XERUSIAN;
			}

			QUERY("XESPERADOS_CHECK", XesperadosCheckString);
			if (XesperadosCheckString)
			{
				if (!strcasecmp(XesperadosCheckString, "ON")) Race += CRace::RACE_XESPERADOS;
			}

			if (Race.is_empty())
			{
				ITEM("ERROR_MESSAGE",
						GETTEXT("You didn't select any races."));
				return output("war/defense_plan_special_error.html");
			}
		} else
		{
			Race += CRace::RACE_HUMAN;
			Race += CRace::RACE_TARGOID;
			Race += CRace::RACE_BUCKANEER;
			Race += CRace::RACE_TECANOID;
			Race += CRace::RACE_EVINTOS;
			Race += CRace::RACE_AGERUS;
			Race += CRace::RACE_BOSALIAN;
			Race += CRace::RACE_XELOSS;
			Race += CRace::RACE_XERUSIAN;
			Race += CRace::RACE_XESPERADOS;
		}
	}

	QUERY("POWER_CHECK", PowerCheckString);
	if (PowerCheckString)
	{
		if (!strcasecmp(PowerCheckString, "ON"))
		{
			QUERY("POWER_FROM", PowerFromString);
			PowerMin = as_atoi(PowerFromString);

			QUERY("POWER_TO", PowerToString);
			PowerMax = as_atoi(PowerToString);
		}
	}

	QUERY("ATTACK_TYPE_CHECK", AttackTypeCheckString);
	if (AttackTypeCheckString)
	{
		if (!strcasecmp(AttackTypeCheckString, "ON"))
		{
			QUERY("SIEGE_CHECK", SiegeCheckString);
			if (SiegeCheckString)
			{
				if (!strcasecmp(SiegeCheckString, "ON")) AttackType += CBattle::WAR_SIEGE;
			}

			QUERY("BLOCKADE_CHECK", BlockadeCheckString);
			if (BlockadeCheckString)
			{
				if (!strcasecmp(BlockadeCheckString, "ON")) AttackType += CBattle::WAR_BLOCKADE;
			}

			if (AttackType.is_empty())
			{
				ITEM("ERROR_MESSAGE",
						GETTEXT("You didn't select any attack types."));
				return output("war/defense_plan_special_error.html");
			}
		} else
		{
			AttackType += CBattle::WAR_SIEGE;
			AttackType += CBattle::WAR_BLOCKADE;
		}
	}

	ITEM("STRING_PLAN_NAME", GETTEXT("Plan Name"));

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	if (FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You do not have any fleets on stand-by.<BR>"
						"Please try again later."));
		return output("war/defense_plan_special_error.html");
	}

	ITEM("SELECT_FLEET_MESSAGE",
			GETTEXT("Select fleets to deploy(up to 20 fleets)."));

	ITEM("FLEET_LIST", FleetList->deployment_fleet_list_html(aPlayer));

	ITEM("RACE_CONDITION", Race.get_string());
	ITEM("POWER_MIN", PowerMin);
	ITEM("POWER_MAX", PowerMax);
	ITEM("ATTACK_TYPE_CONDITION", AttackType.get_string());

//	system_log( "end page handler %s", get_name() );

	return output( "war/defense_plan_special_new.html" );
}

