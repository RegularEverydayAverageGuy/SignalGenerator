#include "CommandInterpreter.h"

void CommandInterpreter::AddCommand(Command command)
{
	supportedCommands.push_back(command);
}

void CommandInterpreter::TokenizeInput(std::string input, char delimiter, std::vector<std::string>& tokens)
{
	tokens.clear();

	std::string token;
	std::istringstream tokenStream(input);

	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
}

bool CommandInterpreter::IsValid(std::string commandName)
{
	for (const Command& cmd : supportedCommands)
	{
		if (commandName == cmd.name)
		{
			return true;  //Ukaz obstaja
		}
	}

	return false;
}

bool CommandInterpreter::IsValid(std::string commandName, float argumentValue)
{
	for (const Command& cmd : supportedCommands) 
	{
		if (commandName == cmd.name && argumentValue >= cmd.minLimit && argumentValue <= cmd.maxLimit) 
		{
			return true;  //Ukaz obstaja in vrednost argumenta je znotraj meja
		}
	}
	
	return false;
}

bool CommandInterpreter::ParseCommand(std::vector<std::string>& tokens, std::string& commandName, float& parameterValue)
{
	commandName = "";
	parameterValue = -1.0f;
	//Imamo zadostno število podatkov?
	if (tokens.size() >= 2)
	{
		try
		{
			parameterValue = std::stof(tokens[1]);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Invalid command parameter " << std::endl << "Detailed description: " << e.what() << std::endl;
			return false;
		}

		commandName = tokens[0];

		//So uporabnikovi vhodi veljavni
		if (IsValid(commandName, parameterValue))
		{
			return true;
		}

		return false;
	}

	return false;
}
