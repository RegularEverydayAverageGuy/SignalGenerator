#include "SineSignal.h"

uint32_t SineSignal::GenerateData()
{
    float timeDelta = _samplingRate / 1000000000.0f;//èas vzorèenja, pretvorimo iz nanosekund v sekunde;
    float signal = _amplitude * sin(2 * M_PI * _frequency * (_counter *timeDelta) + _phase);//sinusni signal

    _exceededThreshold = (signal > _threshold) ? true : false;//Smo prekoraèili prag

    if (signal < 0.0)
    {
        _signalValue = static_cast<int16_t>(abs(signal) * MIN_SIGNAL_VALUE);//Pretvorba v 16-bitno predznaèneno število
    }
    else
    {
        _signalValue = static_cast<int16_t>(signal * MAX_SIGNAL_VALUE);//Pretvorba v 16-bitno predznaèneno število
    }
  
    //Zapiši nove podatke v _data
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
    //V pomnilniku je lahko najveè MAX_BUFFER_SIZE vzorecev, potem se zaènejo najstarejši prepisovati
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

void SineSignal::WriteSignalToData()
{
    _data = _data & 0xFFFF0000;//Poèistimo nazadnje zapisano vrednost signala v _data
    _data = _data | static_cast<uint32_t>(_signalValue);//Zapišemo trenutno vrednost generiranega signala v _data
}

void SineSignal::SetFrequency(float frequency)
{
    _frequency = frequency;
}

void SineSignal::SetAmplitude(float amplitude)
{
    _amplitude = amplitude;
}

void SineSignal::SetPhase(float phase)
{
    _phase = phase;
}

float SineSignal::GetFrequency()
{
    return _frequency;
}

float SineSignal::GetAmplitude()
{
    return _amplitude;
}

float SineSignal::GetPhase()
{
    return _phase;
}

int16_t SineSignal::GetSignal()
{
    return static_cast<int16_t>(_data & 0xFFFF);
}

int16_t SineSignal::GetSignal(uint32_t inData)
{
    return static_cast<int16_t>(inData & 0xFFFF);
}

