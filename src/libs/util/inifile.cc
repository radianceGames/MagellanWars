#include "util.h"
#include <cstdlib>
#include <cstdio>

TZone gIniFileZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CIniFile),
	0,
	0,
	NULL,   
	"Zone CIniFile"
};

#define INIFILE_SPACE " \t"
#define INIFILE_MAX_LINE_LENGTH 		1024

#define INITYPE_SECTION					1
#define INITYPE_DATA					2
#define INITYPE_COMMENT					3

CIniFile::CBranch::~CBranch()
{
	remove_all();
}

bool
CIniFile::CBranch::free_item(TSomething aItem)
{
	// 아이템의 종류를 구분해서 종류마다 메모리 반환 방법에 맞
	// 추어 반환한다.
	if (aItem)
	{
		switch(((TNode*)aItem)->type)
		{
			case INITYPE_SECTION:
				delete (((TNode*)aItem)->section.list);
				::free(((TNode*)aItem)->section.name);
				break;
			case INITYPE_DATA:
				::free(((TNode*)aItem)->data.pointer);
				::free(((TNode*)aItem)->data.name);
				break;
			case INITYPE_COMMENT:
				::free(((TNode*)aItem)->comment.text);
				break;
		}
		// 아이템에 종속되는 다른 아이템도 모두 삭제한다.
		if (((TNode*)aItem)->comments)
			delete ((TNode*)aItem)->comments;
		delete (TNode*)aItem;
	}
	return true;
}

CIniFile::CIniFile(char aCommentChar)
{
	mCommentChar = aCommentChar;
	mDirty = true;
}

CIniFile::CIniFile(const char *aFilename, char aCommentChar)
{
	mCommentChar = aCommentChar;
	mDirty = true;
	load(aFilename);
}

CIniFile::~CIniFile()
{
}

// 파일에서 데이터 라인 한줄을 읽어들인다.
static bool 
read_file_line(TSomething aStream, void *aData, 
		size_t aSize)
{
	if (!fgets((char*)aData, aSize, (FILE*)aStream))
		return false;

	size_t 
		StringLength = strlen((char *)aData);

	if (StringLength >= aSize)
		StringLength = aSize - 1;
	
	while(StringLength && (((char *)aData)[StringLength - 1] < ' '))
		StringLength--;

	((char*)aData)[StringLength] = 0;
	return true;
}

bool 
CIniFile::load(const char *aFilename)
{
	// 파일을 열어 파일 포인터를 넘겨 데이터를 불러들인다.
	FILE *File = fopen(aFilename, "r");
	if (!File)
		return false;

	bool 
		Result = load(read_file_line, File);
	
	fclose(File);
	return Result;
}

