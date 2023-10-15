#pragma once
#include <vector>

class CircularBuffer
{
public:
	CircularBuffer(int bufferSize):_maxBufferSize(bufferSize), _index(0) {};

	void SetMaxBufferSize(int size);
	void AddData(uint32_t inData);

	std::vector<uint32_t>& GetVector();
private:
	std::vector<uint32_t> _dataVector;
	int _maxBufferSize;
	int _index;
};

