#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "lib/config.h"
#include "lib/io.h"
#include "lib/Signal.h"

struct sigaction CSignal::oldsigaction;
bool CSignal::active=false;
bool CSignal::cancel_computation=false;

CSignal::CSignal()
{
}

CSignal::~CSignal()
{
	if (!unset_handler())
		CIO::message(M_ERROR, "error uninitalizing signal handler\n");
}

void CSignal::handler(int signal)
{
#ifdef HAVE_MATLAB
	CIO::message(M_MESSAGEONLY, "\nForce quit (y/n/c)? ");
	char answer=fgetc(stdin);

	if (answer == 'y')
	{
		unset_handler();
		CIO::message(M_ERROR, "gf stopped by SIGINT\n");
	}
	else if (answer == 'c')
		cancel_computation=true;
#else
	CIO::message(M_MESSAGEONLY, "\n");
	CIO::message(M_ERROR, "gf stopped by SIGINT\n");
	unset_handler();
	exit(0);
#endif
}

bool CSignal::set_handler()
{
	if (!active)
	{
		struct sigaction act;
		sigset_t st;

		sigemptyset(&st);

		act.sa_restorer = NULL; //just in case remove
		act.sa_sigaction=NULL; //just in case
		act.sa_handler=CSignal::handler;
		act.sa_mask = st;
		act.sa_flags = 0;

		if (!sigaction(SIGINT, &act, &oldsigaction))
		{
			active=true;
			return true;
		}
		else
		{
			clear();
			return false;
		}
	}
	else
		return false;
}

bool CSignal::unset_handler()
{
	if (active)
	{
		if (!sigaction(SIGINT, &oldsigaction, NULL))
		{
			clear();
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void CSignal::clear()
{
	cancel_computation=false;
	active=false;
	memset(&CSignal::oldsigaction, 0, sizeof(CSignal::oldsigaction));
}
