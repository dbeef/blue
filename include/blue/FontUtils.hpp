#pragma once

#include <vector>
#include <memory>
#include <string>

#include "blue/gpu/GpuEntities.hpp"

#include "stb/truetype.h"
#include "stb/image_write.h"

namespace FontUtils
{
	struct Font
	{
		std::shared_ptr<std::vector<char>> raw_ttf_font;
		stbtt_fontinfo info;
	};

	// TODO: Use flags for reading properties (relative/absolute?)
	Font read_ttf_relative(const std::string& filepath);
	Font read_ttf_absolute(const std::string& filepath);
	CreateTextureEntity create_text(const Font& font, const std::string& text, std::uint16_t width, std::uint16_t height, std::uint16_t line_height);
}
