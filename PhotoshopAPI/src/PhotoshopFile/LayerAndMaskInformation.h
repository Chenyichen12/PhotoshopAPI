#pragma once

#include "../Macros.h"
#include "../Util/Enum.h"
#include "../Util/Struct/File.h"
#include "../Util/Struct/Section.h"
#include "../Util/Struct/ResourceBlock.h"
#include "FileHeader.h"


#include <vector>

PSAPI_NAMESPACE_BEGIN

namespace {

	namespace LayerRecords
	{
		struct ChannelInformation
		{
			Enum::ChannelID m_ChannelID;
			uint64_t m_Size;
		};

		struct LayerMask
		{
			uint32_t m_Top = 0u, m_Left = 0u, m_Bottom = 0u, m_Right = 0u;
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
			std::optional<uint8_t> m_UserMaskDensity = 0u;
			std::optional<float64_t> m_UserMaskFeather = 0.0f;
			std::optional<uint8_t> m_VectorMaskDensity = 0u;
			std::optional<float64_t> m_VectorMaskFeather = 0.0f;

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
			uint32_t m_Size = 0u;
			std::optional<LayerMask> m_LayerMask;
			std::optional<LayerMask> m_VectorMask;


			LayerMaskData() {};
			LayerMaskData(File& document, const uint32_t sectionLength);
		};

		struct LayerBlendingRanges
		{
			LayerBlendingRanges();
		};
	}

	
	struct LayerRecord : public FileSection
	{
		uint32_t m_Top, m_Left, m_Bottom, m_Right;
		uint16_t m_ChannelCount;
		std::vector<LayerRecords::ChannelInformation> m_ChannelInformation;
		Enum::BlendMode m_BlendMode;
		uint8_t m_Opacity; // 0 - 255
		uint8_t m_Clipping;	// 0 or 1
		uint8_t m_BitFlags;

		PascalString m_LayerName;

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
	
	private:


	};


	struct GlobalLayerMaskInfo : public FileSection
	{
		GlobalLayerMaskInfo() {};
		GlobalLayerMaskInfo(File& document, const uint64_t offset) {};
	};


	struct ChannelImageData : public FileSection
	{
		ChannelImageData() {};
		ChannelImageData(File& document, const FileHeader& header, const uint64_t offset) {};
	};

	struct AdditionaLayerInfo : public FileSection
	{
		AdditionaLayerInfo() {};
		AdditionaLayerInfo(File& document, const uint64_t offset) {};
	};

	struct LayerInfo : public FileSection
	{
		std::vector<LayerRecord> m_LayerRecords;
		std::vector<ChannelImageData> m_ChannelImageData;

		LayerInfo(){};
		LayerInfo(File& document, const FileHeader& header, const uint64_t offset);
	};
}

struct LayerAndMaskInformation : public FileSection
{

	LayerInfo m_LayerInfo;
	GlobalLayerMaskInfo m_GlobalLayerMaskInfo;
	ChannelImageData m_ChannelImageData;
	AdditionaLayerInfo m_AdditionalLayerInfo;

	bool read(File& document, const FileHeader& header, const uint64_t offset);
};



PSAPI_NAMESPACE_END