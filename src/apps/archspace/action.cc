#include "game.h"
#include "action.h"

int CAction::mPeriodPlayerCouncilDonation = 4*24; // turn
int CAction::mPeriodPlayerDeclareHostile = 288; // turn
int CAction::mPeriodPlayerBreakPact = 4*72; // turn
int CAction::mPeriodPlayerBreakAlly = 4*72; // turn
int CAction::mPeriodCouncilDeclareTotalWar = 288; // turn
int CAction::mPeriodCouncilDeclareWar = 4*24; // turn
int CAction::mPeriodCouncilBreakPact = 4*72; // turn
int CAction::mPeriodCouncilBreakAlly = 4*72; // turn
int CAction::mPeriodCouncilImproveRelation = 4*72; // turn
int CAction::mPeriodCouncilMergingOffer = 24*60*60; //second
