#include <libintl.h>
#include "../../pages.h"

bool
CPageConcentrationMode::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	static CString
		Message;
	Message.clear();

	ITEM("GENERAL_EXPLAIN", GETTEXT("By selecting the different concentration modes,"
									" the control model is changed."));

	CControlModel *
		ControlModel = aPlayer->get_control_model();

	ITEM("STRING_CONTROL_MODEL", GETTEXT("Control Model"));
	ITEM("STRING_CURRENT", GETTEXT("Current"));
	ITEM("STRING_RESULT", GETTEXT("Result"));
	ITEM("STRING_CHANGE", GETTEXT("Change"));

	ITEM("STRING_PRODUCTION_CM", GETTEXT("Production CM"));
	ITEM("STRING_MILITARY_CM", GETTEXT("Military CM"));
	ITEM("STRING_RESEARCH_CM", GETTEXT("Research CM"));

	ITEM("CURRENT_PRODUCTION_CM", ControlModel->get_real_production());
	ITEM("CURRENT_MILITARY_CM", ControlModel->get_real_military());
	ITEM("CURRENT_RESEARCH_CM", ControlModel->get_real_research());

	ITEM("RESULT_PRODUCTION_CM", ControlModel->get_real_production());
	ITEM("RESULT_MILITARY_CM", ControlModel->get_real_military());
	ITEM("RESULT_RESEARCH_CM", ControlModel->get_real_research());

	ITEM("CHANGE_PRODUCTION_CM", 0);
	ITEM("CHANGE_RESEARCH_CM", 0);
	ITEM("CHANGE_MILITARY_CM", 0);

	ITEM("MODE", aPlayer->get_mode());

	ITEM("BALANCED_EXPLAIN", GETTEXT("By setting your concentration mode to Balanced,"
										" you will work with equal effort toward"
										" all goals."));

	ITEM("INDUSTRY_EXPLAIN", GETTEXT("By concentrating on the Industry, you can make"
										" more PP, but the military and the research"
										" areas are neglected."));

	ITEM("MILITARY_EXPLAIN", GETTEXT("By concentrating on the Military, you can make"
										" more MP, and the working power is concentrated"
										" on building ships, but the other production and"
										" the research areas are neglected."));

	ITEM("RESEARCH_EXPLAIN", GETTEXT("By concentrating on the Research, you can research"
										" new techs very fast, but the other areas are"
										" neglected."));

	Message += GETTEXT("Changing the concentration mode requires the total production"
						" income in one turn.");
	Message += " ";
	Message.format(GETTEXT("Right now it will cost %1$s PP."),
					dec2unit(aPlayer->get_last_turn_production()));

	Message += "<BR><BR>\n";

	Message.format(GETTEXT("Current mode is \"%1$s\"."), aPlayer->get_mode_name());
	Message += "<BR>\n";

	Message += GETTEXT("Select the form of concentration mode that you want.");

	ITEM("CONCENTRATION_MESSAGE", (char *)Message);

	ITEM("CONCENTRATION_SELECT", 
			aPlayer->select_concentration_mode_html())

//	system_log("end page handler %s", get_name());

	return output("domestic/concentration_mode.html");
}
