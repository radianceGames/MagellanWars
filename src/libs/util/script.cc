#include "util.h"
#include "debug.h"
#include <cstdlib>
#include <cstdio>

TZone gScriptZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CScript),
	0,
	0,
	NULL,   
	"Zone CScript"
};

#define SCRIPT_ROOT			0
#define SCRIPT_SECTION		1
#define SCRIPT_ARRAY		2
#define SCRIPT_DATA			3

#define NUMBER				400
#define TEXT				401
#define NAME				402

#define BEGIN_ITEM			500
#define END_ITEM			501

#define BEGIN_SECTION		600
#define END_SECTION			601

#define BEGIN_ARRAY			700
#define END_ARRAY			701

#define CONTINUE			1000
#define UNTERMINATE			1100

#define ETC					2000

CScript::CBranch::~CBranch()
{
	remove_all();
}

bool
CScript::CBranch::free_item(TSomething aItem)
{
	if (aItem)
	{
		switch(((TNode*)aItem)->type)
		{
			case SCRIPT_SECTION:
				MEMORY(delete (((TNode*)aItem)->section.list));
				free(((TNode*)aItem)->section.name);
				break;
			case SCRIPT_ARRAY:
				MEMORY(delete (((TNode*)aItem)->array.list));
				free(((TNode*)aItem)->array.name);
				break;
			case SCRIPT_DATA:
				free(((TNode*)aItem)->data.pointer);
				free(((TNode*)aItem)->data.name);
				break;
			default:
				system_log("unexpected node type");
		}
	}

	return true;
}

extern void _ASrestart(FILE *aFILE);
extern char *_AStext;
extern int _ASlex(void);
extern int LineNo;
extern FILE *_ASin;

CScript::CScript()
{
}

CScript::~CScript()
{
}

void
CScript::error(int aLineNo, const char *aMessage)
{
	system_log("SCRIPT:%d:%s", aLineNo, aMessage);
	exit(-1);
}

#define MAX_STACK			16

bool
CScript::load(const char *aFilename)
{
	if (access(aFilename, F_OK + R_OK))	
	{
		system_log("could not access %s", aFilename);
		return false;
	}

	FILE
		*File = fopen(aFilename, "r");
	
	if (!File)
	{
		system_log("could not open %s", aFilename);
		return false;
	}

	mRoot.remove_all();

	_ASin = File;
	_ASrestart(File);

	int 
		Val;

	LineNo = 1;

	bool
		Continue = true;

	int 
		StackPoint = 0;
	char 
		StackType[MAX_STACK] = { 0, };
	CBranch
		*StackBranch[MAX_STACK] = { NULL, };
		
	StackType[StackPoint] = SCRIPT_ROOT;
	StackBranch[StackPoint] = &mRoot;
	
	while((Val = _ASlex()))
	{
//		printf("%4d:%03d|[%s]\n", LineNo, Val, _AStext);

		if (Val == CONTINUE)
		{
			Continue = true;
//			printf("continue\n");
			continue;
		}

		if (!Continue && Val != END_SECTION && Val != END_ARRAY)
		{
			system_log("Script:expected comma in line:%d", LineNo);
			fclose(File);

			return false;
		}

		switch(Val)
		{
			case UNTERMINATE:
				system_log("Script:unterminated string %s in line %d", 
						_AStext, LineNo);
				fclose(File);

				return false;
			case BEGIN_ITEM:
				{
					Continue = false;

					CString
						Name = _AStext;
					Name.set_size(Name.length()-1);

					Val = _ASlex();
//
//					system_log("%4d:%03d|[%s]\n", LineNo, Val, _AStext);
//
//					system_log("add item ");

					switch(Val)
					{
						case TEXT:
							{
								if (strncmp("\"\"", _AStext, 2) == 0)
								{
									system_log("[\"\"] is found in %d", 
															LineNo);
									return false;
								}
								CString
									Value = strip_slashes(_AStext+1);
								Value.set_size(Value.length()-1);

								while((Val = _ASlex()) == TEXT)
								{
									Value += strip_slashes(_AStext+1);
									Value.set_size(Value.length()-1);
								}

								if (Val != END_ITEM)
								{
									system_log("Script:unterminated"
												" string %s in line %d", 
											_AStext, LineNo);
									fclose(File);
									return false;					
								}
								TNode
									*Data = new TNode;
								
								Data->type = SCRIPT_DATA;
								Data->data.name = strdup((char*)Name);
								Data->data.pointer = 
										strdup((char*)Value);
								StackBranch[StackPoint]->push(Data);
//
//								system_log("add text item %s\n", 
//										(char*)Value);
							}
							break;
						case NUMBER:
							{
								CString
									Value = _AStext;

								Val = _ASlex();

								if (Val != END_ITEM)
								{
									system_log("Script:unterminated"
												" string %s", 
											_AStext);
									fclose(File);
									return false;					
								}
								TNode
									*Data = new TNode;
								
								Data->type = SCRIPT_DATA;
								Data->data.name = strdup((char*)Name);
								Data->data.pointer = 
										strdup((char*)Value);
								StackBranch[StackPoint]->push(Data);
//
//								system_log("add number item %s\n",
//										(char*)Value);
							}
							break;
						case END_ITEM:
							{
								TNode
									*Data = new TNode;
								
								Data->type = SCRIPT_DATA;
								Data->data.name = strdup((char*)Name);
								Data->data.pointer = strdup("");

								StackBranch[StackPoint]->push(Data);
							}
							break;
						default:
							system_log("Script:unexpected string %s"
										" in line %d", 
									_AStext, LineNo);
							fclose(File);
							return false;			
					}
//
//					system_log("end item \n");
				}
				break;	
			case NAME:
				{
					Continue = false;

					CString 
						Name = _AStext+1;
					Name.set_size(Name.length()-1);

					Val = _ASlex();

					switch(Val)
					{
						case BEGIN_SECTION:
							{
								TNode
									*Section = new TNode();

								Section->type = SCRIPT_SECTION;
								Section->section.name = 
											strdup((char*)Name);
								Section->section.list = new CBranch();

								StackBranch[StackPoint]->push(Section);
								StackPoint++;
								StackType[StackPoint] = SCRIPT_SECTION;
								StackBranch[StackPoint] = 
										Section->section.list;

								Continue = true;
//
//								system_log("make section '%s'\n", 
//										(char*)Name);
							}
							break;
						case BEGIN_ARRAY:
							{
								TNode
									*Array = new TNode();

								Array->type = SCRIPT_SECTION;
								Array->array.name = 
											strdup((char*)Name);
								Array->array.list = new CBranch();

								StackBranch[StackPoint]->push(Array);
								StackPoint++;
								StackType[StackPoint] = SCRIPT_ARRAY;
								StackBranch[StackPoint] = 
										Array->array.list;

								Continue = true;
//
//								system_log("make array '%s'\n", 
//										(char*)Name);
							}
							break;
						default:
							system_log("Script::unexpected string "
										"'%s' in line:%d",
										_AStext, LineNo);
							fclose(File);

							return false;
					}
				}
				break;			
			case BEGIN_SECTION:
				{
					if (StackType[StackPoint] != SCRIPT_ARRAY)
					{
						system_log("unexpected string '%s' in line %d",
										_AStext, LineNo);
						fclose(File);
						return false;
					}
					TNode
						*Section = new TNode();

					Section->type = SCRIPT_SECTION;
					Section->section.name = 
							strdup("ARRAY");
					Section->section.list = new CBranch();

					StackBranch[StackPoint]->push(Section);
					StackPoint++;
					StackType[StackPoint] = SCRIPT_SECTION;
					StackBranch[StackPoint] = 
							Section->section.list;

					Continue = true;
//
//					system_log("make section in array \n");
				}		
				break;
			case END_SECTION:
				{
					Continue = false;
					StackPoint--;	

					if (StackPoint < 0) 
					{
						system_log("unexpected string '%s' in line %d",
								_AStext, LineNo);
						fclose(File);
						return false;
					}
//				
//					system_log("end section\n");
				}
				break;
			case END_ARRAY:
				{
					char FirstType;

					for(int i=0; i<StackBranch[StackPoint]->length(); 
														i++)
					{
						TNode *
							Node = (TNode *)StackBranch[StackPoint]->get(i);
						if (i == 0)
						{
							FirstType = Node->type;
						} else {
							if (Node->type != FirstType)
							{
								system_log("unmatched item '%s'"
										" in line %d",
										_AStext, LineNo);
								fclose(File);
								return false;
							}
						}
					}
					Continue = false;
					StackPoint--;	

					if (StackPoint < 0) 
					{
						system_log("unexpected string '%s' in line %d",
								_AStext, LineNo);
						fclose(File);
						return false;
					}
//				
//					system_log("end array\n");
				}
				break;
		}
	}

	if (Continue)
	{
		system_log("Script parsing is uncompleted");
		fclose(File);
		return false;
	}

	fclose(File); 

	return true;
}

