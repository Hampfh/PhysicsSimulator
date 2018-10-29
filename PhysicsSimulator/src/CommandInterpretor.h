#pragma once
#include "Universe.h"

class Universe;

void CheckConsole(Universe* universe);
void ConsoleInterpretation(std::string* command, Universe* universe);
bool IsNumber(const std::string& s);