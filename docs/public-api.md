# Public API

Version: **1.0.0**

Use the aggregate C++ header for normal integrations:

```cpp
#include "clc/CityLifeCore.hpp"
```

Use the minimal C interface header only for C/FFI smoke checks and narrow runtime access:

```c
#include "clc/c/CityLifeCoreC.h"
```

## Public API rules

- Prefer namespaced C++ APIs under `clc::*` for gameplay/runtime integrations.
- Treat installed headers under `include/clc` as the supported SDK surface.
- Validate registries and runtime state before relying on loaded content.
- Prefer tick-based helpers for server-authoritative or real-time runtime flows.
- Use day helpers only where turn/day simulation semantics are intended.
- Keep game-specific rules outside the SDK and compose them around the runtime APIs.

## Common workflow

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

## Related documents

- [Public API status](public-api-status.md)
- [C interface](c-abi.md)
- [Core concepts](core-concepts.md)
- [Compatibility](compatibility.md)
