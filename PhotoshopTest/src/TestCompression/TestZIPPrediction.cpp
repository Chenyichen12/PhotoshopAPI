#include "doctest.h"

#include "Utility.h"
#include "Macros.h"
#include "Compression/RLE.h"
#include "PhotoshopFile/PhotoshopFile.h"

#include <vector>
#include <filesystem>


TEST_CASE("Decompress 16 bit file with ZIP Prediction compression")
{
	// We count all 4 variations of this as the same test case
	SUBCASE("PSD")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_16bit.psd";
		checkCompressionFile<uint16_t>(combined_path, 0, 32895, 65535);
	}
	SUBCASE("PSB")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_16bit.psb";
		checkCompressionFile<uint16_t>(combined_path, 0, 32895, 65535);
	}
	SUBCASE("MaximizeCompatibilityOff_PSD")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_MaximizeCompatibilityOff_16bit.psd";
		checkCompressionFile<uint16_t>(combined_path, 0, 32895, 65535);
	}
	SUBCASE("MaximizeCompatibilityOff_PSB")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_MaximizeCompatibilityOff_16bit.psb";
		checkCompressionFile<uint16_t>(combined_path, 0, 32895, 65535);
	}
}


TEST_CASE("Decompress 32 bit file with ZIP Prediction compression")
{
	// We count all 4 variations of this as the same test case
	SUBCASE("PSD")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_32bit.psd";
		checkCompressionFile<float32_t>(combined_path, 0.0f, 0.501953f, 1.0f);
	}
	SUBCASE("PSB")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_32bit.psb";
		checkCompressionFile<float32_t>(combined_path, 0.0f, 0.501953f, 1.0f);
	}
	SUBCASE("MaximizeCompatibilityOff_PSD")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_MaximizeCompatibilityOff_32bit.psd";
		checkCompressionFile<float32_t>(combined_path, 0.0f, 0.501953f, 1.0f);
	}
	SUBCASE("MaximizeCompatibilityOff_PSB")
	{
		std::filesystem::path combined_path = std::filesystem::current_path();
		combined_path += "\\documents\\Compression\\Compression_ZipPrediction_MaximizeCompatibilityOff_32bit.psb";
		checkCompressionFile<float32_t>(combined_path, 0.0f, 0.501953f, 1.0f);
	}
}