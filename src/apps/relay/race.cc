#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "data.h"

CRace::CRace()
{
	mName = "";
	mID = -1;
	mDescription = "";
}

CRace::~CRace()
{
}

bool
CRace::initialize(const char *aName, int aID, const char *aDescription)
{
	mName = aName;
	mID = aID;
	mDescription = aDescription;

	return true;
}
