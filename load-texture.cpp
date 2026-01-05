#include "load-texture.hpp"
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <fstream>
#include <sstream>
#include <string_view>

static void imageReleaseCb(void * /*ptr*/, void *_userData)
{
  bimg::ImageContainer *imageContainer = (bimg::ImageContainer *)_userData;
  bimg::imageFree(imageContainer);
}

auto loadTexture(const char *fileName, uint64_t _flags) -> bgfx::TextureHandle
{
  bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

  auto file = std::ifstream{fileName};
  if (!file)
    return BGFX_INVALID_HANDLE;

  auto buffer = std::stringstream{};
  buffer << file.rdbuf(); // Read the file buffer into the stream
  const auto content = buffer.str();
  static auto allocator = bx::DefaultAllocator{};

  bimg::ImageContainer *imageContainer = bimg::imageParse(&allocator, content.data(), content.size());

  if (NULL != imageContainer)
  {
    const bgfx::Memory *mem =
      bgfx::makeRef(imageContainer->m_data, imageContainer->m_size, imageReleaseCb, imageContainer);

    if (imageContainer->m_cubeMap)
    {
      handle = bgfx::createTextureCube(uint16_t(imageContainer->m_width),
                                       1 < imageContainer->m_numMips,
                                       imageContainer->m_numLayers,
                                       bgfx::TextureFormat::Enum(imageContainer->m_format),
                                       _flags,
                                       mem);
    }
    else if (1 < imageContainer->m_depth)
    {
      handle = bgfx::createTexture3D(uint16_t(imageContainer->m_width),
                                     uint16_t(imageContainer->m_height),
                                     uint16_t(imageContainer->m_depth),
                                     1 < imageContainer->m_numMips,
                                     bgfx::TextureFormat::Enum(imageContainer->m_format),
                                     _flags,
                                     mem);
    }
    else if (bgfx::isTextureValid(0,
                                  false,
                                  imageContainer->m_numLayers,
                                  bgfx::TextureFormat::Enum(imageContainer->m_format),
                                  _flags))
    {
      handle = bgfx::createTexture2D(uint16_t(imageContainer->m_width),
                                     uint16_t(imageContainer->m_height),
                                     1 < imageContainer->m_numMips,
                                     imageContainer->m_numLayers,
                                     bgfx::TextureFormat::Enum(imageContainer->m_format),
                                     _flags,
                                     mem);
    }

    if (bgfx::isValid(handle))
    {
      const auto name = std::string_view{fileName};
      bgfx::setName(handle, name.data(), name.size());
    }
  }

  return handle;
}
