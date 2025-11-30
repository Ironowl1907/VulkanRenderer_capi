#include "Core/Application.h"

#include "AppLayer.h"

int main() {
  Core::ApplicationSpec appSpec;
  appSpec.Name = "Architecture";
  appSpec.Window.Width = 1920;
  appSpec.Window.Height = 1080;

  Core::Application application(appSpec);
  application.PushLayer<AppLayer>();
  application.Run();
}
