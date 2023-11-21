#pragma once

#include "Macros.h"
#include "Logger.h"

#include <unordered_map>
#include <vector>
#include <optional>

PSAPI_NAMESPACE_BEGIN


// Utility
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
namespace 
{
	// Template function for finding any of the enums by  their value defined in the respective std::map
	template <typename KeyType, typename ValueType>
	inline std::optional<KeyType> findByValue(const std::unordered_map<KeyType, ValueType>& map, const ValueType searchValue)
	{
		for (const auto& pair : map)
		{
			if (pair.second == searchValue)
			{
				return pair.first;
			}
		}
		return std::nullopt;
	}

	// Template function for finding any of the enums by their value defined in the respective std::map
	template <typename KeyType, typename ValueType>
	inline std::optional<std::vector<KeyType>> findMultipleByValue(const std::unordered_map<KeyType, ValueType>& map, const ValueType searchValue)
	{
		std::vector<KeyType> results;
		for (const auto& pair : map)
		{
			if (pair.second == searchValue)
			{
				results.push_back(pair.first);
			}
		}
		if (results.size() == 0)
		{
			return std::nullopt;
		}
		return results;
	}
}


// Header Enums
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
namespace Enum
{
	// File Format Version
	enum class Version
	{
		Psd,
		Psb
	};

	inline std::unordered_map<uint16_t, Version> versionMap
	{
		{static_cast<uint16_t>(1u), Version::Psd},
		{static_cast<uint16_t>(2u), Version::Psb}
	};

	// Bit Depth of the Document
	enum class BitDepth
	{
		BD_1,
		BD_8,
		BD_16,
		BD_32
	};

	inline std::unordered_map<uint16_t, BitDepth> bitDepthMap
	{
		{static_cast<uint16_t>(1u), BitDepth::BD_1},
		{static_cast<uint16_t>(8u), BitDepth::BD_8},
		{static_cast<uint16_t>(16u), BitDepth::BD_16},
		{static_cast<uint16_t>(32u), BitDepth::BD_32},
	};

	enum class ColorMode
	{
		Bitmap,
		Grayscale,
		Indexed,
		RGB,
		CMYK,
		Multichannel,
		Duotone,
		Lab
	};

	inline std::unordered_map<uint16_t, ColorMode> colorModeMap
	{
		{static_cast<uint16_t>(0u), ColorMode::Bitmap},
		{static_cast<uint16_t>(1u), ColorMode::Grayscale},
		{static_cast<uint16_t>(2u), ColorMode::Indexed},
		{static_cast<uint16_t>(3u), ColorMode::RGB},
		{static_cast<uint16_t>(4u), ColorMode::CMYK},
		{static_cast<uint16_t>(7u), ColorMode::Multichannel},
		{static_cast<uint16_t>(8u), ColorMode::Duotone},
		{static_cast<uint16_t>(9u), ColorMode::Lab}
	};
}


// Image Resource Enums
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
namespace Enum
{
	enum class ImageResource
	{
		NotImplemented,
		ResolutionInfo,
		AlphaChannelNames,
		BackgroundColor,
		PrintFlags,
		ColorHalftoningInfo,
		ColorTransferFunctions,
		LayerStateInformation,
		LayerGroupInformation,
		IPTCRecord,
		GridAndGuidesInformation,
		ThumbnailResource,
		GlobalAngle,
		ICCUntaggedProfile,
		SpotHalftone,
		IDSeed,
		UnicodeAlphaNames,
		GlobalAltitude,
		Slices,
		AlphaIdentifiers,
		URLList,
		VersionInfo,
		ExifData1,
		XMPMetadata,
		CaptionDigest,
		PrintScale,
		PixelAspectRatio,
		AlternateSpotColors,
		LayerSelectionID,
		LayerGroupEnabledID,
		DisplayInfo,
		PrintInformation,
		PrintStyle,
		PrintFlagsInfo
	};


