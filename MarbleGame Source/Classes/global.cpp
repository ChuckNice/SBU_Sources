#include "global.h"

// singleton getter for global
Global* s_Global()
{
	static Global s_global;
	return &s_global;
}

Global::Global()
{
}