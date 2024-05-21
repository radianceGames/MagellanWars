#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

CString
	gBribeDescription,
	gRequestBoonDescription,
	gRequestMediationDescription,
	gInvadeMagistrateDominionDescription,
	gInvadeEmpireDescription,
	gDemandTribute,
	gDemandLeaveCouncil,
	gDemandDeclareWar,
	gDemandMakeTruce,
	gDemandFreeSubordinaryCouncil,
	gDemandArmamentReduction,
	gGrantRank,
	gGrantBoon;


CPageEmpireLink::CPageEmpireLink()
{
	if( gBribeDescription.length() == 0 )
		gBribeDescription = GETTEXT("This is one method players have to try and please the Empire. Basically, you give PP to the Magistrate and hope that he says nice things about you to the Emperor. If this works, you may gain some points with the Emperor.");
	
	if (gRequestBoonDescription.length() == 0)
	{
		gRequestBoonDescription = GETTEXT("This is the basic means that all rulers have to request aid of the Empire. Depending on your current status, they may grant many favors, from money, to technology, and rumor has it, even planets!!!<BR>"
											"Warning: Requesting aid too much may hurt your standing with the Emperor.");
	}
	if( gRequestMediationDescription.length() == 0 )
		gRequestMediationDescription = GETTEXT("This is a way to get one of those annoying people to stop bothering you. If the Empire sees reason, they may intercede between you and another player and force a truce. During this forced truce, neither side may declare war.");
	if( gInvadeMagistrateDominionDescription.length() == 0 )
		gInvadeMagistrateDominionDescription = GETTEXT("This is one option that no truly loyal subject would use. This will cause you to lay siege to one of the Magistrates own planets. If you win, you may be able to gain some freedom from the Empire, but even if you succeed, the consequences could be dire if the Emperor chooses to intercede.");
	if( gInvadeEmpireDescription.length() == 0 )
		gInvadeEmpireDescription = GETTEXT("This is one option that no truly loyal subject would use. This will cause you to lay siege to one of central planets controlled by the Empire. By doing this, you will automatically be exiled and will most likely be attacked frequently by the Imperial Fleet.");
	if (gDemandTribute.length() == 0)
	{
		gDemandTribute = GETTEXT("To %1$s,<BR>"
									"The Emperor has requested that you again pay your tax that is due as a true servant of the Empire. The amount requested is %2$d and is due within 2 days.<BR>"
									"Lord Treasuree Constatiream,<BR>"
									"Imperial Treasurer");
		gDemandTribute += "<BR>\n";
	}
	if (gDemandLeaveCouncil.length() == 0)
	{
		gDemandLeaveCouncil = GETTEXT("To %1$s<BR>"
										"The Emperor has requested that you depart from your council. Your Council has been deemed disloyal. It is hereby officially requested that you leave your council within 2 days. If you follow this imperial order, your reward will be the pleasure of the Empire. If you fail to meet expectations, the cost will be mighty.<BR>"
										"Lord Chancellor Milleborm,<BR>"
										"Chancellor to his Supreme Emperor Jo'keric");
		gDemandLeaveCouncil += "<BR>\n";
	}
	if (gDemandDeclareWar.length() == 0)
	{
		gDemandDeclareWar = GETTEXT("To %1$s<BR>"
									"The Emperor has requested that you engage in war for the betterment of the Empire. %2$s has angered his Imperial Majesty. It is hereby officially requested that you attack this party within 2 days. If completed, your reward will be the pleasure of the Empire. If you fail, this cost will be mighty.<BR>"
									"Lord Chancellor Milleborm,<BR>"
									"Chancellor to his Supreme Emperor Jo'keric");
		gDemandDeclareWar += "<BR>\n";
	}
	if (gDemandMakeTruce.length() == 0)
	{
		gDemandMakeTruce = GETTEXT("To %1$s<BR>"
									"The Emperor has requested that you cease your senseless hostilities. %2$s is a loyal servant of his Imperial Majesty. It is hereby officially requested that you make peace within 2 days. If completed, your reward will be the pleasure of the Empire. If you fail to follow orders, the cost will be mighty.<BR>"
									"Lord Chancellor Milleborm,<BR>"
									"Chancellor to his Supreme Emperor Jo'keric");
		gDemandMakeTruce += "<BR>\n";
	}
	if (gDemandFreeSubordinaryCouncil.length() == 0)
	{
		gDemandFreeSubordinaryCouncil = GETTEXT("To %1$s<BR>"
												"The Emperor has requested that you stop trafficking in the lives of his Imperial Subjects. You are hereby ordered to free %2$s from Subordination. If you head this request, your reward will be the pleasure of the Empire. If you fail, the cost will be mighty.<BR>"
												"Lord Chancellor Milleborn,<BR>"
												"Chancellor to his Supreme Emperor Jo'keric");
		gDemandFreeSubordinaryCouncil += "<BR>\n";
	}
	if (gDemandArmamentReduction.length() == 0)
	{
		gDemandArmamentReduction = GETTEXT("To %1$s<BR>"
											"The Emperor has requested that you cease your buildup of forces. It is hereby officially requested that you disassemble a portion of your forces. If completed, your reward will be equal to your sacrifice for the Empire. If you fail to comply, the cost will be mighty.<BR>"
											"Lord Chancellor Milleborn,"
											"Chancellor to his Supreme Emperor Jo'keric");
		gDemandArmamentReduction += "<BR>\n";
	}
	if (gGrantRank.length() == 0)
	{
		gGrantRank = GETTEXT("The Empire has graciously decided to offer you the noble title of %1$s. The processing of the requisite forms will require %2$d PP.<BR>"
								"Will you accept the title?");
		gGrantRank += "<BR>\n";
	}
	if( gGrantBoon.length() == 0 )
		gGrantBoon = GETTEXT("You pleased Empire. Emperor is so happy with your deed so that he decided to grant you a small boon.");


	mBribeDescription = &gBribeDescription;
	mRequestBoonDescription = &gRequestBoonDescription;
	mRequestMediationDescription = &gRequestMediationDescription;
	mInvadeMagistrateDominionDescription = &gInvadeMagistrateDominionDescription;
	mInvadeEmpireDescription = &gInvadeEmpireDescription;
	mDemandTributeDescription = &gDemandTribute;
	mDemandLeaveCouncilDescription = &gDemandLeaveCouncil;
	mDemandDeclareWarDescription = &gDemandDeclareWar;
	mDemandMakeTruceDescription = &gDemandMakeTruce;
	mDemandFreeSubordinaryCouncilDescription = &gDemandFreeSubordinaryCouncil;
	mDemandArmamentReductionDescription = &gDemandArmamentReduction;
	mGrantRankDescription = &gGrantRank;
	mGrantBoonDescription = &gGrantBoon;
}

bool
CPageEmpireLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "EMPIRE");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "EMPIRE");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("EMPIRE_DIPLOMACY", GETTEXT("[Empire Diplomacy]"));

	ITEM("BRIBE", GETTEXT("[Bribe]"));
	ITEM("REQUEST_BOON", GETTEXT("[Request Boon]"));
	ITEM("REQUEST_MEDIATION", GETTEXT("[Request Mediation]"));
	ITEM("INVADE_EMPIRE", GETTEXT("[Invade Empire]"));

	ITEM("EMPIRE_RELATION_MESSAGE", GETTEXT("Empire Relation"));
	ITEM("COURT_RANK_MESSAGE", GETTEXT("Court Rank"));

	return true;
}
