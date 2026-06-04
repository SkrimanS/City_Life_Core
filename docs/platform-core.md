# Platform Core

`PlatformCore.hpp` is the v4.0.0 headless platform foundation.

It provides:

- static module registry;
- core, optional and genre module metadata;
- module dependency rules;
- genre profiles for idle/tycoon, city builder, strategy, settlement RPG and survival economy;
- profile metadata for enabled modules, default rules, content templates, actions and reports;
- content pack manifest validation;
- editor/admin-friendly registry query reports;
- unified platform diagnostics digest and markdown output.

Useful APIs:

- `make_standard_platform_core_registry`
- `validate_platform_core_registry`
- `query_platform_core_registry`
- `validate_content_pack_manifest`
- `make_platform_diagnostics_report`
- `platform_diagnostics_digest`
- `platform_diagnostics_markdown`

The platform core remains headless. It does not add UI, renderer, visual editor, marketplace, accounts, billing, networking or cloud-specific services.

Example:

- `examples/platform_core.cpp`
