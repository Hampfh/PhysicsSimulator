#include "Core.h"
#include "PhysicsEngine.h"

int main(int argc, char* args[]) {
	Core theCore;
	PhysicsEngine pe;
	pe.AddBox(100, 20);
	return theCore.OnExecute();
}