#include "common.h"
#include "util.h"
#include "message.h"
#include <cstdlib>
#include "define.h"
#include "archspace.h"
#include "game.h"
#include <cstdio>
#include <libintl.h>

TZone gDiplomaticMessageZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDiplomaticMessage),
	0,
	0,
	NULL,
	"Zone CDiplomaticMessage"
};

int CDiplomaticMessage::mMaxID = 0;

CDiplomaticMessage::CDiplomaticMessage()
{
	mID = 0;
	mType = TYPE_NORMAL;
	mSenderID = 0;
	mReceiverID = 0;
	mTime = time(0);
	mStatus = STATUS_UNREAD;
	mContent = "";
}

CDiplomaticMessage::CDiplomaticMessage(MYSQL_ROW aRow)
{
	enum
	{
		FIELD_ID = 0,
		FIELD_TYPE,
		FIELD_SENDER,
		FIELD_RECEIVER,
		FIELD_TIME,
		FIELD_STATUS
	};

	mID = atoi(aRow[FIELD_ID]);
	mType = (EType)atoi(aRow[FIELD_TYPE]);
	mSenderID = atoi(aRow[FIELD_SENDER]);
	mReceiverID = atoi(aRow[FIELD_RECEIVER]);
	mTime = (time_t)atoi(aRow[FIELD_TIME]);
	mStatus = (EStatus)atoi(aRow[FIELD_STATUS]);

	if (mID > mMaxID) mMaxID = mID;
}

CDiplomaticMessage::~CDiplomaticMessage()
{
}

int
CDiplomaticMessage::get_id()
{
	return mID;
}

CPlayer*
CDiplomaticMessage::get_sender()
{
	return PLAYER_TABLE->get_by_game_id(mSenderID);
}

CPlayer*
CDiplomaticMessage::get_receiver()
{
	return PLAYER_TABLE->get_by_game_id(mReceiverID);
}

time_t
CDiplomaticMessage::get_time()
{
	return mTime;
}

char *
CDiplomaticMessage::get_content()
{
	if( mContent.length() == 0 ) {
		FILE *fp;
		fp = fopen( get_file_name(), "r" );
		if( fp ){
			char buf[1000];
			while( fgets( buf, 999, fp ) ){
				mContent += buf;
			}
			fclose(fp);
		}
	}

	if( mContent.length() == 0 ) mContent = " ";
	return (char *)mContent;
}

CDiplomaticMessage::EType
CDiplomaticMessage::get_type()
{
	return mType;
}

const char*
CDiplomaticMessage::get_time_string()
{
	static char TimeString[50];
	struct tm *Time;

	Time = localtime(&mTime);
	strftime(TimeString, 50, "%Y/%m/%d/%H:%M", Time);

	return TimeString;
}


void
CDiplomaticMessage::initialize(CDiplomaticMessage::EType aType,
		CPlayer *aSender, CPlayer *aReceiver, const char *aContent)
{
	mMaxID++;
	mType = aType;
	mID = (mMaxID) ? mMaxID:1;
	mSenderID = aSender->get_game_id();
	mReceiverID = aReceiver->get_game_id();
	mContent = (aContent) ? aContent:GETTEXT("(No comment)");
	mStatus = STATUS_UNREAD;
	mTime = time(0);
}

CString&
CDiplomaticMessage::query()
{
	static CString
		Query;

	Query.clear();

	switch(CStore::type())
	{
		case QUERY_INSERT:
			Query.format("INSERT INTO diplomatic_message"
							" (id, type, sender, receiver, time, status) VALUES"
							" (%d, %d, %d, %d, %d, %d)",
							mID,
							(int)mType,
							mSenderID,
							mReceiverID,
							mTime,
							mStatus);

			save();
			break;
		case QUERY_UPDATE:
			Query.format("UPDATE diplomatic_message "
					"SET status = %d WHERE id = %d", mStatus, mID);
			break;
		case QUERY_DELETE:
			Query.format("DELETE FROM diplomatic_message WHERE id = %d", mID);
			erase();
			break;
	}

	mStoreFlag.clear();

	return Query;
}

