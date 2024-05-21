#include <libintl.h>
#include "../../pages.h"

bool
CPageResearchInvestResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	static CString
		Message;
	Message.clear();

	QUERY("INVEST", InvestString);

	CHECK(!InvestString,
			GETTEXT("You didn't enter proper value for investment."));

	int
		Invest = as_atoi(InvestString);
	if (Invest < 1)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't enter proper value for investment.") );
		return output("domestic/research_error.html");
	}

	CHECK(Invest > aPlayer->get_production(),
				GETTEXT("You entered more amount than you have."));
	
	CControlModel *
		ControlModel = aPlayer->get_control_model();

	int
		OldResearch = ControlModel->get_research();
	int
		OldBonusRP;
	if (aPlayer->get_research_invest() < aPlayer->get_research_lab()*10)
	{
		OldBonusRP = (int)(aPlayer->get_research_lab()*10/20);
	} else
	{
		OldBonusRP = (int)(aPlayer->get_research_invest()/20);
	}

	aPlayer->change_research_invest(Invest);
	aPlayer->change_reserved_production(-Invest);	
	refresh_product_point_item();
	aPlayer->build_control_model();

	int
		NewResearch = ControlModel->get_research();
	int
		NewBonusRP;
	if (aPlayer->get_research_invest() < aPlayer->get_research_lab()*10)
	{
		NewBonusRP = (int)(aPlayer->get_research_lab()*10/20);
	} else
	{
		NewBonusRP = (int)(aPlayer->get_research_invest()/20);
	}

	Message.format(GETTEXT("You have successfully invested %1$s PP in Research."),
					dec2unit(Invest));

	if (NewResearch - OldResearch)
	{
		Message += "<BR>\n";
		Message.format("%s %s",
						GETTEXT("Research CM"),
						integer_with_sign(NewResearch - OldResearch));
	}

	if (NewBonusRP - OldBonusRP)
	{
		Message += "<BR>\n";
		Message += GETTEXT("You can get more RP in one turn.");
	}

	if (!(NewResearch - OldResearch) && !(NewBonusRP - OldBonusRP))
	{
		Message += "<BR>\n";
		Message += GETTEXT("However, you have invested so small an amount that no"
							" visible effect can be shown.");
	}

	ITEM("RESULT_MESSAGE", 
			(char *)Message);

//	system_log("end page handler %s", get_name());

	return output("domestic/research_invest_result.html");
}
