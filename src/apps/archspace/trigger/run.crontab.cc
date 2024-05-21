#include "../triggers.h"
#include "../archspace.h"
#include "../game.h"
#include <cstdio>
#include <cstdlib>

void
CCronTabDummy::handler()
{
	SLOG( "DUMMY RUN FOR CRONTAB DUMMY" );
}

CCronTab::CCronTab()
{
	mLastActivatedTime = 0;
}

CCronTab::~CCronTab()
{
}

void
CCronTab::run()
{
	CString
		FileName;
	FileName.format( "%s/%s", ARCHSPACE->configuration().get_string( "Game", "CronTabDir" ), name() );

	handler();
	mLastActivatedTime = time(0);

	FILE
		*fp = fopen( (char*)FileName, "w+" );
	if( fp == NULL ){
		SLOG( "error happened while trying to log last activated time of crontab : %s", (char*)FileName );
		return;
	}
	fprintf( fp, "%d\n", (int)mLastActivatedTime );
	fclose(fp);
}

void
CCronTab::load_last_activated_time()
{
	CString
		FileName;
	FileName.format( "%s/%s", ARCHSPACE->configuration().get_string( "Game", "CronTabDir" ), name() );

	FILE
		*fp = fopen( (char*)FileName, "r" );
	if( fp == NULL ) return;

	char
		buf[100];
	fgets( buf, 99, fp );
	mLastActivatedTime = atoi(buf);
	SLOG( "last activated time loading : %s %s %d", name(), buf, CGameStatus::get_down_time() );
	fclose( fp );
}

bool
CTriggerRunCronTab::handler()
{
	system_log("run crontab jobs");

	for (int i=0 ; i<length() ; i++)
	{
		CCronTab *
			Job = (CCronTab *)get(i);

		#define Week 60 * 60 * 24 * 7
		#define Day 60 * 60 * 24
		#define Hour 60 * 60
		#define Minute 60

		static int
			Interval;
		Interval = Job->get_run_week() * Week + Job->get_run_day() * Day + Job->get_run_hour() * Hour
			+ Job->get_run_minute() * Minute + Job->get_run_second();

		if (Interval > 0)
		{
			if (Interval <= (time(0) - Job->get_last_activated_time())) Job->run();
		}
	}
	system_log("end crontab jobs");
	return true;
}

bool
CTriggerRunCronTab::free_item(TSomething aItem)
{
	CCronTab
		*Cron = (CCronTab*)aItem;

	if( Cron == NULL ) return false;

	delete Cron;

	return true;
}
