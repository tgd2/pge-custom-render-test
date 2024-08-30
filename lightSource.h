#pragma once

#include "olcPixelGameEngine.h"
#include "helpers.h"

class lightSource
{
private:
	int Height{ 8 };
	int Radius{ 32 };
	float Intensity{ 1.0f };

public:
	std::unique_ptr<olc::Renderable> Light{ new olc::Renderable() };

public:
	lightSource(olc::vi2d Size, float Height_, float Radius_, float Intensity_)
	{
		Light = std::make_unique<olc::Renderable>();
		Light->Create(Size.x, Size.y, false, true);
		ForceUpdate(Height_, Radius_, Intensity_);
	}

	void Update(float Height_, float Radius_, float Intensity_)
	{
		if (Height == Height_ && Radius == Radius_ && Intensity == Intensity) return; // no change
		ForceUpdate(Height_, Radius_, Intensity_);
	};

private:
	void ForceUpdate(float Height_, float Radius_, float Intensity_)
	{
		Height = Height_;
		Radius = Radius_;
		Intensity = Intensity;

		olc::vi2d size = Light->Sprite()->Size();

		vf3d Source{ size.x * 0.5f, size.y * 0.5f, (float)Height };
		float RadiusDiagonal = std::sqrtf(Radius * Radius + Height * Height);

		for (int j = 0; j < size.x; ++j)
		{
			for (int i = 0; i < size.y; ++i)
			{
				vf3d Fragment{ (float)j, (float)i , 0.0f };
				vf3d Ray{ Fragment.x - Source.x, Fragment.y - Source.y, Fragment.z - Source.z };

				float DecayLength = Ray.length(); // std::sqrtf(Ray.x * Ray.x + Ray.y * Ray.y);
				float DecayFactor{ 0.0f };
				if (RadiusDiagonal > 0.0f)
				{
					DecayFactor = std::clamp(Intensity - DecayLength / RadiusDiagonal, 0.0f, 1.0f);
				}

				Ray.normalise();

				olc::Pixel RenderPixel
				{
					PlusMinusOne_to_Uint8(Ray.x),
					PlusMinusOne_to_Uint8(Ray.y),
					PlusMinusOne_to_Uint8(Ray.z),
					ZeroToOne_to_Uint8(DecayFactor),
				};

				Light->Sprite()->SetPixel(olc::vi2d{ j, i }, RenderPixel);
			}
		}
		Light->Decal()->Update();
	}

};