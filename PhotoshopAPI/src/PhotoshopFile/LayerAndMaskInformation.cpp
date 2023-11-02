#include "LayerAndMaskInformation.h"

#include "../Macros.h"
#include "../Util/Read.h"
#include "../Util/StringUtil.h"
#include "FileHeader.h"

#include <variant>

PSAPI_NAMESPACE_BEGIN


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void LayerRecords::LayerMask::setFlags(const uint32_t bitFlag)
{
	this->m_PositionRelativeToLayer = (bitFlag & this->m_PositionRelativeToLayerMask) != 0;
	this->m_Disabled = (bitFlag & this->m_DisabledMask) != 0;
	this->m_IsVector = (bitFlag & this->m_IsVectorMask) != 0;
	this->m_HasMaskParams = (bitFlag & this->m_HasMaskParamsMask) != 0;
}


void LayerRecords::LayerMask::setMaskParams(const uint32_t bitFlag)
{
	this->m_HasUserMaskDensity = (bitFlag & this->m_UserMaskDensityMask) != 0;
	this->m_HasUserMaskFeather = (bitFlag & this->m_UserMaskFeatherMask) != 0;
	this->m_HasVectorMaskDensity = (bitFlag & this->m_VectorMaskDensityMask) != 0;
	this->m_HasVectorMaskFeather = (bitFlag & this->m_VectorMaskFeatherMask) != 0;
}


