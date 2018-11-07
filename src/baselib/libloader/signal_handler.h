#pragma once

namespace zq {


using SignalFunT = void(*)(int);

class SignalHandler
{
private:
	SignalHandler() {}
	~SignalHandler() {}

public:
	// ºöÂÔÄ³¸öÐÅºÅ
	static void ignoreSignal(int iSignalValue);

	static void ignoreSignalSet();

	// ÉèÖÃÄ³¸öÐÅºÅµÄ²¶×½º¯Êý
	static void setSignalHandler(int iSignalValue, SignalFunT pfHandler);

	static void setHandlerUsr1(SignalFunT pfUSR1, int iSignalArg);
	static void setHandlerUsr2(SignalFunT pfUSR2, int iSignalArg);
	static void setHandlerQuit(SignalFunT pfQuit, int iSignalArgQuit);

private:

	//×¢²á
	static void registerSignalUsr1();

	//×¢²á
	static void registerSignalUsr2();

	//×¢²á
	static void registerSignalQuit();

	static void notifySignalUsr1(int iSignalValue);

	static void notifySignalUsr2(int iSignalValue);

	static void notifySignalQuit(int iSignalValue);

private:

	static SignalFunT funUsr1_;
	static SignalFunT funUsr2_;
	static SignalFunT funQuit_;

	static int signalArgUsr1_;
	static int signalArgUsr2_;
	static int signalArgQuit_;
};
}
