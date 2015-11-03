#include "stdafx.h"
#include "crc_32.h"


crc_32::crc_32()
{
	const unsigned CRC_POLY = 0xEDB88320;
	unsigned i, j, r;
	for (i = 0; i < 256; i++) {
		for (r = i, j = 8; j; j--)
			r = r & 1 ? (r >> 1) ^ CRC_POLY : r >> 1;
		table[i] = r;
	}
	m_crc32 = 0;
}


crc_32::~crc_32()
{
}

void crc_32::ProcessCRC(void* Data, int nLen)
{
	const unsigned CRC_MASK = 0xD202EF8D;
	unsigned char* data = reinterpret_cast<unsigned char*>(Data);
	unsigned crc = m_crc32;
	while (nLen--) {
		crc = table[(unsigned char)crc ^ *data++] ^ crc >> 8;
		crc ^= CRC_MASK;
	}
	m_crc32 = crc;
}
