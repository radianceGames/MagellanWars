#if !defined(__ARCHSPACE_LIB_UTIL_STD_H__)
#define __ARCHSPACE_LIB_UTIL_STD_H__
#include <vector>

/*STD VECTOR*/

/* sorted add function
*
* !requires compare function!
*
*/
#define STD_VECTOR_ADD_FUNCTION(mclass, mvector, margument) \
	int \
		Middle = 0,\
		Left = 0,\
		Right = mvector.size() - 1;\
	\
	char Compare;\
	\
	mclass *current;\
	\
	while(Left <= Right)\
	{\
		Middle = (Left + Right) / 2;\
		current = &mvector.at(Middle);\
		Compare = current->compare(margument);\
		\
		if (Compare == 0)\
		{\
			\
		} else if (Compare < 0) {\
			Left = Middle + 1;\
		} else {\
			Right = Middle - 1;\
		}\
	}\
	if (Right == Middle) Middle++;\
	std::vector<mclass>::iterator aIter = mvector.begin();\
	aIter += Middle;\
	mvector.insert(aIter, *margument);\
	

#define STD_VECTOR_FIND_FUNCTION(mclass, mvector, margument) \
	int \
		Middle = 0,\
		Left = 0,\
		Right = mvector.size() - 1;\
		\
	char Compare;\
		\
	mclass *current;\
	\
	while(Left <= Right)\
	{\
		Middle = (Left + Right) / 2;\
		current = &mvector.at(Middle);\
		Compare = current->compare(margument);\
		\
		if (Compare == 0)\
		{\
			return current;\
		} else if (Compare < 0) {\
			Left = Middle + 1;\
		} else {\
			Right = Middle - 1;\
		}\
	}\
	return NULL;\
	
#define STD_VECTOR_FIND_ITERATOR_FUNCTION(mclass, mvector, margument) \
	int \
		Middle = 0,\
		Left = 0,\
		Right = mvector.size() - 1;\
		\
	char Compare;\
		\
	mclass *current;\
	std::vector<mclass>::iterator aIter;\
	\
	while(Left <= Right)\
	{\
		Middle = (Left + Right) / 2;\
		current = &mvector.at(Middle);\
		Compare = current->compare(margument);\
		\
		if (Compare == 0)\
		{\
			aIter = mvector.begin();\
			aIter += Middle;\
			break;\
		} else if (Compare < 0) {\
			Left = Middle + 1;\
		} else {\
			Right = Middle - 1;\
		}\
	}\
	
#endif 
