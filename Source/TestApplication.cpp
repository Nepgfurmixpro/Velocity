#pragma once

#include <Core/Application.hpp>
#include <Core/EntryPoint.hpp>

namespace Velocity {
    class TestApp : public Application {
    public:
        TestApp(const ApplicationSettings& settings) : Application(settings) {

        }
    };

    Application* CreateApp() {
        ApplicationSettings settings{};
        settings.Width = 1280;
        settings.Height = 720;
        settings.Name = "Velocity";
        settings.API = GraphicsAPI::Vulkan;
        auto app = new TestApp(settings);
        return app;
    }
}