const char *
CDiplomaticMessage::get_type_description(
		CDiplomaticMessage::EType aType)
{
	switch(aType)
	{
		case TYPE_NORMAL:
			return GETTEXT("Normal");
		case TYPE_SUGGEST_TRUCE:
			return GETTEXT("Suggest Truce");
		case TYPE_DEMAND_TRUCE:
			return GETTEXT("Demand Truce");
		case TYPE_DECLARE_WAR:
			return GETTEXT("Declare War");
		case TYPE_SUGGEST_PACT:
			return GETTEXT("Suggest Pact");
		case TYPE_BREAK_PACT:
			return GETTEXT("Break Pact");
		case TYPE_SUGGEST_ALLY:
			return GETTEXT("Suggest Ally");
		case TYPE_BREAK_ALLY:
			return GETTEXT("Break Ally");
		case TYPE_REPLY:
			return GETTEXT("Reply");
		case TYPE_REPLY_SUGGEST_TRUCE:
			return GETTEXT("Reply Suggest Truce");
		case TYPE_REPLY_DEMAND_TRUCE:
			return GETTEXT("Reply Demand Truce");
		case TYPE_REPLY_SUGGEST_PACT:
			return GETTEXT("Reply Suggest Pact");
		case TYPE_REPLY_SUGGEST_ALLY:
			return GETTEXT("Reply Suggest Ally");
		case TYPE_DECLARE_HOSTILITY:
			return "Declare Hostility";
	}
	return GETTEXT("None");
}

const char*
CDiplomaticMessage::message_select_html(CRelation::ERelation aRelation)
{
	static CString
		HTML;
	HTML.clear();

	switch (aRelation)
	{
		case CRelation::RELATION_WAR:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_SUGGEST_TRUCE,
					get_type_description(TYPE_SUGGEST_TRUCE));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DEMAND_TRUCE,
					get_type_description(TYPE_DEMAND_TRUCE));
			HTML += "</SELECT>\n";
			return (char *)HTML;
		case CRelation::RELATION_HOSTILE:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
								TYPE_DECLARE_WAR,
					get_type_description(TYPE_DECLARE_WAR));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_SUGGEST_TRUCE,
					get_type_description(TYPE_SUGGEST_TRUCE));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DEMAND_TRUCE,
					get_type_description(TYPE_DEMAND_TRUCE));
			HTML += "</SELECT>\n";
			return (char *)HTML;

		case CRelation::RELATION_NONE:
		case CRelation::RELATION_TRUCE:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DECLARE_WAR,
					get_type_description(TYPE_DECLARE_WAR));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_SUGGEST_PACT,
					get_type_description(TYPE_SUGGEST_PACT));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
								TYPE_DECLARE_HOSTILITY,
					get_type_description(TYPE_DECLARE_HOSTILITY));
			HTML += "</SELECT>\n";
			return (char*)HTML;

		case CRelation::RELATION_PEACE:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_BREAK_PACT,
					get_type_description(TYPE_BREAK_PACT));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_SUGGEST_ALLY,
					get_type_description(TYPE_SUGGEST_ALLY));
			HTML += "</SELECT>\n";
			return (char*)HTML;

		case CRelation::RELATION_ALLY:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_BREAK_ALLY,
					get_type_description(TYPE_BREAK_ALLY));

			HTML += "</SELECT>\n";
			return (char*)HTML;

		case CRelation::RELATION_TOTAL_WAR:
		case CRelation::RELATION_SUBORDINARY:
				return NULL;
	}

	return NULL;
}