	inline ImageResource intToImageResource(const uint16_t value)
	{
		switch (value)
		{
		case 1005: return ImageResource::ResolutionInfo;
		case 1006: return ImageResource::AlphaChannelNames;
		case 1010: return ImageResource::BackgroundColor;
		case 1011: return ImageResource::PrintFlags;
		case 1013: return ImageResource::ColorHalftoningInfo;
		case 1016: return ImageResource::ColorTransferFunctions;
		case 1024: return ImageResource::LayerStateInformation;
		case 1026: return ImageResource::LayerGroupInformation;
		case 1028: return ImageResource::IPTCRecord;
		case 1032: return ImageResource::GridAndGuidesInformation;
		case 1036: return ImageResource::ThumbnailResource;
		case 1037: return ImageResource::GlobalAngle;
		case 1041: return ImageResource::ICCUntaggedProfile;
		case 1043: return ImageResource::SpotHalftone;
		case 1044: return ImageResource::IDSeed;
		case 1045: return ImageResource::UnicodeAlphaNames;
		case 1049: return ImageResource::GlobalAltitude;
		case 1050: return ImageResource::Slices;
		case 1053: return ImageResource::AlphaIdentifiers;
		case 1054: return ImageResource::URLList;
		case 1057: return ImageResource::VersionInfo;
		case 1058: return ImageResource::ExifData1;
		case 1060: return ImageResource::XMPMetadata;
		case 1061: return ImageResource::CaptionDigest;
		case 1062: return ImageResource::PrintScale;
		case 1064: return ImageResource::PixelAspectRatio;
		case 1067: return ImageResource::AlternateSpotColors;
		case 1069: return ImageResource::LayerSelectionID;
		case 1072: return ImageResource::LayerGroupEnabledID;
		case 1077: return ImageResource::DisplayInfo;
		case 1082: return ImageResource::PrintInformation;
		case 1083: return ImageResource::PrintStyle;
		case 10000: return ImageResource::PrintFlagsInfo;
		default: return ImageResource::NotImplemented;
		}
	}
}


// Layer and Mask Information Enums
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
namespace Enum
{
	// Channel ID information
	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------
	enum class ChannelID
	{
		Red,		// Channel 0 in RGB Mode
		Green,		// Channel 1 in RGB Mode
		Blue,		// Channel 2 in RGB Mode
		Cyan,		// Channel 0 in CMYK Mode
		Magenta,	// Channel 1 in CMYK Mode
		Yellow,		// Channel 2 in CMYK Mode
		Black,		// Channel 3 in CMYK Mode
		Gray,		// Channel 0 in Grayscale Mode
		Custom,		// Any other channel
		TransparencyMask,			// Vector mask (?)
		UserSuppliedLayerMask,		// Pixel Mask
		RealUserSuppliedLayerMask,	// Vector and Pixel mask combined
	};

	inline ChannelID rgbIntToChannelID(const int16_t value)
	{
		switch (value)
		{
		case 0: return ChannelID::Red;
		case 1: return ChannelID::Green;
		case 2: return ChannelID::Blue;
		case -1: return ChannelID::TransparencyMask;
		case -2: return ChannelID::UserSuppliedLayerMask;
		case -3: return ChannelID::RealUserSuppliedLayerMask;
		default: return ChannelID::Custom;	// These are channels set by the user
		}
	}

	inline ChannelID cmykIntToChannelID(const int16_t value)
	{
		switch (value)
		{
		case 0: return ChannelID::Cyan;
		case 1: return ChannelID::Magenta;
		case 2: return ChannelID::Yellow;
		case 3: return ChannelID::Black;
		case -1: return ChannelID::TransparencyMask;
		case -2: return ChannelID::UserSuppliedLayerMask;
		case -3: return ChannelID::RealUserSuppliedLayerMask;
		default: return ChannelID::Custom;	// These are channels set by the user
		}
	}

	inline ChannelID grayscaleIntToChannelID(const int16_t value)
	{
		switch (value)
		{
		case 0: return ChannelID::Gray;
		case -1: return ChannelID::TransparencyMask;
		case -2: return ChannelID::UserSuppliedLayerMask;
		case -3: return ChannelID::RealUserSuppliedLayerMask;
		default: return ChannelID::Custom;	// These are channels set by the user
		}
	}


	// Blend mode information
	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------
	enum class BlendMode
	{
		Passthrough,
		Normal,
		Dissolve,
		Darken,
		Multiply,
		ColorBurn,
		LinearBurn,
		DarkerColor,
		Lighten,
		Screen,
		ColorDodge,
		LinearDodge,
		LighterColor,
		Overlay,
		SoftLight,
		HardLight,
		VividLight,
		LinearLight,
		PinLight,
		HardMix,
		Difference,
		Exclusion,
		Subtract,
		Divide,
		Hue,
		Saturation,
		Color,
		Luminosity
	};

