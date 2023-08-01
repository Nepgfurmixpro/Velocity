#pragma once

#include <functional>
#include <Core/Base.hpp>

namespace Velocity {
    enum class EventType {
        Quit
    };

    struct EventData {

    };

    struct WindowEvent {
        EventType Type;
        EventData* Data;
    };

    using WindowEventCallback = std::function<void(WindowEvent&)>;
}