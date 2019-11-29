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
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr
#include <glm/gtx/transform.hpp>

class MeshData
{
	public:
	MeshData()
	{
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

	float opacity = 1;
	float specularExponent = 2;
	glm::vec3 ambient = glm::vec3(1.0,1.0,1.0);
	glm::vec3 specular = glm::vec3(1.0,1.0,1.0);
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

	void getBounds(glm::vec3& minPos, glm::vec3& maxPos)
	{
		minPos.x = vertexes[0].x;
		minPos.y = vertexes[0].y;
		minPos.z = vertexes[0].z;
		maxPos.x = vertexes[0].x;
		maxPos.y = vertexes[0].y;
		maxPos.z = vertexes[0].z;
		for(Vertex &v: vertexes)
		{
#define checkIndexMin(I) if(minPos.I>v.I){minPos.I = v.I;}
#define checkIndexMax(I) if(maxPos.I<v.I){maxPos.I = v.I;}
#define checkIndex(I) checkIndexMin(I); checkIndexMax(I);
			checkIndex(x);
			checkIndex(y);
			checkIndex(z);
		}
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
		this->ambient = mesh_data.ambient;
		this->opacity = mesh_data.opacity;
		this->specular = mesh_data.specular;
		this->specularExponent = mesh_data.specularExponent;

		createThisMesh();
	}

	void BindTextures(ShaderProgram program, int startingSlot)
	{
		int hasTextureLocation = glGetUniformLocation(program.ID, "hasTexture");
		glUniform1f(hasTextureLocation, 0.0f);
		for (int i = 0; i < textures.size(); i++)
		{
			glUniform1f(hasTextureLocation, 1.0f);
			textures[i].bind(program, i+startingSlot);
			CheckForOpenGLErrors();
		}
	}

	GLuint EBO;
	GLuint VAO;
	std::vector<Triangle> triangles;
	std::vector<Vertex> vertexes;
	std::vector<Texture> textures;


	float opacity = 1;
	float specularExponent;
	glm::vec3 ambient;
	glm::vec3 specular;
private:
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
	
	void BindTextures(const ShaderProgram& program, int startingSlot){
		for(auto &a: meshes)
		{
			a->BindTextures(program,startingSlot);
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

	void Draw(ShaderProgram program)
	{
		// Adding all matrices up to create combined matrix
		glm::mat4 m = translate*scaling;

		//adding the Uniform to the shader
		//todo move into shader program class
		int mLoc = glGetUniformLocation(program.ID, "m");
		int selectedLoc = glGetUniformLocation(program.ID, "selected");
        glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(m));
		glUniform1f(selectedLoc, selected?0.2:0.0);

		int ambLoc = glGetUniformLocation(program.ID, "ambCol");
		int opacityLoc = glGetUniformLocation(program.ID, "opacity");
		int specLoc = glGetUniformLocation(program.ID, "specCol");
		int specExpLoc = glGetUniformLocation(program.ID, "specExp");

		for (auto& singleMesh : instanceOf->meshes) {
			glUniform3f(ambLoc, singleMesh->ambient.x,singleMesh->ambient.y,singleMesh->ambient.z);
			glUniform1f(opacityLoc, singleMesh->opacity);
			glUniform3f(specLoc, singleMesh->specular.x,singleMesh->specular.y,singleMesh->specular.z);
			glUniform1f(specExpLoc, singleMesh->specularExponent);
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
