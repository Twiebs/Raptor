#include "Pixmap.hpp"

Pixmap::Pixmap(uint16 width, uint16 height, PixmapFormat format) :
	width(width), height(height), format(format) {
	data = new byte[width * height * (uint8)format];
}

Pixmap::~Pixmap() {
	delete[] data;
}