#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"

class CTest;

TZone gTestZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	4,
	0,
	0,
	NULL,
	"Zone Test Zone",
};

class CTest
{
	public:
		CTest() {}
		~CTest() {}

	RECYCLE(gTestZone);
};

class CTestList:public CList
{
	public:
		CTestList(int aLimit, int aThreshold):CList(aLimit, aThreshold) {}
		~CTestList() { remove_all(); }

	protected:
		virtual bool free_item(TSomething aItem) 
		{ 
			CTest *Test = (CTest*)aItem; 
			delete Test; 
			return true; 
		}
};

/*
static void *tester(char *arg)
{
	while(1)
	{
		printf("tester begin");
		CTestList *List = new CTestList(10, 10);	
		for(int i=0; i<number(1000); i++)
		{
			List->push(new CTest());	
		}
		printf("tester end");
		delete List;
		usleep(500);
	}
}
*/

int main()
{
//	size_t testsize[] = { 32*1024+1, 32*1024, 32*1024-1, 16*1024, 4096, 2048, 1025, 1024, 1023, 512, 256, 128, 32, 16, 8 };
	void* test[3];

	pth_init();
/*
	pth_attr_t attr;
	attr = pth_attr_new();
	pth_attr_set(attr, PTH_ATTR_NAME, "ticker");
	pth_attr_set(attr, PTH_ATTR_STACK_SIZE, 64*1024);
	pth_attr_set(attr, PTH_ATTR_JOINABLE, FALSE);
	pth_spawn(attr, tester, NULL);

	while(1)
	{
		printf("main begin");
		CTestList *List = new CTestList(10, 10);	
		for(int i=0; i<number(1000); i++)
		{
			List->push(new CTest());	
		}
		delete List;
		printf("main end");
		usleep(500);
	}*/
/*
	for(unsigned int i=0; i<sizeof(testsize)/sizeof(size_t); i++)
	{
		test[0] = malloc(testsize[i]);
		test[1] = malloc(testsize[i]);
		test[2] = malloc(testsize[i]);

		printf("%d block, first diff %d, second diff %d\n", testsize[i], 
				(int)test[1]-(int)test[0], (int)test[2]-(int)test[1]);
		free(test[0]);
		free(test[1]);
		free(test[2]);
	}
*/
	for(unsigned int i=1; i<512*1024+1; i++)
	{
		test[0] = malloc(i);
		test[1] = malloc(i);
		test[2] = malloc(i);

		printf("%d block, first diff %d, second diff %d\n", i, 
				(int)test[1]-(int)test[0], (int)test[2]-(int)test[1]);

		free(test[0]);
		free(test[1]);
		free(test[2]);
	}


	pth_kill();

	return 0;
}
