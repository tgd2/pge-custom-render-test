#define OLC_PGE_APPLICATION
#define OLC_GFX_OPENGL33
#include "olcPixelGameEngine.h"

#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "imgui\imgui_impl_pge.h"

#include "stage.h"
#include "helpers.h"
#include "lightSource.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

olc::vi2d GAME_RESOLUTION{ 320, 180 }; // Most Amiga games are just using 320x200
olc::vi2d LIGHT_RESOLUTION{ 128, 128 };

olc::vi2d RENDER_SCALE{ 1, 1 };

olc::vi2d RENDER_RESOLUTION{ GAME_RESOLUTION.x * RENDER_SCALE.x, GAME_RESOLUTION.y * RENDER_SCALE.y };
olc::vi2d WINDOW_RESOLUTION{ RENDER_RESOLUTION };

int MAX_LIGHT_COUNT{ 512 };
float MAX_LIGHT_SPEED{ 10.0f };

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Example : public olc::PixelGameEngine
{
private:
	olc::imgui::PGE_ImGUI pge_imgui;

	uint8_t m_LightLayer;
	uint8_t m_NormalLayer;
	uint8_t m_GameLayer;

	gfx* Gfx{ nullptr };
	olc::Renderable* NormalMap{ nullptr };
	olc::Renderable* DiffuseMap{ nullptr };

	lightSource* LightSource{ nullptr };

	int Height{ 8 };
	int Radius{ 64 };
	float Intensity{ 1.0f };
	int CountLightSources{ 24 };

	std::vector<light> Lights;

public:
	Example() : pge_imgui(false)
	{
		sAppName = "Lighting and Shader Test";
	}

public:
	bool OnUserCreate() override
	{
		srand((int)time(NULL));

		// ImGui:
		m_LightLayer = CreateLayer();
		EnableLayer(m_LightLayer, true);

		m_NormalLayer = CreateLayer();
		EnableLayer(m_NormalLayer, true);

		m_GameLayer = CreateLayer();
		EnableLayer(m_GameLayer, true);

		SetLayerCustomRenderFunction(0, std::bind(&Example::DrawUI, this));

		Gfx = new gfx{};
		NormalMap = Gfx->Load("NormalMap.png", false, true);
		DiffuseMap = Gfx->Load("DiffuseMap.png", false, true);

		LightSource = new lightSource(LIGHT_RESOLUTION, Height, Radius, Intensity);

		for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
		{
			light NewLight
			{
				{ std::lerp(0.0f, (float)GAME_RESOLUTION.x, float(rand()) / float(RAND_MAX)), std::lerp(0.0f, (float)GAME_RESOLUTION.y, float(rand()) / float(RAND_MAX))},
				{ std::lerp(-MAX_LIGHT_SPEED, MAX_LIGHT_SPEED, float(rand()) / float(RAND_MAX)), std::lerp(-MAX_LIGHT_SPEED, MAX_LIGHT_SPEED, float(rand()) / float(RAND_MAX))},
				olc::Pixel(rand() % 256, rand() % 256, rand() % 256)
			};

			Lights.push_back(NewLight);
		}

		return true;
	}

	void DrawUI(void)
	{
		pge_imgui.ImGui_ImplPGE_Render();
	}

	bool OnUserDestroy() override
	{
		if (Gfx) delete Gfx;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		ImGui::Begin("Settings");

		ImGui::SliderInt("Number of Lights", &CountLightSources, 0, MAX_LIGHT_COUNT);
		ImGui::SliderInt("Height", &Height, 0, (float)LIGHT_RESOLUTION.x * 0.5f);
		ImGui::SliderInt("Radius", &Radius, 0, (float)LIGHT_RESOLUTION.x * 0.5f);
//		ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 3.0f);
		LightSource->Update( Height, Radius, Intensity);

		// Move lights
		for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
		{
			light& Light { Lights[i] };
			Light.Position += Light.Velocity * fElapsedTime;
			if (Light.Position.x > ((float)GAME_RESOLUTION.x - ((float)LIGHT_RESOLUTION.x * 0.5f)) && Light.Velocity.x > 0.0f ||
				Light.Position.x < -((float)LIGHT_RESOLUTION.x * 0.5f) && Light.Velocity.x < 0.0f) Light.Velocity.x *= -1.0f;
			if (Light.Position.y > ((float)GAME_RESOLUTION.y - ((float)LIGHT_RESOLUTION.y * 0.5f)) && Light.Velocity.y > 0.0f ||
				Light.Position.y < -((float)LIGHT_RESOLUTION.y * 0.5f) && Light.Velocity.y < 0.0f) Light.Velocity.y *= -1.0f;
		}

		SetDrawTarget(m_GameLayer);
		DrawDecal({ 0, 0 }, DiffuseMap->Decal(), RENDER_SCALE);
		DrawStringPropDecal({ 3.0f, 4.0f }, std::format("PGE - Dynamic Lighting and Shader Test - {} fps", GetFPS()), olc::WHITE, { 1.0f, 1.0f });

		SetDrawTarget(m_NormalLayer);
		DrawDecal({ 0, 0 }, NormalMap->Decal(), RENDER_SCALE);

		SetDrawTarget(m_LightLayer);
		SetDecalMode(olc::DecalMode::ADDITIVE);
		for (int i = 0; i < CountLightSources; ++i)
		{
			light& Light{ Lights[i] };
			olc::vf2d LightPosition{ Light.Position - LIGHT_RESOLUTION * 0.5f };
			DrawDecal(LightPosition * RENDER_SCALE, LightSource->Light->Decal(), RENDER_SCALE, Light.Tint);
		}
		SetDecalMode(olc::DecalMode::NORMAL);

		ImGui::End();

		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(320, 180, 4, 4, false, false))
		demo.Start();
	return 0;
}

