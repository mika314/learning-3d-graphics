#include "assimp-tutorial.hpp"
#include "load-program.hpp"
#include "load-texture.hpp"
#include <array>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <log/log.hpp>
#include <ranges>

namespace
{
  struct Vertex
  {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    static bgfx::VertexLayout msLayout;
  };

  bgfx::VertexLayout Vertex::msLayout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
    return r;
  }();

  struct Texture
  {
    bgfx::TextureHandle id;
    std::string type;
    std::string path;
  };
  class Mesh
  {
  public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    std::vector<Texture> textures;
    Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::vector<Texture> textures)
      : vertices(vertices), indices(indices), textures(textures)
    {
      setupMesh();
    }
    ~Mesh()
    {
      bgfx::destroy(ibh);
      bgfx::destroy(vbh);
    }
    void Draw(bgfx::ProgramHandle &shader)
    {
      bgfx::setIndexBuffer(ibh);
      bgfx::setVertexBuffer(0, vbh);
    }

  private:
    // render data
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    void setupMesh()
    {
      vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices.data(), vertices.size() * sizeof(vertices[0])), Vertex::msLayout);
      ibh = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), indices.size() * sizeof(indices[0])));
    }
  };

  class Model
  {
  public:
    Model(char *path) { loadModel(path); }
    void Draw(bgfx::ProgramHandle &shader)
    {
      for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
    }

  private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;
    void loadModel(std::string path)
    {
      Assimp::Importer import;
      const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
      if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
      {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
      }
      directory = path.substr(0, path.find_last_of('/'));
      processNode(scene->mRootNode, scene);
    }
    void processNode(aiNode *node, const aiScene *scene)
    { // process all the node’s meshes (if any)
      for (unsigned int i = 0; i < node->mNumMeshes; i++)
      {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
      }
      // then do the same for each of its children
      for (unsigned int i = 0; i < node->mNumChildren; i++)
      {
        processNode(node->mChildren[i], scene);
      }
    }
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
      std::vector<Vertex> vertices;
      std::vector<uint16_t> indices;
      std::vector<Texture> textures;
      for (auto i = 0U; i < mesh->mNumVertices; ++i)
        vertices.push_back(
          Vertex{.Position = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                 .Normal = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
                 .TexCoords = mesh->mTextureCoords[0]
                                ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
                                : glm::vec2{}});

      // process indices
      for (auto i = 0U; i < mesh->mNumFaces; ++i)
      {
        aiFace face = mesh->mFaces[i];
        for (auto j = 0U; j < face.mNumIndices; j++)
          indices.push_back(face.mIndices[j]);
      }

      // process material
      if (mesh->mMaterialIndex >= 0)
      {
        if (mesh->mMaterialIndex >= 0)
        {
          aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
          std::vector<Texture> diffuseMaps =
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
          textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
          std::vector<Texture> specularMaps =
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
          textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
      }
      return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
      std::vector<Texture> textures;
      for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
      {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = loadTexture(str.C_Str());
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
      }
      return textures;
    }
  };

} // namespace

AssimpTutotrial::AssimpTutotrial(sdl::Window &aWindow, int aWidth, int aHeight)
  : window(aWindow), width(aWidth), height(aHeight)
{
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
  bgfx::setViewRect(0, 0, 0, width, height);
  woodenContainerTexture = loadTexture("wooden-container.png");
  woodenContainerSpecularTexture = loadTexture("wooden-container-specular.png");
  materialDiffuse = bgfx::createUniform("materialDiffuse", bgfx::UniformType::Sampler);
  transUniform = bgfx::createUniform("trans", bgfx::UniformType::Mat4);
  viewPosUniform = bgfx::createUniform("viewPos", bgfx::UniformType::Vec4);
  materialAmbient = bgfx::createUniform("materialAmbient", bgfx::UniformType::Vec4);
  materialSpecular = bgfx::createUniform("materialSpecular", bgfx::UniformType::Sampler);
  materialShininess = bgfx::createUniform("materialShininess", bgfx::UniformType::Vec4);
  lightPositionUniform = bgfx::createUniform("lightPosition", bgfx::UniformType::Vec4);
  lightAmbient = bgfx::createUniform("lightAmbient", bgfx::UniformType::Vec4);
  lightDiffuse = bgfx::createUniform("lightDiffuse", bgfx::UniformType::Vec4);
  lightSpecular = bgfx::createUniform("lightSpecular", bgfx::UniformType::Vec4);

  program = loadProgram("assimp-vs", "assimp-fs");
  programLight = loadProgram("assimp-vs", "assimp-light-fs");
}

