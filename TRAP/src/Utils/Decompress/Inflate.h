#ifndef _TRAP_INFLATE_H_
#define _TRAP_INFLATE_H_

namespace TRAP::Utils::Decompress
{
	namespace INTERNAL
	{
		struct BitReader
		{
			BitReader(const uint8_t* data, std::size_t size);
			
			const uint8_t* Data;
			std::size_t Size; //Size of data in bytes
			std::size_t BitSize; // Size of data in bits, end of valid BP values, should be 8 * size
			std::size_t BP;
			uint32_t Buffer; //Buffer for reading bits.
			bool Error = false;

			bool EnsureBits9(std::size_t nBits);
			bool EnsureBits17(std::size_t nBits);
			bool EnsureBits25(std::size_t nBits);
			bool EnsureBits32(std::size_t nBits);
			//Must have enough bits available with EnsureBits
			uint32_t ReadBits(std::size_t nBits);
			//Safely check if a + b > c, event if overflow could happen.
			static bool GreaterOverflow(std::size_t a, std::size_t b, std::size_t c);			
			//Get bits without advancing the bit pointer. Must have enough bits available with EnsureBits
			uint32_t PeekBits(std::size_t nBits) const;
			//Must have enough bits available with EnsureBits
			void AdvanceBits(std::size_t nBits);
			
		private:			
			//Safely check if multiplying two integers will overflow(no undefined behavior, compiler removing the code, etc...) and output result.
			static bool MultiplyOverflow(std::size_t a, std::size_t b, std::size_t& result);
			//Safely check if adding two integers will overflow(no undefined behavior, compiler removing the code, etc...) and output result.
			static bool AddOverflow(std::size_t a, std::size_t b, std::size_t& result);
		};

		struct HuffmanTree
		{
			HuffmanTree();
			
			std::vector<uint32_t> Codes; //The Huffman codes(bit patterns representing the symbols)
			std::vector<uint32_t> Lengths; //The lengths of the huffman codes
			uint32_t MaxBitLength; //Maximum number of bits a single code can get
			uint32_t NumCodes; //Number of symbols in the alphabet = number of codes

			//Get the tree of a deflated block with fixed tree, as specified in the deflate specification
			static bool GetTreeInflateFixed(HuffmanTree& treeLL, HuffmanTree& treeD);
			//Get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree
			static bool GetTreeInflateDynamic(HuffmanTree& treeLL, HuffmanTree& treeD, BitReader& reader);

			//Returns the code. The bit reader must already have been ensured at least 15 bits
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

			//Get the literal and length code tree of a deflated block with fixed tree, as per the deflate specification
			bool GenerateFixedLiteralLengthTree();
			//Get the distance code tree of a deflated block with fixed tree, as specified in the deflate specification
			bool GenerateFixedDistanceTree();
			//Given the code lengths(as stored in the PNG file), generate the tree as defined by Deflate.
			//MaxBitLength is the maximum bits that a code in the tree can have.
			bool MakeFromLengths(const uint32_t* bitLength, std::size_t numCodes, uint32_t maxBitLength);
			//Second step for the ...MakeFromLengths and ...MakeFromFrequencies functions.
			//numCodes, lengths and maxBitLength must already be filled in correctly.
			bool MakeFromLengths2();
			//Make table for huffman decoding
			bool MakeTable();
			static uint32_t ReverseBits(uint32_t bits, uint32_t num);
			
			//The order in which "code length alphabet code lengths" are stored, out of this the Huffman Tree of the dynamic Huffman Tree lengths is generated
			static constexpr std::array<uint32_t, 19> CLCLOrder{ 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
		};

		static bool InflateNoCompression(std::vector<uint8_t>& out, std::size_t& pos, BitReader& reader);
		static bool InflateHuffmanBlock(std::vector<uint8_t>& out, std::size_t& pos, BitReader& reader, uint32_t btype);
	}
	
	bool Inflate(const uint8_t* source, std::size_t sourceLength, uint8_t* destination, std::size_t destinationLength);	
}

#endif /*_TRAP_INFLATE_H_*/