TSomething 
CScript::get_section(const char *aName, TSomething aParent)
{
	if (aParent == NULL)
	{
		for(int i=0; i<mRoot.length(); i++)
		{
			TNode *
				Node = (TNode *)mRoot.get(i);

			if (!strcmp(Node->data.name, aName))
				return Node;
		}
	} else {
		TNode
			*Parent = (TNode*)aParent;

		if (!Parent) return NULL;
		if (Parent->type != SCRIPT_SECTION && 
						Parent->type != SCRIPT_ARRAY) 
			return NULL;

		for(int i=0; i<Parent->section.list->length(); i++)
		{
			TNode *
				Node = (TNode *)(Parent->section.list)->get(i);

			if (!strcmp(Node->data.name, aName))
				return Node;
		}
	}

	return NULL;
}

TSomething
CScript::get_array(int aIndex, TSomething aArray)
{
	if (aIndex < 0) return NULL;

	if (aArray == NULL)
	{
		if (aIndex >= mRoot.length())
			return NULL;
	
		TNode *
			Node = (TNode *)mRoot.get(aIndex);

		return Node;
	} else {
		TNode
			*Parent = (TNode*)aArray;

		if (!Parent) return NULL;
		if (Parent->type != SCRIPT_SECTION && 
						Parent->type != SCRIPT_ARRAY) 
			return NULL;


		if (aIndex >= Parent->array.list->length()) return NULL;

		TNode *
			Node = (TNode *)(Parent->array.list)->get(aIndex);

		return Node;
	}

	return NULL;
}

int
CScript::get_type(TSomething aData)
{
	TNode
		*Node = (TNode*)aData;

	return Node->type;
}

char*
CScript::get_name(TSomething aData)
{
	TNode
		*Node = (TNode*)aData;

	return Node->data.name;
}

char*
CScript::get_data(TSomething aData)
{
	if (get_type(aData) != SCRIPT_DATA) return NULL;

	TNode
		*Node = (TNode*)aData;

	return (char*)Node->data.pointer;
}

