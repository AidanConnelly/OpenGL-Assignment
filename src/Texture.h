//
// Created by aidan on 30/10/2019.
//

#ifndef OPENGLSETUP_TEXTURE_H
#define OPENGLSETUP_TEXTURE_H
#include <string>
#include <iostream>
#include <GL/glew.h>
#include "../shaderProgram.h"


class Texture {
public:
	Texture(std::string fromFile);

	void bind(ShaderProgram program, int slot)
	{
		//todo
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureId);
		GLint uniformLocation = glGetUniformLocation(program.ID, "ourTexture");
		glUniform1i(uniformLocation, slot);
	}

	// ~Texture();
private:
	unsigned char* data;
	GLuint textureId;
	GLint width, height, nrChannels;

};


#endif //OPENGLSETUP_TEXTURE_H