// 파일을 제외한 메모리 및 네트워크등의 여타 다른 매체를 통해서 
// 설정 데이터를 읽어들일 수 있도록 데이터를 읽어들이는 루틴을
// 펑션화 해서 관리
bool 
CIniFile::load(bool (*aReadLine)(TSomething aStream, void *aData, 
			size_t aSize), 
		TSomething aStream)
{
	char 
		Buffer[INIFILE_MAX_LINE_LENGTH],
		Temp[INIFILE_MAX_LINE_LENGTH];
	int 
		LineNo = 0;

	TNode 
		*Branch;
	CBranch
		*CurrentBranch = NULL,
		*Comments = NULL;

	bool
		SkipComment = false,
		PSkipComment = false;


	mDirty = false;

	while(1)
	{
		char 
			*Current = Buffer;

		// 데이터 문자열 한줄을 읽어들임
		if (!aReadLine(aStream, Current, INIFILE_MAX_LINE_LENGTH))
			break;

		LineNo++;

		// 앞쪽의 공백을 모두 제거함
		Current += strspn(Current, INIFILE_SPACE);
		
		PSkipComment = SkipComment;
		SkipComment = false;

		// 내용이 없거나 주석만 있을때
		if ((*Current == 0) || (*Current == mCommentChar))
		{
			if (PSkipComment && CurrentBranch)
			{
				Branch = (TNode*)(CurrentBranch->get(CurrentBranch->length() - 1));
				if ((Branch->type == INITYPE_DATA) && strstr(Current, Branch->data.name))
					continue;
			}
			Branch = new TNode;
			Branch->type = INITYPE_COMMENT;
			Branch->comments = NULL;
			if (*Current)
				Branch->comment.text = strdup(Current + 1);
			else
				Branch->comment.text = NULL;

			if (!Comments)
				Comments = new CBranch;
			Comments->push(Branch);
		} else if (*Current == '[') {
			char 
				*Cut;
			int 
				i;

			Current++;
			Current += strspn(Current, INIFILE_SPACE);
			Cut = strchr(Current, ']');
			if (!Cut)
				goto error;

			strncpy(Temp, Current, i = int(Cut-Current));
			while(i && strchr(INIFILE_SPACE, Temp[i-1]))
				i--;
			Temp[i] = 0;

			Branch = new TNode;
			Branch->type = INITYPE_SECTION;
			Branch->section.name = strdup(Temp);
			Branch->section.list = CurrentBranch = new CBranch();
			mRoot.push(Branch);
			Comments = NULL;
		} else {
			char 
				*Equal;
			int 
				i;

			Equal = strchr(Current, '=');
			if (!Equal || !CurrentBranch)
			{
error:			if (error(LineNo, Buffer, int (Current - Buffer)))
					goto out;
				else 
					continue;
			}
			strncpy(Temp, Current, i=int(Equal - Current));

			while(i && strchr(INIFILE_SPACE, Temp[i-1]))
				i--;
			Temp[i] = 0;

			Branch = new TNode;
			Branch->type = INITYPE_DATA;
			Branch->comments = Comments;
			Branch->data.name = strdup(Temp);

			Current = Equal + 1;
			Current += strspn(Current, INIFILE_SPACE);

			strcpy(Temp, Current);
			i = strlen(Current);

			if (i)
			{
				while(i && strchr(INIFILE_SPACE, Temp[i-1]))
					i--;
				Temp[i] = 0;
				Branch->data.pointer = 
						::malloc((Branch->data.size = i) + 1);
				strcpy((char *)Branch->data.pointer, Temp);
			} else {
				Branch->data.size = 0;
				Branch->data.pointer = NULL;
				SkipComment = true;
			}
			CurrentBranch->push(Branch);
			Comments = NULL;
		}
	}
out:
	if (Comments)
	{
		while(Comments->length())
			mRoot.push(Comments->pop());
		delete Comments;
		Comments = NULL;
	}

	return true;
}


typedef struct
{
	CIniFile 
		*inifile;
	FILE	
		*file;
	char
		*section;
} EnumStruct, *PEnumStruct;

bool
CIniFile::save_enum_comments(TSomething aStruct, char *aText)
{
	if (aText)
	{
		fwrite(&PEnumStruct(aStruct)->inifile->mCommentChar, 1, 1, 
				PEnumStruct(aStruct)->file);
		fputs(aText, PEnumStruct(aStruct)->file);
	}
	fputs("\n", PEnumStruct(aStruct)->file);

	return false;
}

bool
CIniFile::save_enum_data(TSomething aStruct, char *aName, 
		size_t aDataSize, TSomething aData)
{
	PEnumStruct(aStruct)->inifile->enum_comments(
			PEnumStruct(aStruct)->section, aName, 
			save_enum_comments, aStruct);
	
	fputs(aName, PEnumStruct(aStruct)->file);
	fputs(" = ", PEnumStruct(aStruct)->file);

	if (aData && aDataSize)
	{
		fwrite(aData, 1, aDataSize, PEnumStruct(aStruct)->file);
	}
	fputs("\n", PEnumStruct(aStruct)->file);

	return false;
}

bool
CIniFile::save_enum_section(TSomething aStruct, char *aName)
{
	PEnumStruct(aStruct)->section = aName;
	PEnumStruct(aStruct)->inifile->enum_comments(
			aName, NULL, save_enum_comments, aStruct);
	
	fputs("[", PEnumStruct(aStruct)->file);
	fputs(aName, PEnumStruct(aStruct)->file);
	fputs("]", PEnumStruct(aStruct)->file);

	PEnumStruct(aStruct)->inifile->enum_data(
			aName, save_enum_data, aStruct);

	return false;
}

bool
CIniFile::save(const char *aFilename)
{
	if (mDirty)
	{
		EnumStruct 
			Struct = { this };
		Struct.file = fopen(aFilename, "w");
		if (!Struct.file) 
			return false;
		Struct.inifile->enum_sections(save_enum_section, &Struct);
		fclose(Struct.file);
		mDirty = false;
	}
	return true;
}

