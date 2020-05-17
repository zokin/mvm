// Copyright (c) Tamas Csala

#ifndef OGLWRAP_ENUMS_MAG_FILTER_H_
#define OGLWRAP_ENUMS_MAG_FILTER_H_

#include "../config.h"

namespace OGLWRAP_NAMESPACE_NAME {
namespace enums {

enum class MagFilter : GLenum {
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_NEAREST)
  kNearest = GL_NEAREST,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_LINEAR)
  kLinear = GL_LINEAR,
#endif
};

}  // namespace enums
using namespace enums;
}  // namespace oglwrap

#endif
