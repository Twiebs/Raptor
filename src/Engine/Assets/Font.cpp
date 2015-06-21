#include "Font.hpp"


void Font::GetSize(const std::string& text, uint16* width, uint16* height) {
	std::string::const_iterator iter;
	for (iter = text.begin(); iter != text.end(); iter++) {
		Glyph glyph = glyphs[*iter];
		*width += glyph.width;
		if (*height < glyph.height)
			*height = glyph.height;
	}
}
