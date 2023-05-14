#pragma once

#include <set>
#include "ecscomponent.h"
#include "glgeometry.h"

struct Time
{
    float seconds{0.0f};
    float delta{0.0f};
};

struct Collider
{
    lithium::Geometry* geometry{nullptr};
    unsigned int typeId{0};
};

namespace component
{
    using Time = ecs::Component<Time, 0, true>;
    using Collider = ecs::Component<Collider>;
}