#pragma once
class crc_32
{
protected:
	unsigned table[256];
public:
	unsigned m_crc32;
	crc_32();
	~crc_32();
	void ProcessCRC(void* pData, register int nLen);
};

