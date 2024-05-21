#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilSendDiplomaticMessageResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	QUERY("COUNCIL_ID", CouncilIDString);
	int
		CouncilID = as_atoi(CouncilIDString);
	CCouncil *
		PlayerCouncil = aPlayer->get_council();
		
	CHECK(CouncilID == PlayerCouncil->get_id(), "You cannot send a message to your own council");
    
    CCouncil *
		Council = COUNCIL_TABLE->get_by_id(CouncilID);

	CHECK(!Council, 
			GETTEXT("Target council doesn't exist."));

	QUERY("MESSAGE_TYPE", MessageTypeString);
	CHECK(!MessageTypeString,
			GETTEXT("Message type is not specified."));

	CCouncilMessage::EType
		MessageType = (CCouncilMessage::EType)as_atoi(MessageTypeString);

	CString
		CheckMessage;
	CheckMessage = PlayerCouncil->check_council_message(MessageType, Council, PlayerCouncil);

	if (CheckMessage.length() > 0)
	{
		message_page((char *)CheckMessage);
		return true;
	}

	if (ADMIN_TOOL->are_they_restricted(MessageType, PlayerCouncil, Council) == true)
	{
		message_page(GETTEXT("Your council can't send that kind of message because of the punishment."));
		return true;
	}

	if (Council->check_duplication_diplomatic_message(MessageType, PlayerCouncil) == true)
	{
		message_page(GETTEXT("You already sent this kind of diplomatic message."));
		return true;
	}

	QUERY("COMMENT", Comment);

	if (MessageType == CCouncilMessage::TYPE_DECLARE_WAR)
	{
		if ((long long int)Council->get_power()*100/(long long int)PlayerCouncil->get_power() > COUNCIL_ATTACK_RESTRICTION_POWER_MAX)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("We can't declare war against the council %1$s since it's too powerful."),
									Council->get_nick()));
			return output("council/send_diplomatic_message_error.html");
		}
		if ((long long int)Council->get_power()*100/(long long int)PlayerCouncil->get_power() < COUNCIL_ATTACK_RESTRICTION_POWER_MIN)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("We can't declare war against the council %1$s since it's too weak."),
									Council->get_nick()));
			return output("council/send_diplomatic_message_error.html");
		}

		PlayerCouncil->declare_war(Council);
		PlayerCouncil->disable_merge(CCouncil::mFusionTimeLimit);
		SLOG("DISABLE MERGE IN DECLARE WAR");
		ADMIN_TOOL->add_council_relation_log(
				(char *)format("The council %s has declared war against the council %s.",
								PlayerCouncil->get_nick(), Council->get_nick()));
	}
	else if (MessageType == CCouncilMessage::TYPE_BREAK_PACT)
	{
		PlayerCouncil->break_pact(Council);

		ADMIN_TOOL->add_council_relation_log(
				(char *)format("The council %s has broken pact against the council %s.",
								PlayerCouncil->get_nick(), Council->get_nick()));
	}
	else if (MessageType == CCouncilMessage::TYPE_BREAK_ALLY)
	{
		PlayerCouncil->break_ally(Council);

		ADMIN_TOOL->add_council_relation_log(
				(char *)format("The council %s has broken ally against the council %s.",
								PlayerCouncil->get_nick(), Council->get_nick()));
	}
	else if (MessageType == CCouncilMessage::TYPE_SET_FREE_SUBORDINARY)
	{
		PlayerCouncil->set_free_subordinary(Council);

		ADMIN_TOOL->add_council_relation_log(
				(char *)format("The council %s has set the council %s free.",
								PlayerCouncil->get_nick(), Council->get_nick()));
	}
	else if (MessageType == CCouncilMessage::TYPE_DECLARE_INDEPENDENCE)
	{
		PlayerCouncil->declare_war(Council);

		ADMIN_TOOL->add_council_relation_log(
				(char *)format("The council %s has declared war against the council %s.",
								PlayerCouncil->get_nick(), Council->get_nick()));
	}
	else if (MessageType == CCouncilMessage::TYPE_DECLARE_TOTAL_WAR)
	{
		if ((long long int)Council->get_power()*100/(long long int)PlayerCouncil->get_power() > COUNCIL_ATTACK_RESTRICTION_POWER_MAX)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("We can't declare total war against the council %1$s since it's too powerful."),
									Council->get_nick()));
			return output("council/send_diplomatic_message_error.html");
		}
		if ((long long int)Council->get_power()*100/(long long int)PlayerCouncil->get_power() < COUNCIL_ATTACK_RESTRICTION_POWER_MIN)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("We can't declare total war against the council %1$s since it's too weak."),
									Council->get_nick()));
			return output("council/send_diplomatic_message_error.html");
		}

		CCouncilRelationList *aRelationList = PlayerCouncil->get_relation_list();
		for (int i=0; i < aRelationList->length(); i++)
		{
		    CCouncilRelation *aCouncilRelation = (CCouncilRelation *)aRelationList->get(i);
		    if (!aCouncilRelation)
                  continue;
		    if (aCouncilRelation->get_relation() == CRelation::RELATION_TOTAL_WAR)
		    {
                  CCouncil *aCouncil = aCouncilRelation->get_council1();
                  if (!aCouncil)
                      continue;
                  if (aCouncil->get_id() == PlayerCouncil->get_id())
                  {
                     ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("We can't declare total war against the council %1$s since we currently have a total war declared."),
									Council->get_nick()));
					return output("council/send_diplomatic_message_error.html");

                  }    
            }        
		}    

		PlayerCouncil->declare_total_war(Council);
		
		ADMIN_TOOL->add_council_relation_log(
				(char *)format("The council %s has declared total war against the council %s.",
								PlayerCouncil->get_nick(), Council->get_nick()));
	}
	else if (MessageType == CCouncilMessage::TYPE_DEMAND_SUBMISSION)
	{
		CCouncil *
			SupremacyCouncil = PlayerCouncil->get_supremacy_council();	
		if (SupremacyCouncil != NULL)
		{
			SupremacyCouncil->send_time_news_to_all_member(
					(char *)format(GETTEXT("%1$s has bulldozed your ordinary council %2$s."), 
									PlayerCouncil->get_nick(),
									Council->get_nick()));
		}
	}
	else if (MessageType == CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER)
	{
        CCouncilActionMergingOffer *
			MyMergingOffer = new CCouncilActionMergingOffer();
		MyMergingOffer->set_owner(PlayerCouncil);
		MyMergingOffer->set_start_time(CGame::get_game_time());
		MyMergingOffer->set_argument(Council->get_id());

		MyMergingOffer->type(QUERY_INSERT);
		STORE_CENTER->store(*MyMergingOffer);

		PlayerCouncil->add_action(MyMergingOffer);
		COUNCIL_ACTION_TABLE->add_action(MyMergingOffer);

		CCouncilActionMergingOffer *
			TargetMergingOffer = new CCouncilActionMergingOffer();
		TargetMergingOffer->set_owner(Council);
		TargetMergingOffer->set_start_time(CGame::get_game_time());
		TargetMergingOffer->set_argument(PlayerCouncil->get_id());

		TargetMergingOffer->type(QUERY_INSERT);
		STORE_CENTER->store(*TargetMergingOffer);

		Council->add_action(TargetMergingOffer);
		COUNCIL_ACTION_TABLE->add_action(TargetMergingOffer);
	}

	CCouncilMessage *
		Message = new CCouncilMessage();
	Message->initialize(MessageType, PlayerCouncil, Council, Comment);

	Council->add_council_message(Message);

	Message->type(QUERY_INSERT);
	STORE_CENTER->store(*Message);

	ITEM("MENU_TITLE", GETTEXT("Council Diplomacy - Send Message"));

	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("You successfully sent a message to %1$s."),
							Council->get_nick()));

//	system_log("end page handler %s", get_name());

	return output("council/send_diplomatic_message_result.html");
}

