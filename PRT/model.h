/* Adapted from the online tutorials by Joey de Vries found at 'http://learnopengl.com/'
 * Loads Models
 */

#ifndef _MODEL_H
#define _MODEL_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "mesh.h"
#include "shader.h"

GLint TextureFromFile(const char *path, string directory);

class Model
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(string path);

    // Draws the model, and thus all its meshes
    virtual void Draw(Shader& shader);

    Mesh returnVertices();

    // Default constructor.
    Model();

private:
    /*  Model Data  */
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded; // Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    float hitParam;                  // Stores the hit parameter for the mouse click.
    bool selectParam;                // Stores the selected parameter for the mouse click.
    glm::vec3 translateVector;       // Stores the translate vector for the transformation of model.
    float pitch;                     // Stores the rotation angles for the transformation of the model.
    float yaw;
    float roll;

    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
protected:
	virtual void loadModel(string path);
private:
    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif
