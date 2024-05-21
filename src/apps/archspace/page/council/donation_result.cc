#include <libintl.h>
#include "../../pages.h"
#include "../../triggers.h"
#include "../../archspace.h"

bool
CPageDonationResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

        if (!CGame::mUpdateTurn)
        {
                ITEM("RESULT_MESSAGE", GETTEXT("You cannot perform this action until time starts."));
                return output("council/donation_result.html");
        }


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

	QUERY("DONATION", DonationString);

	if (!DonationString)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Enter amount of PP for donation."));
		return output("council/donation_error.html");
	}

	int
		Donation = as_atoi(DonationString);
	if (Donation < 1)
	{
		ITEM("ERROR_MESSAGE", 
				GETTEXT("Enter proper amount of PP for donation."));
		return output("council/donation_error.html");
	}

	CCouncil *
		Council = aPlayer->get_council();
	if (Donation > aPlayer->get_max_donation())
	{
		ITEM("ERROR_MESSAGE", 
				(char *)format(
						GETTEXT("You can donate %1$s PP at most."),
						dec2unit(aPlayer->get_production()/10)));
		return output("council/donation_error.html");
	}

	int
		CouncilPP = Council->get_production();

	bool
		OverflowCheck = false;

	if(CouncilPP >= MAX_COUNCIL_PP)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("The %1$s PP could not be added to the council safe because it is at its maximum capacity."),
								dec2unit(Donation)));
		
		return output("council/donation_error.html");
	}

	if (Donation > MAX_COUNCIL_PP - CouncilPP)
	{
		Donation = MAX_COUNCIL_PP - CouncilPP;
		OverflowCheck = true;
	}

	aPlayer->change_reserved_production(-Donation);
	aPlayer->change_council_donation(Donation);
	aPlayer->set_council_donation_effect_timer();
	Council->change_production(Donation);
	// start telecard 2001/01/30
	Council->type(QUERY_UPDATE);
	STORE_CENTER->store(*Council);
	// end telecard 2001/01/30

	if(OverflowCheck == false)
	{
		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("%1$s PP has been donated to the council safe."),
								dec2unit(Donation)));
	}
	else if(OverflowCheck == true)
	{
		ITEM("RESULT_MESSAGE",
				(char *)format(
						GETTEXT("Only %1$s PP has been donated to the council safe because the safe has reached its maximum capacity.  The remaining funds have been returned to you."),
								dec2unit(Donation)));
	}


	ADMIN_TOOL->add_donation_log(
			(char *)format("The player %s has donated %d PP to the council %s.",
							aPlayer->get_nick(), Donation, Council->get_nick()));

//	system_log("end page handler %s", get_name());

	return output("council/donation_result.html");
}
