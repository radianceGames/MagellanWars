#if defined(__DEBUG__)

#define LOCATION_STACK_SIZE 102400
static struct 
{
	char 
		*filename;
	int
		lineNumber;
} 
	lLocationStack[LOCATION_STACK_SIZE];

static int
	lLocationStackPointer = 0;

void
push_location(char *aFilename, int aLineNumber)
{
	if (lLocationStackPointer >= LOCATION_STACK_SIZE)
	{
		fprintf(stderr, 
			"Warning: memory debugger location stack exceeded!\n");
		return;
	}

	size_t 
		Size = strlen(aFilename) + 1;

	lLocationStack[lLocationStackPointer].filename 
			= (char*)malloc(Size);
	memcpy(lLocationStack[lLocationStackPointer].filename, 
			aFilename, Size);
	lLocationStack[lLocationStackPointer].lineNumber = aLineNumber;
	lLocationStackPointer++;
}

bool 
get_location(char **aFilename, int *aLineNumber)
{
	if (!lLocationStackPointer || 
			!lLocationStack[lLocationStackPointer-1].filename)
	{
		*aFilename = NULL;
		*aLineNumber = 0;
		return false;
	}
	*aFilename = lLocationStack[lLocationStackPointer-1].filename;
	*aLineNumber = lLocationStack[lLocationStackPointer-1].lineNumber;
	lLocationStack[lLocationStackPointer-1].filename = NULL;
	return true;
}

void pop_location()
{
	if (!lLocationStackPointer)
	{
		fprintf(stderr,
			"Warning: pop_location() on empty location stack!\n");
		return;
	}
	lLocationStackPointer--;
}

static void*
debug_new(size_t aSize)
{
	char 
		*Buffer = (char*)malloc(aSize);
	char
		*Filename;
	int
		LineNumber;

	if (!get_location(&Filename, &LineNumber))
		fprintf(stderr, "Warning! Memory was allocated without MEMORY(%lu bytes)!\n", aSize);

	return (void*)Buffer;
}

static void 
debug_delete(void *aData)
{
	char
		*Filename;
	int
		LineNumber;

	if (!get_location(&Filename, &LineNumber))
		fprintf(stderr, "Warning! Memory was allocated without MEMORY(%lu bytes)!\n", aSize);

	if (!aData) return;

	free(aData);
}

void *operator new (size_t aSize)
{
	return debug_new(aSize);
}

void *operator new[] (size_t aSize)
{
	return debug_new(aSize);
}

void operator delete (void* aData)
{
	debug_delete(aData);
}

void operator delete[] (void* aData)
{
	debug_delete(aData);
}

#endif
