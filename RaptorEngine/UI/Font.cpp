#include<iostream>

#include <UI\Font.h>

using namespace std;

//But first fix the out of bounds exception in the text renderer

Font::Font() {

}

Font::~Font() {

}

void Font::GetSize(std::string text, float* width, float* height) {
	string::const_iterator iter;
	for (iter = text.begin(); iter != text.end(); iter++) {
		Glyph glyph = glyphs[*iter];
		*width += glyph.width;
		if (*height < glyph.height)
			*height = glyph.height;
	}
}