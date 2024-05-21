#include "util.h"

TZone gNodeZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CNode),
	0,
	0,
	NULL,
	"Zone CNode"
};

CNode::CNode()
{
	mNext = NULL;
	mPrevious = NULL;
}

CNode::~CNode()
{
}

CNode*
CNode::next(CNode* aNode)
{
	mNext = aNode;
	return this;
}

CNode*
CNode::previous(CNode* aNode)
{
	mPrevious = aNode;
	return this;
}

CNode*
CNode::last() const 
{
	CNode 
		*Temp = (CNode*)this;
	
	while(Temp->next()) Temp = Temp->next();

	return Temp;
}

CNode*
CNode::first() const 
{
	CNode 
		*Temp = (CNode*)this;
	
	while(Temp->previous()) Temp = Temp->previous();

	return Temp;
}

void
CNode::unlink()
{
	if (mPrevious) mPrevious->next(mNext);
	if (mNext) mNext->previous(mPrevious);
}

