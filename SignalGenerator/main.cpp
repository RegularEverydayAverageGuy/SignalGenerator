// SignalGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <sstream>
#include <mutex>
#include <iomanip>
#include "SineSignal.h"
#include "PseudoRandomSignal.h"
#include "CommandInterpreter.h"
#include "CircularBuffer.h"

enum SignalType :int
{
	Sine,//sinusni signal
	Random//nakljuèni signal
};

/*******************************PRIVZETE VREDNOSTI PARAMETROV****************************/
//Parametri sinusnega signala
float defaultSineFrequency = 100;//V Hz
float defaultSineAmplitude = 1.0;
float defaultSinePhase = 0;

//Parametri nakljuènega signala
float defaultMinAmplitude = 0.0;
float defaultMaxAmplitude = 1.0;

// Skupni parametri
float threshold = 0.0;
float rate = 100;//V Hz

//Parametri sistema
int bufferSize = 100;
int dataSelector = SignalType::Sine;
int savingPeriod = 1000;//V milisekundah
int bufferOffset = 0;
int bufferDuration = 10;

std::atomic<bool> requestData = false;

/********************************TRENUTNO PODPRTI SIGNALI*******************************/

std::vector<std::shared_ptr<Signal>> signals{ 
	std::make_shared<SineSignal>(defaultSineAmplitude, defaultSineFrequency, defaultSinePhase),
	std::make_shared<PseudoRandomSignal>(defaultMinAmplitude, defaultMaxAmplitude)
};
/***************************KROŽNI BUFFER ZA GENERIRANE PODATKE*************************/
CircularBuffer dataBuffer(bufferSize);
/************************************SINHRONIZACIJA*************************************/
std::mutex parameterMutex;
std::mutex bufferMutex;
std::mutex coutMutex;
/*******************************BRALNIK VHODNIK PARAMETROV******************************/
CommandInterpreter commandInterpreter;
/*******************************PRESLIKAVA UAKZOV V AKCIJE******************************/
typedef  std::function<void(float)> CommandAction;
std::map<std::string, CommandAction> commandMapper;
/************************************PODPRTE AKCIJE*************************************/
void SetAmplitude(float amplitude)
{
	std::lock_guard guard(parameterMutex);
	auto sineSignal = std::dynamic_pointer_cast<SineSignal>(signals[Sine]);
	sineSignal->SetAmplitude(amplitude);
}

void SetFrequency(float frequency)
{
	std::lock_guard guard(parameterMutex);
	auto sineSignal = std::dynamic_pointer_cast<SineSignal>(signals[Sine]);
	sineSignal->SetFrequency(frequency);
}

void SetPhase(float phase)
{
	std::lock_guard guard(parameterMutex);
	auto sineSignal = std::dynamic_pointer_cast<SineSignal>(signals[Sine]);
	sineSignal->SetFrequency(phase);
}

//Akcije povezane z nakljuènim signalom
void SetMinAmplitude(float minAmplitude)
{
	std::lock_guard guard(parameterMutex);
	auto randomSignal = std::dynamic_pointer_cast<PseudoRandomSignal>(signals[Random]);
	randomSignal->SetMinAmpitude(minAmplitude);
}

void SetMaxAmplitude(float maxAmplitude)
{
	std::lock_guard guard(parameterMutex);
	auto randomSignal = std::dynamic_pointer_cast<PseudoRandomSignal>(signals[Random]);
	randomSignal->SetMinAmpitude(maxAmplitude);
}

//Akcije povezane z vsemi podprtimi vrstami signalov
void SetThreshold(float threshold)
{
	std::lock_guard guard(parameterMutex);
	for (int i = 0; i < signals.size(); ++i)
	{
		signals[i]->SetThreshold(threshold);
	}
}

void SetRate(float inRate)
{
	std::lock_guard guard(parameterMutex);
	rate = inRate;
}

//Akcije povezane s celotnim sistemom
void SetBufferSize(float inBufferSize)
{
	std::lock_guard guard(parameterMutex);
	bufferSize = inBufferSize;
	dataBuffer.SetMaxBufferSize(bufferSize);
}

void SetDataSelector(float inDataSelector)
{
	std::lock_guard guard(parameterMutex);
	dataSelector = inDataSelector;
}

void SetBufferOffset(float inBufferOffset)
{
	std::lock_guard guard(parameterMutex);
	bufferOffset = inBufferOffset;
}

void SetBufferDuration(float inBufferDuration)
{
	std::lock_guard guard(parameterMutex);
	bufferDuration = inBufferDuration;
}

