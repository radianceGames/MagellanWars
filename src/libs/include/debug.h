#if !defined(__ARCHSPACE_LIB_DEBUG_H__)
#define __ARCHSPACE_LIB_DEBUG_H__
#if defined(___DEBUG___)

#define MEMORY(x) \
	push_location(__FILE__, __LINE__); \
	x; \
	pop_location()

#define BOUNDARY(x) { MEMORY(x); }

/*extern void push_location(char *aFilename, int aLineNumber);
extern void pop_location();*/

#else

#define MEMORY(x) x
#define BOUNDARY(x) { MEMORY(x); }

#endif
#endif
