#pragma once

#include "Commands/CommandManager.h"
#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"

class OneTimeSubmit {
public:
  OneTimeSubmit(CommandManager *p_commandManager)
      : mp_commandManager(p_commandManager) {
    m_cmd = mp_commandManager->beginOneTimeCommands();
  }

  ~OneTimeSubmit() { mp_commandManager->endOneTimeCommands(m_cmd); }

  VkCommandBuffer get() const { return m_cmd; }

private:
  CommandManager *mp_commandManager;
  VkCommandBuffer m_cmd;
};
