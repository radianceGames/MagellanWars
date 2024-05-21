#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageIndependenceDeclaration::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CCouncil *
		Council = aPlayer->get_council();

	if (aPlayer->get_game_id() == Council->get_speaker_id())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You are already the speaker of a council."));
		return output("council/independence_declaration_error.html");
	}

	if (Council->get_number_of_members() == 1)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You are the only member in your council."));
		return output("council/independence_declaration_error.html");
	}

	if (aPlayer->get_admission())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You have requested admission to another council."));
		return output("council/independence_declaration_error.html");
	}

	CKnownTechList *
		KnownTechList = aPlayer->get_tech_list();

	#define PERSONALISM_THEORY 		1111
	#define TOTALISM_THEORY			1110
	#define CLASSISM_THEORY			1109

	if (!KnownTechList->get_by_id(PERSONALISM_THEORY))
	{
		CTech *
			Tech = TECH_TABLE->get_by_id(PERSONALISM_THEORY);
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You need the technology \"%1$s\"."),
								Tech->get_name_with_level()));
		return output("council/independence_declaration_error.html");
	}

	if (!KnownTechList->get_by_id(TOTALISM_THEORY))
	{
		CTech *
			Tech = TECH_TABLE->get_by_id(TOTALISM_THEORY);
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You need the technology \"%1$s\"."),
								Tech->get_name_with_level()));
		return output("council/independence_declaration_error.html");
	}

	if (!KnownTechList->get_by_id(CLASSISM_THEORY))
	{
		CTech *
			Tech = TECH_TABLE->get_by_id(CLASSISM_THEORY);
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You need the technology \"%1$s\"."),
								Tech->get_name_with_level()));
		return output("council/independence_declaration_error.html");
	}

	ITEM("ENTER_COUNCIL_NAME_MESSAGE",
			GETTEXT("Enter the name of your council."));

	ITEM("ENTER_COUNCIL_SLOGAN_MESSAGE",
			GETTEXT("Enter the slogan of your council."));

//	system_log("end page handler %s", get_name());

	return output("council/independence_declaration.html");
}
