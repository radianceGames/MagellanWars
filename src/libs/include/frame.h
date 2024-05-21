#if !defined(__ARCHSPACE_LIB_FRAME_H__)
#define __ARCHSPACE_LIB_FRAME_H__

#include "common.h"
#include "util.h"

/**
*/
class CApplication: public CBase
{
	public:
		CApplication();
		virtual ~CApplication();

		bool initialize(int Argc, char **Argv);
		int run();
		CIniFile& configuration() { return mConfiguration; }
	protected:
		CIniFile 
			mConfiguration;
		sigset_t
			mSignalSet;

		static void signal_ignore(int aSignalNumber);
		static void signal_exits(int aSignalNumber);

		virtual void set_signalset() = 0;
		virtual void set_signalfunction() = 0;

		inline void signal_masking();
		inline void signal_unmasking();
	private:
		sigset_t
			mTempSignalSet;
};

inline void
CApplication::signal_masking()
{
//	sigprocmask(SIG_SETMASK, &mSignalSet, &mTempSignalSet);
	pth_sigmask(SIG_SETMASK, &mSignalSet, &mTempSignalSet);
}

inline void
CApplication::signal_unmasking()
{
//	sigprocmask(SIG_SETMASK, &mTempSignalSet, NULL);
	pth_sigmask(SIG_SETMASK, &mTempSignalSet, NULL);
}

extern CApplication* gApplication;

#endif
