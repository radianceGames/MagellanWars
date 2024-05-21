#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
bool
CPageDeathMenu::handle(CConnection &aConnection)
{
//	system_log("start page handler %s", get_name());
	if (!CPageCommon::handle(aConnection)) return false;

	CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("\"%s/image/as_game/race/",
					(char *)CGame::mImageServerURL);
	int
		PortalID = CONNECTION->get_portal_id();
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);

	if (!Player)
	{
		message_page(GETTEXT("Your character was not found."));

		return true;
	}
	if (!Player->is_dead())
	{
		message_page(GETTEXT("Your character is alive."));

		return true;
	}

	CString Race = Player->get_race_name2();

	CString	Buf;
	Buf = ImageURL + Race + "/back_menu.gif\"";

	ITEM("BG_IMAGE", (char *)Buf);

	ITEM("STRING_MAIN", GETTEXT("MAIN"));

	ITEM("STRING_DOMESTIC", GETTEXT("DOMESTIC"));
	ITEM("STRING_CONCENTRATION_MODE", GETTEXT("Concentration Mode"));
	ITEM("STRING_PLANET_MANAGEMENT", GETTEXT("Planet Management"));
	ITEM("STRING_RESEARCH", GETTEXT("Research"));
	ITEM("STRING_PROJECT", GETTEXT("Project"));
	ITEM("STRING_WHITEBOOK", GETTEXT("Whitebook"));

	ITEM("STRING_DIPLOMACY", GETTEXT("DIPLOMACY"));
	ITEM("STRING_DIPLOMACY_STATUS_LIST", GETTEXT("Diplomacy Status List"));
	ITEM("STRING_READ_MESSAGE", GETTEXT("Read Message"));
	ITEM("STRING_SEND_MESSAGE", GETTEXT("Send Message"));
	ITEM("STRING_INSPECT_OTHER_PLAYER", GETTEXT("Inspect Other Player"));

	ITEM("STRING_EMPIRE_DIPL_", GETTEXT("EMPIRE DIPL."));
	ITEM("STRING_BRIBE", GETTEXT("Bribe"));
	ITEM("STRING_REQUEST_BOON", GETTEXT("Request Boon"));
	ITEM("STRING_REQUEST_MEDIATION", GETTEXT("Request Mediation"));
	ITEM("STRING_INVADE_MAGISTRATE_DOMINION", GETTEXT("Invade Magistrate Dominion"));
	ITEM("STRING_INVADE_EMPIRE", GETTEXT("Invade Empire"));

	ITEM("STRING_FLEET", GETTEXT("FLEET"));
	ITEM("STRING_SHIP_DESIGN", GETTEXT("Ship Design"));
	ITEM("STRING_SHIP_BUILDING", GETTEXT("Ship Building"));
	ITEM("STRING_FLEET_COMMANDER", GETTEXT("Fleet Commander"));
	ITEM("STRING_SHIP_POOL", GETTEXT("Ship Pool"));
	ITEM("STRING_FORM_A_NEW_FLEET", GETTEXT("Form a New Fleet"));
	ITEM("STRING_REASSIGNMENT", GETTEXT("Reassignment"));
	ITEM("STRING_EXPEDITION", GETTEXT("Expedition"));
	ITEM("STRING_MISSION", GETTEXT("Mission"));
	ITEM("STRING_RECALL", GETTEXT("Recall"));
	ITEM("STRING_DISBAND", GETTEXT("Disband"));

	ITEM("STRING_WAR", GETTEXT("WAR"));
	ITEM("STRING_SIEGE_PLANET", GETTEXT("Siege Planet"));
	ITEM("STRING_BLOCKADE", GETTEXT("Blockade"));
	ITEM("STRING_RAID", GETTEXT("Raid"));
	ITEM("STRING_PRIVATEER", GETTEXT("Privateer"));
	ITEM("STRING_DEFENSE_PLAN", GETTEXT("Defense Plan"));
	ITEM("STRING_BATTLE_REPORT", GETTEXT("Battle Report"));

	ITEM("STRING_COUNCIL", GETTEXT("COUNCIL"));
	ITEM("STRING_COUNCIL_VOTE", GETTEXT("Council Vote"));
	ITEM("STRING_ADMISSION_REQUEST", GETTEXT("Admission Request"));
	ITEM("STRING_INDEPENDENCE_DECLARATION", GETTEXT("Independence Declaration"));
	ITEM("STRING_DONATION", GETTEXT("Donation"));
	ITEM("STRING_SPEAKER_MENU", GETTEXT("Speaker Menu"));

	ITEM("STRING_INFO", GETTEXT("INFO"));
	ITEM("STRING_CLUSTERS", GETTEXT("Clusters"));
	ITEM("STRING_RANKING", GETTEXT("Ranking"));

	ITEM("STRING_BLACK_MARKET", GETTEXT("BLACK MARKET"));
	ITEM("STRING_THE_TECH_DECK", GETTEXT("The Tech Deck"));
	ITEM("STRING_THE_FLEET_DECK", GETTEXT("The Fleet Deck"));
	ITEM("STRING_OFFICER_S_LOUNGE", GETTEXT("Officer's Lounge"));
	ITEM("STRING_RARE_GOODS_OFFICE", GETTEXT("Rare Goods Office"));
	ITEM("STRING_LEASER_S_OFFICE", GETTEXT("Leaser's Office"));

	ITEM("STRING_FORUM", GETTEXT("FORUM"));
	ITEM("STRING_COUNCIL_FORUM", GETTEXT("Council Forum"));

	ITEM("STRING_HELP", GETTEXT("HELP"));
	ITEM("STRING_ENCYCLOPEDIA", GETTEXT("Encyclopedia"));

//	system_log("end page handler %s", get_name());

	return output("death_menu.html");
}
