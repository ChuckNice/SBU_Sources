/*	
	Author: Angad Sidhu
*/

#pragma once
#include "stdafx.h"
#include "SSSF_SourceCode\text\TextGenerator.h"

class GroupGameTextGenerator : public TextGenerator
{
private:
	// THIS IS THE TEXT THIS OBJECT WILL UPDATE EACH FRAME
	wstring textToGenerate;
	float fps;
public:
	// NOTHING TO INITIALIZE OR DESTROY
	GroupGameTextGenerator()	{fps=0;}
	~GroupGameTextGenerator()	{}

	// DEFINED IN GroupGameTextGenerator.cpp
	void appendTargetFPS(Game *game);
	void initText(Game *game);
	void updateText(Game *game);
};