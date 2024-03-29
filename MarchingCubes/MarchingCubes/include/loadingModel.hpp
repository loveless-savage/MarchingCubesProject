#ifndef LOADINGMODEL_H
#define LOADINGMODEL_H

#include <vector>
#include <string>
#include <tuple>  
#include <memory>
#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

using glm::vec3;
using glm::vec2;
using glm::vec4;

using std::string;
using std::tuple;
using std::vector;
using std::map;
using std::shared_ptr;
using std::array;


namespace OGLF {


	struct Vertex {
		vec3 position_;
		vec3 normal_;
		vec2 texcoords_;
	};

	struct Texture {
		GLuint id_;
		string type_;
		aiString path_;
	};



	class Mesh {
	public:
		Mesh(const vector<Vertex>& vertices,
			const vector<GLuint>& indices,
			const vector<Texture>& textures
		):
			vertices_(vertices),
			indices_(indices),
			textures_(textures)
		{
			SetupMesh();
		}

		void Draw(GLuint program);

		// attach color vertex attribute to a single mesh
		void addColorBuffer(GLuint layout_loc);

		vector<Vertex> vertices_;
		vector<GLuint> indices_;
		vector<Texture> textures_;
	private:
		GLuint vbo_, vao_, ebo_;
		void SetupMesh();

	};
	// a Model holds multiple Meshes
	class Model {
		static map<string, Texture> loaded_mesh_;
	public:
		Model(string path) {
			LoadModel(path);
		}
		void Draw(GLuint program);

	private:
		vector<Mesh> meshes_;
		string dir_;

		// actually loads the numbers from the file- implements assimp
		void LoadModel(string path);
		void ProcNode(aiNode* node, const aiScene* scene);
		Mesh ProcMesh(aiMesh* mesh, const aiScene* scene);
		vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string type_name);
		GLuint LoadTexture(string filename);

	};


	// load model from file
	shared_ptr<Model> LoadModel(string filename);
}
#endif
