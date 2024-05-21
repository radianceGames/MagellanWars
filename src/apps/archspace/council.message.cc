#include "common.h"
#include "util.h"
#include "message.h"
#include <cstdlib>
#include "define.h"
#include "archspace.h"
#include "game.h"
#include "council.h"
#include <cstdio>
#include <libintl.h>

TZone gCouncilMessageZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCouncilMessage),
	0,
	0,
	NULL,
	"Zone CCouncilMessage"
};

int CCouncilMessage::mMaxID = 0;

CCouncilMessage::CCouncilMessage()
{
	mID = 0;
	mType = TYPE_NORMAL;
	mSenderID = 0;
	mReceiverID =0;
	mTime = time(0);
	mStatus = STATUS_UNREAD;
	mContent = "";
}

CCouncilMessage::CCouncilMessage(MYSQL_ROW aRow)
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

CCouncilMessage::~CCouncilMessage()
{
}

int 
CCouncilMessage::get_id()
{
	return mID;
}

CCouncil*
CCouncilMessage::get_sender()
{
	return COUNCIL_TABLE->get_by_id(mSenderID);
}

CCouncil*
CCouncilMessage::get_receiver()
{
	return COUNCIL_TABLE->get_by_id(mReceiverID);
}

time_t 
CCouncilMessage::get_time()
{
	return mTime;
}

char *
CCouncilMessage::get_content()
{
	if (mContent.length() == 0)
	{
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

CCouncilMessage::EType
CCouncilMessage::get_type()
{
	return mType;
}

const char* 
CCouncilMessage::get_time_string()
{
	static char TimeString[50];
	struct tm *Time;

	Time = localtime(&mTime);
	strftime(TimeString, 50, "%Y/%m/%d/%H:%M", Time);

	return TimeString;
}

void
CCouncilMessage::initialize(CCouncilMessage::EType aType,
		CCouncil *aSender, CCouncil *aReceiver, const char *aContent)
{
	mMaxID++;
	mType = aType;
	mID = (mMaxID) ? mMaxID:1;
	mSenderID = aSender->get_id();
	mReceiverID = aReceiver->get_id();
	CPlayer* Speaker = 
		aSender->get_member_by_game_id(aSender->get_speaker_id());
	mContent.format("Speaker %s said\n", Speaker->get_nick());
	mContent += (aContent) ? aContent:GETTEXT("(No comment)");
	mStatus = STATUS_UNREAD;
	mTime = time(0);
}

CString&
CCouncilMessage::query()
{
	static CString
		Query;

	Query.clear();

	switch(CStore::type())
	{
		case QUERY_INSERT:
			Query.format("INSERT INTO council_message"
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
			Query.format("UPDATE council_message "
					"SET status = %d WHERE id = %d", mStatus, mID);
			break;
		case QUERY_DELETE:
			Query.format("DELETE FROM council_message WHERE id = %d", mID);
			erase();
			break;
	}

	mStoreFlag.clear();

	return Query;
}

const char *
CCouncilMessage::get_type_description(
		CCouncilMessage::EType aType)
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
		case TYPE_DEMAND_SUBMISSION:
			return GETTEXT("Demand Submission");
		case TYPE_COUNCIL_FUSION_OFFER:
			return GETTEXT("Council Fusion Offer");
		case TYPE_SET_FREE_SUBORDINARY:
			return GETTEXT("Set Free Subordinary");
		case TYPE_DECLARE_INDEPENDENCE:
			return GETTEXT("Declare Independence");
		case TYPE_DECLARE_TOTAL_WAR:
			return GETTEXT("Declare Total War");
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
		case TYPE_REPLY_DEMAND_SUBMISSION: 
			return GETTEXT("Reply Demand Submission");
		case TYPE_REPLY_COUNCIL_FUSION_OFFER: 
			return GETTEXT("Reply Council Fusion Offer");
	}
	return GETTEXT("None");
}

const char*
CCouncilMessage::message_select_html(CCouncilRelation *aRelation, CCouncil *aCouncil)
{
	static CString
		HTML;
	HTML.clear();

	if (aRelation == NULL)
	{
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
				TYPE_DEMAND_SUBMISSION,
				get_type_description(TYPE_DEMAND_SUBMISSION));

		HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
				TYPE_COUNCIL_FUSION_OFFER,
				get_type_description(TYPE_COUNCIL_FUSION_OFFER));

		HTML += "</SELECT>\n";
		return (char*)HTML;
	}

	switch (aRelation->get_relation())
	{
		case CRelation::RELATION_WAR:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DEMAND_TRUCE,
					get_type_description(TYPE_DEMAND_TRUCE));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_SUGGEST_TRUCE,
					get_type_description(TYPE_SUGGEST_TRUCE));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DEMAND_SUBMISSION,
					get_type_description(TYPE_DEMAND_SUBMISSION));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DECLARE_TOTAL_WAR,
					get_type_description(TYPE_DECLARE_TOTAL_WAR));

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
					TYPE_DEMAND_SUBMISSION,
					get_type_description(TYPE_DEMAND_SUBMISSION));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_COUNCIL_FUSION_OFFER,
					get_type_description(TYPE_COUNCIL_FUSION_OFFER));

			HTML += "</SELECT>\n";
			return (char *)HTML;

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

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DEMAND_SUBMISSION,
					get_type_description(TYPE_DEMAND_SUBMISSION));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_COUNCIL_FUSION_OFFER,
					get_type_description(TYPE_COUNCIL_FUSION_OFFER));

			HTML += "</SELECT>\n";
			return (char *)HTML;

		case CRelation::RELATION_ALLY:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_BREAK_ALLY,
					get_type_description(TYPE_BREAK_ALLY));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_COUNCIL_FUSION_OFFER,
					get_type_description(TYPE_COUNCIL_FUSION_OFFER));

			HTML += "</SELECT>\n";
			return (char *)HTML;

		case CRelation::RELATION_TOTAL_WAR:
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

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_DEMAND_SUBMISSION,
					get_type_description(TYPE_DEMAND_SUBMISSION));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_COUNCIL_FUSION_OFFER,
					get_type_description(TYPE_COUNCIL_FUSION_OFFER));

			HTML += "</SELECT>\n";
			return (char *)HTML;

		case CRelation::RELATION_SUBORDINARY:
			HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";
	
			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_NORMAL,
					get_type_description(TYPE_NORMAL));

			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					TYPE_COUNCIL_FUSION_OFFER,
					get_type_description(TYPE_COUNCIL_FUSION_OFFER));

			if (aRelation->get_council1() == aCouncil)
			{
				HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
						TYPE_SET_FREE_SUBORDINARY,
						get_type_description(TYPE_SET_FREE_SUBORDINARY));

			}
			else
			{
				HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
						TYPE_DECLARE_INDEPENDENCE,
					get_type_description(TYPE_DECLARE_INDEPENDENCE));
			}

			HTML += "</SELECT>\n";
	
			return (char *)HTML;
	}

	return NULL;
}