//Zahteva za izpis podatkov
//Za testne namene
void GetData(float getData)
{
	uint32_t data;
	uint16_t counter;
	bool thresholdExceeded;

	std::lock_guard guard(parameterMutex);
	int currBufferSize = dataBuffer.GetVector().size();
	if (bufferOffset > currBufferSize || (bufferOffset + bufferDuration) > currBufferSize)
	{
		//Izven dosega bufferja
		std::cout << "Buffer index out of range" << std::endl;
	}
	else
	{
		std::lock_guard coutGuard(coutMutex);
		std::cout << std::boolalpha;
		std::cout << "Counter	" << "Value	" << "Threshold exceeded	" << std::endl;
		for (int i = bufferOffset; i < bufferOffset + bufferDuration; ++i)
		{
			data = dataBuffer.GetVector()[i];
			counter = signals[dataSelector]->GetCounter(data);
			thresholdExceeded = signals[dataSelector]->IsThresholdExceeded();

			if (dataSelector == SignalType::Sine)
			{
				auto sineSignal = std::dynamic_pointer_cast<SineSignal>(signals[Sine]);
				int16_t signal = sineSignal->GetSignal(data);

				std::cout << counter << "	" << signal << "	" << thresholdExceeded << std::endl;
			}
			else
			{
				auto randomSignal = std::dynamic_pointer_cast<PseudoRandomSignal>(signals[Random]);
				uint16_t signal = randomSignal->GetSignal(data);

				std::cout << counter << "	" << signal << "	" << thresholdExceeded << std::endl;
			}

			
		}
	}
}
/*******************************GENERIRANJE SIGNALOV************************************/
void SignalGeneration()
{
	int samplingRate;//v mikrosekundah
	int copyRate;//v mikrosekundah
	int numOfSamples;
	int currentSample = 0;
	const int microInSeconds = 1000000;

	if (signals[dataSelector]->GetSamplingRate() >= 100)//Èas vzorèenja mora biti vsaj 100 mikrosekund
	{
		samplingRate = signals[dataSelector]->GetSamplingRate();
	}
	else
	{
		samplingRate = 100;
	}

	while (true)
	{
		parameterMutex.lock();//Zaklenemo vse paramete in podatke ki se uporabljajo za izraèun podatkov
		
		copyRate = microInSeconds / rate;//pretvorba èasa kopiranja generiranih podatkov v mikrosekunde
		numOfSamples = copyRate / samplingRate;//Koliko vzorec generiramo preden podatke preberemo
		dataBuffer.SetMaxBufferSize(bufferSize);
		
		try
		{	//Generiranje vzorca signala
			signals[dataSelector]->GenerateData();
			currentSample++;

			if (currentSample >= numOfSamples)
			{
				//Preberemo podatke;
				CircularBuffer generatedData = signals[dataSelector]->GetGeneratedData();
				for (const uint32_t data : generatedData.GetVector())
				{
					dataBuffer.AddData(data);
				}

				signals[dataSelector]->ClearDataBuffer();
				currentSample = 0;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
		
		parameterMutex.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(samplingRate));
	}
}
/******************************BRANJE VHODNIH UKAZOV************************************/
void GetInput()
{
	while (true)
	{
		//std:cout bo potrebno sinhronizirati med nitmi
		coutMutex.lock();
		std::cout << ">>>";
		coutMutex.unlock();
		std::string input;
		std::vector<std::string> tokens;
		//Vhod s strani uporabnika
		std::getline(std::cin, input);
		//Iz vhoda pridobi ime ukaza ter njegov argument
		commandInterpreter.TokenizeInput(input, ' ', tokens);

		std::string commandName;
		float parameterValue;
		if (commandInterpreter.ParseCommand(tokens, commandName, parameterValue))
		{
			//Dodaj ukaz v èakalno vrsto
			//Potrebna sinhronizacija 
			commandMapper[commandName](parameterValue);
		}
		else
		{
			//Neveljaven ukaz
			coutMutex.lock();
			std::cout << "Invalid command!" << std::endl;
			std::cout << "Type Help for more information" << std::endl;
			coutMutex.unlock();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void RegisterCommand(const std::string& commandName, CommandAction action)
{
	commandMapper[commandName] = action;
}

int main()
{
	//Podprti ukazi
	commandInterpreter.AddCommand(Command("Frequency", 1.0, 1000.0));
	commandInterpreter.AddCommand(Command("Amplitude", 0.0, 1.0));
	commandInterpreter.AddCommand(Command("Phase", -180.0, 180.0));
	commandInterpreter.AddCommand(Command("MinAmplitude", 0.0, 1.0));
	commandInterpreter.AddCommand(Command("MaxAmplitude", 0.0, 1.0));
	commandInterpreter.AddCommand(Command("Threshold", 0.0, 1.0));
	commandInterpreter.AddCommand(Command("Rate", 1.0f, 1000.0f));
	commandInterpreter.AddCommand(Command("BufferSize", 1.0, 1000.0));
	commandInterpreter.AddCommand(Command("DataSelector", 0, 1.0));
	commandInterpreter.AddCommand(Command("BufferOffset", 1.0f, 1000));
	commandInterpreter.AddCommand(Command("BufferDuration", 1.0, 1000.0));
	commandInterpreter.AddCommand(Command("GetData", -INFINITY, INFINITY));//Za testne namene

	//Povezava ukazov z akcijami
	//Dodati je potrebno še Help in Exit
	RegisterCommand("Frequency", SetFrequency);
	RegisterCommand("Amplitude", SetAmplitude);
	RegisterCommand("Phase", SetPhase);
	RegisterCommand("MinAmplitude", SetMinAmplitude);
	RegisterCommand("MaxAmplitude", SetMaxAmplitude);
	RegisterCommand("Threshold", SetThreshold);
	RegisterCommand("Rate", SetRate);
	RegisterCommand("DataSelector", SetDataSelector);
	RegisterCommand("BufferSize", SetBufferSize);
	RegisterCommand("BufferOffset", SetBufferOffset);
	RegisterCommand("BufferDuration", SetBufferDuration);
	RegisterCommand("GetData", GetData);

	std::thread commandInterpreterThread(GetInput);//Branje ukazov uporabnika

	SignalGeneration();//Gereriranje izbranega signala

	commandInterpreterThread.join();
	std::cout << "Application closed";

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
