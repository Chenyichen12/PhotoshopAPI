#pragma once

#include "FileHeader.h"
#include "Macros.h"
#include "Enum.h"
#include "Struct/File.h"
#include "Struct/Section.h"
#include "Struct/ResourceBlock.h"
#include "Struct/TaggedBlock.h"
#include "Struct/ImageChannel.h"
#include "Compression/Compression.h"


#include <vector>


PSAPI_NAMESPACE_BEGIN


struct AdditionalLayerInfo : public FileSection
{
	std::vector<std::unique_ptr<TaggedBlock::Base>> m_TaggedBlocks;

	AdditionalLayerInfo() = default;
	AdditionalLayerInfo(const AdditionalLayerInfo&) = delete;
	AdditionalLayerInfo(AdditionalLayerInfo&&) = default;
	AdditionalLayerInfo& operator=(const AdditionalLayerInfo&) = delete;
	AdditionalLayerInfo& operator=(AdditionalLayerInfo&&) = default;

	AdditionalLayerInfo(File& document, const FileHeader& header, const uint64_t offset, const uint64_t maxLength, const uint16_t padding = 1u);
};


// Structs to hold the different types of data found in the layer records themselves
namespace LayerRecords
{
	struct ChannelInformation
	{
		Enum::ChannelID m_ChannelID;
		uint64_t m_Size;	// This appears to include the length of the compression marker
	};


	// A singular layer mask as represented in the LayerMaskData section found in the layer records
	struct LayerMask
	{
		int32_t m_Top = 0, m_Left = 0, m_Bottom = 0, m_Right = 0;
		uint8_t m_DefaultColor = 0u;		// 0 or 255

		bool m_PositionRelativeToLayer = false;	// Bit 0 of the flags
		bool m_Disabled = false;				// Bit 1 of the flags
		bool m_IsVector = false;				// Bit 3 of the flags
		bool m_HasMaskParams = false;			// Bit 4 of the flags

		bool m_HasUserMaskDensity = false;		// Bit 0 of the maskParams
		bool m_HasUserMaskFeather = false;		// Bit 1 of the maskParams
		bool m_HasVectorMaskDensity = false;	// Bit 2 of the maskParams
		bool m_HasVectorMaskFeather = false;	// Bit 3 of the maskParams

		// Only exists on one of the two masks
		std::optional<uint8_t> m_UserMaskDensity;
		std::optional<float64_t> m_UserMaskFeather;
		std::optional<uint8_t> m_VectorMaskDensity;
		std::optional<float64_t> m_VectorMaskFeather;

		void setFlags(const uint32_t bitFlag);
		void setMaskParams(const uint32_t bitFlag);
		uint32_t readMaskParams(File& document);

	private:
		// Masks to perform bitwise & operations with to check if certain flags exist
		// Note: we skip bit 2 here as its marked obsolete and there is only 5 total options
		const uint8_t m_PositionRelativeToLayerMask = 1u << 0;
		const uint8_t m_DisabledMask = 1u << 1;
		const uint8_t m_IsVectorMask = 1u << 3;
		const uint8_t m_HasMaskParamsMask = 1u << 4;

		// Mask parameter bitmasks to bitwise & with
		const uint8_t m_UserMaskDensityMask = 1u << 0;
		const uint8_t m_UserMaskFeatherMask = 1u << 1;
		const uint8_t m_VectorMaskDensityMask = 1u << 2;
		const uint8_t m_VectorMaskFeatherMask = 1u << 3;
	};


	// This section can hold either no mask, one mask or two masks depending on the size of the data in it.
	// The layout is a bit confusing here as it reads the second mask in reverse order. The mask parameters
	// exist only on one of the masks rather than both as they cover both cases
	struct LayerMaskData
	{
		uint32_t m_Size = 4u;	// Includes the section length marker
		std::optional<LayerMask> m_LayerMask;
		std::optional<LayerMask> m_VectorMask;


