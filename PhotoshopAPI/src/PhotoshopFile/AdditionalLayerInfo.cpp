#include "AdditionalLayerInfo.h"
#include "LayerAndMaskInformation.h"
#include "FileHeader.h"
#include "Struct/TaggedBlock.h"

#include <memory>
#include <optional>

PSAPI_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
uint64_t AdditionalLayerInfo::calculateSize(std::shared_ptr<FileHeader> header /*= nullptr*/) const
{	
	uint64_t size = m_TaggedBlocks.calculateSize();
	return size;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void AdditionalLayerInfo::read(File& document, const FileHeader& header, const uint64_t offset, const uint64_t maxLength, const uint16_t padding)
{
	m_Offset = offset;
	document.setOffset(offset);
	m_Size = 0u;

	int64_t toRead = maxLength;
	while (toRead >= 12u)
	{
		const std::shared_ptr<TaggedBlock> taggedBlock = m_TaggedBlocks.readTaggedBlock(document, header, padding);
		toRead -= taggedBlock->getTotalSize();
		m_Size += taggedBlock->getTotalSize();
	}
	if (toRead >= 0)
	{
		m_Size += toRead;
		document.skip(toRead);
		return;
	}

	if (toRead <= 0)
	{
		PSAPI_LOG_WARNING("AdditionalLayerInfo", "Read too much data for the additional layer info, was allowed %" PRIu64 " but read %" PRIu64 " instead",
			maxLength, maxLength - toRead);
	}
}

PSAPI_NAMESPACE_END