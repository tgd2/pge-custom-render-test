#pragma once

#include "olcPixelGameEngine.h"

// Graphics
class gfx
{
private:
	std::map<std::string, olc::Renderable*> Graphics;

public:
	gfx() {};
	~gfx()
	{
		for (auto Gfx : Graphics) // Not reference to avoid memory error
		{
			if (Gfx.second) delete Gfx.second;
		}
	};

	olc::Renderable* Load(std::string sFileName, bool bFilter, bool bClamp)
	{
		std::string hash = sFileName + std::to_string(bFilter) + std::to_string(bClamp);
		if (Graphics.count(hash) > 0)
		{
			return Graphics.at(hash);
		}
		else
		{
			olc::Renderable* NewGfx = new olc::Renderable();
			NewGfx->Load("./assets/gfx/" + sFileName, nullptr, bFilter, bClamp);
			if (NewGfx->Sprite()->width == 0) std::cout << "Missing file: " << sFileName;
			Graphics.insert(std::pair(hash, NewGfx));
			return NewGfx;
		}
	}
};