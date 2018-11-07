#include "signal_handler.h"
#include "lib/interface_header/platform.h"

#if ZQ_PLATFORM == ZQ_PLATFORM_UNIX
#include <signal.h>


namespace zq {

SignalFunT SignalHandler::funUsr1_ = nullptr;
SignalFunT SignalHandler::funUsr2_ = nullptr;
SignalFunT SignalHandler::funQuit_ = nullptr;
int SignalHandler::signalArgUsr1_ = 0;
int SignalHandler::signalArgUsr2_ = 0;
int SignalHandler::signalArgQuit_ = 0;

void SignalHandler::ignoreSignal(int iSignalValue)
{
	struct sigaction sig;
	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(iSignalValue, &sig, NULL);
}

void SignalHandler::ignoreSignalSet()
{
	ignoreSignal(SIGINT);
	ignoreSignal(SIGHUP);
	ignoreSignal(SIGQUIT);
	ignoreSignal(SIGTTOU);
	ignoreSignal(SIGTTIN);
	ignoreSignal(SIGCHLD);
	ignoreSignal(SIGTERM);
	ignoreSignal(SIGHUP);
	ignoreSignal(SIGPIPE);
}

void SignalHandler::setSignalHandler(int iSignalValue, SignalFunT pfHandler)
{
	signal(iSignalValue, pfHandler);
}

void SignalHandler::setHandlerUsr1(SignalFunT pfUSR1, int iSignalArg)
{
	SignalHandler::funUsr1_ = pfUSR1;
	SignalHandler::signalArgUsr1_ = iSignalArg;
	registerSignalUsr1();
}

void SignalHandler::setHandlerUsr2(SignalFunT pfUSR2, int iSignalArg)
{
	SignalHandler::funUsr2_ = pfUSR2;
	SignalHandler::signalArgUsr2_ = iSignalArg;
	registerSignalUsr2();
}

void SignalHandler::setHandlerQuit(SignalFunT pfQuit, int iSignalArgQuit)
{
	SignalHandler::funQuit_ = pfQuit;
	SignalHandler::signalArgQuit_ = iSignalArgQuit;
	registerSignalQuit();
}

void SignalHandler::registerSignalUsr1()
{
	signal(SIGUSR1, notifySignalUsr1);
}

void SignalHandler::registerSignalUsr2()
{
	signal(SIGUSR2, notifySignalUsr2);
}

void SignalHandler::registerSignalQuit()
{
	signal(SIGQUIT, notifySignalQuit);
}

void SignalHandler::notifySignalUsr1(int iSignalValue)
{
	(*SignalHandler::funUsr1_)(SignalHandler::signalArgUsr1_);
	registerSignalUsr1();
}

void SignalHandler::notifySignalUsr2(int iSignalValue)
{
	(*SignalHandler::funUsr2_)(SignalHandler::signalArgUsr2_);
	registerSignalUsr2();
}

void SignalHandler::notifySignalQuit(int iSignalValue)
{
	(*SignalHandler::funQuit_)(SignalHandler::signalArgQuit_);
	registerSignalQuit();
}


}

#endif