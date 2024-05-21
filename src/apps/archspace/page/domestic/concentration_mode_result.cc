#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>

bool
CPageConcentrationModeResult::handler(CPlayer *aPlayer)
{
//	SLOG("start page handler %s", get_name());

	QUERY("CONCENTRATION_MODE", ModeString);

	CHECK(!ModeString, 
		GETTEXT("Could not find which concentration mode you wanted."));

	int
		Mode = as_atoi(ModeString);
	CHECK(Mode<CPlayer::MODE_BALANCED || Mode>CPlayer::MODE_RESEARCH,
		GETTEXT("That concentration mode doesn't exist."));

	if (aPlayer->get_mode() == Mode)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't change anything."));
		return output("domestic/concentration_mode_error.html");
	}

	int
		Cost = aPlayer->get_last_turn_production();
	if (Cost > aPlayer->get_production())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have enough PP to change concentration mode."));
		return output("domestic/concentration_mode_error.html");
	}

	ITEM("STRING_CONTROL_MODEL", GETTEXT("Control Model"));
	ITEM("STRING_BEFORE", GETTEXT("Before"));
	ITEM("STRING_AFTER", GETTEXT("After"));
	ITEM("STRING_CHANGE", GETTEXT("Change"));
	ITEM("STRING_PRODUCTION_CM", GETTEXT("Production CM"));
	ITEM("STRING_MILITARY_CM", GETTEXT("Military CM"));
	ITEM("STRING_RESEARCH_CM", GETTEXT("Research CM"));

	CControlModel *
		ControlModel;
	ControlModel = aPlayer->get_control_model();

	int
		OldProductionCM = ControlModel->get_real_production(),
		OldMilitaryCM = ControlModel->get_real_military(),
		OldResearchCM = ControlModel->get_real_research();

	ITEM("BEFORE_PRODUCTION_CM", OldProductionCM);
	ITEM("BEFORE_MILITARY_CM", OldMilitaryCM);
	ITEM("BEFORE_RESEARCH_CM", OldResearchCM);

	aPlayer->change_reserved_production(-Cost);

	refresh_product_point_item();

	aPlayer->set_mode(Mode);
	aPlayer->build_control_model();

	ITEM("RESULT_MESSAGE",
		format(GETTEXT("You have switched your concentration mode to %1$s."),
					aPlayer->get_mode_name()));

	ControlModel = aPlayer->get_control_model();

	int
		NewProductionCM = ControlModel->get_real_production(),
		NewMilitaryCM = ControlModel->get_real_military(),
		NewResearchCM = ControlModel->get_real_research();

	ITEM("AFTER_PRODUCTION_CM", NewProductionCM);
	ITEM("AFTER_MILITARY_CM", NewMilitaryCM);
	ITEM("AFTER_RESEARCH_CM", NewResearchCM);

	ITEM("CHANGE_PRODUCTION_CM", NewProductionCM - OldProductionCM);
	ITEM("CHANGE_MILITARY_CM", NewMilitaryCM - OldMilitaryCM);
	ITEM("CHANGE_RESEARCH_CM", NewResearchCM - OldResearchCM);

//	SLOG("end page handler %s", get_name());

	return output("domestic/concentration_mode_result.html");
}
