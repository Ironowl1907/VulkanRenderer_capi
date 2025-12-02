#pragma once

#include <stdint.h>

#include "Core/Events/Event.h"
#include "Core/Events/WindowEvents.h"
#include "Core/Layer.h"
#include "Renderer.h"

class AppLayer : public Core::Layer {
public:
  AppLayer();
  virtual ~AppLayer();

  virtual void OnUpdate(float ts) override;
  virtual void OnRender() override;

  virtual void OnEvent(Event &event) override;

private:
  bool onWindownResize(Core::WindowResizeEvent &e);
  bool onWindowClose(Event &e);

private:
  uint32_t m_Shader = 0;
  uint32_t m_VertexArray = 0;
  uint32_t m_VertexBuffer = 0;

  Renderer m_renderer;
};
