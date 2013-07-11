#pragma once
#include "stdafx.h"
class BuildingButton{
public:
	int upImg,downImg;
	string title, tooltip;
	string titleFont,tooltipFont;
	wstring type;
	int titleColor[4],tooltipColor[4];
	int backgroundID;
	bool canBuild;
};