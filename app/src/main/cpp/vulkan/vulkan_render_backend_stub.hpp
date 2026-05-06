#pragma once

#include <memory>

#include "common/render_backend.hpp"

namespace skity::demo {

std::unique_ptr<RenderBackend> CreateVulkanRenderBackendStub();

}  // namespace skity::demo
