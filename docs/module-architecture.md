# Module Architecture

City Life Core uses a static module registry for the v4.0.0 platform foundation.

Module kinds:

- `core`: required reusable SDK systems such as runtime, resources, production, market, save/replay and diagnostics;
- `optional`: systems enabled by game needs, such as regional simulation, authority and editor/admin query helpers;
- `genre`: profile-oriented module metadata for game categories.

Modules declare dependency module ids. Validation fails when a dependency is missing. Dynamic plugins are intentionally out of scope for this foundation.
