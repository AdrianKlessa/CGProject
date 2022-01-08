#include "Texture.h"

#include <fstream>
#include <iterator>
#include <vector>
#include "picopng.h"

typedef unsigned char byte;

GLuint Core::LoadTexture( const char * filepath )
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::ifstream input( filepath, std::ios::binary );
	std::vector<char> buffer((
		std::istreambuf_iterator<char>(input)), 
		(std::istreambuf_iterator<char>()));

	unsigned long w, h;
	std::vector<unsigned char> decoded;
	decodePNG(decoded, w, h, (unsigned char*)&buffer[0], buffer.size(), true);
	
    // invert vertically
    std::vector<unsigned char> image(decoded.size(), 0);
    if (w && h) {
        for (int y = 0; y < h; y++) {
            int y2 = h - 1 - y;
            int n = 4 * w; // rgba
            memcpy(image.data() + (y2 * n), decoded.data() + (y * n), n * sizeof(unsigned char));
        }
    }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}

void Core::SetActiveTexture(GLuint textureID, const char * shaderVariableName, GLuint programID, int textureUnit)
{
	glUniform1i(glGetUniformLocation(programID, shaderVariableName), textureUnit);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}
