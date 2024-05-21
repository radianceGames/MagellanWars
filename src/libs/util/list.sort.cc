#include "util.h"

CSortedList::CSortedList(int aLimit, int aThreshold):
	CList(aLimit, aThreshold)
{
}

CSortedList::~CSortedList()
{
}

int 
CSortedList::find_key(TConstSomething aKey) const
{
	for(int Index=0; Index < length(); Index++)
		if (compare_key(mRoot[Index], aKey) == 0)
			return Index;
	return -1;
}

int 
CSortedList::find_sorted_key(TConstSomething aKey) const
{
	int 
		Middle = 0,
		Left = 0,
		Right = length() - 1,
		Compare;

	while(Left <= Right)
	{
		Middle = (Left + Right) / 2;
		Compare = compare_key(mRoot[Middle], aKey);
		
		if (Compare == 0)
			return Middle;
		else if (Compare < 0)
			Left = Middle + 1;
		else 
			Right = Middle - 1;
	}

	return -1;
}

int 
CSortedList::insert_sorted(TSomething aItem, int *aEqual)
{
	int 
		Middle = 0,
		Left = 0,
		Right = length() - 1,
		Compare;

	while(Left <= Right)
	{
		Middle = (Left + Right) / 2;
		Compare = compare(mRoot[Middle], aItem);

		if (Compare == 0)
		{
			insert(Middle, aItem);
			if (aEqual)
				*aEqual = Middle+1;
			return Middle;
		} else if (Compare < 0)
			Left = Middle + 1;
		else 
			Right = Middle - 1;
	}
	if (Right == Middle)
		Middle++;
	insert(Middle, aItem);
	if (aEqual)
		*aEqual = -1;
	
	return Middle;
}

void 
CSortedList::quick_sort(int aLeft, int aRight)
{
recurse:
	int 
		Left = aLeft,
		Right = aRight,
		Middle = (Left + Right) / 2;

	do
	{
		while ((Left != Middle) && 
				(compare(get(Left), get(Middle)) < 0))
			Left++;
		while ((Right != Middle) && 
				(compare(get(Right), get(Middle)) > 0))
			Right--;

		if (Left < Right)
		{
			exchange(Left, Right);
			if (Middle == Left)
				Middle = Right;
			else if (Middle == Right)
				Middle = Left;
		}
		if (Left <= Right)
		{
			Left++;
			if (Right > aLeft)
				Right--;
		}
	} while (Left < Right);

	if (Right - aLeft < aRight - Left)
	{
		if (aLeft < Right)
			quick_sort(aLeft, Right);
		if (Left < aRight)
		{
			aLeft = Left;
			goto recurse;
		}
	} else {
		if (Left < aRight)
			quick_sort(Left, aRight);
		if (aLeft < Right)
		{
			aRight = Right;
			goto recurse;
		}
	}
}
