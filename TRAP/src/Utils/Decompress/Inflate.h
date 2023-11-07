/*
LodePNG version 20230410

Copyright (c) 2005-2023 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.

Modified by Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_INFLATE_H
#define TRAP_INFLATE_H

#include <array>
#include <cstdint>
#include <vector>

#include "Maths/Math.h"

namespace TRAP::Utils::Decompress
{
	namespace INTERNAL
	{
		struct BitReader
		{
			/// @brief Constructor.
			/// BitReader reads byte data bit by bit.
			/// @param data Data to read in bytes.
			constexpr explicit BitReader(std::span<const u8> data);

			std::span<const u8> Data;
			usize BitSize{}; // Size of data in bits, end of valid BP values, should be 8 * size
			usize BP{};
			u32 Buffer{}; //Buffer for reading bits.
			bool Error = false;

			/// @brief Ensure the reader can at least read 9 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			constexpr void EnsureBits9();
			/// @brief Ensure the reader can at least read 17 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			constexpr void EnsureBits17();
			/// @brief Ensure the reader can at least read 25 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			constexpr void EnsureBits25();
			/// @brief Ensure the reader can at least read 32 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			constexpr void EnsureBits32();
			/// @brief Read n amount of bits.
			/// @param nBits How many bits.
			/// @return N bits read.
			/// @note Must have enough bits available with EnsureBits.
			[[nodiscard]] constexpr u32 ReadBits(usize nBits);
			/// @brief Safely check if a + b > c, even if overflow could happen.
			/// @return True if no overflow will happen, false otherwise.
			[[nodiscard]] static constexpr bool GreaterOverflow(usize a, usize b, usize c);
			/// @brief Get bits without advancing the bit pointer.
			/// @param nBits How many bits up to 31.
			/// @return N bits read.
			/// @note Must have enough bits available with EnsureBits.
			[[nodiscard]] constexpr u32 PeekBits(usize nBits) const noexcept;
			/// @brief Advance n amount of bits in the reader.
			/// @param nBits How many bits.
			/// @note Must have enough bits available with EnsureBits.
			void constexpr AdvanceBits(usize nBits) noexcept;

		private:
			/// @brief Safely check if multiplying two integers will overflow(no undefined behavior,
			/// compiler removing the code, etc...) and output result.
			/// @param result Output variable for the result of the multiplication.
			/// @param a First integer.
			/// @param b Second integer.
			/// @return True if no overflow happens, false otherwise.
			[[nodiscard]] static constexpr bool MultiplyOverflow(usize a, usize b, usize& result) noexcept;
			/// @brief Safely check if adding two integers will overflow(no undefined behavior,
			/// compiler removing the code, etc...) and output result.
			/// @param a First integer.
			/// @param b Second integer.
			/// @param result Output variable for the result of the sum.
			/// @return True if no overflow happens, false otherwise.
			[[nodiscard]] static constexpr bool AddOverflow(usize a, usize b, usize& result) noexcept;
		};

		/// @brief Huffman tree struct, containing multiple representations of the tree
		struct HuffmanTree
		{
			/// @brief Constructor.
			constexpr HuffmanTree() noexcept = default;

			std::vector<u32> Codes; //The Huffman codes(bit patterns representing the symbols)
			std::vector<u32> Lengths; //The lengths of the huffman codes
			u32 MaxBitLength{}; //Maximum number of bits a single code can get
			u32 NumCodes{}; //Number of symbols in the alphabet = number of codes

			/// @brief Get the tree of a deflated block with fixed tree, as specified in the deflate specification.
			/// @param treeLL Literal length huffman tree.
			/// @param treeD Distance huffman tree.
			/// @return True on success, false otherwise.
			[[nodiscard]] static constexpr bool GetTreeInflateFixed(HuffmanTree& treeLL, HuffmanTree& treeD);
			/// @brief Get the tree of a deflated block with dynamic tree, the tree itself is also Huffman
			/// compressed with a known tree.
			/// @param treeLL Literal length huffman tree.
			/// @param treeD Distance huffman tree.
			/// @param reader BitReader.
			/// @return True on success, false otherwise.
			[[nodiscard]] static constexpr bool GetTreeInflateDynamic(HuffmanTree& treeLL, HuffmanTree& treeD, BitReader& reader);

			/// @brief The bit reader must already have been ensured at least 15 bits.
			/// @param reader BitReader to decode symbol from.
			/// @return Code.
			[[nodiscard]] constexpr u32 DecodeSymbol(BitReader& reader) const;

			//The base lengths represented by codes 257-285
			static constexpr std::array<u32, 29> LengthBase
			{
				3, 4, 5, 6,
				7, 8, 9, 10,
				11, 13, 15, 17,
				19, 23, 27, 31,
				35, 43, 51, 59,
				67, 83, 99, 115,
				131, 163, 195, 227,
				258
			};
			//The Extra bits used by codes 257-285(added to base length)
			static constexpr std::array<u32, 29> LengthExtra
			{
				0, 0, 0, 0,
				0, 0, 0, 0,
				1, 1, 1, 1,
				2, 2, 2, 2,
				3, 3, 3, 3,
				4, 4, 4, 4,
				5, 5, 5, 5,
				0
			};
			//The base backwards distances(the bits of distance codes appear after length codes
			//and use their own Huffman Tree)
			static constexpr std::array<u32, 30> DistanceBase
			{
				1, 2, 3, 4,
				5, 7, 9, 13,
				17, 25, 33, 49,
				65, 97, 129, 193,
				257, 385, 513, 769,
				1025, 1537, 2049, 3073,
				4097, 6145, 8193, 12289,
				16385, 24577
			};
			//The extra bits of backwards distances(added to base)
			static constexpr std::array<u32, 30> DistanceExtra
			{
				0, 0, 0, 0,
				1, 1, 2, 2,
				3, 3, 4, 4,
				5, 5, 6, 6,
				7, 7, 8, 8,
				9, 9, 10, 10,
				11, 11, 12, 12,
				13, 13
			};

		private:
			std::vector<u8> TableLength; //Length of symbol from lookup table, or max length if secondary lookup needed
			std::vector<u16> TableValue; //Value of symbol from lookup table, or pointer to secondary table if needed

			/// @brief Get the literal and length code tree of a deflated block with fixed tree,
			/// as per the deflate specification.
			/// @return True on success, false otherwise.
			[[nodiscard]] constexpr bool GenerateFixedLiteralLengthTree();

			/// @brief Get the distance code tree of a deflated block with fixed tree,
			/// as specified in the deflate specification.
			/// @return True on success, false otherwise.
			[[nodiscard]] constexpr bool GenerateFixedDistanceTree();

			/// @brief Given the code lengths(as stored in the PNG file), generate the tree as defined by Deflate.
			/// @param bitLength Code length in bits.
			/// @param numCodes Amount of codes.
			/// @param maxBitLength Maximum bits that a code in the tree can have.
			/// @return True on success, false otherwise.
			[[nodiscard]] constexpr bool MakeFromLengths(const u32* bitLength, usize numCodes, u32 maxBitLength);

			/// @brief Second step for the ...MakeFromLengths and ...MakeFromFrequencies functions.
			/// numCodes, lengths and maxBitLength must already be filled in correctly.
			/// @return True on success, false otherwise.
			[[nodiscard]] constexpr bool MakeFromLengths2();

			/// @brief Make table for huffman decoding.
			/// @return True on success, false otherwise.
			[[nodiscard]] constexpr bool MakeTable();
			/// @brief Reverse bits.
			[[nodiscard]] static constexpr u32 ReverseBits(u32 bits, u32 num) noexcept;

			static constexpr u16 NumDeflateCodeSymbols = 288;
			static constexpr u8 NumDistanceSymbols = 32;
			static constexpr u8 NumCodeLengthCodes = 19;
			static constexpr u8 FirstBits = 9u;
			static constexpr u16 InvalidSymbol = 65535u;
			//The order in which "code length alphabet code lengths" are stored, out of this the
			//Huffman Tree of the dynamic Huffman Tree lengths is generated
			static constexpr std::array<u32, 19> CLCLOrder
			{
				16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
			};
		};

		static constexpr u16 FirstLengthCodeIndex = 257;
		static constexpr u16 LastLengthCodeIndex = 285;

		[[nodiscard]] constexpr bool InflateNoCompression(std::span<u8> out, usize& pos, BitReader& reader);
		[[nodiscard]] constexpr bool InflateHuffmanBlock(std::span<u8> out, usize& pos, BitReader& reader,
		                                                 u32 btype);
	}

	/// @brief Inflate algorithm.
	/// @param source Source data in bytes.
	/// @param destination Destination where to put inflated data to.
	/// @return True on success, false otherwise.
	[[nodiscard]] constexpr bool Inflate(std::span<const u8> source, std::span<u8> destination);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Utils::Decompress::INTERNAL::BitReader::BitReader(const std::span<const u8> data)
	: Data(data)
{
	usize temp = 0;
	if (MultiplyOverflow(data.size_bytes(), 8u, BitSize))
		Error = true;
	if (AddOverflow(BitSize, 64u, temp))
		Error = true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits9()
{
	const usize start = BP >> 3u;
	const usize size = Data.size_bytes();
	if (start + 1u < size)
	{
		Buffer = static_cast<u32>(Data[start + 0]) | (static_cast<u32>(Data[start + 1] << 8u));
		Buffer >>= (BP & 7u);
	}
	else
	{
		Buffer = 0;
		if (start + 0u < size)
			Buffer = Data[start + 0];
		Buffer >>= (BP & 7u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits17()
{
	const usize start = BP >> 3u;
	const usize size = Data.size_bytes();
	if (start + 2u < size)
	{
		Buffer = static_cast<u32>(Data[start + 0]) | (static_cast<u32>(Data[start + 1]) << 8u) |
		         (static_cast<u32>(Data[start + 2]) << 16u);
		Buffer >>= (BP & 7u);
	}
	else
	{
		Buffer = 0;
		if (start + 0u < size)
			Buffer |= Data[start + 0];
		if (start + 1u < size)
			Buffer |= (static_cast<u32>(Data[start + 1]) << 8u);
		Buffer >>= (BP & 7u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits25()
{
	const usize start = BP >> 3u;
	const usize size = Data.size_bytes();
	if (start + 3u < size)
	{
		Buffer = static_cast<u32>(Data[start + 0]) | (static_cast<u32>(Data[start + 1]) << 8u) |
		         (static_cast<u32>(Data[start + 2]) << 16u) | (static_cast<u32>(Data[start + 3]) << 24u);
		Buffer >>= (BP & 7u);
	}
	else
	{
		Buffer = 0;
		if (start + 0u < size)
			Buffer |= Data[start + 0];
		if (start + 1u < size)
			Buffer |= (static_cast<u32>(Data[start + 1]) << 8u);
		if (start + 2u < size)
			Buffer |= (static_cast<u32>(Data[start + 2]) << 16u);
		Buffer >>= (BP & 7u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits32()
{
	const usize start = BP >> 3u;
	const usize size = Data.size_bytes();
	if (start + 4u < size)
	{
		Buffer = static_cast<u32>(Data[start + 0]) | (static_cast<u32>(Data[start + 1]) << 8u) |
		         (static_cast<u32>(Data[start + 2]) << 16u) | (static_cast<u32>(Data[start + 3]) << 24u);
		Buffer >>= (BP & 7u);
		Buffer |= ((static_cast<u32>(Data[start + 4]) << 24u) << (8u - (BP & 7u)));
	}
	else
	{
		Buffer = 0;
		if (start + 0u < size)
			Buffer |= Data[start + 0];
		if (start + 1u < size)
			Buffer |= (static_cast<u32>(Data[start + 1]) << 8u);
		if (start + 2u < size)
			Buffer |= (static_cast<u32>(Data[start + 2]) << 16u);
		if (start + 3u < size)
			Buffer |= (static_cast<u32>(Data[start + 3]) << 24u);
		Buffer >>= (BP & 7u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Must have enough bits available with EnsureBits
[[nodiscard]] constexpr u32 TRAP::Utils::Decompress::INTERNAL::BitReader::ReadBits(const usize nBits)
{
	const u32 result = PeekBits(nBits);
	AdvanceBits(nBits);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::BitReader::GreaterOverflow(const usize a, const usize b,
                                                                                           const usize c)
{
	usize d = 0;
	if (AddOverflow(a, b, d))
		return true;

	return d > c;
}

//-------------------------------------------------------------------------------------------------------------------//

//Get bits without advancing the bit pointer. Must have enough bits available with EnsureBits
[[nodiscard]] constexpr u32 TRAP::Utils::Decompress::INTERNAL::BitReader::PeekBits(const usize nBits) const noexcept
{
	return Buffer & ((1u << nBits) - 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

//Must have enough bits available with EnsureBits
constexpr void TRAP::Utils::Decompress::INTERNAL::BitReader::AdvanceBits(const usize nBits) noexcept
{
	Buffer >>= nBits;
	BP += nBits;
}

//-------------------------------------------------------------------------------------------------------------------//

//Safely check if multiplying two integers will overflow(no undefined behavior, compiler removing the code, etc...)
//and output result
[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::BitReader::MultiplyOverflow(const usize a, const usize b,
                                                                                            usize& result) noexcept
{
	result = a * b; //Unsigned multiplication is well defined and safe

	return (a != 0 && result / a != b);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::BitReader::AddOverflow(const usize a, const usize b,
                                                                                       usize& result) noexcept
{
	result = a + b; //Unsigned addition is well defined and safe

	return result < a;
}

//-------------------------------------------------------------------------------------------------------------------//

//Get the tree of a deflated block with fixed tree, as specified in the deflate specification
[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GetTreeInflateFixed(HuffmanTree& treeLL, HuffmanTree& treeD)
{
	if (!treeLL.GenerateFixedLiteralLengthTree())
		return false;

	if (!treeD.GenerateFixedDistanceTree())
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree
[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GetTreeInflateDynamic(HuffmanTree& treeLL, HuffmanTree& treeD,
                                                                                                   BitReader& reader)
{
	//Make sure that length values that arent filled in will be 0, or a wrong tree will be generated
	u32 n = 0, i = 0;

	//See comments in deflateDynamic for explanation of the context and these variables, it is analogous
	HuffmanTree treeCL; //The code tree for code length codes(the Huffman Tree for compressed Huffman Trees)

	if (reader.BitSize - reader.BP < 14)
		return false; //Error: The bit pointer is or will go past memory
	reader.EnsureBits17();

	//Number of literal/length codes + 257.
	//Unlike the spec, the value 257 is added to it here already
	const u32 HLIT = reader.ReadBits(5) + 257;
	//Number of distance codes.
	//Unlike the spec, the value 1 is added to it here already
	const u32 HDIST = reader.ReadBits(5) + 1;
	//Number of code length codes.
	//Unlike the spec, the value 4 is added to it here already
	const u32 HCLEN = reader.ReadBits(4) + 4;

	//Code length code lengths ("clcl"), the bit lengths of the Huffman Tree used to compress
	//bitLengthLL and bitLengthD
	std::array<u32, NumCodeLengthCodes> bitLengthCL{};

	bool error = false;
	while(!error)
	{
		//Read the code length codes out of 3 * (amount of code length codes) bits
		if (BitReader::GreaterOverflow(reader.BP, static_cast<usize>(HCLEN) * 3, reader.BitSize))
		{
			error = true; //Error: the bit pointer is or will go past the memory
			break;
		}
		for(i = 0; i != HCLEN; ++i)
		{
			reader.EnsureBits9(); //Out of bounds already checked above
			bitLengthCL[CLCLOrder[i]] = reader.ReadBits(3);
		}
		for (i = HCLEN; i != NumCodeLengthCodes; ++i)
			bitLengthCL[CLCLOrder[i]] = 0;

		error = !treeCL.MakeFromLengths(bitLengthCL.data(), NumCodeLengthCodes, 7);
		if(error)
			break;

		//Now we can use this tree to read the lengths for the tree that this function will return
		std::array<u32, NumDeflateCodeSymbols> bitLengthLL{};
		std::array<u32, NumDistanceSymbols> bitLengthD{};

		//i is the current symbol we are reading in the part that contains the code lengths of
		//literal/length and distance codes
		i = 0;
		while(i < HLIT + HDIST)
		{
			reader.EnsureBits25(); //Up to 15bits for Huffman code, up to 7 extra bits below
			const u32 code = treeCL.DecodeSymbol(reader);
			if(code <= 15) //A length code
			{
				if (i < HLIT)
					bitLengthLL[i] = code;
				else
					bitLengthD[i - HLIT] = code;
				++i;
			}
			else if(code == 16) //Repeat previous
			{
				u32 repeatLength = 3; //Read in the 2 bits that indicate repeat length (3-6)
				u32 value = 0; //Set value to the previous code

				if(i == 0)
				{
					error = true; //Cant repeat previous if i is 0
					break;
				}

				repeatLength += reader.ReadBits(2);

				if (i < HLIT + 1)
					value = bitLengthLL[i - 1];
				else
					value = bitLengthD[i - HLIT - 1];
				//Repeat this value in the next lengths
				for(n = 0; n < repeatLength; ++n)
				{
					if(i >= HLIT + HDIST)
					{
						error = true; //Error: i is larger than the amount of codes
						break;
					}

					if (i < HLIT)
						bitLengthLL[i] = value;
					else
						bitLengthD[i - HLIT] = value;
					++i;
				}
			}
			else if(code == 17) //Repeat "0" 3-10 times
			{
				u32 repeatLength = 3; //Read in the bits that indicate repeat length
				repeatLength += reader.ReadBits(3);

				//Repeat this value in the next lengths
				for(n = 0; n < repeatLength; ++n)
				{
					if(i >= HLIT + HDIST)
					{
						error = true;
						break; //Error: i is larger than the amount of codes
					}

					if (i < HLIT)
						bitLengthLL[i] = 0;
					else
						bitLengthD[i - HLIT] = 0;
					++i;
				}
			}
			else if(code == 18) //Repeat "0" 11-138 times
			{
				u32 repeatLength = 11; //Read in the bits that indicate repeat length
				repeatLength += reader.ReadBits(7);

				//Repeat this value in the next lengths
				for(n = 0; n < repeatLength; ++n)
				{
					if(i >= HLIT + HDIST)
					{
						error = true; //Error: i is larger than the amount of codes
						break;
					}

					if (i < HLIT)
						bitLengthLL[i] = 0;
					else
						bitLengthD[i - HLIT] = 0;
					++i;
				}
			}
			else //if(code == 65535u)
			{
				error = true; //Error: Tried to read disallowed Huffman symbol
				break;
			}
			//Check if any of the EnsureBits above went out of bounds
			if(reader.BP > reader.BitSize)
			{
				error = true; //Error: Bit pointer jumps past memory
				break;
			}
		}
		if (error)
			break;

		if(std::get<256>(bitLengthLL) == 0)
		{
			error = true; //Error: The length of the end code 256 must be larger than 0
			break;
		}

		//Now we have finally got HLIT and HDIST, so generate the code trees, and the function is done
		error = !treeLL.MakeFromLengths(bitLengthLL.data(), NumDeflateCodeSymbols, 15);
		if (error)
			break;
		error = !treeD.MakeFromLengths(bitLengthD.data(), NumDistanceSymbols, 15);

		break; //End of error-while
	}

	return !error;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the code. The bit reader must already have been ensured at least 15bits
[[nodiscard]] constexpr u32 TRAP::Utils::Decompress::INTERNAL::HuffmanTree::DecodeSymbol(BitReader& reader) const
{
	const u16 code = static_cast<u16>(reader.PeekBits(FirstBits));
	const u16 l = TableLength[code];
	u16 value = TableValue[code];
	if(l <= FirstBits)
	{
		reader.AdvanceBits(l);

		return value;
	}

	reader.AdvanceBits(FirstBits);
	value += static_cast<u16>(reader.PeekBits(l - FirstBits));
	reader.AdvanceBits(TableLength[value] - FirstBits);

	return TableValue[value];
}

//-------------------------------------------------------------------------------------------------------------------//

//Get the literal and length code of a deflated block with fixed tree, as per the deflate specification
[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GenerateFixedLiteralLengthTree()
{
	u32 i = 0;
	//256 literals, the end code, some length codes, and 2 unused codes
	std::array<u32, NumDeflateCodeSymbols> bitLength{};

	//288 possible codes: 0-255 = Literals, 256 = EndCode, 257-285 = LengthCodes, 286-287 = Unused
	for (i = 0; i <= 143; ++i)
		bitLength[i] = 8;
	for (i = 144; i <= 255; ++i)
		bitLength[i] = 9;
	for (i = 256; i <= 279; ++i)
		bitLength[i] = 7;
	for (i = 280; i <= 287; ++i)
		bitLength[i] = 8;

	//256 literals, the end code, some length codes, and 2 unused codes

	return MakeFromLengths(bitLength.data(), NumDeflateCodeSymbols, 15);
}

//-------------------------------------------------------------------------------------------------------------------//

//Get the distance code tree of a deflated block with fixed tree, as specified in the deflate specification
[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GenerateFixedDistanceTree()
{
	//The distance codes have their own symbols, 30 used, 2 unused
	std::array<u32, NumDistanceSymbols> bitLength{};

	//There are 32 distance codes, but 30-31 are unused
	for (u32 i = 0; i != NumDistanceSymbols; ++i)
		bitLength[i] = 5;

	return MakeFromLengths(bitLength.data(), NumDistanceSymbols, 15);
}

//-------------------------------------------------------------------------------------------------------------------//

//Given the code lengths(as stored in the PNG file), generate the tree as defined by Deflate.
//MaxBitLength is the maximum bits that a code in the tree can have.
[[nodiscard]] constexpr  bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::MakeFromLengths(const u32* const bitLength,
                                                                                              const usize numCodes,
																	                          const u32 maxBitLength)
{
	Lengths.resize(numCodes);
	for (u32 i = 0; i != numCodes; ++i)
		Lengths[i] = bitLength[i];
	NumCodes = static_cast<u32>(numCodes); //Number of symbols
	MaxBitLength = maxBitLength;

	return MakeFromLengths2();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::MakeFromLengths2()
{
	u32 bits = 0;

	Codes.resize(NumCodes);
	std::vector<u32> bitLengthCount((MaxBitLength + 1), 0);
	std::vector<u32> nextCode((MaxBitLength + 1), 0);

	//Step 1: Count number of instances of each code length
	for (bits = 0; bits != NumCodes; ++bits)
		++bitLengthCount[Lengths[bits]];
	//Step 2: Generate the nextCode values
	for (bits = 1; bits <= MaxBitLength; ++bits)
		nextCode[bits] = (nextCode[bits - 1] + bitLengthCount[bits - 1]) << 1u;
	//Step 3: Generate all the codes
	for(u32 n = 0; n != NumCodes; ++n)
	{
		if(Lengths[n] != 0)
		{
			Codes[n] = nextCode[Lengths[n]]++;
			//Remove superfluous bits from the code
			Codes[n] &= ((1u << Lengths[n]) - 1u);
		}
	}

	return MakeTable();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::MakeTable()
{
	constexpr u32 headSize = 1u << FirstBits; //Size of the first table
	constexpr u32 mask = (1u << FirstBits) /*headSize*/ - 1u;
	usize i = 0; //Total table size
	std::vector<u32> maxLengths(headSize, 0);

	//Compute maxLengths: Max total bit length of symbols sharing prefix in the first table
	for(i = 0; i < NumCodes; i++)
	{
		const u32 symbol = Codes[i];
		const u32 l = Lengths[i];
		if (l <= FirstBits)
			continue; //Symbols that fit in first table dont increase secondary table size
		//Get the FIRSTBITS(9u) MSBs, the MSBs of the symbol are encoded first.
		//See later comment about the reversing
		const u32 index = ReverseBits(symbol >> (l - FirstBits), FirstBits);
		maxLengths[index] = Math::Max(maxLengths[index], l);
	}
	//Compute total table size: Size of first table plus all secondary tables for symbols longer than FIRSTBITS(9u)
	usize size = headSize;
	for(i = 0; i < headSize; ++i)
	{
		const u32 l = maxLengths[i];
		if (l > FirstBits)
			size += static_cast<usize>(1u) << (l - FirstBits);
	}
	TableLength.resize(size, 16); //Initialize with an invalid length to indicate unused entries
	TableValue.resize(size);

	//Fill in the first table for long symbols: Max prefix size and pointer to secondary tables
	usize ptr = headSize;
	for(i = 0; i < headSize; ++i)
	{
		const u32 l = maxLengths[i];
		if (l <= FirstBits)
			continue;
		TableLength[i] = static_cast<u8>(l);
		TableValue[i] = static_cast<u16>(ptr);
		ptr += static_cast<usize>(1u) << (l - FirstBits);
	}

	//Fill in the first table for short symbols, or secondary table for long symbols
	usize numPresent = 0;
	for(i = 0; i < NumCodes; ++i)
	{
		const u32 l = Lengths[i];
		if (l == 0)
			continue;

		const u32 symbol = Codes[i]; //The Huffman bit pattern. i itself is the value
		//Reverse bits, because the Huffman bits are given in MSB first order but the bit reader reads LSB first
		const u32 reverse = ReverseBits(symbol, l);

		numPresent++;

		if(l <= FirstBits)
		{
			//Short symbol, fully in first table, replicated num times if l < FIRSTBITS(9u)
			const u32 num = 1u << (FirstBits - l);
			for(u32 j = 0; j < num; ++j)
			{
				//Bit reader will read the 1 bits of symbol first, the remaining FIRSTBITS(9u) - l bits
				//go to the MSBs
				const u32 index = reverse | (j << l);
				if (TableLength[index] != 16)
					return false; //Invalid tree: Long symbol shares prefix with short symbol
				TableLength[index] = static_cast<u8>(l);
				TableValue[index] = static_cast<u16>(i);
			}
		}
		else
		{
			//Long symbol, shares prefix with other long symbols in first lookup table, needs second lookup
			//The FIRSTBITS(9u) MSBs of the symbol are the first table index
			const u32 index = reverse & mask;
			const u32 maxLength = TableLength[index];
			//log2 of secondary table length, should be >= l - FIRSTBITS(9u)
			const u32 tableLength = maxLength - FirstBits;
			const u32 start = TableValue[index]; //Starting index in secondary table
			//Amount of entries of this symbol in secondary table
			const u32 num = 1u << (tableLength - (l - FirstBits));
			if (maxLength < l)
				return false; //Invalid tree: Long symbol shares prefix with short symbol
			for(u32 j = 0; j < num; ++j)
			{
				const u32 reverse2 = reverse >> FirstBits; //l - FIRSTBITS(9u) bits
				const u32 index2 = start + (reverse2 | (j << (l - FirstBits)));
				TableLength[index2] = static_cast<u8>(l);
				TableValue[index2] = static_cast<u16>(i);
			}
		}
	}

	if(numPresent < 2)
	{
		//In case of exactly 1 symbol, in theory the Huffman symbol needs 0 bits, but deflate uses 1 bit instead.
		//In case of 0 symbols, no symbols can appear at all, but such Huffman Tree could still
		//exist(e.g. if distance codes are never used).
		//In both cases, not all symbols of the table will be filled in.
		//Fill them in with an invalid symbol value so returning them from HuffmanTree::DecodeSymbol
		//will cause error
		for (i = 0; i < size; ++i)
		{
			if (TableLength[i] == 16)
			{
				//As Length, use a value smaller than FIRSTBITS(9u) for the head table, and a value larger
				//than FIRSTBITS(9u) for the secondary table,
				//to ensure valid behavior for advanceBits when reading this symbol.
				TableLength[i] = (i < headSize) ? 1 : (FirstBits + 1);
				TableValue[i] = InvalidSymbol;
			}
		}
	}
	else
	{
		//A good Huffman Tree has N * 2 - 1 nodes, of which N - 1 are internal nodes.
		//If that is not the case(due to too long length codes), the table will not have been fully used,
		//and this is an error
		//(not all bit combinations can be decoded)
		for (i = 0; i < size; ++i)
		{
			if (TableLength[i] == 16)
				return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Utils::Decompress::INTERNAL::HuffmanTree::ReverseBits(const u32 bits, const u32 num) noexcept
{
	u32 result = 0;
	for (u32 i = 0; i < num; i++)
		result |= ((bits >> (num - i - 1u)) & 1u) << i;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::InflateNoCompression(const std::span<u8> out, usize& pos,
                                                                                     BitReader& reader)
{
	const usize size = reader.Data.size_bytes();

	//Go to first boundary of byte
	usize bytePos = (reader.BP + 7u) >> 3u;

	//Read LEN(2Bytes) and NLEN(2Bytes)
	if (bytePos + 4 >= size)
		return false; //Error, bit pointer will jump past memory
	const u32 LEN = static_cast<u32>(reader.Data[bytePos]) +
	                     (static_cast<u32>(reader.Data[bytePos + 1]) << 8u);
	bytePos += 2;
	const u32 NLEN = static_cast<u32>(reader.Data[bytePos]) +
	                      (static_cast<u32>(reader.Data[bytePos + 1]) << 8u);
	bytePos += 2;

	//Check if 16-bit NLEN is really the ones complement of LEN
	if (LEN + NLEN != 65535)
		return false; //Error: NLEN is not ones complement of LEN

	// out.resize(pos + LEN);

	//Read the literal data: LEN bytes are now stored in the out buffer
	if (bytePos + LEN > size)
		return false; //Error: Reading outside of in buffer

	std::copy_n(reader.Data.data() + bytePos, LEN, out.data() + pos);
	pos += LEN;
	bytePos += LEN;

	reader.BP = bytePos << 3u;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::Decompress::INTERNAL::InflateHuffmanBlock(const std::span<u8> out, usize& pos,
                                                                                    BitReader& reader, const u32 btype)
{
	HuffmanTree treeLL; //The Huffman tree for literal and length codes
	HuffmanTree treeD; //The Huffman tree for distance codes

	if (btype != 1 && btype != 2)
		return false;

	if (btype == 1)
		if (!HuffmanTree::GetTreeInflateFixed(treeLL, treeD))
			return false;

	if(btype == 2)
		if (!HuffmanTree::GetTreeInflateDynamic(treeLL, treeD, reader))
			return false;

	bool error = false, done = false;
	while(!error && !done) //Decode all symbols until end reached, breaks at end code
	{
		//Ensure enough bits for 2 huffman code reads (15 bits each): if the first is a literal,
		//a second literal is read at once. This appears to be slightly faster, than ensuring 20
		//bits here for 1 huffman symbol and the potential 5 extra bits for the length symbol.
		reader.EnsureBits32();
		u32 codeLL = treeLL.DecodeSymbol(reader);
		if(codeLL <= 255)
		{
			//Slightly faster code path if multiple literals in a row
			// out.resize(pos + 1);
			out[pos] = static_cast<u8>(codeLL);
			++(pos);
			codeLL = treeLL.DecodeSymbol(reader);
		}

		if(codeLL <= 255) //Literal symbol
		{
			// out.resize(pos + 1);
			out[pos] = static_cast<u8>(codeLL);
			++(pos);
		}
		else if(codeLL >= FirstLengthCodeIndex && codeLL <= LastLengthCodeIndex) //Length code
		{
			//Part 1: Get Length base
			usize length = HuffmanTree::LengthBase[codeLL - FirstLengthCodeIndex];

			//Part 2: Get Extra bits and add the value of that to length
			const u32 numExtraBitsL = HuffmanTree::LengthExtra[codeLL - FirstLengthCodeIndex];
			if (numExtraBitsL != 0)
			{
				//Bits already ensured above
				reader.EnsureBits25();
				length += reader.ReadBits(numExtraBitsL);
			}

			//Part 3: Get Distance code
			reader.EnsureBits32(); //Up to 15 for the Huffman symbol, up to 13 for the extra bits
			const u32 codeD = treeD.DecodeSymbol(reader);
			if(codeD > 29)
			{
				if(codeD <= 31)
				{
					error = true; //Error: Invalid distance code (30-31 are never used)
					break;
				}

				error = true; //Error: Tried to read disallowed Huffman symbol
				break;
			}
			u32 distance = HuffmanTree::DistanceBase[codeD];

			//Part 4: Get Extra bits from distance
			const u32 numExtraBitsD = HuffmanTree::DistanceExtra[codeD];
			if(numExtraBitsD != 0)
				//Bits already ensured above
				distance += reader.ReadBits(numExtraBitsD);

			//Part 5: Fill in all the out[n] values based on the length and distance
			const usize start = pos;
			if(distance > start)
			{
				error = true; //Error: Too long backward distance
				break;
			}
			usize backward = start - distance;

			// out.resize(pos + length);
			if(distance < length)
			{
				std::copy_n(out.data() + backward, distance, out.data() + pos);
				pos += distance;
				for (usize forward = distance; forward < length; ++forward)
					out[pos++] = out[backward++];
			}
			else
			{
				std::copy_n(out.data() + backward, length, out.data() + pos);
				pos += length;
			}
		}
		else if(codeLL == 256)
			done = true; //End code, break the loop
		else //if(codeLL == 66535u)
		{
			error = true; //Error: Tried to read disallowed Huffman symbol
			break;
		}
		//Check if any of the EnsureBits above went out of bounds
		if(reader.BP > reader.BitSize)
		{
			error = true; //Error: Bit pointer jumps past memory
			break;
		}
	}

	return !error;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::Decompress::Inflate(const std::span<const u8> source,
                                                              const std::span<u8> destination)
{
	u32 BFINAL = 0;
	usize pos = 0; //Byte position in the destination buffer
	INTERNAL::BitReader reader(source);
	if (reader.Error)
		return false;

	while(BFINAL == 0u)
	{
		if(reader.BitSize - reader.BP < 3) //Error, bit pointer will jump past memory
			return false;
		reader.EnsureBits9();
		BFINAL = reader.ReadBits(1);
		const u32 BTYPE = reader.ReadBits(2);

		if (BTYPE == 3)
			return false; //Invalid BTYPE
		if (BTYPE == 0)
		{
			if (!TRAP::Utils::Decompress::INTERNAL::InflateNoCompression(destination, pos, reader))
				return false;
		}
		else
		{
			//Compression, BTYPE 01 or 10
			if (!TRAP::Utils::Decompress::INTERNAL::InflateHuffmanBlock(destination, pos, reader, BTYPE))
				return false;
		}
	}

	return true;
}

#endif /*TRAP_INFLATE_H*/