const char*
CCouncilMessage::record_html(bool aIsForAdmin)
{
	static CString HTML;

	HTML.clear();

	HTML = "<TR>\n";
	HTML += "<TD CLASS=\"tabletxt\" WIDTH=32 ALIGN=CENTER>\n";
	HTML.format("<FONT COLOR=\"#666666\">&nbsp;%d</FONT>\n", get_id());
	HTML += "</TD>\n";

	if (get_sender() == NULL)
	{
		HTML.format("<TD WIDTH=149 CLASS=\"tabletxt\" BGCOLOR=\"#171717\">"
			"&nbsp;%s</TD>\n",
					GETTEXT("N/A"));
	}
	else
	{
		HTML.format("<TD WIDTH=149 CLASS=\"tabletxt\" BGCOLOR=\"#171717\">"
			"&nbsp;%s</TD>\n",
					get_sender()->get_nick());
	}

	HTML += "<TD WIDTH=119 CLASS=\"tabletxt\">\n";
	HTML.format("<FONT COLOR=\"#666666\">&nbsp;%s</FONT>",
				get_time_string());
	HTML += "</TD>\n";

	HTML += "<TD CLASS=\"tabletxt\" WIDTH=119>\n";
	HTML.format("<FONT COLOR=\"#666666\">&nbsp;%s</FONT>",
				get_type_description(get_type()));
	HTML += "</TD>\n";

	HTML += "<TD CLASS=\"tabletxt\" WIDTH=47 ALIGN=CENTER VALIGN=MIDDLE>\n";
	HTML.format("<IMG SRC=\"%s/image/as_game/council/%s\" WIDTH=45 HEIGHT=16>", 
				(char *)CGame::mImageServerURL,
				(check_need_required()) ? 
						"required1_icon.gif":"notrequired1_icon.gif");
	HTML += "</TD>\n";

	HTML += "<TD CLASS=\"tabletxt\" WIDTH=70 ALIGN=CENTER>\n";
	if (aIsForAdmin == true)
	{
		HTML += "<FORM METHOD=post ACTION=\"council_message_read.as\">\n";
		HTML += "<INPUT TYPE=image SRC=\"%s/image/as_game/bu_read.gif\">\n";
		HTML.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n", mReceiverID);
		HTML.format("<INPUT TYPE=hidden NAME=MESSAGE_ID VALUE=%d>\n", mID);
		HTML += "</FORM>\n";
	}
	else
	{
		HTML.format("<A HREF=\"diplomatic_message_read.as?MESSAGE_ID=%d\">", get_id());
	}
	HTML.format("<IMG SRC=\"%s/image/as_game/bu_read.gif\" WIDTH=45 HEIGHT=11 BORDER=0>\n",
				(char *)CGame::mImageServerURL);
	HTML += "</A>\n";
	HTML += "</TD>\n";

	HTML += "</TR>\n";

	return (char*)HTML;
}