AssimpTutotrial::~AssimpTutotrial()
{
  bgfx::destroy(programLight);
  bgfx::destroy(program);

  bgfx::destroy(lightSpecular);
  bgfx::destroy(lightDiffuse);
  bgfx::destroy(lightAmbient);
  bgfx::destroy(lightPositionUniform);
  bgfx::destroy(materialShininess);
  bgfx::destroy(materialSpecular);
  bgfx::destroy(materialDiffuse);
  bgfx::destroy(materialAmbient);
  bgfx::destroy(viewPosUniform);
  bgfx::destroy(woodenContainerSpecularTexture);
  bgfx::destroy(woodenContainerTexture);
}

auto AssimpTutotrial::update() -> void
{
  {
    const auto view =
      glm::lookAt(camPos,
                  camPos + glm::vec3{glm::rotate(glm::mat4{1.0f}, camYaw, glm::vec3{0.0f, 0.0f, 1.f}) *
                                     glm::rotate(glm::mat4{1.0f}, camPitch, glm::vec3{0.0f, 1.f, 0.0f}) *
                                     glm::vec4{1.f, 0.f, 0.f, 1.f}},
                  glm::vec3(0.0, 0.0, 1.0));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), 1.f * width / height, 0.1f, 100.0f);

    bgfx::setViewTransform(0, &view, &projection);
  }
  const auto secs = SDL_GetTicks() / 1000.f;
  const auto lightPos = glm::vec3{glm::rotate(glm::mat4{1.0f}, secs, glm::vec3{0.0f, 0.0f, 1.f}) *
                                  glm::vec4{3.f, 0.0f, -2.0f, 1.f}};

  auto i = 0;
  for (auto cubePosition : std::array{glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(2.0f, 5.0f, -15.0f),
                                      glm::vec3(-1.5f, -2.2f, -2.5f),
                                      glm::vec3(-3.8f, -2.0f, -12.3f),
                                      glm::vec3(2.4f, -0.4f, -3.5f),
                                      glm::vec3(-1.7f, 3.0f, -7.5f),
                                      glm::vec3(1.3f, -2.0f, -2.5f),
                                      glm::vec3(1.5f, 2.0f, -2.5f),
                                      glm::vec3(1.5f, 0.2f, -1.5f),
                                      glm::vec3(-1.3f, 1.0f, -1.5f)})

  {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, cubePosition);
    float angle = 20.0f * i++;
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

    model = glm::rotate(model, glm::radians(-20 * secs), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-5 * secs), glm::vec3(1.0f, 0.0f, 0.0f));

    bgfx::setTransform(&model);

    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/
                   BGFX_STATE_MSAA);
    bgfx::setTexture(0, materialDiffuse, woodenContainerTexture);
    bgfx::setTexture(1, materialSpecular, woodenContainerSpecularTexture);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(50.f * secs), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(1., 1080. / 1920., 1.));
    bgfx::setUniform(transUniform, &trans);
    bgfx::setUniform(lightPositionUniform, &lightPos);
    glm::vec3 diffuseColor = glm::vec3(1.0f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
    bgfx::setUniform(lightAmbient, &ambientColor);
    bgfx::setUniform(lightDiffuse, &diffuseColor);
    bgfx::setUniform(lightSpecular, std::array{1.f, 1.f, 1.f, 0.0f}.data());
    bgfx::setUniform(materialAmbient, std::array{1.f, .5f, .31f, 0.0f}.data());
    bgfx::setUniform(materialShininess, std::array{64.f, 0.0f, 0.0f, 0.0f}.data());
    bgfx::setUniform(viewPosUniform, &camPos);

    bgfx::submit(0, program);
  }

  bgfx::frame();
}

auto AssimpTutotrial::strafe(float forward, float right) -> void
{
  camPos += 0.2f *
              glm::vec3{glm::rotate(glm::mat4{1.0f}, camYaw, glm::vec3{0.0f, 0.0f, 1.f}) *
                        glm::rotate(glm::mat4{1.0f}, camPitch, glm::vec3{0.0f, 1.f, 0.0f}) *
                        glm::vec4{1.f, 0.f, 0.f, 1.f}} *
              forward +
            0.2f *
              glm::vec3{glm::rotate(glm::mat4{1.0f}, camYaw, glm::vec3{0.0f, 0.0f, 1.f}) *
                        glm::rotate(glm::mat4{1.0f}, camPitch, glm::vec3{0.0f, 1.f, 0.0f}) *
                        glm::vec4{0.f, -1.f, 0.f, 1.f}} *
              right;
}

auto AssimpTutotrial::look(float dx, float dy) -> void
{
  camYaw -= dx;
  camPitch += dy;
  camPitch =
    std::min(static_cast<float>(M_PI / 2.f), std::max(static_cast<float>(-M_PI / 2.f), camPitch));
}

REG(AssimpTutotrial, "assimp");
