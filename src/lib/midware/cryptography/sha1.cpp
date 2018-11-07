#include "sha1.h"
#include "big_number.h"
#include <cstring>
#include <stdarg.h>

#if ZQ_PLATFORM == ZQ_PLATFORM_WIN
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

namespace zq {

SHA1Hash::SHA1Hash()
{
	SHA1_Init(&mC);
	memset(mDigest, 0, SHA_DIGEST_LENGTH * sizeof(uint8));
}

SHA1Hash::~SHA1Hash()
{
	SHA1_Init(&mC);
}

void SHA1Hash::UpdateData(const uint8 *dta, size_t len)
{
	SHA1_Update(&mC, dta, len);
}

void SHA1Hash::UpdateData(const std::string &str)
{
	UpdateData((uint8 const*)str.c_str(), str.length());
}

void SHA1Hash::UpdateBigNumbers(BigNumber* bn0, ...)
{
	va_list v;
	BigNumber* bn;

	va_start(v, bn0);
	bn = bn0;
	while (bn)
	{
		UpdateData(bn->AsByteArray().get(), bn->GetNumBytes());
		bn = va_arg(v, BigNumber*);
	}
	va_end(v);
}

void SHA1Hash::Initialize()
{
	SHA1_Init(&mC);
}

void SHA1Hash::Finalize(void)
{
	SHA1_Final(mDigest, &mC);
}

std::string SHA1Hash::calculateSHA1Hash(std::string const& content)
{
	unsigned char digest[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*)content.c_str(), content.length(), (unsigned char*)&digest);

	return byteArrayToHexStr(digest, SHA_DIGEST_LENGTH);
}

std::string SHA1Hash::byteArrayToHexStr(uint8 const* bytes, uint32 length, bool reverse)
{
	int32 init = 0;
	int32 end = length;
	int8 op = 1;

	if (reverse)
	{
		init = length - 1;
		end = -1;
		op = -1;
	}

	std::ostringstream ss;
	for (int32 i = init; i != end; i += op)
	{
		char buffer[4];
		sprintf(buffer, "%02X", bytes[i]);
		ss << buffer;
	}

	return ss.str();
}

void SHA1Hash::hexStrToByteArray(std::string const& str, uint8* out, bool reverse)
{
	// string must have even number of characters
	if (str.length() & 1)
		return;

	int32 init = 0;
	int32 end = int32(str.length());
	int8 op = 1;

	if (reverse)
	{
		init = int32(str.length() - 2);
		end = -2;
		op = -1;
	}

	uint32 j = 0;
	for (int32 i = init; i != end; i += 2 * op)
	{
		char buffer[3] = { str[i], str[i + 1], '\0' };
		out[j++] = uint8(strtoul(buffer, nullptr, 16));
	}
}

}

