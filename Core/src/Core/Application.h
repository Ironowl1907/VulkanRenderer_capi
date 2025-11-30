#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Core/Window.h"
#include "Layer.h"

namespace Core {

struct ApplicationSpec {
  std::string Name = "Unamed Application";
  WindowSpec Window;
};

class Application {
public:
  Application(const ApplicationSpec &specification = ApplicationSpec());
  ~Application();

  void Run();
  void Stop();

  template <typename TLayer> void PushLayer() {
    m_LayerStack.push_back(std::make_unique<TLayer>());
  }

  glm::vec2 getFramebufferSize() const;

  static Application &Get();
  static float GetTime();
  std::shared_ptr<Window> getWindow();

private:
  ApplicationSpec m_Specification;
  std::shared_ptr<Window> m_Window;
  bool m_Running = false;

  std::vector<std::unique_ptr<Layer>> m_LayerStack;

  friend class Layer;
};

} // namespace Core
