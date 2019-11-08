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

	void bind(ShaderProgram program)
	{
		//todo
		glUniform1i(glGetUniformLocation(program.ID, "texture1"), 0);
	}
private:
	GLuint textureId;
	GLint width, height, nrChannels;
};


#endif //OPENGLSETUP_TEXTURE_H
