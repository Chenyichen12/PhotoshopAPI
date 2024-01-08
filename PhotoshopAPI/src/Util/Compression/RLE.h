#pragma once

#include "Macros.h"
#include "FileIO/Read.h"
#include "FileIO/Util.h"
#include "Logger.h"
#include "Endian/EndianByteSwap.h"
#include "Endian/EndianByteSwapArr.h"
#include "Struct/File.h"
#include "Struct/ByteStream.h"
#include "PhotoshopFile/FileHeader.h"
#include "Profiling/Perf/Instrumentor.h"

#include <vector>
#include <limits>

#include <inttypes.h>


PSAPI_NAMESPACE_BEGIN

// This is the packbits algorithm described here: https://en.wikipedia.org/wiki/PackBits we iterate byte by byte and decompress
// a singular scanline at a time
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<uint8_t> DecompressPackBits(const std::vector<uint8_t>& compressedData, const uint32_t width, const uint32_t height)
{
    PROFILE_FUNCTION();
    std::vector<uint8_t> decompressedData;
    decompressedData.reserve((sizeof(T) * static_cast<uint64_t>(width) * static_cast<uint64_t>(height)));

    uint64_t i = 0;
    while (i < compressedData.size()) {
        uint8_t value = compressedData[i];

        if (value == 128) 
        {
            // Do nothing, nop. Equivalent to 0 in int8_t
        }
        else if (value > 128) 
        {
            // Repeat the next byte after this n times
            value = 256 - value;
            for (int j = 0; j <= value; ++j)
            {
                decompressedData.push_back(compressedData.at(i + 1));
            }
            ++i;
        }
        else 
        {
            // Header byte indicates the next n bytes are to be read as values
            for (int j = 0; j <= value; ++j)
            {
                decompressedData.push_back(compressedData.at(i + j + 1));
            }
            i += static_cast<uint64_t>(value) + 1;
        }
        ++i;
    }

    return decompressedData;
}



// This is the packbits algorithm described here: https://en.wikipedia.org/wiki/PackBits we iterate byte by byte and 
// compress. The logic is heavily adapted from MolecularMatters and credit goes to them:
// https://github.com/MolecularMatters/psd_sdk/blob/master/src/Psd/PsdDecompressRle.cpp
// We assume a compression of a single scanline as they are all independant of each other
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
inline std::vector<uint8_t> CompressPackBits(const std::span<uint8_t> uncompressedScanline, uint32_t& scanlineSize)
{
    PROFILE_FUNCTION();
    // We assume a ~4x compression ratio for RLE to avoid continuously reserving more size
    std::vector<uint8_t> compressedData;
    compressedData.reserve(uncompressedScanline.size() / 4);

    // Keep track of how long our run / no run is
    uint8_t runLen = 0u;
    uint8_t nonRunLen = 0u;

	for (int i = 1; i < uncompressedScanline.size(); ++i)
	{
		const uint8_t prev = uncompressedScanline[i - 1];
		const uint8_t curr = uncompressedScanline[i];

        // We have a run of at least 2 bytes
        if (prev == curr)
        {
            if (nonRunLen != 0)
            {
                // This is the first repeat of an item so we set the non run length to 0 and copy over the 
                // all the non-run length bytes we have accumulated

                // Write the amount of bytes that are in our non-run length. Note that we subtract one as the curr 
                // value is part of a run
                compressedData.push_back(nonRunLen - 1u);
                for (int j = 0; j < nonRunLen - 1u; ++i)
                {
                    compressedData.push_back(uncompressedScanline[i - nonRunLen + j]);
                }
                nonRunLen = 0;
            }

            ++runLen;

            // runs cant be any longer than this due to the way that they are encoded so we are forced to terminate here
            if (runLen == 128u)
            {
                compressedData.push_back(static_cast<uint8_t>(257u) - runLen);
                compressedData.push_back(prev);
                runLen = 0u;
            }
        }
        else
        {
            // End the run if there is one going on
            if (runLen != 0)
            {
                compressedData.push_back(static_cast<uint8_t>(256u) - runLen);
                compressedData.push_back(prev);
                runLen = 0u;
            }
            else
            {
                ++nonRunLen;
            }

            // Same as the termination condition on run lengths
            if (nonRunLen == 128u)
            {
				compressedData.push_back(nonRunLen - 1u);
				for (int j = 0; j < nonRunLen - 1u; ++i)
				{
					compressedData.push_back(uncompressedScanline[i - nonRunLen + j]);
				}
				nonRunLen = 0;
            }
        }
	}

    // After having iterated all the items we must now encode the last item
    if (runLen != 0)
    {
        ++runLen;
        // Push back the last element as the run
        compressedData.push_back(static_cast<uint8_t>(257u) - runLen);
        compressedData.push_back(uncompressedScanline[uncompressedScanline.size() - 1u]);
    }
    else
    {
        ++nonRunLen;
		compressedData.push_back(nonRunLen - 1u);
		for (int j = 0; j < nonRunLen - 1u; ++j)
		{
            compressedData.push_back(uncompressedScanline[uncompressedScanline.size() - nonRunLen + j]);
		}
		nonRunLen = 0;
    }

    // The section is padded to 2 bytes, if we need to insert a padding byte we use the no-op
    // value of 128
    if (compressedData.size() % 2 != 0)
    {
        compressedData.push_back(128u);
    }

    // Store and return
    scanlineSize = compressedData.size();
    return compressedData;
}