const char*
CDiplomaticMessage::record_html(bool aIsForAdmin)
{
	static CString HTML;

	HTML.clear();

	HTML = "<TR>\n";

	HTML += "<TD CLASS=\"tabletxt\" WIDTH=35 ALIGN=CENTER>";
	HTML.format("<FONT COLOR=\"#666666\">&nbsp;%d</FONT>", mID);
	HTML += "</TD>\n";

	HTML += "<TD WIDTH=145 CLASS=\"tabletxt\">";
	if(get_sender() == NULL)
	{
		HTML.format("&nbsp;%s", GETTEXT("N/A"));
	}
	else if (get_sender()->get_game_id() == EMPIRE_GAME_ID)
	{
		HTML.format("&nbsp;%s", GETTEXT("GM"));
	}
	else
	{
		HTML.format("&nbsp;%s", get_sender()->get_nick());
	}

	HTML += "</TD>\n";

	HTML += "<TD WIDTH=100 CLASS=\"tabletxt\">";
	HTML.format("&nbsp;%s", get_time_string());
	HTML += "</TD>\n";

	HTML += "<TD WIDTH=105 CLASS=\"tabletxt\">";
	HTML.format("<FONT COLOR=\"#666666\">&nbsp;%s</FONT>",
				get_type_description(get_type()));
	HTML += "</TD>\n";

	HTML += "<TD CLASS=\"tabletxt\" WIDTH=55 ALIGN=CENTER VALIGN=MIDDLE>";
	HTML.format("<IMG SRC=\"%s/image/as_game/council/%s\">",
				(char *)CGame::mImageServerURL,
				(check_need_required()) ?
						"required1_icon.gif" : "notrequired1_icon.gif");
	HTML += "</TD>\n";

	HTML += "<TD CLASS=\"tabletxt\" WIDTH=55 ALIGN=CENTER>";
	if (aIsForAdmin == false)
	{
		HTML.format("<A HREF=\"read_diplomatic_message_unanswered.as?MESSAGE_ID=%d\">",
					mID);
		HTML.format("<IMG SRC=\"%s/image/as_game/bu_read.gif\" BORDER=0>",
					(char *)CGame::mImageServerURL);
		HTML += "</A>\n";
	}
	else
	{

		HTML += "<FORM METHOD=post ACTION=\"player_message_read.as?MESSAGE_ID VALUE=\">\n";
		HTML.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_read.gif\">\n",
					(char *)CGame::mImageServerURL);
		HTML.format("<INPUT TYPE=hidden NAME=PLAYER_GAME_ID VALUE=%d>\n", mReceiverID);
		HTML.format("<INPUT TYPE=hidden NAME=MESSAGE_ID VALUE=%d>\n", mID);
		HTML += "</FORM>\n";
	}
	HTML += "</TD>\n";

	HTML += "<TD CLASS=\"tabletxt\" WIDTH=55 ALIGN=CENTER>";
/*	if (aIsForAdmin == true)
	{
		HTML.format("<A HREF=\"player_message_delete_confirm.as?MESSAGE_ID=%d\">",
					get_id());
		HTML.format("<IMG SRC=\"%s/image/as_game/bu_delete.gif\" BORDER=0></A>",
					(char *)CGame::mImageServerURL);
	}
	else
	{
*/
	//	HTML.format("<FORM METHOD=GET ACTION=\"read_diplomatic_message_delete.as?MESSAGE_ID=%d\">\n", mID);
	//	HTML += "<FORM METHOD=post ACTION=\"player_message_delete.as\">\n";
	//	HTML.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_delete.gif\">\n",
	//				(char *)CGame::mImageServerURL);
	//	HTML.format("<INPUT TYPE=hidden NAME=PLAYER_GAME_ID VALUE=%d>\n", mReceiverID);
	//	HTML.format("<INPUT TYPE=hidden NAME=MESSAGE_ID VALUE=%d>\n", get_id());
	//	HTML += "</FORM>\n";
		HTML.format("<INPUT TYPE=CHECKBOX NAME=\"MESSAGE_ID%d\">", mID);
	//  HTML.format("<IMG SRC=\"%s/image/as_game/bu_delete.gif\" BORDER=0></A>\n",  (char *)CGame::mImageServerURL);
//	}
	HTML += "</TD>\n";

	HTML += "</TR>\n";

	return (char*)HTML;
}

bool
CDiplomaticMessage::check_need_required()
{
	if (mStatus == STATUS_WELL) return true;
	if (mStatus == STATUS_ANSWERED) return false;

	return CDiplomaticMessage::check_need_required(mType);
}

bool
CDiplomaticMessage::check_need_required(int aMessageType)
{
	switch (aMessageType)
	{
		case TYPE_NORMAL: return false;
		case TYPE_SUGGEST_TRUCE: return true;
		case TYPE_DEMAND_TRUCE: return true;
		case TYPE_DECLARE_WAR: return false;
		case TYPE_SUGGEST_PACT: return true;
		case TYPE_BREAK_PACT: return false;
		case TYPE_SUGGEST_ALLY: return true;
		case TYPE_BREAK_ALLY: return false;
		case TYPE_REPLY: return false;
		case TYPE_REPLY_SUGGEST_TRUCE: return false;
		case TYPE_REPLY_DEMAND_TRUCE: return false;
		case TYPE_REPLY_SUGGEST_PACT: return false;
		case TYPE_REPLY_SUGGEST_ALLY: return false;
		case TYPE_DECLARE_HOSTILITY: return false;
	}
	return false;
}

