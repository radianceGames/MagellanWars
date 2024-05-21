#if !defined(__ARCHSPACE_LIB_COMMON_H__)
#define __ARCHSPACE_LIB_COMMON_H__

#include <pth.h>

typedef struct tagZone TZone;
struct tagZone
{
	pth_mutex_t mutex;

	void *(*malloc)(struct tagZone *aZone, size_t aSize);
	void (*free)(void *aObject);

	size_t object_size;

	size_t object_total;
	size_t object_for_recycle;

	void *recycle;

	char name[80];
};

#ifdef MEMORY_DEBUG
typedef struct tagEnd TEnd;
struct tagEnd
{
	int magic;
	int heaven;
	TZone *zone;
};
#endif

typedef struct tagChunk TChunk;
struct tagChunk
{
#ifdef MEMORY_DEBUG
	int magic;
#endif
	TChunk *next;
	TZone *zone;
#ifdef MEMORY_DEBUG
	char info[40];
#endif
};

#ifdef MEMORY_DEBUG
typedef struct tagHeaven THeaven;
struct tagHeaven
{
	TChunk *chunk;
	TZone *zone;
	TEnd *end;
};
#endif

extern TZone gGeneralZone;

#define RECYCLE(Zone) \
	public: \
		void *operator new(size_t aSize) \
		{ \
			return (Zone.malloc)(&Zone, aSize); \
		} \
		\
		void operator delete(void *aItem) \
		{ \
			(Zone.free)(aItem); \
		}

/**
*/
class CBase
{
	public:
		CBase() {}
		virtual ~CBase() {}
};

#endif
