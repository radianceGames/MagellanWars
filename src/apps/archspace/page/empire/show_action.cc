#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageEmpireShowAction::handler(CPlayer *aPlayer)
{
	QUERY("ACTION_ID", ActionIDStr);

	if (!ActionIDStr)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Any action was not chosen."));
		return output("empire/show_action.html");
	}

	int
		ActionID = as_atoi(ActionIDStr);
	CEmpireAction
		*Action = aPlayer->get_empire_action_list()->get_by_id(ActionID);
	if (Action == NULL)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("You chose the action that doesn't exist."));
		return output("empire/show_action.html");
	}

	CString
		Desc,
		Message;

	switch (Action->get_action())
	{
		case CEmpireAction::EA_ACTION_BRIBE :
			ITEM("DESCRIPTION", (char *)(*mBribeDescription));
			Message.format(GETTEXT("You supported the magistrate with %s PP."),
							dec2unit(Action->get_amount()));
			Message += "<BR>\n";
			if (Action->get_target())
				Message += GETTEXT("He was most pleased and spread word of your loyalty to the Empire.");
			else
				Message += GETTEXT("However, he ignored you.");
			break;
		case CEmpireAction::EA_ACTION_REQUEST_BOON :
			ITEM( "DESCRIPTION", (char*)(*mRequestBoonDescription) );
			if (Action->get_target())
				Message.format(
						GETTEXT("You requested the Empire to reward your loyalty. They accepted and sent you %s."),
						Action->get_target_amount_str(aPlayer));
			else
				Message += GETTEXT("You requested the Empire to reward your loyalty. They responded that they did not think you worthy of their blessing.");
			break;
		case CEmpireAction::EA_ACTION_REQUEST_MEDIATION :
			ITEM( "DESCRIPTION", (char*)(*mRequestMediationDescription) );
			if (Action->get_target())
			{
				Message = GETTEXT("You requested the Empire to intercede in your war. They responded by forcing peace each side.");
				Message += "<BR>\n";
			}
			else
			{
				Message = GETTEXT("You requested the Empire to intercede in your war. The Empire said that the struggle was a good thing and was a fitting way to prove your loyalty to them.");
				Message += "<BR>\n";
			}
			break;
		case CEmpireAction::EA_ACTION_INVADE_MAGISTRATE_DOMINION :
			ITEM( "DESCRIPTION", (char*)(*mInvadeMagistrateDominionDescription) );
			break;
		case CEmpireAction::EA_ACTION_INVADE_EMPIRE :
			ITEM( "DESCRIPTION", (char*)(*mInvadeEmpireDescription) );
			break;
		case CEmpireAction::ED_DEMAND_TRIBUTE :
			Desc.format( (char*)(*mDemandTributeDescription), aPlayer->get_nick(), Action->get_amount() );
			ITEM( "DESCRIPTION", (char*)Desc );
			if (Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING)
			{
				Message.format("<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
			}
			else if (Action->get_answer() == CEmpireAction::EA_ANSWER_YES)
			{
				Message.format(GETTEXT("You paid the Empire Tribute %s PP and pleased the Emperor."),
								dec2unit(Action->get_amount()));
			}
			else if (Action->get_answer() == CEmpireAction::EA_ANSWER_NO)
			{
				Message = GETTEXT("You declined the payment of Empire Tribute and angered the Emperor.");
			}
			break;
		case CEmpireAction::ED_DEMAND_LEAVE_COUNCIL :
			Desc.format( (char*)(*mDemandLeaveCouncilDescription), aPlayer->get_nick() );
			ITEM( "DESCRIPTION", (char*)Desc );
			if( Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING ){
				Message.format("<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
			}
			else if( Action->get_answer() == CEmpireAction::EA_ANSWER_YES )
			{
				Message = GETTEXT("The Empire commanded that you leave the former council. You complied and pleased them greatly.");
			}
			else if( Action->get_answer() == CEmpireAction::EA_ANSWER_NO )
			{
				Message = GETTEXT("The Empire commanded that you leave the former council. You refused and angered them.");
			}
			break;
		case CEmpireAction::ED_DEMAND_DECLARE_WAR :
			if( Action->get_target() ) {
				CCouncil
					*Foe = COUNCIL_TABLE->get_by_id(Action->get_amount());
				Desc.format( (char*)(*mDemandDeclareWarDescription), aPlayer->get_nick(), (Foe)?Foe->get_nick():"(null)" );
				if( Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING ){
					Message.format( "<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
				} else if( Action->get_answer() == CEmpireAction::EA_ANSWER_YES ){
					if (Foe)
					{
						Message.format(GETTEXT("The Empire commanded that you attack %s the enemy of empire. You complied and pleased them greatly."),
										Foe->get_nick() );
					}
					else
					{
						Message = GETTEXT("The Empire commanded that you attack the enemy of empire. You complied and pleased them greatly.");
					}
				}
				else if( Action->get_answer() == CEmpireAction::EA_ANSWER_NO ){
					if (Foe)
					{
						Message.format(GETTEXT("The Empire commanded that you attack %s the enemy of empire. You refused and angered them."),
										Foe->get_nick());
					}
					else
					{
						Message = GETTEXT("The Empire commanded that you attack the enemy of empire. You refused and angered them.");
					}
				}
			}
			else
			{
				CPlayer
					*Foe = PLAYER_TABLE->get_by_game_id(Action->get_amount());
				Desc.format( (char*)(*mDemandDeclareWarDescription), aPlayer->get_nick(), (Foe)?Foe->get_nick():"(null)" );
				if( Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING ){
					Message.format( "<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
				} else if( Action->get_answer() == CEmpireAction::EA_ANSWER_YES ){
					if (Foe)
					{
						Message.format(GETTEXT("The Empire commanded that you attack %s the enemy of empire. You complied and pleased them greatly."),
										Foe->get_nick());
					}
					else
					{
						Message = GETTEXT("The Empire commanded that you attack the enemy of empire. You complied and pleased them greatly.");
					}
				}
				else if( Action->get_answer() == CEmpireAction::EA_ANSWER_NO )
				{
					if (Foe)
					{
						Message.format(GETTEXT("The Empire commanded that you attack %s the enemy of empire. You refused and angered them."),
										Foe->get_nick());
					}
					else
					{
						Message = GETTEXT("The Empire commanded that you attack the enemy of empire. You refused and angered them.");
					}
				}
			}
			ITEM( "DESCRIPTION", (char*)Desc );
			break;
		case CEmpireAction::ED_DEMAND_MAKE_TRUCE :
			if (Action->get_target())
			{
				CCouncil *
					Foe = COUNCIL_TABLE->get_by_id(Action->get_amount());
				Desc.format((char *)(*mDemandMakeTruceDescription),
							aPlayer->get_nick(),
							(Foe)?Foe->get_nick():"(null)");
				if (Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING)
				{
					Message.format("<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
				}
				else if (Action->get_answer() == CEmpireAction::EA_ANSWER_YES)
				{
					if (Foe)
						Message.format(GETTEXT("The Empire commanded that you declare peace with %s the friend of empire. You complied and pleased them greatly."),
										Foe->get_nick());
					else
						Message = GETTEXT("The Empire commanded that you declare peace with the friend of empire. You complied and pleased them greatly.");
				}
				else if (Action->get_answer() == CEmpireAction::EA_ANSWER_NO)
				{
					if (Foe)
						Message.format(GETTEXT("The Empire commanded that you declare peace with %s the friend of empire. You refused and angered them."),
										Foe->get_nick());
					else
						Message = GETTEXT("The Empire commanded that you declare peace with the friend of empire. You refused and angered them.");
				}
			}
			else
			{
				CPlayer *
					Foe = PLAYER_TABLE->get_by_game_id(Action->get_amount());
				Desc.format( (char*)(*mDemandMakeTruceDescription), aPlayer->get_nick(), (Foe)?Foe->get_nick():"(null)" );
				if( Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING ){
					Message.format( "<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
				}
				else if( Action->get_answer() == CEmpireAction::EA_ANSWER_YES )
				{
					if (Foe)
						Message.format(GETTEXT("The Empire commanded that you declare peace with %s the friend of empire. You complied and pleased them greatly."),
										Foe->get_nick());
					else
						Message = GETTEXT("The Empire commanded that you declare peace with the friend of empire. You complied and pleased them greatly.");
				}
				else if( Action->get_answer() == CEmpireAction::EA_ANSWER_NO )
				{
					if (Foe)
						Message.format(GETTEXT("The Empire commanded that you declare peace with %s the friend of empire. You refused and angered them."),
										Foe->get_nick());
					else
						Message = GETTEXT("The Empire commanded that you declare peace with the friend of empire. You refused and angered them.");
				}
			}
			ITEM( "DESCRIPTION", (char*)Desc );
			break;
		case CEmpireAction::ED_DEMAND_FREE_SUBORDINARY_COUNCIL :
			{
				CCouncil
					*Foe = COUNCIL_TABLE->get_by_id(Action->get_amount());
				Desc.format( (char*)(*mDemandFreeSubordinaryCouncilDescription), aPlayer->get_nick(), (Foe)?Foe->get_nick():"(null)" );
				ITEM( "DESCRIPTION", (char*)Desc );
				if( Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING ){
					Message.format( "<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
				}
				else if( Action->get_answer() == CEmpireAction::EA_ANSWER_YES )
				{
					if (Foe)
						Message.format(GETTEXT("The Empire commanded that you end subjugating of %s the friend of empire. You complied and pleased them greatly."),
										Foe->get_nick() );
					else
						Message = GETTEXT("The Empire commanded that you end subjugating of the friend of empire. You complied and pleased them greatly.");
				}
				else if (Action->get_answer() == CEmpireAction::EA_ANSWER_NO)
				{
					if (Foe)
						Message.format(GETTEXT("The Empire commanded that you end subjugating of %s the friend of empire. You refused and angered them."),
										Foe->get_nick());
					else
						Message = GETTEXT("The Empire commanded that you end subjugating of the friend of empire. You refused and angered them.");
				}
			}
			break;
		case CEmpireAction::ED_DEMAND_ARMAMENT_REDUCTION :
			Desc.format( (char*)(*mDemandArmamentReductionDescription), aPlayer->get_nick() );
			ITEM( "DESCRIPTION", (char*)Desc );
			if( Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING ){
				Message.format( "<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
			}
			else if( Action->get_answer() == CEmpireAction::EA_ANSWER_YES )
			{
				Message.format(GETTEXT("The Empire commanded that you reduct armament. You complied and destroyed $1%d%% of your fleets. It pleased them greatly."),
								Action->get_amount());
			}
			else if( Action->get_answer() == CEmpireAction::EA_ANSWER_NO )
			{
				Message = GETTEXT("The Empire commanded that you reduct armament. You refused and angered them.");
			}
			break;
		case CEmpireAction::ED_GRANT_RANK :
			Desc.format( (char*)(*mGrantRankDescription), aPlayer->get_court_rank_name(Action->get_target()), Action->get_amount() );
			ITEM( "DESCRIPTION", (char*)Desc );
			if (Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING)
			{
				Message.format( "<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_action.as?ACTION_ID=%d&ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER = 0></A>\n", Action->get_id(), (char *)CGame::mImageServerURL, Action->get_id(), (char *)CGame::mImageServerURL);
			}
			else if (Action->get_answer() == CEmpireAction::EA_ANSWER_YES)
			{
				Message.format( GETTEXT("The Emperor offered you the title of %s by the price of %d PP. You accepted this honor and became %s the %s."), aPlayer->get_court_rank_name(Action->get_target()), Action->get_amount(), aPlayer->get_name(), aPlayer->get_court_rank_name(Action->get_target()) );
			}
			else if (Action->get_answer() == CEmpireAction::EA_ANSWER_NO)
			{
				Message.format( GETTEXT("The Emperor offered you the title of %s by the price of %d PP. You rejected and angered them."), aPlayer->get_court_rank_name(Action->get_target()), Action->get_amount() );
			}
			break;
		case CEmpireAction::ED_GRANT_BOON :
			ITEM( "DESCRIPTION", (char*)(*mGrantBoonDescription) );
			if (Action->get_target_amount_str(aPlayer))
			{
				Message.format( GETTEXT("Empire rewarded your loyalty. They sent you %s."), Action->get_target_amount_str( aPlayer ) );
			}
			else
			{
				Message = GETTEXT("Empire rewarded your loyalty. But you had the tech already.");
			}
			break;
		default :
			Message.format( "illegal action - %d", Action->get_action() );
			break;
	}

	ITEM("RESULT_MESSAGE", (char*)Message);

	return output("empire/show_action.html");
}
