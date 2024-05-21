#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../council.h"

bool
CPagePlayerAdmissionReply::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *
		council = aPlayer->get_council();

	ITEM("MENU_TITLE", GETTEXT("Player Admission"));

	QUERY( "REACTION", reaction );
	if ( !reaction )
	{
		ITEM( "ERROR_MESSAGE", GETTEXT("You haven't decided on your reply.") );
		return output("council/speaker_menu_error.html");
	}
	QUERY( "ADMISSION", admissionNo );
	if ( !admissionNo )
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Could not find the admission request record."));
		return output("council/speaker_menu_error.html");
	}
	CAdmission *admission = council->get_admission_by_index(as_atoi(admissionNo)-1);
	if (!admission)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Could not find the admission request record."));
		return output("council/speaker_menu_error.html");
    }
	QUERY( "PLAYER_NO", playerNo );
	int
		playerID = as_atoi(playerNo);
	CPlayer *
		player = (CPlayer *)PLAYER_TABLE->get_by_game_id(playerID);
	if (player == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no player with ID %1$s."),
								dec2unit(playerID)));
		return output("council/speaker_menu_error.html");
	}
	
	if(!player->can_request_admission())
    {
        ITEM("ERROR_MESSAGE", player->admission_limit_time_left());
        return output("council/speaker_menu_error.html");
    }
    
	QUERY("MESSAGE", message)

	CDiplomaticMessage *
		reply = new CDiplomaticMessage();
	CString
		buf,
		buf2;
	buf.clear();
	buf2.clear();
	if( strcmp(reaction, "YES") == 0)
	{
		if (council->get_number_of_members() == council->max_member() )
		{
			ITEM("ERROR_MESSAGE", GETTEXT("Your council is full.") );
			return output("council/speaker_menu_error.html");
		}
		CCouncil *
			OldCouncil = player->get_council();
		CPlayerList *
			OldCouncilMembers = OldCouncil->get_members();

		if (OldCouncilMembers->length() == 1)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The player %1$s is the only member in the council %2$s right now."),
									player->get_nick(),
									OldCouncil->get_nick()));
			return output("council/speaker_menu_error.html");
		}
		if (OldCouncil->get_speaker_id() == player->get_game_id())
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The player %1$s is the speaker in the council %2$s right now."),
									player->get_nick(),
									OldCouncil->get_nick()));
			return output("council/speaker_menu_error.html");
		}

		admission->type(QUERY_DELETE);
		STORE_CENTER->store(*admission);
		council->remove_admission(player);
		player->reset_admission();

        /*
        if (player->get_power() > (council->get_power()) && council->check_war()) {
            council->change_honor(-25);
        }
        if (council->check_war()) {
            council->change_honor(-5);
            aPlayer->change_honor(-5);
            player->change_honor(-5);
        }
        if (player->get_council()->check_war()) {
            council->change_honor(-10);
            aPlayer->change_honor(-5);
            player->change_honor(-10);
        }
        */

		OldCouncil->remove_member(player->get_game_id());
		council->add_member(player);
		player->set_council(council);
		player->disable_admission(CPlayer::mAdmissionLimitTime);
		
		int HonorLoss;

		if (council->get_honor() > player->get_honor()) {
			HonorLoss = council->get_honor() - player->get_honor();
			//HonorLoss /= 2;
			council->change_honor(-HonorLoss);
		} 
/*        else if ((council->get_honor() < player->get_honor()) && council->get_honor() < 25) {
            HonorLoss = (int) (player->get_honor() * 1 / 4);
            if (HonorLoss < 1) {
                HonorLoss = 1;
            }
            player->change_honor(-HonorLoss);
            aPlayer->change_honor(-HonorLoss);
        }     */


		buf.format( GETTEXT("You accepted %1$s (#%2$d) as your council member."), player->get_name(), player->get_game_id() );
		buf2.format("%s\n",
				GETTEXT("Your admission has been accepted."));
		if (message == NULL)
		{
			buf2 += " ";
		}
		else
		{
			buf2.format(" %s", message);
		}
		reply->initialize(CDiplomaticMessage::TYPE_REPLY, aPlayer, player, buf2 );

		ADMIN_TOOL->add_emigration_log(
				(char *)format("The player %s has emigrated from the council %s to the council %s.",
								player->get_nick(),
								OldCouncil->get_nick(),
								council->get_nick()));
	}
	else if( strcmp(reaction, "NO" ) == 0)
	{
		admission->type(QUERY_DELETE);
		STORE_CENTER->store(*admission);
		player->reset_admission();
		council->remove_admission(player);
		
		buf.format(GETTEXT("You refused the admission request from %1$s."),
					player->get_nick());
		buf2.format("%s\n",
				GETTEXT("Your admission has been rejected."));
		if (message == NULL)
		{
			buf2 += " ";
		}
		else
		{
			buf2.format(" %s", message);
		}

		reply->initialize(CDiplomaticMessage::TYPE_REPLY, aPlayer, player, buf2 );
	}
	else	//WELL
	{
		buf.format(GETTEXT("You will answer %1$s later."),
					player->get_nick());
		buf2.format(GETTEXT("Your admission will be replied later."),
					message );
		buf2 += "<BR>\n";
		reply->initialize(CDiplomaticMessage::TYPE_REPLY, aPlayer, player, buf2 );
	}

	player->add_diplomatic_message(reply);
	reply->CStore::type(QUERY_INSERT);
	STORE_CENTER->store(*reply);

	if( !message )
	{
		buf.clear();
		buf.format("%s : %s",
					GETTEXT("Your Comment"),
					GETTEXT("None"));
	}
	ITEM("RESULT_MESSAGE", buf);

//	system_log("end page handler %s", get_name());

	return output("council/player_admission_reply.html");
}