bool
CIniFile::error(int aLineNo, const char *aLine, int aPosition)
{
	(void)aLineNo;
	(void)aLine;
	(void)aPosition;
	return false;
}

bool 
CIniFile::enum_sections(
		bool (*aIterator)(TSomething aParameter, char *aName),
		TSomething aParameter)
{
	int 
		i,
		j;

	CBranch 
		*Section = find_node(NULL);

	if (Section) 
		return false;

	j = Section->length();

	for(i=0; i<j; i++)
	{
		TNode 
			*Node = (TNode*)Section->get(i);
		if (Node->type == INITYPE_SECTION)
			if (aIterator(aParameter, Node->section.name))
				break;
	}
	return true;
}
		
bool 
CIniFile::enum_sections(CStringList *aList)
{
	int 
		i,
		j;

	CBranch 
		*Section = find_node(NULL);

	if (Section) 
		return false;

	j = Section->length();

	for(i=0; i<j; i++)
	{
		TNode 
			*Node = (TNode*)Section->get(i);
		if (Node->type == INITYPE_SECTION)
			aList->push(string_new(Node->section.name));
	}
	return true;
}

bool
CIniFile::enum_data(const char *aSectionPath, bool (*aIterator)
			(TSomething aParameter, char *aName, size_t aDataSize,
			TSomething aData), 
		TSomething aParameter)
{
	int 
		i,
		j;

	CBranch
		*Section = find_node(aSectionPath);

	if (!Section) 
		return false;

	j = Section->length();

	for(i=0; i<j; i++)
	{
		TNode *
			Node = (TNode *)Section->get(i);

		if (Node->type == INITYPE_DATA)
			if (aIterator(aParameter, Node->data.name, 
					Node->data.size, Node->data.pointer))
				break;
	}
	return true;
}

bool
CIniFile::enum_data(const char *SectionPath, CStringList *aList)
{
	int 
		i,
		j;

	CBranch
		*Section = find_node(SectionPath);
	
	if (!Section)
		return false;

	j = Section->length();

	for(i=0; i<j; i++)
	{
		TNode *
			Node = (TNode *)Section->get(i);

		if (Node->type == INITYPE_DATA)
			aList->push(string_new(Node->data.name));
	}
	return true;
}

bool
CIniFile::enum_comments(const char *aSectionPath, const char *aKeyName,
		bool (*aIterator)(TSomething aParameter, char *aText), 
		TSomething aParameter)
{
	CBranch 
		*Section,
		*Comments = NULL;
	
	if (aKeyName)
	{
		Section = find_node(aSectionPath);
		if (!Section) return false;

		for (int i=0 ; i<Section->length() ; i++)
		{
			TNode *
				Node = (TNode *)Section->get(i);

			if ((Node->type == INITYPE_DATA) &&
				(strcmp(aKeyName, Node->data.name) == 0))
			{
				Comments = Node->comments;
				break;
			}
		}
	}
	else
	{
		for (int i=0 ; i<mRoot.length() ; i++)
		{
			TNode *
				Node = (TNode *)mRoot.get(i);

			if ((Node->type == INITYPE_SECTION) &&
				(strcmp(aSectionPath, Node->section.name) == 0))
			{
				Comments = Node->comments;
				break;
			}
		}
	}

	if (Comments == NULL) return false;

	for(int i=0 ; i<Comments->length() ; i++)
	{
		TNode 
			*Node = (TNode*)Comments->get(i);

		if (Node->type == INITYPE_COMMENT)
			if (aIterator(aParameter, Node->comment.text))
				break;
	}

	return true;
}

bool 
CIniFile::enum_comments(const char *aSectionPath, 
		const char *aKeyName, CStringList *aList)
{
	CBranch 
		*Section,
		*Comments = NULL;

	if (aKeyName)
	{
		Section = find_node(aSectionPath);
		if (!Section) return false;

		for (int i=0 ; i<Section->length() ; i++)
		{
			TNode *
				Node = (TNode *)Section->get(i);

			if ((Node->type == INITYPE_DATA) &&
					(strcmp(aKeyName, Node->data.name) == 0))
			{
				Comments = Node->comments;
				break;
			}
		}
	}
	else
	{
		for (int i=0 ; i<mRoot.length() ; i++)
		{
			TNode *
				Node = (TNode *)mRoot.get(i);

			if ((Node->type == INITYPE_SECTION) &&
					(strcmp(aSectionPath, Node->section.name) == 0))
			{
				Comments = Node->comments;
				break;
			}
		}
	}

	if (!Comments)
		return false;
	
	for (int i=0 ; i<Comments->length() ; i++)
	{
		TNode *
			Node = (TNode *)Comments->get(i);

		if (Node->type == INITYPE_COMMENT)
			aList->push(string_new(Node->comment.text));
	}
	return true;
}


