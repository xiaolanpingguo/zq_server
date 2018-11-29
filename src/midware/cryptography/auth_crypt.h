#pragma once


#include "arc4.h"

namespace zq{

class BigNumber;
class AuthCrypt
{
public:
    AuthCrypt();
	~AuthCrypt() = default;

    void Init(BigNumber* K);
    void DecryptRecv(uint8 *, size_t);
    void EncryptSend(uint8 *, size_t);

    bool IsInitialized() const { return _initialized; }

private:
    ARC4 _clientDecrypt;
    ARC4 _serverEncrypt;
    bool _initialized;
};

}



