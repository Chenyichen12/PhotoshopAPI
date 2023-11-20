#pragma once

#include "../Macros.h"
#include "../Util/Enum.h"
#include "../Util/Struct/File.h"
#include "../Util/Struct/Section.h"
#include "FileHeader.h"

#include <vector>

#include <cstdint>

PSAPI_NAMESPACE_BEGIN

struct ColorModeData : public FileSection
{
	std::vector<uint8_t> m_Data;

	bool read(File& document);
};

PSAPI_NAMESPACE_END