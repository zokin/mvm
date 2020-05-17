// Copyright (c) Tamas Csala

#ifndef OGLWRAP_ENUMS_MEMORY_BARRIER_BIT_H_
#define OGLWRAP_ENUMS_MEMORY_BARRIER_BIT_H_

#include "../config.h"

namespace OGLWRAP_NAMESPACE_NAME {
namespace enums {

enum class MemoryBarrierBit : GLenum {
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT)
  kVertexAttribArrayBarrierBit = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_ELEMENT_ARRAY_BARRIER_BIT)
  kElementArrayBarrierBit = GL_ELEMENT_ARRAY_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_UNIFORM_BARRIER_BIT)
  kUniformBarrierBit = GL_UNIFORM_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_TEXTURE_FETCH_BARRIER_BIT)
  kTextureFetchBarrierBit = GL_TEXTURE_FETCH_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)
  kShaderImageAccessBarrierBit = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_COMMAND_BARRIER_BIT)
  kCommandBarrierBit = GL_COMMAND_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_PIXEL_BUFFER_BARRIER_BIT)
  kPixelBufferBarrierBit = GL_PIXEL_BUFFER_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_TEXTURE_UPDATE_BARRIER_BIT)
  kTextureUpdateBarrierBit = GL_TEXTURE_UPDATE_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_BUFFER_UPDATE_BARRIER_BIT)
  kBufferUpdateBarrierBit = GL_BUFFER_UPDATE_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_FRAMEBUFFER_BARRIER_BIT)
  kFramebufferBarrierBit = GL_FRAMEBUFFER_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_TRANSFORM_FEEDBACK_BARRIER_BIT)
  kTransformFeedbackBarrierBit = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_ATOMIC_COUNTER_BARRIER_BIT)
  kAtomicCounterBarrierBit = GL_ATOMIC_COUNTER_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_SHADER_STORAGE_BARRIER_BIT)
  kShaderStorageBarrierBit = GL_SHADER_STORAGE_BARRIER_BIT,
#endif
#if OGLWRAP_DEFINE_EVERYTHING || defined(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT)
  kClientMappedBufferBarrierBit = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
#endif
};

}  // namespace enums
using namespace enums;
}  // namespace oglwrap

#endif