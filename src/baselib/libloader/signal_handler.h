#pragma once

namespace zq {


using SignalFunT = void(*)(int);

class SignalHandler
{
private:
	SignalHandler() {}
	~SignalHandler() {}

public:
	// ����ĳ���ź�
	static void ignoreSignal(int iSignalValue);

	static void ignoreSignalSet();

	// ����ĳ���źŵĲ�׽����
	static void setSignalHandler(int iSignalValue, SignalFunT pfHandler);

	static void setHandlerUsr1(SignalFunT pfUSR1, int iSignalArg);
	static void setHandlerUsr2(SignalFunT pfUSR2, int iSignalArg);
	static void setHandlerQuit(SignalFunT pfQuit, int iSignalArgQuit);

private:

	//ע��
	static void registerSignalUsr1();

	//ע��
	static void registerSignalUsr2();

	//ע��
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
