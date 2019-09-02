#define _USE_MATH_DEFINES
#include <string>
#include <fstream>
#include <sstream>

#include <iostream>
#include <stdexcept>
#include <FreeImage.h>
#include <random>
#include <cmath>
#include <cfloat>
#include <ctime>

#include <QDebug>

#include "loadingModel.hpp"

using namespace std;
using namespace OGLF;

extern string suffix_;

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0, 1);

map<string, Texture> Model::loaded_mesh_;

 // TODO
GLuint Model::LoadTexture(string filename)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	// load image
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(filename.c_str(), 0);//Automatocally detects the format(from over 20 formats!)
	FIBITMAP* imagen = FreeImage_Load(formato, filename.c_str());

	FIBITMAP* temp = imagen;
	imagen = FreeImage_ConvertTo32Bits(imagen);
	FreeImage_Unload(temp);

	int w = FreeImage_GetWidth(imagen);
	int h = FreeImage_GetHeight(imagen);
	//cout<<"The size of the image is: "<<textureFile<<" es "<<w<<"*"<<h<<endl; //Some debugging code

	char* pixeles = (char*)FreeImage_GetBits(imagen);
	//FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixeles);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	FreeImage_Unload(imagen);
	return textureID;
}

void Mesh::Draw(GLuint program) {
	// setup materials
	GLuint diffuse_count = 1;
	GLuint specular_count = 1;
	for (GLuint i = 0; i < textures_.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		stringstream ss;
		string num;
		string type = textures_[i].type_;
		if (type == "texture_diffuse")
			ss << diffuse_count++;
		else if (type == "texture_specular")
			ss << specular_count++;
		num = ss.str();
		string mattype = "material." + type + num;
		GLuint loc = glGetUniformLocation(program, mattype.c_str());
		glUniform1i(loc, i);
		glBindTexture(GL_TEXTURE_2D, textures_[i].id_);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw
	glBindVertexArray(vao_);
	//glDrawArrays(GL_TRIANGLES, 0, indices_.size());
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void Mesh::SetupMesh() {

// set up VBO: all the vertices of the model in an array
	// make a buffer and store its ID in vbo_
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	// copy the vertex coordinates into the buffer we just bound
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

	// Vertex array object setup
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	// Elememt buffer object setup- stores indices to connect vertices
	glGenBuffers(1, &ebo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	// copy indices into our freshly baked element buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), &indices_[0], GL_STATIC_DRAW);

// set up buffers available to the vertex shader
// glVertexAttribPointer( layout_location_#, buffer_element_size, type, normalized, stride, array_buffer_offset );
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal_));
	// texture coordination
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoords_));

}

// draw all meshes inside a model
void Model::Draw(GLuint program) {
	for (auto& mesh : meshes_)
		mesh.Draw(program);
}

// loads the models using assimp and processes them into our own defined formats
void Model::LoadModel(string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	// make sure we got a complete model!
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		throw runtime_error(string("Loading model error:") + importer.GetErrorString());
	}
	dir_ = path.substr(0, path.find_last_of('/'));
	// do all the node transformations, starting at the root node
	ProcNode(scene->mRootNode, scene);
}

void Model::ProcNode(aiNode* node, const aiScene* scene) {
	// convert all aiMeshes into our own Mesh objects
	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(ProcMesh(mesh, scene));
	}

	// process all children of the current node- here be recursion
	for (GLuint i = 0; i < node->mNumChildren; i++) {
		ProcNode(node->mChildren[i], scene);
	}
}


// convert an assimp-produced aiMesh object into our own Mesh object
Mesh Model::ProcMesh(aiMesh* mesh, const aiScene* scene) {
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	// process vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;
		// mv points to the current vertex
		const auto& mv = mesh->mVertices[i];
		if (!mesh->mNormals)
			throw runtime_error("model missing normal");
		// mn points to the current vertex's normal
		const auto& mn = mesh->mNormals[i];

		// capture the current vertex's position
		v.position_ = glm::vec3(mv.x, mv.y, mv.z);
		// ...and its normal into v
		v.normal_ = glm::vec3(mn.x, mn.y, mn.z);
		// if there are textures attached, capture its texture coordinates
		if (mesh->HasTextureCoords(0)) {
			// mtex points to the current vertex's texture coordinate
			const auto& mtex = mesh->mTextureCoords[0][i];
			v.texcoords_ = glm::vec2(mtex.x, mtex.y);
		}else{ // if no textures, give v.texcoords_ some comfort food
			v.texcoords_ = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(v);
	}

	// process indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++) {
		// face stores a copy of the current face
		aiFace face = mesh->mFaces[i];
		// store all indices involved in defining the current face
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process material // TODO
	if (mesh->mMaterialIndex >= 0) {
		// material stores whichever scene material was selected for this mesh
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// find in recoreds
		string texfile = str.C_Str();
		auto texiter = loaded_mesh_.find(texfile);
		if (texiter != loaded_mesh_.end())
		{
			// used loaded texture
			textures.push_back(texiter->second);
		}
		else {
			Texture texture;
			texture.id_ = LoadTexture(dir_ + '/' + str.C_Str());
			texture.type_ = typeName;
			texture.path_ = str;
			textures.push_back(texture);
			loaded_mesh_[texfile] = texture;
		}
	}
	return textures;
}



shared_ptr<Model> OGLF::LoadModel(string filename) {
	shared_ptr<Model> model = std::make_shared<Model>(filename);
	return model;
}



