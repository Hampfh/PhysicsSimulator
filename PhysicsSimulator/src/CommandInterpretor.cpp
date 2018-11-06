#include "CommandInterpretor.h"

void RunInterpreter(Universe* universe, float *simulation_speed) {
	while (true) {
		std::string input;
		std::getline(std::cin, input);
		ConsoleInterpretation(&input, universe, simulation_speed);	
	}
}

void ConsoleInterpretation(std::string* command, Universe* universe, float* simulation_speed) {
	const int commandMaxArguments = 4;
	std::string input[commandMaxArguments];
	std::stringstream ss(*command);
	std::string tok;

	int i = 0;
	while (std::getline(ss, tok, ' ')) {
		if (i > commandMaxArguments) { break; }
		input[i] = tok;
		i++;
	}

	// Commands
	if (input[0] == "/target") {
		if (IsNumber(input[1])) {
			const int id = std::stoi(input[1]);
			auto object = universe->GetObjectWithId(id);

			if (object == nullptr) {
				std::cerr << "ERROR: No object with ID:" << id << " found" << std::endl;
				return;
			}

			if (input[2] == "setMass") {
				if (IsNumber(input[3])) {
					const float value = std::stof(input[3]);

					// Perform request
					object->SetMass(value);
					std::cout << "Mass successfully changed to " << value << std::endl;
				}
				else { std::cerr << "ERROR: No number was entered" << std::endl; }
			}
			else if (input[2] == "setRadius") {
				if (IsNumber(input[3])) {
					const int value = std::stoi(input[3]);

					// Perform request
					object->SetRadius(value);
					std::cout << "Radius successfully changed to " << value << std::endl;
				}
				else { std::cerr << "ERROR: No number was entered" << std::endl; }
			}
			else if (input[2].empty()) { std::cout << "System: Found object <" << object << ">" << std::endl; }
			else { std::cerr << "ERROR: No such command" << std::endl; }
		}
		else { std::cout << "ERROR: Input was not number" << std::endl; }
	} else if (input[0] == "/toggle") {
		if (input[1] == "forces") {
			std::cout << "SETTING OFF" << std::endl;
		} else if (input[1] == "velocity") {
			std::cout << "SETTING OFF" << std::endl;
		} else {
			std::cerr << "ERROR: No specifier" << std::endl;
		}
	} else if (input[0] == "/speed") {
		if (input[1] == "set") {
			if (IsNumber(input[2])) {
				*simulation_speed = std::stof(input[2]);
				std::cout << "Simulation speed changed to: " << *simulation_speed << std::endl;
			} 
		} else if (input[1] == "get") {
			std::cout << "Simulation speed: " << *simulation_speed << std::endl;
		}
	} else {
		std::cerr << "ERROR: No such command" << std::endl;
	}
}

bool IsNumber(const std::string& s) {
	// Checks the first character and the last character
	return (!(s.empty()) && isdigit(s[0]));
}