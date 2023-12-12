#include "ResourceBlock.h"

#include "FileIO/Read.h"
#include "FileIO/Write.h"
#include "FileIO/Util.h"
#include "Enum.h"
#include "Logger.h"
#include "Profiling/Perf/Instrumentor.h"

PSAPI_NAMESPACE_BEGIN

// Sequential Read of a single Image Resource Block, in its current state it just dumps the 
// data rather than parsing it.
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void ResourceBlock::read(File& document)
{
	PROFILE_FUNCTION();

	Signature signature = Signature(ReadBinaryData<uint32_t>(document));
	if (signature != Signature("8BIM"))
	{
		PSAPI_LOG_ERROR("ResourceBlock", "Signature does not match '8BIM', got '%c%c%c%c' instead",
			signature.m_Representation[0],
			signature.m_Representation[1],
			signature.m_Representation[2],
			signature.m_Representation[3])
	}
	m_UniqueId = Enum::intToImageResource(ReadBinaryData<uint16_t>(document));
	m_Name.read(document, 2u);
	m_Size = RoundUpToMultiple(ReadBinaryData<uint32_t>(document), 2u);
	m_Data = ReadBinaryArray<uint8_t>(document, m_Size);

	m_BlockSize = 4u + 2u + m_Name.m_Size + 4u + m_Size;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void ResourceBlock::write(File& document)
{
	PROFILE_FUNCTION();

	Signature sig = Signature("8BPS");
	WriteBinaryData<uint32_t>(document, sig.m_Value);
	
	WriteBinaryData<uint16_t>(document, Enum::imageResourceToInt(m_UniqueId));
	m_Name.write(document, 2u);
	WriteBinaryData<uint32_t>(document, m_Size);	// This value is already padded
	WriteBinaryArray<uint8_t>(document, m_Data);
}


PSAPI_NAMESPACE_END