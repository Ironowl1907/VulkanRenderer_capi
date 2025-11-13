#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>

class Application {
public:
  Application();

  virtual ~Application() = default;

  /**
   * @brief Prepares the application for execution
   */
  virtual bool prepare(int width, int height) = 0;

  /**
   * @brief Updates the application
   * @param delta_time The time since the last update
   */
  virtual void update(float delta_time) = 0;

  /**
   * @brief Main loop sample overlay events
   * @param delta_time The time taken since the last frame
   * @param additional_ui Function that implements an additional Gui
   */
  virtual void update_overlay(float delta_time) = 0;

  /**
   * @brief Handles cleaning up the application
   */
  virtual void finish() = 0;

  /**
   * @brief Handles resizing of the window
   * @param width New width of the window
   * @param height New height of the window
   */
  virtual bool resize(const uint32_t width, const uint32_t height) = 0;

  const std::string &get_name() const { return m_applicationName; }

  void set_name(const std::string &name);

  inline bool should_close() const { return m_requestedClose; }

  // request the app to close
  // does not guarantee that the app will close immediately
  inline void close() { m_requestedClose = true; }

protected:
  GLFWwindow *m_window{nullptr};

private:
  std::string m_applicationName{};

  bool m_requestedClose{false};
};
