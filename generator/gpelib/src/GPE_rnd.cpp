#include "GPE_base.h"

GPEBASE::CRND::CRND(void)
: m_dwSeed(RND_DEFAULT_SEED)
{
	Init();
}
GPEBASE::CRND::CRND(uint32_t dwSeed)
: m_dwSeed(dwSeed)
{
	Init();
}
GPEBASE::CRND::~CRND(void)
{
	m_dwSeed=0;
}
void GPEBASE::CRND::Init(void)
{
	for(int i=0;i<RND_DUMMY_ROUDS;i++) GenByte();
}
void GPEBASE::CRND::SetSeed(uint32_t dwSeed)
{
	m_dwSeed=dwSeed;
	Init();
}
uint8_t GPEBASE::CRND::GenByte(void)
{
	uint8_t cbRnd=0;
	for(int i=0;i<8;i++)
	{
		m_dwSeed=((((m_dwSeed>>31)
			    ^(m_dwSeed>>30)
			    ^(m_dwSeed>>29)
			    ^(m_dwSeed>>27)
			    ^(m_dwSeed>>25)
			    ^m_dwSeed)
			    &0x00000001)<<31)|(m_dwSeed>>1);
		cbRnd|=(uint8_t)(m_dwSeed&0x00000001);
		if(i!=7) cbRnd<<=1;
	}
	return cbRnd;
}
// ***********************************************************************************
// Генератор для перемежителей
// ***********************************************************************************
GPEBASE::CGpeRND::CGpeRND(uint64_t nSeed)
: m_nRnd(nSeed)
{
}
unsigned int GPEBASE::CGpeRND::RandInt(void)
{
    uint64_t nTmp=m_nRnd;
    nTmp=((uint64_t)(m_nRnd&0x00000000FFFFFFFFUL)
	    *(uint64_t)0x00000000F83F630AUL)
	    +((uint64_t)m_nRnd>>32);
    m_nRnd=nTmp;
	return (unsigned int)(nTmp&0x00000000FFFFFFFFUL);
}
double GPEBASE::CGpeRND::RandReal(void)
{
	return ((double)RandInt()*(double)2.3283064365387E-10);
}
