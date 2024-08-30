#pragma once

#include "olcPixelGameEngine.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct light
{
	olc::vf2d Position{ 0.0f, 0.0f };
	olc::vf2d Velocity{ 0.0f, 0.0f };
	olc::Pixel Tint{ olc::WHITE };
};

struct lightMap
{
	olc::Renderable* r;
	olc::Renderable* g;
	olc::Renderable* b;

	void Create(uint32_t Width, uint32_t Height, bool Filter, bool Clamp)
	{
		r = new olc::Renderable();
		r->Create(Width, Height, Filter, Clamp);

		g = new olc::Renderable();
		g->Create(Width, Height, Filter, Clamp);

		b = new olc::Renderable();
		b->Create(Width, Height, Filter, Clamp);	
	}
};

struct vf3d
{
	float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
	float length() { return std::sqrtf(x * x + y * y + z * z); };
	void normalise()
	{
		float Length{ length() };
		if (Length != 0.0f)
		{
			x /= Length;
			y /= Length;
			z /= Length;
		}
	};
};

inline auto Uint8_to_PlusMinusOne = [](const uint8_t& x)
	{
		return static_cast<float>(x) / 255.0f * 2.0f - 1.0f;
	};

inline auto PlusMinusOne_to_Uint8 = [](const float& x)
	{
		float ClampedX = std::clamp(x, -1.0f, 1.0f);
		return static_cast<uint8_t>((ClampedX + 1.0f) * 0.5f * 255.0f);
	};

inline auto ZeroToOne_to_Uint8 = [](const float& x)
	{
		float ClampedX = std::clamp(x, 0.0f, 1.0f);
		return static_cast<uint8_t>(ClampedX * 255.0f);
	};


