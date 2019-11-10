#include <blue/FontUtils.hpp>
#include "blue/TextureUtils.hpp"
#include "blue/ResourcesPath.h"
#include "blue/Context.hpp"

#include <SDL2/SDL.h>
#include "stb/image.h"

#include "stb/truetype.h"
#include "stb/image_write.h"

namespace
{
	auto stb_buffer = std::make_shared<std::vector<char>>();
	void my_stbi_write_func(void* context, void* data, int size)
	{
		for (std::size_t index = 0; index < size; index++)
		{
			stb_buffer->push_back(*(reinterpret_cast<char*>(data) + index));
		}
	}

	FontUtils::Font read_ttf(const std::string& filepath)
	{
		auto data = std::make_shared<std::vector<char>>();
		SDL_RWops* file = SDL_RWFromFile(filepath.c_str(), "rb");

		if (file != nullptr)
		{
			int size = file->size(file);
			//Initialize data
			data->resize(size);
			file->read(file, &(*data)[0], size, 1);
			//Close file handler
			SDL_RWclose(file);
			blue::Context::logger().info("Succesfuly read TTF data.");
		}
		else
		{
			blue::Context::logger().error("Unable to open file, SDL error: {}", SDL_GetError());
		}

		/* prepare font */
		stbtt_fontinfo info;
		if (!stbtt_InitFont(&info, reinterpret_cast<unsigned char*>(data->data()), 0))
		{
			blue::Context::logger().error("Failed to load TTF: {}", filepath);
			return { };
		}

		FontUtils::Font font;
		font.raw_ttf_font = data;
		font.info = info;
		return font;
	}
}

namespace FontUtils
{
	FontUtils::Font FontUtils::read_ttf_absolute(const std::string& filepath)
	{
		return ::read_ttf(filepath);
	}

	CreateTextureEntity create_text(const Font& font, const std::string& text, std::uint16_t width, std::uint16_t height, std::uint16_t line_height)
	{
		stb_buffer->clear();

		/* create a bitmap for the phrase */
		auto bitmap = std::make_shared<std::vector<char>>();
		bitmap->resize(width * height);

		/* calculate font scaling */
		float scale = stbtt_ScaleForPixelHeight(&font.info, line_height);

		int x = 0;

		int ascent, descent, lineGap;
		stbtt_GetFontVMetrics(&font.info, &ascent, &descent, &lineGap);

		ascent *= scale;
		descent *= scale;

		int i;
		for (i = 0; i < strlen(text.c_str()); ++i)
		{
			/* get bounding box for character (may be offset to account for chars that dip above or below the line */
			int c_x1, c_y1, c_x2, c_y2;
			stbtt_GetCodepointBitmapBox(&font.info, text.c_str()[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

			/* compute y (different characters have different heights */
			int y = ascent + c_y1;

			/* render character (stride and offset is important here) */
			int byteOffset = x + (y * width);
			stbtt_MakeCodepointBitmap(&font.info, reinterpret_cast<unsigned char*>(bitmap->data()) + byteOffset, c_x2 - c_x1, c_y2 - c_y1, width, scale, scale, text.c_str()[i]);

			/* how wide is this character */
			int ax;
			stbtt_GetCodepointHMetrics(&font.info, text.c_str()[i], &ax, 0);
			x += ax * scale;

			/* add kerning */
			int kern;
			kern = stbtt_GetCodepointKernAdvance(&font.info, text.c_str()[i], text.c_str()[i + 1]);
			x += kern * scale;
		}

		stbi_write_bmp_to_func(&my_stbi_write_func, nullptr, width, height, 1, bitmap->data());

		CreateTextureEntity entity{};
		entity.data = stb_buffer;
		entity.width = width;
		entity.height = height;
		entity.passedDataFormat = TexturePassedDataFormat::RED_INTEGER;
		entity.passedDataComponentSize = TexturePassedDataComponentSize::UNSIGNED_BYTE;
		return entity;
	}

	FontUtils::Font FontUtils::read_ttf_relative(const std::string& filepath)
	{
		auto absolute_path = paths::getResourcesPath() + filepath;
		return ::read_ttf(absolute_path);
	}
}