bool
CCouncilMessage::check_need_required()
{
	if (mStatus == STATUS_WELL) return true;
	if (mStatus == STATUS_ANSWERED) return false;

	return CCouncilMessage::check_need_required(mType);
}

bool
CCouncilMessage::check_need_required(int aMessageType)
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
		case TYPE_DEMAND_SUBMISSION: return true;
		case TYPE_DECLARE_TOTAL_WAR: return false;
		case TYPE_COUNCIL_FUSION_OFFER: return true;
		case TYPE_DECLARE_INDEPENDENCE: return true;
		case TYPE_SET_FREE_SUBORDINARY: return true;
		case TYPE_REPLY: return false;
		case TYPE_REPLY_SUGGEST_TRUCE: return false;
		case TYPE_REPLY_DEMAND_TRUCE: return false;
		case TYPE_REPLY_SUGGEST_PACT: return false;
		case TYPE_REPLY_SUGGEST_ALLY: return false;
		case TYPE_REPLY_DEMAND_SUBMISSION: return false;
		case TYPE_REPLY_COUNCIL_FUSION_OFFER: return false;
	}
	return false;
}

CCouncilMessage::EStatus 
CCouncilMessage::get_status()
{
	return mStatus;
}

void
CCouncilMessage::set_status(CCouncilMessage::EStatus aStatus)
{
	mStatus = aStatus;
}

const char*
CCouncilMessage::get_type_description()
{
	return get_type_description(CCouncilMessage::mType);
}

const char*
CCouncilMessage::get_status_description()
{
	return get_status_description(mStatus);
}

const char*
CCouncilMessage::get_status_description(
					CCouncilMessage::EStatus aStatus)
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
CCouncilMessage::get_reply_html()
{
	static CString HTML;

	HTML = "<TABLE WIDTH=506 BORDER=1 CELLSPACING=0 "
			"CELLPADDING=0 BORDERCOLOR=\"#2A2A2A\">\n";
	if (check_need_required())
	{
		HTML +=	"<TR CLASS=\"tabletxt\">\n";

		HTML += "<TH CLASS=\"tabletxt\" WIDTH=113 BGCOLOR=\"#171717\" ALIGN=RIGHT>";
		HTML += "<FONT COLOR=\"666666\">";
		HTML.format("&nbsp;%s", GETTEXT("Your Reply"));
		HTML += "</FONT>\n";
		HTML += "</TH>\n";

		HTML += "<TD COLSPAN=3 CLASS=\"tabletxt\" ALIGN=LEFT WIDTH=387>\n";
		HTML.format("&nbsp;%s", GETTEXT("Accept"));
		HTML += "<INPUT TYPE=\"radio\" NAME=\"REPLY\" VALUE=\"ACCEPT\">";
		HTML.format("&nbsp;&nbsp;&nbsp;&nbsp;%s", GETTEXT("Reject"));
		HTML += "<INPUT TYPE=\"radio\" NAME=\"REPLY\" VALUE=\"REJECT\">";
		HTML.format("&nbsp;&nbsp;&nbsp;&nbsp;%s", GETTEXT("Well"));
		HTML += "<INPUT TYPE=\"radio\" NAME=\"REPLY\" VALUE=\"WELL\">";
		HTML += "</TD>\n";

		HTML += "</TR>\n";
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
CCouncilMessage::get_file_name()
{
	static CString
		FileName;

	FileName.clear();
	FileName.format( "%s/%d", ARCHSPACE->configuration().get_string( "Game", "CouncilMessageDir" ), get_id() );

	return (char*)FileName;
}

void
CCouncilMessage::save()
{
	FILE
		*fp;
	CString
		FileName;

	FileName = get_file_name();

	fp = fopen( (char*)FileName, "w+" );
	if( fp == NULL ){
		SLOG( "cannot open council message file %s for save", (char*)FileName );
	} else {
		fputs( (char*)mContent, fp );
		fclose(fp);
	}
}

void
CCouncilMessage::erase()
{
	CString
		FileName;

	FileName = get_file_name();

	unlink( (char*)FileName );
}