CIniFile::CBranch *
CIniFile::find_node(const char *aSectionPath)
{
	if (!aSectionPath)
		return (const_cast<CBranch*>(&mRoot));
	
	for(int i=0, Length = mRoot.length(); i<Length; i++)
	{
		TNode *
			Node = (TNode *)mRoot.get(i);

		if ((Node->type == INITYPE_SECTION) && 
				(!strcmp(aSectionPath, Node->section.name)))
			return Node->section.list;
	}
	return NULL;
}

bool
CIniFile::get_data(const char *aSectionPath, const char *aKeyName,
		TSomething &aData, size_t &aDataSize)
{
	CBranch 
		*Section = find_node(aSectionPath);
	
	int 
		i,
		Length;

	aData = NULL;
	aDataSize = 0;

	if (!Section)
		return false;

	for (i=0, Length = Section->length(); i<Length; i++)
	{
		TNode *
			Node = (TNode *)Section->get(i);

		if ((Node->type == INITYPE_DATA) &&
				(strcmp(aKeyName, Node->data.name) == 0))
		{
			aData = Node->data.pointer;
			aDataSize = Node->data.size;
			return true;
		}
	}

	return false;
}

bool
CIniFile::set_data(const char *aSectionPath, const char *aKeyName, 
		TConstSomething aData, size_t aDataSize)
{
	CBranch
		*Section = find_node(aSectionPath);

	TNode
		*Branch;

	int 
		i,
		Length;

	mDirty = true;

	if (!Section)
	{
		Branch = new TNode;
		Branch->type = INITYPE_SECTION;
		Branch->comments = NULL;
		Branch->section.name = strdup(aSectionPath);
		Branch->section.list = Section = new CBranch();
		mRoot.push(Branch);

		if (mRoot.length() > 0)
			set_comment(aSectionPath, NULL, NULL);
	}

	for (i=0, Length = Section->length(); i<Length; i++)
	{
		Branch = (TNode *)Section->get(i);

		if ((Branch->type == INITYPE_DATA) && 
				(strcmp(aKeyName, Branch->data.name) == 0))
		{
			if (aData && aDataSize)
			{
				::free(Branch->data.pointer);
				goto set_value;
			} else 
				Section->remove(i);
			return true;
		}
	}

	if (!aData || !aDataSize)
		return true;

	Branch = new TNode;
	Section->push(Branch);
	Branch->type = INITYPE_DATA;
	Branch->data.name = strdup(aKeyName);
set_value:
	Branch->data.pointer = ::malloc(aDataSize+1);
	Branch->data.size = aDataSize;
	memcpy(Branch->data.pointer, aData, aDataSize);
	((char*)Branch->data.pointer)[aDataSize] = 0;
	return true;
}

bool
CIniFile::set_string(const char *aSectionPath, const char *aKeyName,
		const char* aValue)
{
	return (set_data(aSectionPath, aKeyName, aValue, 
								aValue ? strlen(aValue): 0));
}

bool
CIniFile::set_integer(const char *aSectionPath, const char *aKeyName,
		int aValue)
{
	char 
		Output[20];
	sprintf(Output, "%d", aValue);

	return set_string(aSectionPath, aKeyName, Output);
}

bool
CIniFile::set_float(const char *aSectionPath, const char *aKeyName, 
		float aValue)
{
	char 
		Output[20];
	
	sprintf(Output, "%f", aValue);

	return set_string(aSectionPath, aKeyName, Output);
}

