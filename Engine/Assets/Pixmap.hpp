#pragma once

#include <Core\Common.hpp>

enum class PixmapFormat : uint8 {
	RGB8 = 3,
	RGBA8 = 4,
};

class Pixmap {
public:
	Pixmap(uint16 width, uint16 height, PixmapFormat format);
	~Pixmap();

	uint16 width, height;
	PixmapFormat format;
	byte* data;

private:

};
