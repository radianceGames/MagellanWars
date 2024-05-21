#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageReadDiplomaticMessageDelete::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

//	int
//		MessageID = as_atoi(MessageIDString);

	int Index;
	bool some = false;
	Index = 0;
	CDiplomaticMessageBox *
		MessageBox = aPlayer->get_diplomatic_message_box();

	if (!MessageBox->length())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have any diplomatic messages received."));
		return output("diplomacy/read_diplomatic_message_error.html");
	}

	while (Index < MessageBox->length())
	{
		CDiplomaticMessage *
			Message = (CDiplomaticMessage *)MessageBox->get(Index);
		assert(Message);

		CString QueryVar;
		QueryVar.clear();
		QueryVar.format("MESSAGE_ID%d", Message->get_id());

		QUERY((char *)QueryVar, MessageIndexString);
				

		if (MessageIndexString)
		{
			if (!strcasecmp(MessageIndexString, "ON"))
			{
				Message->type(QUERY_DELETE);
				STORE_CENTER->store(*Message);
				MessageBox->remove_diplomatic_message(Message->get_id());
				some=true;
			}
			else
			{
				Index++;
			}
			
		}
		else
		{
			Index++;
		}
	}

//	system_log("end page handler %s", get_name());
	if(some==true)
	{
		ITEM("RESULT_MESSAGE",
						GETTEXT("The message(s) you selected were successfully deleted."));
				return output("diplomacy/read_diplomatic_message_delete.html");
	}

	ITEM("ERROR_MESSAGE",
					GETTEXT("You didn't select any message(s)."));
			return output("diplomacy/read_diplomatic_message_error.html");
	
}

