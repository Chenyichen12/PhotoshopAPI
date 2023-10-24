#include "ColorModeData.h"

#include "FileHeader.h"
#include "../Macros.h"
#include "../Util/Enum.h"
#include "../Util/Read.h"
#include "../Util/Struct/File.h"
#include "../Util/Struct/Section.h"

#include <vector>

#include <cstdint>

PSAPI_NAMESPACE_BEGIN

bool ColorModeData::read(File& document, const FileHeader& header)
{
	this->m_Offset = 26;
	document.setOffset(this->m_Offset);

	this->m_Size = ReadBinaryData<uint32_t>(document) + 4u;

	// Just dump the data without parsing it
	if (this->m_Size > 0)
	{
		this->m_Data = ReadBinaryArray<char>(document, this->m_Size);
	}
	else
	{
		this->m_Data = std::vector<char>(0);
	}

	return true;
}

PSAPI_NAMESPACE_END