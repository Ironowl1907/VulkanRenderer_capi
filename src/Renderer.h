#pragma once

#include <vulkan/vulkan.h>

struct RendererContext {};

class Renderer {
public:
  void init();
  void shutdown();
  void render();

private:
  void init_pipline();
  RendererContext m_context;
};
