/*
LodePNG version 20221108
Copyright (c) 2005-2022 Lode Vandevenne
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

#include "TRAPPCH.h"
#include "Inflate.h"

#include "Maths/Math.h"

TRAP::Utils::Decompress::INTERNAL::BitReader::BitReader(const uint8_t* const data, const std::size_t size)
	: Data(data), Size(size), BitSize(0), BP(0), Buffer(0)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	std::size_t temp = 0;
	if (MultiplyOverflow(size, 8u, BitSize))
		Error = true;
	if (AddOverflow(BitSize, 64u, temp))
		Error = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits9()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::size_t start = BP >> 3u;
	const std::size_t size = Size;
	if (start + 1u < size)
	{
		Buffer = static_cast<uint32_t>(Data[start + 0]) | (static_cast<uint32_t>(Data[start + 1] << 8u));
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

void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits17()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::size_t start = BP >> 3u;
	const std::size_t size = Size;
	if (start + 2u < size)
	{
		Buffer = static_cast<uint32_t>(Data[start + 0]) | (static_cast<uint32_t>(Data[start + 1]) << 8u) |
		         (static_cast<uint32_t>(Data[start + 2]) << 16u);
		Buffer >>= (BP & 7u);
	}
	else
	{
		Buffer = 0;
		if (start + 0u < size)
			Buffer |= Data[start + 0];
		if (start + 1u < size)
			Buffer |= (static_cast<uint32_t>(Data[start + 1]) << 8u);
		Buffer >>= (BP & 7u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits25()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::size_t start = BP >> 3u;
	const std::size_t size = Size;
	if (start + 3u < size)
	{
		Buffer = static_cast<uint32_t>(Data[start + 0]) | (static_cast<uint32_t>(Data[start + 1]) << 8u) |
		         (static_cast<uint32_t>(Data[start + 2]) << 16u) | (static_cast<uint32_t>(Data[start + 3]) << 24u);
		Buffer >>= (BP & 7u);
	}
	else
	{
		Buffer = 0;
		if (start + 0u < size)
			Buffer |= Data[start + 0];
		if (start + 1u < size)
			Buffer |= (static_cast<uint32_t>(Data[start + 1]) << 8u);
		if (start + 2u < size)
			Buffer |= (static_cast<uint32_t>(Data[start + 2]) << 16u);
		Buffer >>= (BP & 7u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Decompress::INTERNAL::BitReader::EnsureBits32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::size_t start = BP >> 3u;
	const std::size_t size = Size;
	if (start + 4u < size)
	{
		Buffer = static_cast<uint32_t>(Data[start + 0]) | (static_cast<uint32_t>(Data[start + 1]) << 8u) |
		         (static_cast<uint32_t>(Data[start + 2]) << 16u) | (static_cast<uint32_t>(Data[start + 3]) << 24u);
		Buffer >>= (BP & 7u);
		Buffer |= ((static_cast<uint32_t>(Data[start + 4]) << 24u) << (8u - (BP & 7u)));
	}
	else
	{
		Buffer = 0;
		if (start + 0u < size)
			Buffer |= Data[start + 0];
		if (start + 1u < size)
			Buffer |= (static_cast<uint32_t>(Data[start + 1]) << 8u);
		if (start + 2u < size)
			Buffer |= (static_cast<uint32_t>(Data[start + 2]) << 16u);
		if (start + 3u < size)
			Buffer |= (static_cast<uint32_t>(Data[start + 3]) << 24u);
		Buffer >>= (BP & 7u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Must have enough bits available with EnsureBits
uint32_t TRAP::Utils::Decompress::INTERNAL::BitReader::ReadBits(const std::size_t nBits)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const uint32_t result = PeekBits(nBits);
	AdvanceBits(nBits);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Decompress::INTERNAL::BitReader::GreaterOverflow(const std::size_t a, const std::size_t b,
                                                                   const std::size_t c)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	std::size_t d = 0;
	if (AddOverflow(a, b, d))
		return true;

	return d > c;
}

//-------------------------------------------------------------------------------------------------------------------//

//Get bits without advancing the bit pointer. Must have enough bits available with EnsureBits
uint32_t TRAP::Utils::Decompress::INTERNAL::BitReader::PeekBits(const std::size_t nBits) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Buffer & ((1u << nBits) - 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

//Must have enough bits available with EnsureBits
void TRAP::Utils::Decompress::INTERNAL::BitReader::AdvanceBits(const std::size_t nBits)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Buffer >>= nBits;
	BP += nBits;
}

//-------------------------------------------------------------------------------------------------------------------//

//Safely check if multiplying two integers will overflow(no undefined behavior, compiler removing the code, etc...)
//and output result
bool TRAP::Utils::Decompress::INTERNAL::BitReader::MultiplyOverflow(const std::size_t a, const std::size_t b,
                                                                    std::size_t& result)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	result = a * b; //Unsigned multiplication is well defined and safe

	return (a != 0 && result / a != b);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Decompress::INTERNAL::BitReader::AddOverflow(const std::size_t a, const std::size_t b,
                                                               std::size_t& result)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	result = a + b; //Unsigned addition is well defined and safe

	return result < a;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Decompress::INTERNAL::HuffmanTree::HuffmanTree()
	: MaxBitLength(0), NumCodes(0)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);
}

//-------------------------------------------------------------------------------------------------------------------//

//Get the tree of a deflated block with fixed tree, as specified in the deflate specification
bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GetTreeInflateFixed(HuffmanTree& treeLL, HuffmanTree& treeD)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	if (!treeLL.GenerateFixedLiteralLengthTree())
		return false;

	if (!treeD.GenerateFixedDistanceTree())
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree
bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GetTreeInflateDynamic(HuffmanTree& treeLL, HuffmanTree& treeD,
                                                                           BitReader& reader)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	//Make sure that length values that arent filled in will be 0, or a wrong tree will be generated
	uint32_t n = 0, i = 0;

	//See comments in deflateDynamic for explanation of the context and these variables, it is analogous
	HuffmanTree treeCL; //The code tree for code length codes(the Huffman Tree for compressed Huffman Trees)

	if (reader.BitSize - reader.BP < 14)
		return false; //Error: The bit pointer is or will go past memory
	reader.EnsureBits17();

	//Number of literal/length codes + 257.
	//Unlike the spec, the value 257 is added to it here already
	const uint32_t HLIT = reader.ReadBits(5) + 257;
	//Number of distance codes.
	//Unlike the spec, the value 1 is added to it here already
	const uint32_t HDIST = reader.ReadBits(5) + 1;
	//Number of code length codes.
	//Unlike the spec, the value 4 is added to it here already
	const uint32_t HCLEN = reader.ReadBits(4) + 4;

	//Code length code lengths ("clcl"), the bit lengths of the Huffman Tree used to compress
	//bitLengthLL and bitLengthD
	std::array<uint32_t, NumCodeLengthCodes> bitLengthCL{};

	bool error = false;
	while(!error)
	{
		//Read the code length codes out of 3 * (amount of code length codes) bits
		if (BitReader::GreaterOverflow(reader.BP, static_cast<std::size_t>(HCLEN) * 3, reader.BitSize))
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
		std::array<uint32_t, NumDeflateCodeSymbols> bitLengthLL{};
		std::array<uint32_t, NumDistanceSymbols> bitLengthD{};

		//i is the current symbol we are reading in the part that contains the code lengths of
		//literal/length and distance codes
		i = 0;
		while(i < HLIT + HDIST)
		{
			reader.EnsureBits25(); //Up to 15bits for Huffman code, up to 7 extra bits below
			const uint32_t code = treeCL.DecodeSymbol(reader);
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
				uint32_t repeatLength = 3; //Read in the 2 bits that indicate repeat length (3-6)
				uint32_t value = 0; //Set value to the previous code

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
				uint32_t repeatLength = 3; //Read in the bits that indicate repeat length
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
				uint32_t repeatLength = 11; //Read in the bits that indicate repeat length
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

		if(bitLengthLL[256] == 0)
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
uint32_t TRAP::Utils::Decompress::INTERNAL::HuffmanTree::DecodeSymbol(BitReader& reader) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const uint16_t code = static_cast<uint16_t>(reader.PeekBits(FirstBits));
	const uint16_t l = TableLength[code];
	uint16_t value = TableValue[code];
	if(l <= FirstBits)
	{
		reader.AdvanceBits(l);

		return value;
	}

	reader.AdvanceBits(FirstBits);
	value += static_cast<uint16_t>(reader.PeekBits(l - FirstBits));
	reader.AdvanceBits(TableLength[value] - FirstBits);

	return TableValue[value];
}

//-------------------------------------------------------------------------------------------------------------------//

//Get the literal and length code of a deflated block with fixed tree, as per the deflate specification
bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GenerateFixedLiteralLengthTree()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	uint32_t i = 0;
	//256 literals, the end code, some length codes, and 2 unused codes
	std::array<uint32_t, NumDeflateCodeSymbols> bitLength{};

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
bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::GenerateFixedDistanceTree()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	//The distance codes have their own symbols, 30 used, 2 unused
	std::array<uint32_t, NumDistanceSymbols> bitLength{};

	//There are 32 distance codes, but 30-31 are unused
	for (uint32_t i = 0; i != NumDistanceSymbols; ++i)
		bitLength[i] = 5;

	return MakeFromLengths(bitLength.data(), NumDistanceSymbols, 15);
}

//-------------------------------------------------------------------------------------------------------------------//

//Given the code lengths(as stored in the PNG file), generate the tree as defined by Deflate.
//MaxBitLength is the maximum bits that a code in the tree can have.
bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::MakeFromLengths(const uint32_t* const bitLength,
                                                                     const std::size_t numCodes,
																	 const uint32_t maxBitLength)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	Lengths.resize(numCodes);
	for (uint32_t i = 0; i != numCodes; ++i)
		Lengths[i] = bitLength[i];
	NumCodes = static_cast<uint32_t>(numCodes); //Number of symbols
	MaxBitLength = maxBitLength;

	return MakeFromLengths2();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::MakeFromLengths2()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	uint32_t bits = 0;

	Codes.resize(NumCodes);
	std::vector<uint32_t> bitLengthCount((MaxBitLength + 1), 0);
	std::vector<uint32_t> nextCode((MaxBitLength + 1), 0);

	//Step 1: Count number of instances of each code length
	for (bits = 0; bits != NumCodes; ++bits)
		++bitLengthCount[Lengths[bits]];
	//Step 2: Generate the nextCode values
	for (bits = 1; bits <= MaxBitLength; ++bits)
		nextCode[bits] = (nextCode[bits - 1] + bitLengthCount[bits - 1]) << 1u;
	//Step 3: Generate all the codes
	for(uint32_t n = 0; n != NumCodes; ++n)
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

bool TRAP::Utils::Decompress::INTERNAL::HuffmanTree::MakeTable()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	static const uint32_t headSize = 1u << FirstBits; //Size of the first table
	static const uint32_t mask = (1u << FirstBits) /*headSize*/ - 1u;
	std::size_t i = 0; //Total table size
	std::vector<uint32_t> maxLengths(headSize, 0);

	//Compute maxLengths: Max total bit length of symbols sharing prefix in the first table
	for(i = 0; i < NumCodes; i++)
	{
		const uint32_t symbol = Codes[i];
		const uint32_t l = Lengths[i];
		if (l <= FirstBits)
			continue; //Symbols that fit in first table dont increase secondary table size
		//Get the FIRSTBITS(9u) MSBs, the MSBs of the symbol are encoded first.
		//See later comment about the reversing
		const uint32_t index = ReverseBits(symbol >> (l - FirstBits), FirstBits);
		maxLengths[index] = Math::Max(maxLengths[index], l);
	}
	//Compute total table size: Size of first table plus all secondary tables for symbols longer than FIRSTBITS(9u)
	std::size_t size = headSize;
	for(i = 0; i < headSize; ++i)
	{
		const uint32_t l = maxLengths[i];
		if (l > FirstBits)
			size += static_cast<std::size_t>(1u) << (l - FirstBits);
	}
	TableLength.resize(size, 16); //Initialize with an invalid length to indicate unused entries
	TableValue.resize(size);

	//Fill in the first table for long symbols: Max prefix size and pointer to secondary tables
	std::size_t ptr = headSize;
	for(i = 0; i < headSize; ++i)
	{
		const uint32_t l = maxLengths[i];
		if (l <= FirstBits)
			continue;
		TableLength[i] = static_cast<uint8_t>(l);
		TableValue[i] = static_cast<uint16_t>(ptr);
		ptr += static_cast<std::size_t>(1u) << (l - FirstBits);
	}

	//Fill in the first table for short symbols, or secondary table for long symbols
	std::size_t numPresent = 0;
	for(i = 0; i < NumCodes; ++i)
	{
		const uint32_t l = Lengths[i];
		if (l == 0)
			continue;

		const uint32_t symbol = Codes[i]; //The Huffman bit pattern. i itself is the value
		//Reverse bits, because the Huffman bits are given in MSB first order but the bit reader reads LSB first
		const uint32_t reverse = ReverseBits(symbol, l);

		numPresent++;

		if(l <= FirstBits)
		{
			//Short symbol, fully in first table, replicated num times if l < FIRSTBITS(9u)
			const uint32_t num = 1u << (FirstBits - l);
			for(uint32_t j = 0; j < num; ++j)
			{
				//Bit reader will read the 1 bits of symbol first, the remaining FIRSTBITS(9u) - l bits
				//go to the MSBs
				const uint32_t index = reverse | (j << l);
				if (TableLength[index] != 16)
					return false; //Invalid tree: Long symbol shares prefix with short symbol
				TableLength[index] = static_cast<uint8_t>(l);
				TableValue[index] = static_cast<uint16_t>(i);
			}
		}
		else
		{
			//Long symbol, shares prefix with other long symbols in first lookup table, needs second lookup
			//The FIRSTBITS(9u) MSBs of the symbol are the first table index
			const uint32_t index = reverse & mask;
			const uint32_t maxLength = TableLength[index];
			//log2 of secondary table length, should be >= l - FIRSTBITS(9u)
			const uint32_t tableLength = maxLength - FirstBits;
			const uint32_t start = TableValue[index]; //Starting index in secondary table
			//Amount of entries of this symbol in secondary table
			const uint32_t num = 1u << (tableLength - (l - FirstBits));
			if (maxLength < l)
				return false; //Invalid tree: Long symbol shares prefix with short symbol
			for(uint32_t j = 0; j < num; ++j)
			{
				const uint32_t reverse2 = reverse >> FirstBits; //l - FIRSTBITS(9u) bits
				const uint32_t index2 = start + (reverse2 | (j << (l - FirstBits)));
				TableLength[index2] = static_cast<uint8_t>(l);
				TableValue[index2] = static_cast<uint16_t>(i);
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

uint32_t TRAP::Utils::Decompress::INTERNAL::HuffmanTree::ReverseBits(const uint32_t bits, const uint32_t num)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	uint32_t result = 0;
	for (uint32_t i = 0; i < num; i++)
		result |= ((bits >> (num - i - 1u)) & 1u) << i;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Decompress::INTERNAL::InflateNoCompression(std::vector<uint8_t>& out, std::size_t& pos,
                                                             BitReader& reader)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const std::size_t size = reader.Size;

	//Go to first boundary of byte
	std::size_t bytePos = (reader.BP + 7u) >> 3u;

	//Read LEN(2Bytes) and NLEN(2Bytes)
	if (bytePos + 4 >= size)
		return false; //Error, bit pointer will jump past memory
	const uint32_t LEN = static_cast<uint32_t>(reader.Data[bytePos]) +
	                     (static_cast<uint32_t>(reader.Data[bytePos + 1]) << 8u);
	bytePos += 2;
	const uint32_t NLEN = static_cast<uint32_t>(reader.Data[bytePos]) +
	                      (static_cast<uint32_t>(reader.Data[bytePos + 1]) << 8u);
	bytePos += 2;

	//Check if 16-bit NLEN is really the ones complement of LEN
	if (LEN + NLEN != 65535)
		return false; //Error: NLEN is not ones complement of LEN

	out.resize(pos + LEN);

	//Read the literal data: LEN bytes are now stored in the out buffer
	if (bytePos + LEN > size)
		return false; //Error: Reading outside of in buffer

	std::copy_n(reader.Data + bytePos, LEN, out.data() + pos);
	pos += LEN;
	bytePos += LEN;

	reader.BP = bytePos << 3u;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Decompress::INTERNAL::InflateHuffmanBlock(std::vector<uint8_t>& out, std::size_t& pos,
                                                            BitReader& reader, const uint32_t btype)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

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

	uint32_t error = false, done = false;
	while(!error && !done) //Decode all symbols until end reached, breaks at end code
	{
		//Ensure enough bits for 2 huffman code reads (15 bits each): if the first is a literal,
		//a second literal is read at once. This appears to be slightly faster, than ensuring 20
		//bits here for 1 huffman symbol and the potential 5 extra bits for the length symbol.
		reader.EnsureBits32();
		uint32_t codeLL = treeLL.DecodeSymbol(reader);
		if(codeLL <= 255)
		{
			//Slightly faster code path if multiple literals in a row
			out.resize(pos + 1);
			out[pos] = static_cast<uint8_t>(codeLL);
			++(pos);
			codeLL = treeLL.DecodeSymbol(reader);
		}

		if(codeLL <= 255) //Literal symbol
		{
			out.resize(pos + 1);
			out[pos] = static_cast<uint8_t>(codeLL);
			++(pos);
		}
		else if(codeLL >= FirstLengthCodeIndex && codeLL <= LastLengthCodeIndex) //Length code
		{
			//Part 1: Get Length base
			std::size_t length = HuffmanTree::LengthBase[codeLL - FirstLengthCodeIndex];

			//Part 2: Get Extra bits and add the value of that to length
			const uint32_t numExtraBitsL = HuffmanTree::LengthExtra[codeLL - FirstLengthCodeIndex];
			if (numExtraBitsL != 0)
			{
				//Bits already ensured above
				reader.EnsureBits25();
				length += reader.ReadBits(numExtraBitsL);
			}

			//Part 3: Get Distance code
			reader.EnsureBits32(); //Up to 15 for the Huffman symbol, up to 13 for the extra bits
			const uint32_t codeD = treeD.DecodeSymbol(reader);
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
			uint32_t distance = HuffmanTree::DistanceBase[codeD];

			//Part 4: Get Extra bits from distance
			const uint32_t numExtraBitsD = HuffmanTree::DistanceExtra[codeD];
			if(numExtraBitsD != 0)
				//Bits already ensured above
				distance += reader.ReadBits(numExtraBitsD);

			//Part 5: Fill in all the out[n] values based on the length and distance
			const std::size_t start = pos;
			if(distance > start)
			{
				error = true; //Error: Too long backward distance
				break;
			}
			std::size_t backward = start - distance;

			out.resize(pos + length);
			if(distance < length)
			{
				std::copy_n(out.data() + backward, distance, out.data() + pos);
				pos += distance;
				for (std::size_t forward = distance; forward < length; ++forward)
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

bool TRAP::Utils::Decompress::Inflate(const uint8_t* const source, const std::size_t sourceLength, uint8_t* const destination,
                                      const std::size_t destinationLength)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	std::vector<uint8_t> result(destinationLength, 0);

	uint32_t BFINAL = 0;
	std::size_t pos = 0; //Byte position in the destination buffer
	INTERNAL::BitReader reader(source, sourceLength);
	if (reader.Error)
		return false;

	while(!BFINAL)
	{
		if(reader.BitSize - reader.BP < 3) //Error, bit pointer will jump past memory
			return false;
		reader.EnsureBits9();
		BFINAL = reader.ReadBits(1);
		const uint32_t BTYPE = reader.ReadBits(2);

		if (BTYPE == 3)
			return false; //Invalid BTYPE
		if (BTYPE == 0)
		{
			if (!TRAP::Utils::Decompress::INTERNAL::InflateNoCompression(result, pos, reader))
				return false;
		}
		else
		{
			//Compression, BTYPE 01 or 10
			if (!TRAP::Utils::Decompress::INTERNAL::InflateHuffmanBlock(result, pos, reader, BTYPE))
				return false;
		}
	}

	std::copy(result.begin(), result.end(), destination);

	return true;
}
