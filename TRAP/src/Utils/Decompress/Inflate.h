/*
Copyright (c) 2005-2018 Lode Vandevenne

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

Modified by: Jan "GamesTrap" Schuerkamp
*/

#ifndef _TRAP_INFLATE_H_
#define _TRAP_INFLATE_H_

namespace TRAP::Utils::Decompress
{
	namespace INTERNAL
	{
		struct BitReader
		{
			/// <summary>
			/// Constructor.
			/// BitReader reads byte data bit by bit.
			/// </summary>
			/// <param name="data">Data to read in bytes.</param>
			/// <param name="size">Size of data in bytes.</param>
			BitReader(const uint8_t* data, std::size_t size);

			/// <summary>
			/// Default Destructor.
			/// </summary>
			~BitReader() = default;
			/// <summary>
			/// Default Copy Constructor.
			/// </summary>
			BitReader(const BitReader&) = default;
			/// <summary>
			/// Default Move Constructor.
			/// </summary>
			BitReader(BitReader&&) = default;
			/// <summary>
			/// Default Copy Assignment Operator.
			/// </summary>
			BitReader& operator=(const BitReader&) = default;
			/// <summary>
			/// Default Move Assignment Operator.
			/// </summary>
			BitReader& operator=(BitReader&&) = default;
			
			const uint8_t* Data;
			std::size_t Size; //Size of data in bytes
			std::size_t BitSize; // Size of data in bits, end of valid BP values, should be 8 * size
			std::size_t BP;
			uint32_t Buffer; //Buffer for reading bits.
			bool Error = false;

			/// <summary>
			/// Ensure the reader can at least read 9 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			/// </summary>
			/// <param name="nBits">How many bits up to 9.</param>
			/// <returns>True if there are enough bits available, false otherwise.</returns>
			bool EnsureBits9(std::size_t nBits);
			/// <summary>
			/// Ensure the reader can at least read 17 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			/// </summary>
			/// <param name="nBits">How many bits up to 17.</param>
			/// <returns>True if there are enough bits available, false otherwise.</returns>
			bool EnsureBits17(std::size_t nBits);
			/// <summary>
			/// Ensure the reader can at least read 25 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			/// </summary>
			/// <param name="nBits">How many bits up to 25.</param>
			/// <returns>True if there are enough bits available, false otherwise.</returns>
			bool EnsureBits25(std::size_t nBits);
			/// <summary>
			/// Ensure the reader can at least read 32 bits in one or more ReadBits calls,
			/// safely even if not enough bits are available.
			/// </summary>
			/// <param name="nBits">How many bits up to 32.</param>
			/// <returns>True if there are enough bits available, false otherwise.</returns>
			bool EnsureBits32(std::size_t nBits);
			/// <summary>
			/// Read n amount of bits.<br>
			/// Note: Must have enough bits available with EnsureBits.
			/// </summary>
			/// <param name="nBits">How many bits.</param>
			/// <returns>N bits read.</returns>
			uint32_t ReadBits(std::size_t nBits);
			/// <summary>
			/// Safely check if a + b > c, even if overflow could happen.
			/// </summary>
			/// <returns>True if no overflow will happen, false otherwise.</returns>
			static bool GreaterOverflow(std::size_t a, std::size_t b, std::size_t c);
			/// <summary>
			/// Get bits without advancing the bit pointer.<br>
			/// Note: Must have enough bits available with EnsureBits.
			/// </summary>
			/// <param name="nBits">How many bits up to 31.</param>
			/// <returns>N bits read.</returns>
			uint32_t PeekBits(std::size_t nBits) const;
			/// <summary>
			/// Advance n amount of bits in the reader.<br>
			/// Note: Must have enough bits available with EnsureBits.
			/// </summary>
			/// <param name="nBits">How many bits.</param>
			void AdvanceBits(std::size_t nBits);
			
		private:
			/// <summary>
			/// Safely check if multiplying two integers will overflow(no undefined behavior, compiler removing the code, etc...) and output result.
			/// </summary>
			/// <param name="result">Output variable for the result of the multiplication.</param>
			/// <returns>True if no overflow happens, false otherwise.</returns>
			static bool MultiplyOverflow(std::size_t a, std::size_t b, std::size_t& result);
			/// <summary>
			/// Safely check if adding two integers will overflow(no undefined behavior, compiler removing the code, etc...) and output result.
			/// </summary>
			/// <param name="result">Output variable for the result of the sum.</param>
			/// <returns>True if no overflow happens, false otherwise.</returns>
			static bool AddOverflow(std::size_t a, std::size_t b, std::size_t& result);
		};

		/// <summary>
		/// Huffman tree struct, containing multiple representations of the tree
		/// </summary>
		struct HuffmanTree
		{
			/// <summary>
			/// Constructor.
			/// </summary>
			HuffmanTree();

			/// <summary>
			/// Default Destructor.
			/// </summary>
			~HuffmanTree() = default;
			/// <summary>
			/// Default Copy Constructor.
			/// </summary>
			HuffmanTree(const HuffmanTree&) = default;
			/// <summary>
			/// Default Move Constructor.
			/// </summary>
			HuffmanTree(HuffmanTree&&) = default;
			/// <summary>
			/// Default Copy Assignment Operator.
			/// </summary>
			HuffmanTree& operator=(const HuffmanTree&) = default;
			/// <summary>
			/// Default Move Assignment Operator.
			/// </summary>
			HuffmanTree& operator=(HuffmanTree&&) = default;
			
