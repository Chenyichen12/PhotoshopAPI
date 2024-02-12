/*
Example of creating a simple document with a single layer and a mask using the PhotoshopAPI. The preferred method is always to interact through the LayeredFile
struct rather than directly with the low level PhotoshopAPI::PhotoshopFile although that is also possible but requires a deep understanding of the
underlying structure whereas LayeredFile abstracts the implementation details.
*/

#include "PhotoshopAPI.h"

#include <unordered_map>
#include <vector>


int main()
{
	// Initialize some constants that we will need throughout the program
	const static uint32_t width = 64u;
	const static uint32_t height = 64u;

	// Create an 8-bit LayeredFile as our starting point, 8- 16- and 32-bit are fully supported
	PhotoshopAPI::LayeredFile<PhotoshopAPI::bpp8_t> document = { PhotoshopAPI::Enum::ColorMode::RGB, width, height };
	// Create our individual channels to add to our image layer. Keep in mind that all these 3 channels need to 
	// be specified for RGB mode
	std::unordered_map <PhotoshopAPI::Enum::ChannelID, std::vector<PhotoshopAPI::bpp8_t>> channelMap;
	channelMap[PhotoshopAPI::Enum::ChannelID::Red] = std::vector<PhotoshopAPI::bpp8_t>(width * height, 255u);
	channelMap[PhotoshopAPI::Enum::ChannelID::Green] = std::vector<PhotoshopAPI::bpp8_t>(width * height, 0u);
	channelMap[PhotoshopAPI::Enum::ChannelID::Blue] = std::vector<PhotoshopAPI::bpp8_t>(width * height, 0u);

	// Create a mask channel which for now is just a semi grey channel. This channel for the time being
	// needs to be the exact same size as the layer even though Photoshop officially supports masks being smaller
	// or larger than channels
	auto maskchannel = std::vector<PhotoshopAPI::bpp8_t>(width * height, 128u);

	PhotoshopAPI::ImageLayer<PhotoshopAPI::bpp8_t>::Params layerParams = {};
	layerParams.layerName = "Layer Red";
	layerParams.width = width;
	layerParams.height = height;
	layerParams.layerMask = maskchannel;

	auto layer = std::make_shared<PhotoshopAPI::ImageLayer<PhotoshopAPI::bpp8_t>>(
		std::move(channelMap),
		layerParams
	);

	document.addLayer(layer);

	// Convert to PhotoshopDocument and write to disk. Note that from this point onwards 
	// our LayeredFile instance is no longer usable
	PhotoshopAPI::File::FileParams params = { .doRead = false, .forceOverwrite = true };
	auto outputFile = PhotoshopAPI::File("./WriteLayerMask.psd", params);
	auto psdDocumentPtr = PhotoshopAPI::LayeredToPhotoshopFile(std::move(document));
	psdDocumentPtr->write(outputFile);
}