CDiplomaticMessage::EStatus
CDiplomaticMessage::get_status()
{
	return mStatus;
}

void
CDiplomaticMessage::set_status(CDiplomaticMessage::EStatus aStatus)
{
	mStatus = aStatus;
}

const char*
CDiplomaticMessage::get_type_description()
{
	return get_type_description(CDiplomaticMessage::mType);
}

const char*
CDiplomaticMessage::get_status_description()
{
	return get_status_description(mStatus);
}

const char*
CDiplomaticMessage::get_status_description(
					CDiplomaticMessage::EStatus aStatus)
{
	switch(aStatus)
	{
		case STATUS_UNREAD: return GETTEXT("Unread");
		case STATUS_READ: return GETTEXT("Read");
		case STATUS_ANSWERED: return GETTEXT("Answer");
		case STATUS_WELL: return GETTEXT("Well");
	}
	return GETTEXT("None");
}

const char*
CDiplomaticMessage::get_reply_html()
{
	static CString HTML;

	HTML = "<TABLE WIDTH=506 BORDER=1 CELLSPACING=0 "
			"CELLPADDING=0 BORDERCOLOR=\"#2A2A2A\">\n";
	if (check_need_required())
	{
		HTML +=	"<TR CLASS=\"tabletxt\">\n"
					"<TH CLASS=\"tabletxt\" WIDTH=113 "
					"BGCOLOR=\"#171717\" ALIGN=RIGHT>"
						"<FONT COLOR=\"666666\">";
		HTML.format("&nbsp;%s", GETTEXT("Your Reply"));
		HTML += 		"</FONT>\n"
					"</TH>\n"
					"<TD COLSPAN=3 CLASS=\"tabletxt\" ALIGN=LEFT "
					"WIDTH=387>\n";
		HTML.format("&nbsp;%s", GETTEXT("Accept"));
		HTML +=			"<INPUT TYPE=\"radio\" "
							"NAME=\"REPLY\" VALUE=\"ACCEPT\">\n";
		HTML.format("&nbsp;&nbsp;&nbsp;&nbsp;%s", GETTEXT("Reject"));
		HTML += 		"<INPUT TYPE=\"radio\" "
							"NAME=\"REPLY\" VALUE=\"REJECT\">\n";
		HTML.format("&nbsp;&nbsp;&nbsp;&nbsp;%s", GETTEXT("Well"));
		HTML +=			"<INPUT TYPE=\"radio\" "
							"NAME=\"REPLY\" VALUE=\"WELL\">\n"
					"</TD>\n"
				"</TR>\n";
	}
	HTML +=		"<TR CLASS=\"tabletxt\">\n"
					"<TH CLASS=\"tabletxt\" WIDTH=113 ALIGN=RIGHT "
					"BGCOLOR=\"#171717\" VALIGN=TOP>\n"
						"<FONT COLOR=\"666666\">\n";
	HTML.format("&nbsp;%s", GETTEXT("Your Comment"));
	HTML +=				"</FONT>\n"
					"</TH>\n"
				"<TD COLSPAN=3 CLASS=\"tabletxt\" VALIGN=MIDDLE "
				"WIDTH=387>\n"
				"<TEXTAREA NAME=YOUR_COMMENT WARP=VIRTUAL COLS=50 "
				"ROWS=5></TEXTAREA>\n"
				"</TD>\n"
			"</TR>\n"
		"</TABLE>\n";

	if (!HTML.length()) return " ";
	return (char*)HTML;
}

const char *
CDiplomaticMessage::get_file_name()
{
	static CString
		FileName;

	FileName.clear();

	FileName.format( "%s/%d", ARCHSPACE->configuration().get_string( "Game", "DiplomaticMessageDir" ), get_id() );

	return (char*)FileName;
}

void
CDiplomaticMessage::save()
{
	FILE
		*fp;
	CString
		FileName;

	FileName = get_file_name();

	fp = fopen( (char*)FileName, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open diplomatic message file %s for save", (char*)FileName );
	} else {
		fprintf(fp, "%s", (char *)mContent);
		fclose(fp);
	}
}

void
CDiplomaticMessage::erase()
{
	CString
		FileName;

	FileName = get_file_name();

	unlink( (char*)FileName );
}
