#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>

bool
CPageEmpireRequestMediationResult::handler(CPlayer *aPlayer)
{
	QUERY("FOE", FoeStr);

	if (!FoeStr)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Who do you want to sign truce with?"));
		return output("empire/request_mediation_result.html");
	}

	int
		FoeID = as_atoi(FoeStr+1);
	CCouncil *
		Council = aPlayer->get_council();
	CString
		Message;
	bool
		Mediated = false;

	if (*FoeStr == 'c')
	{
		if (Council->get_speaker_id() != aPlayer->get_game_id())
		{
			Message = GETTEXT("You are not the speaker of your council. Only speaker may request the mediation between councils.");
		}
		else
		{
			CCouncil *
				Foe = Council->relation_council(FoeID);
			if (Foe == NULL)
			{
				Message.format(
						GETTEXT("The council with ID %1$s doesn't exist."),
						dec2unit(FoeID));
			}
			else
			{
				CCouncilRelation *
					Relation = Council->get_relation(Foe);
				if( Relation == NULL ||
					( Relation->get_relation() != CRelation::RELATION_WAR &&
					Relation->get_relation() != CRelation::RELATION_TOTAL_WAR ) ){
					Message.format( GETTEXT("You are not at war with %1$s."), Foe->get_nick() );
				}
				else
				{
					if (aPlayer->test_degree_of_amity())
					{
						Message.format(GETTEXT("You requested that the Empire intercede in your war with %1$s. They responded by forcing peace between you and %2$s."),
										Foe->get_nick(),
										Foe->get_nick());
						CString
							News;
						News.format( GETTEXT("%1$s requested the Empire to intercede the war between your council and %2$s. The Empire responded by forcing peace to %3$s."), aPlayer->get_nick(), Foe->get_nick(), Foe->get_nick() );
						Council->send_time_news_to_all_member( (char*)News );
						News.clear();
						News.format( GETTEXT("Cowardly %1$s of %2$s requested the Empire to mediate the dispute between your council and %3$s. Your council cannot but accept it. However, you know this is not the end. They'll soon pay the price of their act."), aPlayer->get_nick(), Council->get_nick(), Council->get_nick() );
						Foe->send_time_news_to_all_member( (char*)News );
						Council->improve_relationship( CCouncilMessage::TYPE_SUGGEST_TRUCE, Foe );
						if (Council->get_power() < (Foe->get_power()/8)) {
                            aPlayer->change_empire_relation( -5 );
                        } else if (Council->get_power() < (Foe->get_power()/4)) {
                            aPlayer->change_empire_relation( -10 );
                            aPlayer->change_honor_with_news( -5 );
                            Council->change_honor_with_news( -15 );
                        } else {
						    Council->change_honor_with_news( -25 );
						    aPlayer->change_honor_with_news( -10 );
						    aPlayer->change_empire_relation( -25 );
                        }
						Mediated = true;
					}
					else
					{
						Message.format(GETTEXT("You requested the Empire to intercede in your war with %1$s. The Empire said that they thought the struggle was a good thing and was a fitting way to prove your loyalty to them."),
										Foe->get_nick());
					}
				}
			}
		}
	}
	else
	{
		CPlayer *
			Foe = Council->get_member_by_game_id(FoeID);
		if (Foe == NULL)
		{
			Message.format(
						GETTEXT("The council with ID %1$s doesn't exist."),
						dec2unit(FoeID));
		}
		else
		{
			CPlayerRelation *
				Relation = aPlayer->get_relation(Foe);

			if (Relation == NULL ||
				( Relation->get_relation() != CRelation::RELATION_WAR &&
				Relation->get_relation() != CRelation::RELATION_TOTAL_WAR ) ){
				Message.format( GETTEXT("You are not at war with %1$s."), Foe->get_nick() );
			}
			else
			{
				if (aPlayer->test_degree_of_amity())
				{
					Message.format(GETTEXT("You requested that the Empire intercede in your war with %1$s. They responded by forcing peace between you and %2$s."),
									Foe->get_nick(),
									Foe->get_nick());
					CString
						News;
					News.format( GETTEXT("Cowardly %1$s requested the Empire to mediate the disbute between you and %1$s. You cannot but accept it. However, you know this is not the end. They'll soon pay the price of their act."), aPlayer->get_nick(), aPlayer->get_nick() );
					Foe->time_news( (char*)News );
					aPlayer->improve_relationship( CDiplomaticMessage::TYPE_SUGGEST_TRUCE, Foe );
					aPlayer->change_honor_with_news( -10 );
					aPlayer->change_empire_relation( -15 );
					Mediated = true;
				}
				else
				{
					Message.format(GETTEXT("You requested the Empire to intercede in your war with %s. The Empire said that they thought the struggle was a good thing and was a fitting way to prove your loyalty to them."),
									Foe->get_nick());
				}
			}
		}
	}

	CEmpireAction
		*Action = new CEmpireAction();
	Action->set_id( aPlayer->get_empire_action_list()->get_new_id() );
	Action->set_owner( aPlayer->get_game_id() );
	Action->set_action( CEmpireAction::EA_ACTION_REQUEST_MEDIATION );
	Action->set_answer( CEmpireAction::EA_ANSWER_COMPLETE );
	Action->set_time();
	Action->set_target((int)Mediated);

	Action->save_new_action();

	aPlayer->get_empire_action_list()->add_empire_action(Action);


	ITEM("RESULT_MESSAGE", (char*)Message);

	return output("empire/request_mediation_result.html");
}