	namespace {
		inline std::unordered_map<std::string, BlendMode> blendModeMap
		{
			{"pass", BlendMode::Passthrough},
			{"norm", BlendMode::Normal},
			{"diss", BlendMode::Dissolve},
			{"dark", BlendMode::Darken},
			{"mul ", BlendMode::Multiply},
			{"idiv", BlendMode::ColorBurn},
			{"lbrn", BlendMode::LinearBurn},
			{"dkCl", BlendMode::DarkerColor},
			{"lite", BlendMode::Lighten},
			{"scrn", BlendMode::Screen},
			{"div ", BlendMode::ColorDodge},
			{"lddg", BlendMode::LinearDodge},
			{"lgCl", BlendMode::LighterColor},
			{"over", BlendMode::Overlay},
			{"sLit", BlendMode::SoftLight},
			{"hLit", BlendMode::HardLight},
			{"vLit", BlendMode::VividLight},
			{"lLit", BlendMode::LinearLight},
			{"pLit", BlendMode::PinLight},
			{"hMix", BlendMode::HardMix},
			{"diff", BlendMode::Difference},
			{"smud", BlendMode::Exclusion},
			{"fsub", BlendMode::Subtract},
			{"fdiv", BlendMode::Divide},
			{"hue ", BlendMode::Hue},
			{"sat ", BlendMode::Saturation},
			{"colr", BlendMode::Color},
			{"lum ", BlendMode::Luminosity}
		};
	}

	// Bidirectional mapping of blend modes
	template<typename TKey, typename TValue>
	inline std::optional<TValue> getBlendMode(TKey key)
	{
		PSAPI_LOG_ERROR("getBlendMode", "No overload for the specific search type found")
	}

	template<>
	inline std::optional<BlendMode> getBlendMode(std::string key)
	{
		auto it = blendModeMap.find(key);

		if (it != blendModeMap.end()) {
			return std::optional<BlendMode>(it->second);
		}
		else {
			return std::nullopt;
		}
	}

	template <>
	inline std::optional<std::string> getBlendMode(BlendMode key)
	{
		return findByValue(blendModeMap, key);
	}


	// Tagged Block keys for Additional Layer information
	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------
	enum class TaggedBlockKey
	{
		Unknown,
		// Adjustment Layers, for now unimplemented
		adjSolidColor,
		adjGradient,
		adjPattern,
		adjBrightnessContrast,
		adjLevels,
		adjCurves,
		adjExposure,
		adjVibrance,
		adjOldHueSat,	// This should never be encountered
		adjNewHueSat,	// New referring to post photoshop 5.0 here (1998)
		adjColorBalance,
		adjBlackandWhite,
		adjPhotoFilter,
		adjChannelMixer,
		adjColorLookup,
		adjInvert,
		adjPosterize,
		adjThreshold,
		adjGradientMap,
		adjSelectiveColor,
		// Effects Layer, not planned
		// Gets its own prefix as it encompasses many different types of other layers
		fxLayer,
		// Tagged blocks with information about the layer (likely only present for the additional layer info
		// at the end of each layer record. Not the one at the end of the layer and mask section
		lrUnicodeName,
		lrId,
		lrSectionDivider,	// This stores information about if it is a group layer and if its open or closed
		lrMetaData,
		lrAnnotations,
		// Non-Pixel layers
		lrTypeTool,		// This is the superseeded version 'TySh', not 'tySh' as that was phased out in 2000
		lrPatternData,
		lrLinked,		// Probably Smart objects 
		lrLinked_8Byte,	// Same as lrLinked but for some reason 'lnk2' has a 8-byte wide lenght field
		// Additional layer specific data
		lrCompositorUsed,
		lrSavingMergedTransparency,	// Holds no data, just indicates channel Image data section includes transparency (needs to be tested)
		lrPixelSourceData,	// Data for 3d or video layers
		lrUserMask,
		// 16- and 32-bit files store their layer records under these tagged blocks at the end of the layer
		// and mask information section
		Lr16,
		Lr32,
		Layr,
		// Unknown 8-byte wide types
		Alph,
		lrFilterMask,
		lrFilterEffects,
		//
		lrBlendClippingElements,
		lrBlendInteriorElements,
		lrKnockoutSetting,
		lrProtectedSetting,
		lrSheetColorSetting,
		lrReferencePoint,

	};

