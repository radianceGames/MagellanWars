#include <libintl.h>
#include "../../pages.h"
#include "../../triggers.h"

bool
CPageDonation::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());



        if (aPlayer->check_council_donation_effect_timer())
        {
                // Shouldn't need null checks cause of the timer check above
                CPlayerActionList *PlayerActionList = aPlayer->get_action_list();
                CPlayerAction *DonationAction = PlayerActionList->get_council_donation_action();

                ITEM("RESULT_MESSAGE",
                                (char *)format(GETTEXT("You may not donate again"
                                                        " for another %1$s."),
                                                        timetostring(DonationAction->get_start_time()+DonationAction->get_period()-CGame::get_game_time())));
                return output("council/donation_result.html");
        }

	ITEM("STRING_COUNCIL_SAFE", GETTEXT("Current PP in Council Safe"));
	ITEM("STRING_PLAYER_SAFE", GETTEXT("Current PP in Your Safe"));
	ITEM("STRING_AVAILABLE_LIMIT", GETTEXT("Available PP Limit"));

	CCouncil* Council = aPlayer->get_council();
	ITEM("COUNCIL_SAFE", dec2unit(Council->get_production()));
	ITEM("PLAYER_SAFE", dec2unit(aPlayer->get_production()));
	ITEM("AVAILABLE_LIMIT", 
					dec2unit((int)(aPlayer->get_production()/10)));

	ITEM("HOW_MUCH_MESSAGE", 
					GETTEXT("How much PP do you want to donate?"));

//	system_log("end page handler %s", get_name());

	return output("council/donation.html");
}