bool 
CIniFile::set_comment(const char *aSectionPath, const char *aKeyName,
		const char *aText)
{
	CBranch 
		*Section,
		**Comments = NULL;

	TNode
		*Branch;

	mDirty = true;

	if (aKeyName)
	{
		Section = find_node(aSectionPath);
		if (!Section)
			return false;

		for(int i=0, Length = Section->length(); i<Length; i++)
		{
			TNode *
				Node = (TNode *)Section->get(i);

			if ((Node->type == INITYPE_DATA) && 
					(strcmp(aKeyName, Node->data.name) == 0))
			{
				Comments = &Node->comments;
				break;
			}
		}
	}
	else
	{
		for (int i=0 ; i<mRoot.length() ; i++)
		{
			TNode *
				Node = (TNode *)mRoot.get(i);

			if ((Node->type == INITYPE_SECTION) 
					&& (strcmp(aSectionPath, Node->section.name) == 0))
			{
				Comments = &Node->comments;
				break;
			}
		}
	}

	if (!Comments)
		return false;

	if (!*Comments)
		*Comments = new CBranch();
	
	Branch = new TNode;
	Branch->type = INITYPE_COMMENT;
	Branch->comments = NULL;
	if (aText)
		Branch->comment.text = strdup(aText);
	else
		Branch->comment.text = NULL;
	(*Comments)->push(Branch);

	return true;
}

bool 
CIniFile::remove_comment(const char *aSectionPath, const char *aKeyName)
{
	int
		i,
		Length;

	CBranch 
		*Section,
		**Comments = NULL;

	mDirty = true;

	if (aKeyName)
	{
		Section = find_node(aSectionPath);
		if (!Section)
			return false;

		for(i=0, Length = Section->length(); i<Length; i++)
		{
			TNode *
				Node = (TNode *)Section->get(i);

			if ((Node->type == INITYPE_DATA) && 
					(strcmp(aKeyName, Node->data.name) == 0))
			{
				Comments = &Node->comments;
				break;
			}
		}
	} else {
		Length = mRoot.length();

		for (i=0; i<Length; i++)
		{
			TNode *
				Node = (TNode *)mRoot.get(i);

			if ((Node->type == INITYPE_SECTION) 
					&& (strcmp(aSectionPath, Node->section.name) == 0))
			{
				Comments = &Node->comments;
				break;
			}
		}
	}

	if (!Comments)
		return false;

	if (!*Comments)
		return true;

	delete *Comments;
	*Comments = NULL;

	return true;
}


bool
CIniFile::remove(const char *aSectionPath, const char *aKeyName)
{
	return set_data(aSectionPath, aKeyName, NULL, 0);
}

bool
CIniFile::section_exists(const char *aSectionPath)
{
	CBranch 
		*Section = find_node(aSectionPath);
	
	return (Section != NULL);
}
	
bool 
CIniFile::key_exists(const char *aSectionPath, 
		const char *aKeyName)
{
	CBranch 
		*Section = find_node(aSectionPath);

	if (Section)
	{
		for(int i=0, Length = Section->length(); i<Length; i++)
		{	
			TNode *
				Node = (TNode *)Section->get(i);
			if ((Node->type == INITYPE_DATA) 
					&& (strcmp(aKeyName, Node->data.name) == 0))
				return true;
		}
	}
	return false;
}

char *
CIniFile::get_string(const char *aSectionPath, const char *aKeyName, char *aDefault)
{
	TSomething
		Something;
	size_t
		Size;

	get_data(aSectionPath, aKeyName, Something, Size);

	if (Something)
		return (char*)Something;
	else 
		return aDefault;
}

float 
CIniFile::get_float(const char *aSectionPath, const char *aKeyName, float aDefault)
{
	char 
		*String = get_string(aSectionPath, aKeyName, NULL);

	if (!String)
		return aDefault;
	
	float 
		Float;

	sscanf(String, "%f", &Float);

	return Float;
}

int
CIniFile::get_integer(const char *aSectionPath, const char *aKeyName, int aDefault)
{
	char 
		*String = get_string(aSectionPath, aKeyName, NULL);

	if (!String)
		return aDefault;
	
	int 
		Integer;

	sscanf(String, "%d", &Integer);

	return Integer;
}

bool
CIniFile::get_boolean(const char *aSectionPath, const char *aKeyName, bool aDefault)
{
	char 
		*String = get_string(aSectionPath, aKeyName, NULL);

	if (!String)
		return aDefault;
	
	return (!strcasecmp(String, "yes")) || 
			(!strcasecmp(String, "true")) ||
			(!strcasecmp(String, "on"));
}