	namespace {
		inline std::unordered_map<std::string, TaggedBlockKey> taggedBlockMap
		{
			{"SoCo", TaggedBlockKey::adjSolidColor},
			{"GdFl", TaggedBlockKey::adjGradient},
			{"PtFl", TaggedBlockKey::adjPattern},
			{"brit", TaggedBlockKey::adjBrightnessContrast},
			{"levl", TaggedBlockKey::adjLevels},
			{"curv", TaggedBlockKey::adjCurves},
			{"expA", TaggedBlockKey::adjExposure},
			{"vibA", TaggedBlockKey::adjVibrance},
			{"hue ", TaggedBlockKey::adjOldHueSat},
			{"hue2", TaggedBlockKey::adjNewHueSat},
			{"blnc", TaggedBlockKey::adjColorBalance},
			{"blwh", TaggedBlockKey::adjBlackandWhite},
			{"phfl", TaggedBlockKey::adjPhotoFilter},
			{"mixr", TaggedBlockKey::adjChannelMixer},
			{"clrL", TaggedBlockKey::adjColorLookup},
			{"nvrt", TaggedBlockKey::adjInvert},
			{"post", TaggedBlockKey::adjPosterize},
			{"thrs", TaggedBlockKey::adjThreshold},
			{"grdm", TaggedBlockKey::adjGradientMap},
			{"selc", TaggedBlockKey::adjSelectiveColor},
			{"lrFX", TaggedBlockKey::fxLayer},
			{"luni", TaggedBlockKey::lrUnicodeName},
			{"lyid", TaggedBlockKey::lrId},
			{"lsct", TaggedBlockKey::lrSectionDivider},
			{"shmd", TaggedBlockKey::lrMetaData},
			{"Anno", TaggedBlockKey::lrAnnotations},
			{"TySh", TaggedBlockKey::lrTypeTool},
			{"shpa", TaggedBlockKey::lrPatternData},
			{"lnkD", TaggedBlockKey::lrLinked},
			{"lnk3", TaggedBlockKey::lrLinked},
			{"lnk2", TaggedBlockKey::lrLinked_8Byte},
			{"cinf", TaggedBlockKey::lrCompositorUsed},
			{"Mtrn", TaggedBlockKey::lrSavingMergedTransparency},
			{"Mt16", TaggedBlockKey::lrSavingMergedTransparency},
			{"Mt32", TaggedBlockKey::lrSavingMergedTransparency},
			{"PxSD", TaggedBlockKey::lrPixelSourceData},
			{"LMsk", TaggedBlockKey::lrUserMask},
			{"Lr16", TaggedBlockKey::Lr16},
			{"Lr32", TaggedBlockKey::Lr32},
			{"Layr", TaggedBlockKey::Layr},
			{"Alph", TaggedBlockKey::Alph},
			{"FMsk", TaggedBlockKey::lrFilterMask},
			{"FXid", TaggedBlockKey::lrFilterEffects},
			{"FEid", TaggedBlockKey::lrFilterEffects},
			{"clbl", TaggedBlockKey::lrBlendClippingElements},
			{"infx", TaggedBlockKey::lrBlendInteriorElements},
			{"knko", TaggedBlockKey::lrKnockoutSetting},
			{"lspf", TaggedBlockKey::lrProtectedSetting},
			{"lclr", TaggedBlockKey::lrSheetColorSetting},
			{"fxrp", TaggedBlockKey::lrReferencePoint},
		};
	}

	// Bidirectional mapping of Tagged block keys
	template<typename TKey, typename TValue>
	inline std::optional<TValue> getTaggedBlockKey(TKey key)
	{
		PSAPI_LOG_ERROR("getTaggedBlockKey", "No overload for the specific search type found")
	}

	template<>
	inline std::optional<TaggedBlockKey> getTaggedBlockKey(std::string key)
	{
		auto it = taggedBlockMap.find(key);

		if (it != taggedBlockMap.end()) {
			return std::optional<TaggedBlockKey>(it->second);
		}
		else {
			return std::optional<TaggedBlockKey>(TaggedBlockKey::Unknown);
		}
	}

	// Sometimes multiple strings can match a singular key, therefore we return a vector here
	template <>
	inline std::optional<std::vector<std::string>> getTaggedBlockKey(TaggedBlockKey key)
	{
		return findMultipleByValue(taggedBlockMap, key);
	}

	inline bool constexpr isTaggedBlockSizeUint64(TaggedBlockKey key)
	{
		if (
			key == TaggedBlockKey::lrUserMask
			|| key == TaggedBlockKey::Lr16
			|| key == TaggedBlockKey::Lr32
			|| key == TaggedBlockKey::Layr
			|| key == TaggedBlockKey::lrSavingMergedTransparency
			|| key == TaggedBlockKey::Alph
			|| key == TaggedBlockKey::lrFilterMask
			|| key == TaggedBlockKey::lrFilterEffects
			|| key == TaggedBlockKey::lrLinked_8Byte
			|| key == TaggedBlockKey::lrPixelSourceData
			|| key == TaggedBlockKey::lrCompositorUsed
			)
		{
			return true;
		}
		return false;
	}
}


// Image Data enums
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
namespace Enum
{
	enum class Compression
	{
		Raw,
		Rle,			// Run-length encoded data using the PackBits algorithm
		Zip,			// Regular ZIP compression
		ZipPrediction	// ZIP compression where 
	};

	inline std::unordered_map<uint16_t, Compression> compressionMap
	{
		{static_cast<uint16_t>(0u), Compression::Raw},
		{static_cast<uint16_t>(1u), Compression::Rle},
		{static_cast<uint16_t>(2u), Compression::Zip},
		{static_cast<uint16_t>(3u), Compression::ZipPrediction}
	};
}

PSAPI_NAMESPACE_END