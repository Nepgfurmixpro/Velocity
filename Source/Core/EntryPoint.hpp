#include <Core/Application.hpp>

extern Velocity::Application* Velocity::CreateApp();

int main(int argc, char** argv) {
    try {
        auto app = Velocity::CreateApp();

        app->Run();

        app->Close();
    } catch (std::exception& e) {
        spdlog::critical("Exception: {0}", e.what());
    }
}