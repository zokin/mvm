// Copyright (c) Tamas Csala

#ifndef OGLWRAP_ENUMS_TEXTURE2D_TYPE_H_
#define OGLWRAP_ENUMS_TEXTURE2D_TYPE_H_

#include "../config.h"

namespace OGLWRAP_NAMESPACE_NAME {
namespace enums {

enum class Texture2DType : GLenum {
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_TEXTURE_2D)
  kTexture2D = GL_TEXTURE_2D,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_TEXTURE_1D_ARRAY)
  kTexture1DArray = GL_TEXTURE_1D_ARRAY,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_TEXTURE_RECTANGLE)
  kTextureRectangle = GL_TEXTURE_RECTANGLE,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_TEXTURE_CUBE_MAP)
  kTextureCubeMap = GL_TEXTURE_CUBE_MAP,
#endif
};

}  // namespace enums
using namespace enums;
}  // namespace oglwrap

#endif
