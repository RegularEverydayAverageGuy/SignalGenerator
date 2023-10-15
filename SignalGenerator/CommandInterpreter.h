#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <map>
#include <sstream>
#include <queue>

struct Command
{
	Command(std::string inName, float inMinLimit, float inMaxLimit) : name(inName), minLimit(inMinLimit), maxLimit(inMaxLimit) {};

	std::string name;
	float minLimit;
	float maxLimit;
};

class CommandInterpreter
{
public:
	CommandInterpreter() {};

	void AddCommand(Command command);
	void TokenizeInput(std::string input, char delimiter, std::vector<std::string>& tokens);

	bool IsValid(std::string commandName);
	bool IsValid(std::string commandName, float argumentValue);
	bool ParseCommand(std::vector<std::string>& tokens, std::string& commandName, float& parameterValue);

	std::vector<Command> supportedCommands;
};

