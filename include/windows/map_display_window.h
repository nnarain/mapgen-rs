#ifndef WINDOWS_MAP_DISPLAY_WINDOW_H
#define WINDOWS_MAP_DISPLAY_WINDOW_H

#include "windows/window.h"
#include "generator/map_generator_manager.h"

#include <imgui.h>
#include "imgui_extension/imgui_extension.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <vector>

class MapDisplayWindow : Window
{
public:
	MapDisplayWindow(MapGeneratorManager& manager) :
		manager_(manager),
		selected_generator_(0)
	{
		generator_names_ = manager.getGeneratorNames();


		// DEBUG: Generate a texture
		sf::Image img;
		img.create(512, 512);
		for (int x = 0; x < 512; ++x)
		{
			for (int y = 0; y < 512; ++y)
			{
				img.setPixel(x, y, sf::Color(255, 0, 0, 255));
			}
		}
		texture_.create(512, 512);
		texture_.update(img);
		sprite_.setTexture(texture_);
	}

	virtual bool update() override
	{
		//auto flags = ImGuiWindowFlags_MenuBar;

		if (ImGui::Begin("Map"))
		{
			drawMapSelection();
			drawMap();

			ImGui::End();
		}

		return false;
	}

	void updateTexture(sf::Uint8* buffer)
	{
		texture_.update(buffer);
	}

private:
	void drawMapSelection()
	{
		static int current_generator = 0;
		bool generator_changed = ImGui::Combo("Generators", &current_generator, generator_names_, generator_names_.size());

		if (generator_changed)
		{
			manager_.setCurrentGenerator(current_generator);
		}
	}

	void drawMap()
	{
		auto window_size = ImGui::GetContentRegionAvail();
		auto texture_size = texture_.getSize();
		auto x_scale = window_size.x / texture_size.x;
		auto y_scale = window_size.y / texture_size.y;
		sprite_.setScale(x_scale, y_scale);

		ImGui::Image(sprite_);
	}

	MapGeneratorManager& manager_;
	std::vector<std::string> generator_names_;
	std::vector<std::string> layer_names_;
	int selected_generator_;

	sf::Texture texture_;
	sf::Sprite sprite_;
};

#endif  // WINDOWS_MAP_DISPLAY_WINDOW_H