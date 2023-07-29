#pragma once

#include <Core/WindowEvent.hpp>
#include <Core/Base.hpp>

namespace Velocity {
    struct ApplicationSettings {
        uint32_t Width;
        uint32_t Height;
        const char* Name;

    };

    class Application {
    public:
        Application();

        void Run();
        void Close();

        static Ref<Application> Get();
    private:
        static Ref<Application> s_Instance;

        void OnEvent(WindowEvent& event);
        void OnUpdate();
    };
}