// Reads and decompresses a single channel using the packbits algorithm
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<T> DecompressRLE(ByteStream& stream, uint64_t offset, const FileHeader& header, const uint32_t width, const uint32_t height, const uint64_t compressedSize)
{
    PROFILE_FUNCTION();
	// Photoshop first stores the byte counts of all the scanlines, this is 2 or 4 bytes depending on 
	// if the document is PSD or PSB
	uint64_t scanlineTotalSize = 0u;
    if (header.m_Version == Enum::Version::Psd)
    {
        std::vector<uint16_t> buff(height);
        stream.setOffsetAndRead(reinterpret_cast<char*>(buff.data()), offset, height * sizeof(uint16_t));
        endianDecodeBEArray<uint16_t>(buff);
        for (auto item : buff)
        {
            scanlineTotalSize += item;
        }
    }
    else
    {
        std::vector<uint32_t> buff(height);
        stream.setOffsetAndRead(reinterpret_cast<char*>(buff.data()), offset, height * sizeof(uint32_t));
        endianDecodeBEArray<uint32_t>(buff);
        for (auto item : buff)
        {
            scanlineTotalSize += item;
        }
    }

    // Find out the size of the data without the scanline sizes. For example, if the document is 64x64 pixels in 8 bit mode we have 128 bytes of memory to store the scanline size
    uint64_t dataSize = compressedSize - static_cast<uint64_t>(SwapPsdPsb<uint16_t, uint32_t>(header.m_Version)) * height;

    if (scanlineTotalSize != dataSize)
    {
        PSAPI_LOG_ERROR("DecompressRLE", "Size of compressed data is not what was expected. Expected: %" PRIu64 " but got %" PRIu64 " instead",
            dataSize,
            scanlineTotalSize)
    }

	// Read the data without converting from BE to native as we need to decompress first
	std::vector<uint8_t> compressedData(scanlineTotalSize);
    stream.setOffsetAndRead(reinterpret_cast<char*>(compressedData.data()), offset + SwapPsdPsb<uint16_t, uint32_t>(header.m_Version) * height, scanlineTotalSize);

	// Decompress using the PackBits algorithm
    std::vector<uint8_t> decompressedData = DecompressPackBits<T>(compressedData, width, height);

    // Convert decompressed data to native endianness
    std::vector<T> bitShiftedData = endianDecodeBEBinaryArray<T>(decompressedData);


    if (bitShiftedData.size() != static_cast<uint64_t>(width) * static_cast<uint64_t>(height))
    {
        PSAPI_LOG_ERROR("DecompressRLE", "Size of decompressed data is not what was expected. Expected: %" PRIu64 " but got %" PRIu64 " instead", 
            static_cast<uint64_t>(width) * static_cast<uint64_t>(height),
            bitShiftedData.size())
    }
	
	return bitShiftedData;
}


// Compresses a single channel using the packbits algorithm into a binary array as well as big endian encoding it. Returns a binary vector of data
// with the size of each scanline as either a 2- or 4-byte unsigned int preceding it
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<uint8_t> CompressRLE(std::vector<T>& uncompressedData, const FileHeader& header, const uint32_t width, const uint32_t height)
{
    endianEncodeBEArray(uncompressedData);

    std::vector<std::span<uint8_t>> uncompressedDataViews;
    for (int i = 0; i < height; ++i)
    {
        // Generate a span for each scanline
        std::span<uint8_t> data(reinterpret_cast<uint8_t*>(uncompressedData.data() + width * i), width * sizeof(T));
        uncompressedDataViews.push_back(data);
    }

    // Allocate the size required for all the scanline sizes ahead of time
    std::vector<uint8_t> compressedData(SwapPsdPsb<uint16_t, uint32_t>(header.m_Version) * height, 0u);
    // Compress each scanline of the uncompressed data individually and push it into the compressed data
    // While also filling out the scanlineSizes vector
    for (int i = 0; i < uncompressedDataViews.size(); ++i)
    {
        uint32_t scanlineSize = 0u;
        std::vector<uint8_t> data = CompressPackBits(uncompressedDataViews[i], scanlineSize);

        // Insert the scanline size at the start of the data in our pre-allocated buffer.
        // For PSD we must shrink the value to uint16_t
		const size_t scanlineIndex = i * SwapPsdPsb<uint16_t, uint32_t>(header.m_Version);
        if (header.m_Version == Enum::Version::Psd)
        {
            if (scanlineSize > (std::numeric_limits<uint16_t>::max)()) [[unlikely]]
			{
				PSAPI_LOG_ERROR("CompressRLE", "Scanline sizes cannot exceed the numeric limits of 16-bit values when writing a PSD file")
			}
            uint16_t scanlineSizeu16 = static_cast<uint16_t>(scanlineSize);
            scanlineSizeu16 = endianEncodeBE(scanlineSizeu16);
            // Set the data at the correct index
            std::memcpy(reinterpret_cast<void*>(compressedData.data() + scanlineIndex), &scanlineSizeu16, sizeof(uint16_t));
        }
        else
        {
            scanlineSize = endianEncodeBE(scanlineSize);
			std::memcpy(reinterpret_cast<void*>(compressedData.data() + scanlineIndex), &scanlineSize, sizeof(uint32_t));
        }

        // Since our compressed data has the scanline sizes preallocated we can just insert at the end and that will be correct
        compressedData.insert(std::end(compressedData), std::begin(data), std::end(data));
    }

    
    return compressedData;
}

PSAPI_NAMESPACE_END