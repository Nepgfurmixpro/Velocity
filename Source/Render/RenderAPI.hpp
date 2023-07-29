#pragma once

namespace Velocity {
    enum class GraphicsAPI {
        None,
        Vulkan
    };
    class RenderAPI {
    private:
        static GraphicsAPI s_API;
    public:
        static GraphicsAPI GetAPI();
        static void SetAPI(GraphicsAPI api);
    };
}