// Read the mask parameters according to which mask parameter bit flags are set and return the total
// length of all the bytes read
uint32_t LayerRecords::LayerMask::readMaskParams(File& document)
{
	uint32_t bytesRead = 0u;
	if (this->m_HasUserMaskDensity)
	{
		this->m_UserMaskDensity.emplace(ReadBinaryData<uint8_t>(document));
		bytesRead += 1u;
	}
	if (this->m_HasUserMaskFeather)
	{
		this->m_UserMaskFeather.emplace(ReadBinaryData<float64_t>(document));
		bytesRead += 8u;
	}
	if (this->m_HasVectorMaskDensity)
	{
		this->m_VectorMaskDensity.emplace(ReadBinaryData<uint8_t>(document));
		bytesRead += 1u;
	}
	if (this->m_HasVectorMaskFeather)
	{
		this->m_VectorMaskFeather.emplace(ReadBinaryData<float64_t>(document));
		bytesRead += 8u;
	}

	return bytesRead;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
LayerRecords::LayerMaskData::LayerMaskData(File& document)
{
	this->m_Size = ReadBinaryData<uint32_t>(document) + 4u;
	int64_t toRead = static_cast<int64_t>(this->m_Size) - 4u;

	// Empty section;
	if (toRead == 0)
	{
		return;
	}

	// Use this to make sure we only read one set of mask parameters
	bool hasMaskParams = false;

	// Read the first layer mask, we dont yet know which one it is
	{
		LayerMask mask = LayerMask();

		mask.m_Top = ReadBinaryData<int32_t>(document);
		mask.m_Left = ReadBinaryData<int32_t>(document);
		mask.m_Bottom = ReadBinaryData<int32_t>(document);
		mask.m_Right = ReadBinaryData<int32_t>(document);
		toRead -= 16u;

		const uint8_t defaultColor = ReadBinaryData<uint8_t>(document);
		toRead -= 1u;
		if (defaultColor != 0 && defaultColor != 255)
		{
			PSAPI_LOG_ERROR("LayerMaskData", "Layer Mask default color can only be 0 or 255, not %u", defaultColor);
		}

		const uint8_t bitFlags = ReadBinaryData<uint8_t>(document);
		mask.setFlags(bitFlags);
		toRead -= 1u;

		// Store this value to compare against later
		hasMaskParams = mask.m_HasMaskParams;
		if (hasMaskParams && this->m_Size <= 28)
		{
			const uint8_t maskParams = ReadBinaryData<uint8_t>(document);
			mask.setMaskParams(maskParams);
			toRead -= 1u;
			toRead -= mask.readMaskParams(document);
		}

		if ((bitFlags & 1u << 3) != 0u)
		{
			this->m_VectorMask.emplace(mask);
		}
		else
		{
			this->m_LayerMask.emplace(mask);
		}
	}

	// Check if there is still enough space left to read another section
	// If there is 2 masks the vector mask will always be first, therefore we push 
	// back into the real user mask instead (pixel mask)
	if (toRead >= 18u)
	{
		LayerMask layerMask = LayerMask();

		const uint8_t bitFlags = ReadBinaryData<uint8_t>(document);
		layerMask.setFlags(bitFlags);
		toRead -= 1u;

		layerMask.m_DefaultColor = ReadBinaryData<uint8_t>(document);
		if (layerMask.m_DefaultColor != 0 && layerMask.m_DefaultColor != 255)
		{
			PSAPI_LOG_ERROR("LayerMaskData", "Layer Mask default color can only be 0 or 255, not %u", layerMask.m_DefaultColor);
		}
		toRead -= 1u;

		layerMask.m_Top = ReadBinaryData<int32_t>(document);
		layerMask.m_Left = ReadBinaryData<int32_t>(document);
		layerMask.m_Bottom = ReadBinaryData<int32_t>(document);
		layerMask.m_Right = ReadBinaryData<int32_t>(document);
		toRead -= 16u;

		if (hasMaskParams || layerMask.m_HasMaskParams)
		{
			const uint8_t maskParams = ReadBinaryData<uint8_t>(document);
			layerMask.setMaskParams(maskParams);
			toRead -= 1u;
			toRead -= layerMask.readMaskParams(document);
		}

		this->m_LayerMask.emplace(layerMask);
	}

	if (toRead < 0 || toRead > 2)
	{
		PSAPI_LOG_WARNING("LayerMaskData", "Expected either 0 or 2 padding bytes, got %i instead", toRead)
	}

	document.skip(toRead);
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
LayerRecords::LayerBlendingRanges::LayerBlendingRanges(File& document)
{
	this->m_Size = ReadBinaryData<uint32_t>(document) + 4u;
	uint32_t toRead = this->m_Size - 4u;

	// This appears to always be 5 different layer blending ranges. In photoshop (as of CC 23.3.2)
	// we only have control over Combined, Red, Green and Blue. My guess is that the 5th blending range is 
	// for the alpha channel
	while (toRead >= 8u)
	{
		uint8_t sourceLow1 = ReadBinaryData<uint8_t>(document);
		uint8_t sourceLow2 = ReadBinaryData<uint8_t>(document);
		uint8_t sourceHigh1 = ReadBinaryData<uint8_t>(document);
		uint8_t sourceHigh2 = ReadBinaryData<uint8_t>(document);

		this->m_SourceRanges.emplace_back(std::tuple(sourceLow1, sourceLow2, sourceHigh1, sourceHigh2));

		uint8_t destinationLow1 = ReadBinaryData<uint8_t>(document);
		uint8_t destinationLow2 = ReadBinaryData<uint8_t>(document);
		uint8_t destinationHigh1 = ReadBinaryData<uint8_t>(document);
		uint8_t destinationHigh2 = ReadBinaryData<uint8_t>(document);

		this->m_DestinationRanges.emplace_back(std::tuple(destinationLow1, destinationLow2, destinationHigh1, destinationHigh2));

		toRead -= 8u;
	}
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
LayerRecord::LayerRecord(File& document, const FileHeader& header, const uint64_t offset)
{
	this->m_Offset = offset;
	document.setOffset(offset);

	this->m_Top = ReadBinaryData<uint32_t>(document);
	this->m_Left = ReadBinaryData<uint32_t>(document);
	this->m_Bottom = ReadBinaryData<uint32_t>(document);
	this->m_Right = ReadBinaryData<uint32_t>(document);

	this->m_ChannelCount = ReadBinaryData<uint16_t>(document);
	if (this->m_ChannelCount > 56)
	{
		PSAPI_LOG_ERROR("LayerRecord", "A Photoshop document cannot have more than 56 channels at once")
	}
	this->m_ChannelInformation.reserve(this->m_ChannelCount);

	// Read the Channel Information, there is one of these for each channel in the layer record
	for (int i = 0; i < this->m_ChannelCount; i++)
	{
		LayerRecords::ChannelInformation channelInfo{};

		channelInfo.m_ChannelID = Enum::intToChannelID(ReadBinaryData<uint16_t>(document));

		std::variant<uint32_t, uint64_t> size = ReadBinaryDataVariadic<uint32_t, uint64_t>(document, header.m_Version);
		channelInfo.m_Size = ExtractWidestValue<uint32_t, uint64_t>(size);

		this->m_ChannelInformation.emplace_back(channelInfo);
	}

	// Perform a signature check but do not store it as it isnt required
	Signature signature = Signature(ReadBinaryData<uint32_t>(document));
	if (signature != Signature("8BIM"))
	{
		PSAPI_LOG_ERROR("LayerRecord", "Signature does not match '8BIM', got '%s' instead",
			uint32ToString(signature.m_Value))
	}
	
	std::string blendModeStr = uint32ToString(ReadBinaryData<uint32_t>(document));
	std::optional<Enum::BlendMode> blendMode = Enum::getBlendMode<std::string, Enum::BlendMode>(blendModeStr);
	if (blendMode.has_value())
	{
		this->m_BlendMode = blendMode.value();
	}
	else
	{
		this->m_BlendMode = Enum::BlendMode::Normal;
		PSAPI_LOG_ERROR("LayerRecord", "Got invalid blend mode: %s", blendModeStr.c_str());
	}

	this->m_Opacity = ReadBinaryData<uint8_t>(document);
	this->m_Clipping = ReadBinaryData<uint8_t>(document);
	this->m_BitFlags = ReadBinaryData<uint8_t>(document);

	document.skip(1u);	// Filler byte;

	// This is the length of the next fields, we need this to find the length of the additional layer info
	const uint32_t extraDataLen = ReadBinaryData<uint32_t>(document);
	uint32_t toRead = extraDataLen;
	{
		LayerRecords::LayerMaskData layerMaskSection = LayerRecords::LayerMaskData(document);
		if (layerMaskSection.m_Size > 4u)
		{
			this->m_LayerMaskData.emplace(layerMaskSection);
			toRead -= layerMaskSection.m_Size;
		}
		else
		{
			toRead -= 4u;
		}

		this->m_LayerBlendingRanges = LayerRecords::LayerBlendingRanges(document);
		toRead -= this->m_LayerBlendingRanges.m_Size;

		this->m_LayerName = PascalString(document, 4u);
		toRead -= this->m_LayerName.m_Size;

	}

	document.skip(toRead);

}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
LayerInfo::LayerInfo(File& document, const FileHeader& header, const uint64_t offset)
{
	this->m_Offset = offset;
	document.setOffset(offset);

	// Read the layer info length marker which is 4 bytes in psd and 8 bytes in psb mode 
	// (note, this value is padded to a multiple of 4, despite what the docs say)
	std::variant<uint32_t, uint64_t> size = ReadBinaryDataVariadic<uint32_t, uint64_t>(document, header.m_Version);
	this->m_Size = ExtractWidestValue<uint32_t, uint64_t>(size);

	// If this value is negative the first alpha channel of the layer records holds the merged image result (Image Data Section) alpha channel
	// TODO this isnt yet implemented
	int16_t layerCount = std::abs(ReadBinaryData<int16_t>(document));
	this->m_LayerRecords.reserve(layerCount);
	this->m_ChannelImageData.reserve(layerCount);

	// Extract layer records
	for (int i = 0; i < layerCount; i++)
	{
		this->m_LayerRecords.emplace_back(LayerRecord(document, header, document.getOffset()));
	}

	// Extract Channel Image Data
	for (int i = 0; i < layerCount; i++)
	{
		this->m_ChannelImageData.emplace_back(ChannelImageData(document, header, document.getOffset()));
	}

}


// Extract the layer and mask information section
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
bool LayerAndMaskInformation::read(File& document, const FileHeader& header, const uint64_t offset)
{
	this->m_Offset = offset;
	document.setOffset(offset);

	// Read the layer mask info length marker which is 4 bytes in psd and 8 bytes in psb mode
	// This value is
	std::variant<uint32_t, uint64_t> size = ReadBinaryDataVariadic<uint32_t, uint64_t>(document, header.m_Version);
	this->m_Size = ExtractWidestValue<uint32_t, uint64_t>(size);
	
	
	this->m_LayerInfo = LayerInfo(document, header, document.getOffset());

	return true;
}


PSAPI_NAMESPACE_END