#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageIndependenceDeclarationResult::handler(CPlayer *aPlayer)
{
	//system_log("start page handler %s", get_name());

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

	#undef	PERSONALISM_THEORY
	#undef	TOTALISM_THEORY
	#undef	CLASSISM_THEORY

	QUERY("COUNCIL_NAME", TempName);
	QUERY("COUNCIL_SLOGAN", TempSlogan);
    CString Name = TempName;
    Name.htmlspecialchars();
	Name.nl2br();
    CString Slogan = TempSlogan;
    Slogan.htmlspecialchars();
    Slogan.nl2br();

	if (!is_valid_name((char *)Name))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("This name is invalid. Please enter a proper name."));
		return output("council/independence_declaration_error.html");
	} else if (COUNCIL_TABLE->get_by_name((char *)Name))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("This name is used by another council."));
		return output("council/independence_declaration_error.html");
	}

	if (!Slogan)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Enter the council slogan."))
		return output("council/independence_declaration_error.html");
	}
    else if (!is_valid_slogan((char *)Slogan))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The slogan is invalid. Please enter a proper slogan."));
		return output("council/independence_declaration_error.html");
	}
	ITEM("RESULT_MESSAGE", aPlayer->independence_declaration((char *)Name, Slogan))

	CCouncil *
		NewCouncil = aPlayer->get_council();
	NewCouncil->disable_merge(CCouncil::mFusionTimeLimit);
	ADMIN_TOOL->add_independence_log(
			(char *)format("The player %s has declared independence from the council %s with making new council %s.",
							aPlayer->get_nick(),
							Council->get_nick(),
							NewCouncil->get_nick()));

	//system_log("end page handler %s", get_name());

	return output("council/independence_declaration_result.html");
}
