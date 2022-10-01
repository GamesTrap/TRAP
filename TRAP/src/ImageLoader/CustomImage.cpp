#include "TRAPPCH.h"
#include "CustomImage.h"

#include "Embed.h"

const void* TRAP::INTERNAL::CustomImage::GetPixelData() const
{
	ZoneScopedC(tracy::Color::Green);

	if(!m_dataHDR.empty())
		return m_dataHDR.data();

	if(!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::CustomImage::GetPixelDataSize() const
{
	ZoneScopedC(tracy::Color::Green);

	if(!m_dataHDR.empty())
		return m_dataHDR.size() * sizeof(float);

	if(!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(uint16_t);

	return m_data.size();
}