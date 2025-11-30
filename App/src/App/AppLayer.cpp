#include "AppLayer.h"

#include "Core/Application.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <iostream>

AppLayer::AppLayer() { m_renderer.initVulkan(); }

AppLayer::~AppLayer() { m_renderer.cleanup(); }

void AppLayer::OnUpdate(float ts) {}

void AppLayer::OnRender() { m_renderer.update(); }