		LayerMaskData() {};
		LayerMaskData(File& document);
	};


	struct LayerBlendingRanges
	{
		uint32_t m_Size = 4u;	// Includes the section length marker

		// Blending ranges hold 2 low and 2 high values, if the marker wasnt split in photoshop 
		// the low and high values are identical
		using Data = std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>>;
		Data m_SourceRanges;
		Data m_DestinationRanges;

		LayerBlendingRanges() {};
		LayerBlendingRanges(File& document);
	};
}


// A layer record describes a single layer in a photoshop document and may include up to 56 channels
struct LayerRecord : public FileSection
{
	PascalString m_LayerName;

	uint32_t m_Top, m_Left, m_Bottom, m_Right;
	uint16_t m_ChannelCount;
	std::vector<LayerRecords::ChannelInformation> m_ChannelInformation;
	Enum::BlendMode m_BlendMode;
	uint8_t m_Opacity; // 0 - 255
	uint8_t m_Clipping;	// 0 or 1
	uint8_t m_BitFlags;

	std::optional<LayerRecords::LayerMaskData> m_LayerMaskData;
	LayerRecords::LayerBlendingRanges m_LayerBlendingRanges;
	std::optional<AdditionalLayerInfo> m_AdditionalLayerInfo;

	LayerRecord() :
		m_Top(0u),
		m_Left(0u),
		m_Bottom(0u),
		m_Right(0u),
		m_ChannelCount(0u),
		m_BlendMode(Enum::BlendMode::Normal),
		m_Opacity(0u),
		m_Clipping(0u),
		m_BitFlags(0u) {};
	LayerRecord(File& document, const FileHeader& header, const uint64_t offset);
};


// This currently just gets skipped
struct GlobalLayerMaskInfo : public FileSection
{
	GlobalLayerMaskInfo() {};
	GlobalLayerMaskInfo(File& document, const uint64_t offset);
};


struct BaseChannelImageData : public FileSection
{
	virtual ~BaseChannelImageData() = default;
};

// Channel Image Data for a single layer, there is at most 56 channels in a given layer
template <typename T>
struct ChannelImageData : BaseChannelImageData
{
	// TODO add blosc2 compression to this data
	std::vector<ImageChannel<T>> m_ImageData;

	ChannelImageData() {};
	ChannelImageData(File& document, const FileHeader& header, const uint64_t offset, const LayerRecord& layerRecord);

	// Get an index to a specific channel based on the identifier
	// returns -1 if no matching channel is found
	int getChannelIndex(Enum::ChannelID channelID)
	{
		for (int i = 0; i < m_ImageData.size(); ++i)
		{
			if (m_ImageData[i].m_ChannelID == channelID)
			{
				return i;
			}
		}
		return -1;
	}
};


struct LayerInfo : public FileSection
{
	// These two are guaranteed to be in the same order based on Photoshop specification
	std::vector<LayerRecord> m_LayerRecords;
	std::vector<std::unique_ptr<BaseChannelImageData>> m_ChannelImageData;

	LayerInfo(){};
	LayerInfo(File& document, const FileHeader& header, const uint64_t offset, const bool isFromAdditionalLayerInfo = false, std::optional<uint64_t> sectionSize = std::nullopt);

	// Find the index to a layer based on a layer name that is given
	// if no layer with the name is found, return -1. In the case of multiple name matches the last in the photoshop document
	// is returned. This can also be used to get an index into the ChannelImageData vector
	int getLayerIndex(const std::string& layerName);
};


struct LayerAndMaskInformation : public FileSection
{

	LayerInfo m_LayerInfo;
	GlobalLayerMaskInfo m_GlobalLayerMaskInfo;
	std::optional<AdditionalLayerInfo> m_AdditionalLayerInfo;

	bool read(File& document, const FileHeader& header, const uint64_t offset);
};


PSAPI_NAMESPACE_END