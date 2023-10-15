#pragma once
#include "Signal.h"
#include <cstdint>
#include <cmath>

constexpr auto M_PI = 3.14;

class SineSignal : public Signal
{
public:
	SineSignal(): _amplitude(1.0), _frequency(1), _phase(0), _signalValue(0) {};
	SineSignal(float amplitude, float frequency, float phase) :_amplitude(amplitude), _frequency(frequency), _phase(phase), _signalValue(0) {};
	
	void SetFrequency(float frequency);
	void SetAmplitude(float amplitude);
	void SetPhase(float phase);

	float GetFrequency();
	float GetAmplitude();
	float GetPhase();

	int16_t GetSignal();
	static int16_t GetSignal(uint32_t inData);

	virtual uint32_t GenerateData() override;
private:
	int16_t _signalValue;

	float _amplitude;
	float _frequency;
	float _phase;

	const int16_t MAX_SIGNAL_VALUE = std::numeric_limits<int16_t>::max();
	const int16_t MIN_SIGNAL_VALUE = std::numeric_limits<int16_t>::min();

	const uint16_t MAX_BUFFER_SIZE = 10000;
private:
	void WriteSignalToData();
};