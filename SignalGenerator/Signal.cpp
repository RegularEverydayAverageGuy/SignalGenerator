#include "Signal.h"


void Signal::SetThreshold(float threshold)
{

    this->_threshold = threshold;
}

void Signal::SetSamplingRate(int nanoseconds)
{
    _samplingRate = nanoseconds;
}

void Signal::ClearDataBuffer()
{
    _dataBuffer.GetVector().clear();
}

uint32_t Signal::GetData()
{
    return _data;
}

uint16_t Signal::GetCounter()
{
    return static_cast<uint16_t>((_data >> 16) & 0x7FFF);//biti med 15-30
}

uint16_t Signal::GetCounter(uint32_t inData)
{
    return static_cast<uint16_t>((inData >> 16) & 0x7FFF);//biti med 15-30
}

int Signal::GetSamplingRate()
{
    return _samplingRate;
}

bool Signal::IsThresholdExceeded()
{
    return static_cast<bool>((_data >> 31) & 1);//31 bit
}

bool Signal::IsThresholdExceeded(uint32_t inData)
{
    return static_cast<bool>((inData >> 31) & 1);//31 bit
}

CircularBuffer& Signal::GetGeneratedData()
{
    return _dataBuffer;
}

uint32_t Signal::GenerateData()
{
    return 0;
}

void Signal::WriteCounterToData()
{
    _data = _data & 0x8000FFFF;//Po�istimo nazadnje zapisano vrednost �tevca v _data
    _data = _data | static_cast<uint32_t>(_counter << 16);//Zapi�emo trenutno vrednost �tevca v _data
}

void Signal::WriteExceededThresholdTotData()
{
    _data = _data & 0x7FFFFFFF;//Po�istimo nazadnje zapisano vrednost prekora�enega praga v _data
    _data = _data | static_cast<uint32_t>(int(_exceededThreshold) << 31);//Zapi�emo trenutno vrednost prekora�enega praga v _data
}
