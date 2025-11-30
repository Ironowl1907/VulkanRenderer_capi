#include "AppLayer.h"

#include "Core/Application.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <iostream>

#define VERT_SHADER_PATH "../App/Shaders/vert.spv"
#define FRAG_SHADER_PATH "../App/Shaders/frag.spv"

AppLayer::AppLayer() { m_renderer.init(VERT_SHADER_PATH, FRAG_SHADER_PATH); }

AppLayer::~AppLayer() { m_renderer.cleanup(); }

void AppLayer::OnUpdate(float ts) {}

void AppLayer::OnRender() { m_renderer.update(); }
