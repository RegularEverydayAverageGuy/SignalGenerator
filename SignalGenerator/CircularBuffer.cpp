#include "CircularBuffer.h"

void CircularBuffer::SetMaxBufferSize(int size)
{
	_maxBufferSize = size;
}

std::vector<uint32_t>& CircularBuffer::GetVector()
{
	return _dataVector;
}

void CircularBuffer::AddData(uint32_t inData)
{
	if (_index >= _maxBufferSize)
	{
		_index = 0;
	}

	if (_dataVector.size() >= _maxBufferSize)
	{
		_dataVector[_index] = inData;
	}

	else
	{
		_dataVector.push_back(inData);
	}

	_index++;
}
