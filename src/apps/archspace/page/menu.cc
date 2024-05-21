#include <libintl.h>
#include "../pages.h"
#include "../game.h"
#include "../archspace.h"
#include "../define.h"

bool
	CPageMenu::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

// Generics
	CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("\"%s/image/as_game/race/", (char *)CGame::mImageServerURL);
	CString Race = aPlayer->get_race_name2();

	CString	Buf;
	Buf = ImageURL + Race + "/back_menu.gif\"";

	ITEM("BG_IMAGE", (char *)Buf);
	ITEM("VERSION", GETTEXT("Archspace Version"));
    ITEM("VERSION_NUMBER", GAME_VERSION);
    ITEM("VERSION_INFORMATION_LINK", GETTEXT("http://www.magellanwars.com/board/viewtopic.php?t=16235"));

	// Tree Members
	char *ins_fld = "aux1 = insFld(foldersTree, gFld(\"R\",\"%s\", \"%s\"))\n";
	char *ins_fld_base = "aux1 = insFld(foldersTree, gFld(\"P\",\"%s\", \"%s\"))\n";
	char *ins_doc = "insDoc(aux1, gLnk(\"R\", \"%s\", \"%s\"))\n";
	char *ins_doc_chat = "insDoc(aux1, gLnk(\"B\", \"%s\", \"%s%s\"))\n";
	char *ins_doc_window = "insDoc(aux1, gLnk(\"B\", \"%s\", \"%s\"))\n";
	CString TreeFields;
	TreeFields.clear();
	TreeFields.format(ins_fld, GETTEXT("MAIN"), "/archspace/main.as");
	TreeFields.format(ins_fld, GETTEXT("DOMESTIC"), "/archspace/domestic/domestic.as");
	TreeFields.format(ins_doc, GETTEXT("Concentration Mode"), "/archspace/domestic/concentration_mode.as");
	TreeFields.format(ins_doc, GETTEXT("Planet Management"), "/archspace/domestic/planet_management.as");
	TreeFields.format(ins_doc, GETTEXT("Planet Invest Pool"), "/archspace/domestic/planet_invest_pool.as");
	TreeFields.format(ins_doc, GETTEXT("Research"), "/archspace/domestic/research.as");
	TreeFields.format(ins_doc, GETTEXT("Project"), "/archspace/domestic/project.as");
	TreeFields.format(ins_doc, GETTEXT("Whitebook"), "/archspace/domestic/whitebook.as");
	TreeFields.format(ins_fld, GETTEXT("DIPLOMACY"), "/archspace/diplomacy/diplomacy.as");
	TreeFields.format(ins_doc, GETTEXT("Status List"), "/archspace/diplomacy/diplomacy_status_list.as");
	TreeFields.format(ins_doc, GETTEXT("Management"), "/archspace/diplomacy/diplomacy_management.as");
	TreeFields.format(ins_doc, GETTEXT("Read Message"), "/archspace/diplomacy/read_diplomatic_message.as");
	TreeFields.format(ins_doc, GETTEXT("Send Message"), "/archspace/diplomacy/send_diplomatic_message.as");
	TreeFields.format(ins_doc, GETTEXT("Inspect Other Player"), "/archspace/diplomacy/inspect_other_player.as");
	TreeFields.format(ins_doc, GETTEXT("Special Operation"), "/archspace/diplomacy/spy.as");
	TreeFields.format(ins_fld, GETTEXT("EMPIRE DIPL."), "/archspace/empire/empire_diplomacy.as");
	TreeFields.format(ins_doc, GETTEXT("Bribe"), "/archspace/empire/bribe.as");
//		TreeFields.format(ins_doc, GETTEXT("Bounties"), "/archspace/empire/bounty.as");
	TreeFields.format(ins_doc, GETTEXT("Request Boon"), "/archspace/empire/request_boon.as");
	TreeFields.format(ins_doc, GETTEXT("Request Mediation"), "/archspace/empire/request_mediation.as");
	TreeFields.format(ins_doc, GETTEXT("Invade Empire"), "/archspace/empire/invade_empire.as");
//		TreeFields.format(ins_doc, GETTEXT("Summary"), "/archspace/special/special_event_summary.as");
	TreeFields.format(ins_fld, GETTEXT("FLEET"), "/archspace/fleet/fleet.as");
	TreeFields.format(ins_doc, GETTEXT("Ship Design"), "/archspace/fleet/ship_design.as");
	TreeFields.format(ins_doc, GETTEXT("Ship Building"), "/archspace/fleet/ship_building.as");
	TreeFields.format(ins_doc, GETTEXT("Fleet Commander"), "/archspace/fleet/fleet_commander.as");
	TreeFields.format(ins_doc, GETTEXT("Ship Pool"), "/archspace/fleet/ship_pool.as");
	TreeFields.format(ins_doc, GETTEXT("Form a New Fleet"), "/archspace/fleet/form_new_fleet.as");
	TreeFields.format(ins_doc, GETTEXT("Reassignment"), "/archspace/fleet/reassignment.as");
	TreeFields.format(ins_doc, GETTEXT("Expedition"), "/archspace/fleet/expedition.as");
	TreeFields.format(ins_doc, GETTEXT("Mission"), "/archspace/fleet/mission.as");
	TreeFields.format(ins_doc, GETTEXT("Recall"), "/archspace/fleet/recall.as");
	TreeFields.format(ins_doc, GETTEXT("Disband"), "/archspace/fleet/disband.as");
	TreeFields.format(ins_fld, GETTEXT("WAR"), "/archspace/war/war.as");
	TreeFields.format(ins_doc, GETTEXT("Siege Planet"), "/archspace/war/siege_planet.as");
	TreeFields.format(ins_doc, GETTEXT("Blockade"), "/archspace/war/blockade.as");
	TreeFields.format(ins_doc, GETTEXT("Raid"), "/archspace/war/raid.as");
	TreeFields.format(ins_doc, GETTEXT("Privateer"), "/archspace/war/privateer.as");
	TreeFields.format(ins_doc, GETTEXT("Defense Plan"), "/archspace/war/defense_plan.as");
	TreeFields.format(ins_doc, GETTEXT("Battle Report"), "/archspace/war/battle_report.as");
	TreeFields.format(ins_fld, GETTEXT("COUNCIL"), "/archspace/council/council.as");
