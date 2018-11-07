#pragma once

#include "baselib/interface_header/platform.h"
#include <string>
#include <openssl/sha.h>

namespace zq {

class BigNumber;
class SHA1Hash
{
public:
	SHA1Hash();
	~SHA1Hash();

	void UpdateBigNumbers(BigNumber* bn0, ...);

	void UpdateData(const uint8 *dta, size_t len);
	void UpdateData(const std::string &str);

	void Initialize();
	void Finalize();

	uint8 *GetDigest(void) { return mDigest; }
	int GetLength(void) const { return SHA_DIGEST_LENGTH; }

public:

	static std::string calculateSHA1Hash(std::string const& content);
	static std::string byteArrayToHexStr(uint8 const* bytes, uint32 length, bool reverse = false);
	static void hexStrToByteArray(std::string const& str, uint8* out, bool reverse = false);

private:
	SHA_CTX mC;
	uint8 mDigest[SHA_DIGEST_LENGTH];
};

}

