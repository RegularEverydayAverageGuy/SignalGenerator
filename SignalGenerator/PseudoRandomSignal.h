#pragma once
#include "Signal.h"
#include <cstdint>
#include <cstdlib> 
#include <ctime>

class PseudoRandomSignal : public Signal
{
public:
	PseudoRandomSignal() :_minAmplitude(0.0), _maxAmplitude(1.0), _signalValue(0) { srand(std::time(nullptr));};
	PseudoRandomSignal(float minAmplitude, float maxAmplitude) :_minAmplitude(minAmplitude), _maxAmplitude(maxAmplitude), _signalValue(0) { srand(std::time(nullptr)); };

	void SetMinAmpitude(float minAmplitude);
	void SetMaxAmplitude(float maxAmplitude);

	float GetMinAmplitude();
	float GetMaxAmplitude();

	uint16_t GetSignal();
	static uint16_t GetSignal(uint32_t inData);

	virtual uint32_t GenerateData() override;
private:
	uint16_t _signalValue;
	float _minAmplitude;
	float _maxAmplitude;

	const uint16_t MAX_SIGNAL_VALUE = std::numeric_limits<uint16_t>::max();
	const uint16_t MAX_BUFFER_SIZE = 10000;
private:
	void WriteSignalToData();
};

