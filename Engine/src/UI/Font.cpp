#include<iostream>

#include <UI\Font.h>

using namespace std;

Font::Font(const char* filename, int fontSize) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, filename, 0, &face))
		std::cout << "ERROR: Failed to load font: " << filename << std::endl;

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR: Freetype failed to load glyph";
			continue;
		}

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		//Texture Options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,

			textureID,
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	InitGLBuffers();
}

Font::~Font() {

}

void Font::InitGLBuffers() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Font::GetSize(std::string text, float* width, float* height) {
	string::const_iterator iter;
	for (iter = text.begin(); iter != text.end(); iter++) {
		Character glyph = characters[*iter];
		*width += glyph.width;
		if (*height < glyph.height)
			*height = glyph.height;
	}
}

void Font::RenderText(ShaderProgram* shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, Color* color) {
	shader->Use();

	glUniform3f(glGetUniformLocation(shader->GetProgram(), "textColor"), color->r, color->g, color->b);
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(vertexArrayID);

	string::const_iterator iter;
	for (iter = text.begin(); iter != text.end(); iter++) {
		Character glyph = characters[*iter];

		GLfloat xpos = x + glyph.bearingX * scale;
		GLfloat ypos = y - (glyph.height - glyph.bearingY) * scale;

		GLfloat width = glyph.width * scale;
		GLfloat height = glyph.height * scale;

		GLfloat vertices[6][4] = {
			{ xpos, ypos + height, 0.0, 0.0 },
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + width, ypos, 1.0, 1.0 },

			{ xpos, ypos + height, 0.0, 0.0 },
			{ xpos + width, ypos, 1.0, 1.0 },
			{ xpos + width, ypos + height, 1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, glyph.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (glyph.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}