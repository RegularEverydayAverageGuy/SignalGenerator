#pragma once
#include <cstdint>
#include <limits>
#include <mutex>
#include <vector>
#include "CircularBuffer.h"

class Signal
{
public:
	Signal(): _threshold(0.0), _counter(0), _data(0),_samplingRate(100), _exceededThreshold(false), _dataBuffer(10000) {};

public:
	void SetThreshold(float threshold);
	void SetSamplingRate(int nanoseconds );
	void ClearDataBuffer();

	uint32_t GetData();
	uint16_t GetCounter();
	uint16_t GetCounter(uint32_t inData);

	int GetSamplingRate();

	bool IsThresholdExceeded();
	static bool IsThresholdExceeded(uint32_t inData);

	virtual uint32_t GenerateData();

	CircularBuffer & GetGeneratedData();

protected:
	CircularBuffer _dataBuffer;

	uint16_t _counter;//16-30 bit v _data
	uint16_t _bufferSample = 0;
	
	uint32_t _data;//[31-bit _exceededThreshold , 30-16-bit _counter , 15-0-bit _signalValue]

	int _samplingRate;//Èas vzorèenja v nanosekundah

	float _threshold;

	bool _exceededThreshold;//31 bit v _data
	
	const uint16_t MAX_COUNTER_VALUE = pow(2, 15) - 1;
protected:
	void WriteCounterToData();
	void WriteExceededThresholdTotData();
};