#include "PascalString.h"

#include "../../Macros.h"
#include "../Read.h"
#include "File.h"

#include <string>

PSAPI_NAMESPACE_BEGIN


PascalString::PascalString(File& document, const uint8_t padding)
{
	this->m_Size = RoundUpToMultiple<uint8_t>(ReadBinaryData<uint8_t>(document), 2u);
	std::vector<uint8_t> stringData = ReadBinaryArray<uint8_t>(document, this->m_Size - 1u);
	this->m_String = std::string(stringData.begin(), stringData.end());
}

PSAPI_NAMESPACE_END