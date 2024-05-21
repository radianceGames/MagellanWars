#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"

bool
CPageDiplomaticMessageReply::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	QUERY("MESSAGE_ID", IDString);
	CHECK(!IDString,
			GETTEXT("There is no way to find a message"
					" you want to answer."
					" Please ask Archspace Development Team."));

	int
		MessageID = as_atoi(IDString);

	CCouncilMessage *
		Message = aPlayer->get_council()->get_council_message(MessageID);
	CHECK(!Message, GETTEXT("Could not find such a message.")); 

	CCouncil *
		SenderCouncil = Message->get_sender();
	CHECK(SenderCouncil == NULL, GETTEXT("The sender does not exist."));

	QUERY("YOUR_COMMENT", YourCommentString);

	const char *YourComment;
	YourComment = (YourCommentString) ?
			YourCommentString : GETTEXT("(No comment)");

	ITEM("MENU_TITLE", GETTEXT("Council Diplomacy"));

	if (Message->check_need_required())
	{
		QUERY("REPLY", ReplyString);
		CHECK(!ReplyString,
				GETTEXT("You have to answer this message."));

		if (!strcmp(ReplyString, "ACCEPT"))
		{
			CCouncilMessage::EType Type;

			if (Message->get_type() == 
						CCouncilMessage::TYPE_SUGGEST_TRUCE)
				Type = CCouncilMessage::TYPE_REPLY_SUGGEST_TRUCE;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_DEMAND_TRUCE)
				Type = CCouncilMessage::TYPE_REPLY_DEMAND_TRUCE;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_SUGGEST_PACT)
				Type = CCouncilMessage::TYPE_REPLY_SUGGEST_PACT;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_SUGGEST_ALLY)
				Type = CCouncilMessage::TYPE_REPLY_SUGGEST_ALLY;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_DEMAND_SUBMISSION)
				Type = CCouncilMessage::TYPE_REPLY_DEMAND_SUBMISSION;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER)
				Type = CCouncilMessage::TYPE_REPLY_COUNCIL_FUSION_OFFER;
			else
			{
				CHECK(true, GETTEXT("That message is an unknown type."));
			}

			CCouncil *
				MyCouncil = aPlayer->get_council();
			if (Message->get_type() == CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER)
			{
				CString
					CanMerge;
				if (MyCouncil->get_power() >= SenderCouncil->get_power())
				{
					CanMerge = MyCouncil->can_merge_with(SenderCouncil, MyCouncil);
				}
				else if (MyCouncil->get_power() < SenderCouncil->get_power())
				{
					CanMerge = SenderCouncil->can_merge_with(MyCouncil, MyCouncil);
				}
				CHECK(CanMerge.length() > 0, (char *)CanMerge);

				Message->set_status(CCouncilMessage::STATUS_ANSWERED);
				Message->type(QUERY_UPDATE);
				STORE_CENTER->store(*Message);

				CString Comment;
				Comment.format(GETTEXT("I accept your offer about %1$s."),
							Message->get_type_description());
				Comment += "\n";
				Comment.format("%s\n", YourComment);
				Comment += "---------------------------\n";
				Comment.format("%s %s\n",
								GETTEXT("Original message"),
								Message->get_type_description());
				Comment.format("%s %s, %s\n",
								GETTEXT("From"),
								Message->get_sender()->get_nick(),
								Message->get_time_string());
				Comment += Message->get_content();

				CCouncilMessage *
					SMessage = new CCouncilMessage();
				SMessage->initialize(Type, aPlayer->get_council(), 
								Message->get_sender(), (char*)Comment);
				Message->get_sender()->add_council_message(SMessage);
				SMessage->type(QUERY_INSERT);
				STORE_CENTER->store(*SMessage);

				CString Msg;
				Msg.format(GETTEXT("You successfully replied to the message(#%1$s) from %2$s."), 
							dec2unit(Message->get_id()),
							Message->get_sender()->get_nick());
					
				Msg += "\n\n";
				Msg.format("%s<BR>\n", GETTEXT("Your Comment :"));
				Msg += (char *)Comment;

				Msg.htmlspecialchars();
				Msg.nl2br();

				ITEM("RESULT_MESSAGE", (char *)Msg);

				if (MyCouncil->get_power() >= SenderCouncil->get_power())
				{
					MyCouncil->process_merge_with(SenderCouncil, MyCouncil);
				}
				else if (MyCouncil->get_power() < SenderCouncil->get_power())
				{
					SenderCouncil->process_merge_with(MyCouncil, MyCouncil);
				}
			}
			else
			{
				const char *
					Ret = MyCouncil->improve_relationship(
							Message->get_type(), Message->get_sender());

				CHECK(Ret != NULL, Ret);

				Message->set_status(CCouncilMessage::STATUS_ANSWERED);
				Message->type(QUERY_UPDATE);
				STORE_CENTER->store(*Message);

				CString Comment;
				Comment.format(GETTEXT("I accept your offer about %1$s."),
							Message->get_type_description());
				Comment += "\n";
				Comment.format("%s\n", YourComment);
				Comment += "---------------------------\n";
				Comment.format("%s %s\n",
								GETTEXT("Original message"),
								Message->get_type_description());
				Comment.format("%s %s, %s\n",
								GETTEXT("From"),
								Message->get_sender()->get_nick(),
								Message->get_time_string());
				Comment += Message->get_content();

				CCouncilMessage *
					SMessage = new CCouncilMessage();
				SMessage->initialize(Type, aPlayer->get_council(), 
								Message->get_sender(), (char*)Comment);
				Message->get_sender()->add_council_message(SMessage);
				SMessage->type(QUERY_INSERT);
				STORE_CENTER->store(*SMessage);

				CString Msg;
				Msg.format(GETTEXT("You successfully replied to the message(#%1$s) from %2$s."), 
							dec2unit(Message->get_id()),
							Message->get_sender()->get_nick());
					
				Msg += "\n\n";
				Msg.format("%s<BR>\n", GETTEXT("Your Comment :"));
				Msg += (char *)Comment;

				Msg.htmlspecialchars();
				Msg.nl2br();

				ITEM("RESULT_MESSAGE", (char *)Msg);
			}
		}
		else if (!strcmp(ReplyString, "REJECT"))
		{
			CCouncilMessage::EType Type;

			if (Message->get_type() == 
						CCouncilMessage::TYPE_SUGGEST_TRUCE)
				Type = CCouncilMessage::TYPE_REPLY_SUGGEST_TRUCE;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_DEMAND_TRUCE)
				Type = CCouncilMessage::TYPE_REPLY_DEMAND_TRUCE;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_SUGGEST_PACT)
				Type = CCouncilMessage::TYPE_REPLY_SUGGEST_PACT;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_SUGGEST_ALLY)
				Type = CCouncilMessage::TYPE_REPLY_SUGGEST_ALLY;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_DEMAND_SUBMISSION)
				Type = CCouncilMessage::TYPE_REPLY_DEMAND_SUBMISSION;
			else if (Message->get_type() == 
						CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER) {
				Type = CCouncilMessage::TYPE_REPLY_COUNCIL_FUSION_OFFER;
				
				//added by zbyte - 2004-03-09
				CCouncil *MyCouncil = aPlayer->get_council();
				CCouncilActionMergingOffer
					*MyMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(MyCouncil, CAction::ACTION_COUNCIL_MERGING_OFFER),
					*TargetMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(SenderCouncil, CAction::ACTION_COUNCIL_MERGING_OFFER);

				MyMergingOffer->type(QUERY_DELETE);
				STORE_CENTER->store(*MyMergingOffer);

				TargetMergingOffer->type(QUERY_DELETE);
				STORE_CENTER->store(*TargetMergingOffer);

				COUNCIL_ACTION_TABLE->remove_action(MyMergingOffer->get_id());
				MyCouncil->remove_action(MyMergingOffer->get_id());
				COUNCIL_ACTION_TABLE->remove_action(TargetMergingOffer->get_id());
				SenderCouncil->remove_action(TargetMergingOffer->get_id());
				//end of added code
				
			} else {
				CHECK(true, GETTEXT("That message is an unknown type."));
			}

			Message->set_status(CCouncilMessage::STATUS_ANSWERED);
			Message->type(QUERY_UPDATE);
			STORE_CENTER->store(*Message);

			CString Comment;
			Comment.format(GETTEXT("I reject your offer about %1$s."),
						Message->get_type_description());
			Comment += "\n";
			Comment.format("%s\n", YourComment);

			Comment += "---------------------------\n";
			Comment.format("%s %s\n",
							GETTEXT("Original message"),
							Message->get_type_description());
			Comment.format("%s %s, %s\n",
							GETTEXT("From"),
							Message->get_sender()->get_nick(),
							Message->get_time_string());
			Comment += Message->get_content();

			CCouncilMessage *
				SMessage = new CCouncilMessage();
			SMessage->initialize(Type, aPlayer->get_council(), 
							Message->get_sender(), (char*)Comment);
			Message->get_sender()->add_council_message(SMessage);
			SMessage->type(QUERY_INSERT);
			STORE_CENTER->store(*SMessage);

			CString Msg;
			Msg.format(GETTEXT("You successfully replied to the message(#%1$s) from %2$s."), 
						dec2unit(Message->get_id()),
						Message->get_sender()->get_nick());
					
			Msg += "\n\n";
			Msg.format("%s\n", GETTEXT("Your Comment :"));
			Msg += (char *)Comment;

			Msg.htmlspecialchars();
			Msg.nl2br();

			ITEM("RESULT_MESSAGE", (char *)Msg);
		}
		else if (!strcmp(ReplyString, "WELL"))
		{
			Message->set_status(CCouncilMessage::STATUS_WELL);
			Message->type(QUERY_UPDATE);
			STORE_CENTER->store(*Message);

			ITEM("RESULT_MESSAGE", 
				GETTEXT("You hold off your decision to reply this message."));
		}
		else
		{
			CHECK(true, GETTEXT("Could not understand your answer."));
		}
	}
	else
	{
		CString
			Comment;
		Comment.clear();
		Comment.format("%s\n", YourComment);

		Comment += "---------------------------\n";
		Comment.format("%s %s\n",
						GETTEXT("Original message"),
						Message->get_type_description());
		Comment.format("%s %s, %s\n",
						GETTEXT("From"),
						Message->get_sender()->get_nick(),
						Message->get_time_string());
		Comment += Message->get_content();

		CCouncilMessage *
			SMessage = new CCouncilMessage();
		SMessage->initialize(CCouncilMessage::TYPE_REPLY,
					aPlayer->get_council(), Message->get_sender(), 
					(char *)Comment);
		Message->get_sender()->add_council_message(SMessage);
		SMessage->type(QUERY_INSERT);
		STORE_CENTER->store(*SMessage);

		CString Msg;
		Msg.format(GETTEXT("You successfully replied to the message(#%1$s) from %2$s."), 
					dec2unit(Message->get_id()),
					Message->get_sender()->get_nick());
				
		Msg += "\n\n";
		Msg.format("%s\n", GETTEXT("Your Comment :"));
		Msg += (char *)Comment;

		Msg.htmlspecialchars();
		Msg.nl2br();

		ITEM("RESULT_MESSAGE", (char *)Msg);
	}

//	system_log("end page handler %s", get_name());

	return output("council/diplomatic_message_reply.html");
}