//        if (CONFIG->get_string("Game", "IRCPHPScript", NULL) != NULL)
		//TreeFields.format(ins_doc, GETTEXT("Council Chat"), "/circ/");
	TreeFields.format(ins_doc, GETTEXT("Council Vote"), "/archspace/council/council_vote.as");
	TreeFields.format(ins_doc, GETTEXT("Admission Request"), "/archspace/council/admission_request.as");
	TreeFields.format(ins_doc, GETTEXT("Independence Declaration"), "/archspace/council/independence_declaration.as");
	TreeFields.format(ins_doc, GETTEXT("Donation"), "/archspace/council/donation.as");
	TreeFields.format(ins_doc, GETTEXT("Diplomatic Status"), "/archspace/council/diplomacy_status_list.as");
	TreeFields.format(ins_doc, GETTEXT("Speaker Menu"), "/archspace/council/speaker_menu.as");
	TreeFields.format(ins_fld, GETTEXT("INFO"), "/archspace/info/info.as");
	TreeFields.format(ins_doc, GETTEXT("Clusters"), "/archspace/info/clusters.as");
	TreeFields.format(ins_doc, GETTEXT("Ranking"), "/archspace/info/rank_player_overall.as");
	TreeFields.format(ins_fld, GETTEXT("BLACK MARKET"), "/archspace/black_market/black_market.as");
	TreeFields.format(ins_doc, GETTEXT("The Tech Deck"), "/archspace/black_market/tech_deck.as");
	TreeFields.format(ins_doc, GETTEXT("The Fleet Deck"), "/archspace/black_market/fleet_deck.as");
	TreeFields.format(ins_doc, GETTEXT("Officer's Lounge"), "/archspace/black_market/officers_lounge.as");
	TreeFields.format(ins_doc, GETTEXT("Rare Goods Office"), "/archspace/black_market/rare_goods.as");
	TreeFields.format(ins_doc, GETTEXT("Leaser's Office"), "/archspace/black_market/leasers_office.as");
	TreeFields.format(ins_fld, GETTEXT("FORUM"), (char *)CGame::mForumServerURL);
	TreeFields.format(ins_doc, GETTEXT("Council Forum"), (char*)format("/board/viewforum.php?f=%d", 1000+aPlayer->get_council_id()));
	TreeFields.format(ins_fld, GETTEXT("HELP"), "");
	TreeFields.format(ins_doc_chat, GETTEXT("General Chat"), "/cgi-bin/cgiirc/irc.cgi?nick=",aPlayer->get_name() );
	TreeFields.format(ins_doc, GETTEXT("Encyclopedia"), (char*)format("%s%s",CONFIG->get_string(
				"Game", "EncyclopediaWebPath", NULL), "/encyclopedia.html"));
	TreeFields.format(ins_doc, GETTEXT("Donate to MW"), "http://www.magellanwars.com/donate.html");
//	TreeFields.format(ins_doc, GETTEXT("Beginner's Guide"), "/beginners/beginners_guide.html");
//	TreeFields.format(ins_doc, GETTEXT("Developer Blog"), "http://www.katserv.info/asblog/");
//	TreeFields.format(ins_doc, GETTEXT("FAQ"), "/faq/faq.html");
//	TreeFields.format(ins_doc, GETTEXT("Rules"), "/galactic_law/galactic_law.html");
	if (ADMIN_TOOL->has_access(aPlayer))
	{
		TreeFields.format(ins_doc, GETTEXT("Administration"), "/archspace/admin/admin.as");
	}
	TreeFields.format(ins_doc, GETTEXT("Retire"), "/archspace/retire.as");
	TreeFields.format(ins_fld, GETTEXT("SUPPORT"), "");
	TreeFields.format(ins_doc_window, GETTEXT("Donate"), "http://magellanwars.com/donate.html");
	TreeFields.format(ins_doc, GETTEXT("Vote at Mpogd"), "http://www.mpogd.com");
	TreeFields.format(ins_doc, GETTEXT("Vote at Mpog"), "http://www.gamesites200.com/mpog/vote.php?id=1310");
	TreeFields.format(ins_doc, GETTEXT("Vote at OnRPG"), "http://top50.onrpg.com/in.php?id=231");
	TreeFields.format(ins_doc, GETTEXT("Vote at OzGamers"), "http://www.oz-games200.com/in.php?gameid=126");
//		TreeFields.format(ins_doc, GETTEXT("Vote at Apex"), "http://www.apexwebgaming.com");
	TreeFields.format(ins_doc, GETTEXT("Vote at DoG"), "http://www.directoryofgames.com/main.php?view=topgames&action=vote&v_tgame=214");

	TreeFields.format(ins_fld_base, GETTEXT("LOGOUT"), "/game_logout.phtml");

	ITEM("TREE_FIELDS", (char *)TreeFields);
//	system_log("end page handler %s", get_name());

	return output("menu.html");
}
