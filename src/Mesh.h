//
// Created by aidan on 30/10/2019.
//

#ifndef OPENGLSETUP_MESH_H
#define OPENGLSETUP_MESH_H


#include <utility>
#include <vector>
#include "../shaderProgram.h"
#include "Texture.h"
#include "Vertex.h"
#include <glm/glm.hpp> //includes GLM
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/ext/matrix_clip_space.hpp> // GLM: perspective and ortho 
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr

#define BUFFER_OFFSET(a) ((void*)(a))

class MeshData
{
	public:
	MeshData()
	{
		std::cout << "calling empty constructor";
	}
	
	MeshData(std::vector<Vertex> vertexes, std::vector<Triangle> triangles, std::vector<std::string> texturePaths)
	{
		this->vertexes = std::move(vertexes);
		this->triangles = std::move(triangles);
		this->texturePaths = texturePaths;
	}

	void BindTextures()
	{
		for(auto &x : texturePaths)
		{
			textures.push_back(Texture(x));
		}
	}

	std::vector<std::string> texturePaths;
	std::vector<Triangle> triangles;
	std::vector<Vertex> vertexes;
	std::vector<Texture> textures;
};

class Mesh
{
public:
	void createThisMesh()
	{
		CheckForOpenGLErrors();
		glGenVertexArrays(1, &VAO);
		CheckForOpenGLErrors();
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		CheckForOpenGLErrors();
		
		unsigned int cnt = vertexes.size();
		auto* pos = (float*)&vertexes[0];
		float x0 = pos[0];
		float x1 = pos[1];
		size_t sizeOfVertex = sizeof(Vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeOfVertex * cnt, pos, GL_STATIC_DRAW);
		CheckForOpenGLErrors();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(0 * sizeof(float))); //xyz
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(3 * sizeof(float))); //nrm
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(6 * sizeof(float))); //rgb
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(9 * sizeof(float))); //uv
		CheckForOpenGLErrors();
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		CheckForOpenGLErrors();

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		CheckForOpenGLErrors();

		unsigned int trigCount = triangles.size();
		size_t shouldb12Bytes = sizeof(Triangle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shouldb12Bytes * trigCount, &triangles[0], GL_STATIC_DRAW);
		CheckForOpenGLErrors();
	}

	void bind()
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	}

	Mesh(
		std::vector<Vertex>* vrtxes,
		std::vector<Triangle>* triags,
		std::vector<Texture>* textures
	)
	{
		this->vertexes = *vrtxes;
		this->triangles = *triags;
		this->textures = *textures;

		createThisMesh();
	}

	Mesh(MeshData mesh_data){

		this->vertexes = mesh_data.vertexes;
		this->triangles = mesh_data.triangles;
		this->textures = mesh_data.textures;
		
		createThisMesh();
	}

	void BindTextures(ShaderProgram program)
	{
		int hasTextureLocation = glGetUniformLocation(program.ID, "hasTexture");
		glUniform1f(hasTextureLocation, 0.0f);
		for (int i = 0; i < textures.size(); i++)
		{
			int hasTextureLocation = glGetUniformLocation(program.ID, "hasTexture");
			glUniform1f(hasTextureLocation, 1.0f);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0 + i);
			textures[i].bind(program, i);
			CheckForOpenGLErrors();
		}
	}

	GLuint EBO;
	GLuint VAO;
	std::vector<Triangle> triangles;
private:
	std::vector<Vertex> vertexes;
	std::vector<Texture> textures;
	GLuint VBO;

	void CheckForOpenGLErrors()
	{
		auto ErrorCheckValue = glGetError();
		if (ErrorCheckValue != GL_NO_ERROR)
		{
			fprintf(
				stderr,
				"ERROR: Could not create a VBO: %s \n");
			std::cout << std::endl;
			exit(-1);
		}
	}
};

class MultiMesh
{
public:
	MultiMesh(std::vector<Mesh *> meshes)
	{
		this->meshes = meshes;
	}
	
	std::vector<Mesh*> meshes;
	
	void BindTextures(const ShaderProgram& program){
		for(auto &a: meshes)
		{
			a->BindTextures(program);
		}
	}
};

class MeshInstance
{
public:
	explicit MeshInstance(MultiMesh* toMakeInstanceOf)
	{
		this->instanceOf = toMakeInstanceOf;
		scaling = glm::mat4(1.0f);
		translate = glm::mat4(1.0f);
	}

	void Draw(ShaderProgram program, glm::mat4 pv)
	{
		// Adding all matrices up to create combined matrix
		glm::mat4 mvp = pv * translate*scaling;

		//adding the Uniform to the shader
		//todo move into shader program class
		int mvpLoc = glGetUniformLocation(program.ID, "mvp");
		int selectedLoc = glGetUniformLocation(program.ID, "selected");
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1f(selectedLoc, selected?0.2:0.0);

		for (auto& singleMesh : instanceOf->meshes) {
			singleMesh->bind();
			glDrawElements(GL_TRIANGLES, 3 * singleMesh->triangles.size(), GL_UNSIGNED_INT, 0);
		}

		auto ErrorCheckValue = glGetError();
		std::cout << "";
	}

	MultiMesh* instanceOf;
	bool selected;

	void move(glm::vec3 translateBy)
	{
		translate = glm::translate(translate, translateBy);
	}

	void scale(float toScaleBy)
	{
		scaling = glm::scale(scaling, glm::vec3(1 + toScaleBy));
	}
private:
	glm::mat4 scaling;
	glm::mat4 translate;
};


#endif //OPENGLSETUP_MESH_H
