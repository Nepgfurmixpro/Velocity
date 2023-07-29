#include <Render/RenderAPI.hpp>

namespace Velocity {
    GraphicsAPI RenderAPI::s_API = GraphicsAPI::None;
    void RenderAPI::SetAPI(GraphicsAPI api) {
        s_API = api;
    }

    GraphicsAPI RenderAPI::GetAPI() {
        return s_API;
    }
}