			std::vector<uint32_t> Codes; //The Huffman codes(bit patterns representing the symbols)
			std::vector<uint32_t> Lengths; //The lengths of the huffman codes
			uint32_t MaxBitLength; //Maximum number of bits a single code can get
			uint32_t NumCodes; //Number of symbols in the alphabet = number of codes

			/// <summary>
			/// Get the tree of a deflated block with fixed tree, as specified in the deflate specification.
			/// </summary>
			/// <param name="treeLL">Literal length huffman tree.</param>
			/// <param name="treeD">Distance huffman tree.</param>
			/// <returns>True on success, false otherwise.</returns>
			static bool GetTreeInflateFixed(HuffmanTree& treeLL, HuffmanTree& treeD);
			/// <summary>
			/// Get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree.
			/// </summary>
			/// <param name="treeLL">Literal length huffman tree.</param>
			/// <param name="treeD">Distance huffman tree.</param>
			/// <param name="reader">BitReader.</param>
			/// <returns>True on success, false otherwise.</returns>
			static bool GetTreeInflateDynamic(HuffmanTree& treeLL, HuffmanTree& treeD, BitReader& reader);

			/// <summary>
			/// The bit reader must already have been ensured at least 15 bits.
			/// </summary>
			/// <param name="reader">BitReader to decode symbol from.</param>
			/// <returns>Code.</returns>
			uint32_t DecodeSymbol(BitReader& reader);

			//The Base lengths represented by codes 257-285
			static constexpr std::array<uint32_t, 29> LengthBase
			{
				3, 4, 5, 6,
				7, 8, 9, 10,
				11, 13, 15, 17,
				19, 23, 27, 31,
				35, 43, 51,59,
				67, 83, 99, 115,
				131, 163, 195, 227,
				258
			};
			//The Extra bits used by codes 257-285(added to base length)
			static constexpr std::array<uint32_t, 29> LengthExtra
			{ 0, 0, 0, 0,
				0, 0, 0, 0,
				1, 1, 1, 1,
				2, 2, 2, 2,
				3, 3, 3, 3,
				4, 4, 4, 4,
				5, 5, 5, 5,
				0
			};
			//The base backwards distances(the bits of distance codes appear after length codes and use their own Huffman Tree)
			static constexpr std::array<uint32_t, 30> DistanceBase
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
			//The Extra bits of backwards distances(added to base)
			static constexpr std::array<uint32_t, 30> DistanceExtra
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
			std::vector<uint8_t> TableLength; //Length of symbol from lookup table, or max length if secondary lookup needed
			std::vector<uint16_t> TableValue; //Value of symbol from lookup table, or pointer to secondary table if needed

			/// <summary>
			/// Get the literal and length code tree of a deflated block with fixed tree, as per the deflate specification.
			/// </summary>
			/// <returns>True on success, false otherwise.</returns>
			bool GenerateFixedLiteralLengthTree();

			/// <summary>
			/// Get the distance code tree of a deflated block with fixed tree, as specified in the deflate specification.
			/// </summary>
			/// <returns>True on success, false otherwise.</returns>
			bool GenerateFixedDistanceTree();

			/// <summary>
			/// Given the code lengths(as stored in the PNG file), generate the tree as defined by Deflate.<br>
			/// </summary>
			/// <param name="bitLength">Code length in bits.</param>
			/// <param name="numCodes">Amount of codes.</param>
			/// <param name="maxBitLength">Maximum bits that a code in the tree can have.</param>
			/// <returns>True on success, false otherwise.</returns>
			bool MakeFromLengths(const uint32_t* bitLength, std::size_t numCodes, uint32_t maxBitLength);

			/// <summary>
			/// Second step for the ...MakeFromLengths and ...MakeFromFrequencies functions.<br>
			/// numCodes, lengths and maxBitLength must already be filled in correctly.
			/// </summary>
			/// <returns>True on success, false otherwise.</returns>
			bool MakeFromLengths2();

			/// <summary>
			/// Make table for huffman decoding.
			/// </summary>
			/// <returns>True on success, false otherwise.</returns>
			bool MakeTable();
			/// <summary>
			/// Reverse bits.
			/// </summary>
			static uint32_t ReverseBits(uint32_t bits, uint32_t num);

			static constexpr uint16_t NumDeflateCodeSymbols = 288;
			static constexpr uint8_t NumDistanceSymbols = 32;
			static constexpr uint8_t NumCodeLengthCodes = 19;
			static constexpr uint8_t FirstBits = 9u;
			static constexpr uint16_t InvalidSymbol = 65535u;
			//The order in which "code length alphabet code lengths" are stored, out of this the Huffman Tree of the dynamic Huffman Tree lengths is generated
			static constexpr std::array<uint32_t, 19> CLCLOrder{ 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
		};

		static constexpr uint16_t FirstLengthCodeIndex = 257;
		static constexpr uint16_t LastLengthCodeIndex = 285;
		
		static bool InflateNoCompression(std::vector<uint8_t>& out, std::size_t& pos, BitReader& reader);
		static bool InflateHuffmanBlock(std::vector<uint8_t>& out, std::size_t& pos, BitReader& reader, uint32_t btype);
	}

	/// <summary>
	/// Inflate algorithm.
	/// </summary>
	/// <param name="source">Source data in bytes.</param>
	/// <param name="sourceLength">Source data length in bytes.</param>
	/// <param name="destination">Destination where to put inflated data to.</param>
	/// <param name="destinationLength">Destination data length in bytes.</param>
	/// <returns>True on success, false otherwise.</returns>
	bool Inflate(const uint8_t* source, std::size_t sourceLength, uint8_t* destination, std::size_t destinationLength);
}

#endif /*_TRAP_INFLATE_H_*/