#include "util.h"

TZone gCollectionZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCollection),
	0,
	0,
	NULL,   
	"Zone CCollection"
};

CCollection::CCollection()
{
	mRoot = mCurrent = NULL;
	mLength = 0;
}

CCollection::~CCollection()
{
	if (mRoot) remove_all();
}

CNode*
CCollection::first()
{
	mCurrent = mRoot;
	return mCurrent;
}

CNode*
CCollection::next()
{
	if (!mRoot) return NULL;
	if (!mCurrent) return NULL;
	mCurrent = mCurrent->next();
	return mCurrent;
}

CNode*
CCollection::previous()
{
	if (!mRoot) return NULL;
	if (!mCurrent) return NULL;
	mCurrent = mCurrent->previous();
	return mCurrent;
}

CNode*
CCollection::last()
{
	if (!mRoot) return NULL;
	return (mCurrent = mTail);
}

CNode*
CCollection::operator + (int aNumber)
{
	if (!mRoot) return NULL;
	if (!mCurrent) return NULL;
	int index = 0;
	while((mCurrent != NULL)&&(index < aNumber))
	{
		mCurrent = mCurrent->next();
		index++;
	}
	return mCurrent;
}

CNode*
CCollection::operator - (int aNumber)
{
	if (!mRoot) return NULL;
	if (!mCurrent) return NULL;
	int index = 0;
	while((mCurrent != NULL)&&(index < aNumber))
	{
		mCurrent = mCurrent->previous();
		index++;
	}
	return mCurrent;
}

void
CCollection::remove_all()
{
	CNode
		*Temp = mRoot;

	while(Temp)
	{
		CNode
			*Next = Temp->next();
		delete Temp;
		Temp = Next;
	}
	mRoot = mTail = mCurrent = NULL;
	mLength = 0;
}

CNode*
CCollection::append(CNode* aNode)
{
	aNode->next(NULL);
	if (!mRoot)
	{
		aNode->previous(NULL);
		mRoot = mTail = mCurrent = aNode;
	} else {
		mTail->next(aNode);
		aNode->previous(mTail);
		mTail = mCurrent = aNode;
	}
	mLength++;
	return aNode;
}

CNode*
CCollection::insert(CNode* aNode)
{
	aNode->previous(NULL);
	if (!mRoot)
	{
		aNode->next(NULL);
		mRoot = mTail = mCurrent = aNode;
	} else {
		aNode->next(mRoot);
		mRoot->previous(aNode);
		mRoot = mCurrent = aNode;
	}
	mLength++;
	return aNode;
}

CNode*
CCollection::insert(CNode* pNode, CNode* aNode)
{
	if(pNode == mRoot) return insert(aNode);
//	if(!pNode->first() != mRoot) return NULL; //lengthy check
	aNode->previous(pNode->previous());
	aNode->previous()->next(aNode);
	aNode->next(pNode);
	pNode->previous(aNode);
	mLength++;
	return aNode;
}

bool
CCollection::remove(CNode* aNode)
{
	if (!unlink(aNode)) return false;
	delete aNode;
	return true;
}

bool
CCollection::unlink(CNode* aNode)
{
	if (!aNode) return false;
	if (!mRoot) return false;
	if (mRoot == aNode) mRoot = mRoot->next();
	if (mTail == aNode) mTail = mTail->previous();
	mLength--;
	aNode->unlink();
	return true;
}

bool
CCollection::is_empty()
{
	if (mRoot == NULL) return true;
	return false;
}

