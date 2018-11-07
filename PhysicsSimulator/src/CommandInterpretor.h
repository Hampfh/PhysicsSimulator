#pragma once
#include "Universe.h"

class Universe;

void RunInterpreter(Universe* universe, float* simulation_speed);
void ConsoleInterpretation(std::string* command, Universe* universe, float* simulation_speed);
bool IsNumber(const std::string& s);