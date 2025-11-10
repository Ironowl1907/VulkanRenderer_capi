#include "Renderer.h"
#include <iostream>

int main() {
  Renderer renderer;

  try {
    renderer.init();
    renderer.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
