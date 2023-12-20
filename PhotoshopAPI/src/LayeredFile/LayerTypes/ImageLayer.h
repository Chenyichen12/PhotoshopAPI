#pragma once

#include "Macros.h"
#include "Enum.h"
#include "Layer.h"
#include "Struct/ImageChannel.h"
#include "PhotoshopFile/LayerAndMaskInformation.h"

#include <unordered_map>

PSAPI_NAMESPACE_BEGIN

// A pixel based image layer
template <typename T>
struct ImageLayer : public Layer<T>
{
	// Store the image data as a per-channel map to be used later
	std::unordered_map<Enum::ChannelIDInfo, ImageChannel<T>> m_ImageData;

	// Generate a photoshop layerRecord and imageData based on the current layer. if doCopy is set to false this will likely
	// invalidate both our m_ImageData as well as our m_LayerMask since we perform move operations on them. If doCopy is 
	// set to true we can safely keep using the ImageLayer instance. it is advised to only set doCopy to false on parsing of
	// the whole layeredFile -> PhotoshopFile.
	std::tuple<LayerRecord, ChannelImageData> toPhotoshop(const Enum::ColorMode colorMode, const bool doCopy);

	// Initialize our imageLayer by first parsing the base Layer instance and then moving
	// the additional channels into our representation
	ImageLayer(const LayerRecord& layerRecord, const ChannelImageData& channelImageData);
private:
	// Extracts the m_ImageData as well as the layer mask into two vectors holding channel information as well as the image data 
	// itself. This also takes care of generating our layer mask channel if it is present
	std::tuple<std::vector<LayerRecords::ChannelInformation>, ChannelImageData> extractImageData(const bool doCopy);
};

PSAPI_NAMESPACE_END