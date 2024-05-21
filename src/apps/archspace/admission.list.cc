#include "council.h"
#include "game.h"

CAdmissionList::CAdmissionList()
{
}

CAdmissionList::~CAdmissionList()
{
	remove_all();
}

bool
CAdmissionList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CAdmissionList::compare(TSomething aItem1, TSomething aItem2) const
{
	CAdmission
		*Admission1 = (CAdmission*)aItem1,
		*Admission2 = (CAdmission*)aItem2;

	if (Admission1->get_time() > Admission2->get_time()) return 1;
	if (Admission1->get_time() < Admission2->get_time()) return -1;

	return 0;
}

int
CAdmissionList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CAdmission
		*Admission = (CAdmission*)aItem;

	if (Admission->get_time() > (time_t)aKey) return 1;
	if (Admission->get_time() < (time_t)aKey) return -1;

	return 0;
}

bool
CAdmissionList::remove_admission(int aPlayerID)
{
	for(int i=0; i<length(); i++)
	{
		CAdmission *
			Admission = (CAdmission *)get(i);
		if (Admission->get_player()->get_game_id() == aPlayerID)
		{
			return CSortedList::remove(i);
		}
	}

	return false;
}

int 
CAdmissionList::add_admission(CAdmission* aAdmission)
{
	if (!aAdmission) return -1;

	for(int i=0; i<length(); i++)
	{
		CAdmission* Admission = (CAdmission*)get(i);
		if (Admission->get_player()->get_game_id() 
					== aAdmission->get_player()->get_game_id())
			return -1;
	}

	insert_sorted(aAdmission);

	return aAdmission->get_player()->get_game_id();
}

CAdmission*
CAdmissionList::get_by_player_id(int aPlayerID)
{
	if (!length()) return NULL;

	for(int i=0; i<length(); i++)
	{
		CAdmission* Admission = (CAdmission*)get(i);
		if (Admission->get_player()->get_game_id() == aPlayerID)
			return Admission;
	}

	return NULL;
}

#define TABLE_SIZE 2

const char*
CAdmissionList::html_table()
{
	if (!length()) return NULL;

	static CString HTML;

	HTML.clear();

	for(int i=0; i<length(); i++) 
	{
		CAdmission *Admission = (CAdmission*)get(i);	
		assert(Admission);
		HTML += "<TR>\n";
		HTML.format("<TD CLASS=\"tabletxt\" WIDTH=36 ALIGN=CENTER>"
					"<FONT COLOR=\"#666666\">&nbsp;%d</FONT></TD>\n", i+1);
		HTML.format("<TD CLASS=\"tabletxt\" WIDTH=216 ALIGN=CENTER>%s</TD>\n",
					Admission->get_player()->get_nick());
		HTML.format("<TD CLASS=\"tabletxt\" WIDTH=180 ALIGN=CENTER>"
					"<FONT COLOR=\"#666666\">%s</FONT></TD>\n", 
					Admission->get_time_string());
		HTML.format("<TD CLASS=\"tabletxt\" WIDTH=64 ALIGN=CENTER>"
					"<A HREF=\"/archspace/council/player_admission_read.as?NO=%d\">"
					"<IMG SRC=\"%s/image/as_game/bu_read.gif\""
					" WIDTH=45 HEIGHT=11 BORDER=0></A></TD>\n",
					i+1,
					(char *)CGame::mImageServerURL);
		HTML += "</TR>\n";
	}

	return (char*)HTML;
}
