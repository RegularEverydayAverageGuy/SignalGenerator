#include "PseudoRandomSignal.h"

void PseudoRandomSignal::SetMinAmpitude(float minAmplitude)
{
    this->_minAmplitude = minAmplitude;
}

void PseudoRandomSignal::SetMaxAmplitude(float maxAmplitude)
{
    this->_maxAmplitude = maxAmplitude;
}

float PseudoRandomSignal::GetMinAmplitude()
{
    return _minAmplitude;
}

float PseudoRandomSignal::GetMaxAmplitude()
{
    return _maxAmplitude;
}

uint32_t PseudoRandomSignal::GenerateData()
{
    uint16_t minSignal = _minAmplitude * std::numeric_limits<uint16_t>::min();
    uint16_t maxSignal = _maxAmplitude * std::numeric_limits<uint16_t>::max();

    float currentSignal = (rand() % (maxSignal - minSignal + 1)) + _minAmplitude;//Generiramo celo�tevilska naklju�na �tevila
    _signalValue = currentSignal;

    _exceededThreshold = (_signalValue > _threshold*MAX_SIGNAL_VALUE) ? true : false;//Smo prekora�ili prag

    WriteSignalToData();
    WriteCounterToData();
    WriteExceededThresholdTotData();

    _counter++;
    if (_counter > MAX_COUNTER_VALUE)
    {
        _counter = 0;
    }

    _dataBuffer.AddData(_data);
    /*
    //V pomnilniku je lahko najve� MAX_BUFFER_SIZE vzorecev, potem se za�nejo najstarej�i prepisovati
    if (_bufferSample >= MAX_BUFFER_SIZE)
    {
        _bufferSample = 0;
    }

    if (_dataBuffer.size() >= MAX_BUFFER_SIZE)
    {
        _dataBuffer[_bufferSample] = _data;
    }

    else
    {
        _dataBuffer.push_back(_data);
    }
    _bufferSample++;
    */
    return _data;
}

void PseudoRandomSignal::WriteSignalToData()
{
    _data = _data & 0xFFFF0000;//Po�istimo nazadnje zapisano vrednost signala v _data
    _data = _data | static_cast<uint32_t>(_signalValue);//Zapi�emo trenutno vrednost generiranega signala v _data
}

uint16_t PseudoRandomSignal::GetSignal()
{
    return static_cast<uint16_t>(_data & 0xFFFF);
}

uint16_t PseudoRandomSignal::GetSignal(uint32_t inData)
{
    return static_cast<uint16_t>(inData & 0xFFFF);
}

