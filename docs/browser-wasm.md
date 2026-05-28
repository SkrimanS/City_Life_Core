# Browser and WebAssembly Integration

Status: **planned**

City Life Core is a native C++ simulation SDK. Browser support should be provided through a WebAssembly integration layer, not by making the core depend on browser APIs.

This document records the intended direction for browser games, web tools and web-hosted simulation demos.

---

## Intended architecture

Browser support should follow this shape:

```text
City Life Core C++ core
  -> C ABI / stable native boundary
  -> WebAssembly build
  -> JavaScript or TypeScript adapter
  -> browser game, web tool or web demo
```

The browser adapter should stay thin. It should not duplicate simulation rules that belong in the native core.

---

## Current status

Browser/WebAssembly support is not implemented yet.

Currently available foundations:

- headless native C++ core;
- C ABI for a minimal stable foreign-function boundary;
- tick/time utilities;
- opaque world handle;
- basic world advancement;
- read-only world event access;
- SDK packaging and install documentation.

Missing pieces:

- Emscripten build profile;
- browser/WASM CI smoke check;
- JavaScript or TypeScript adapter;
- browser example app;
- documented memory ownership rules for JS/WASM;
- data loading strategy for browser-hosted `.clcd` content;
- wider C ABI for useful gameplay systems.

---

## Non-goals

Browser support should not turn City Life Core into a web framework.

The core should not directly own:

- DOM rendering;
- browser input handling;
- browser storage APIs;
- web networking;
- canvas/WebGL/WebGPU rendering;
- JavaScript gameplay framework code.

Those belong in the consuming game, tool or adapter layer.

---

## Planned implementation path

### Phase 1 - Build feasibility

Goal: prove that the native core can be compiled to WebAssembly.

Tasks:

- add an Emscripten-oriented CMake configure example;
- document required CMake options;
- identify unsupported platform assumptions;
- confirm that the minimal C ABI can be exported;
- run a tiny smoke test for version and tick functions.

Expected result: a documented way to build a minimal WebAssembly module.

### Phase 2 - Minimal JavaScript adapter

Goal: expose the current minimal C ABI to JavaScript or TypeScript.

Tasks:

- wrap version and tick helpers;
- wrap opaque world create/destroy;
- wrap basic world advancement;
- copy strings safely from WASM memory;
- expose read-only event access;
- document ownership and lifetime rules.

Expected result: a small browser-side smoke test that creates a world, advances it and prints events.

### Phase 3 - Browser data loading

Goal: let browser examples load game data safely.

Tasks:

- document `.clcd` loading constraints in browser environments;
- define how data packs are fetched and passed into the core;
- avoid hidden filesystem assumptions;
- document save/load limitations for browser-hosted demos.

Expected result: a browser example that can use explicit data loaded by the web app.

### Phase 4 - Useful simulation surface

Goal: expose enough stable API for real browser games or tools.

Tasks:

- expand C ABI only where the native API is stable enough;
- expose validation results in a JS-friendly shape;
- expose runtime diagnostics;
- expose persistence/replay helpers when stable;
- avoid leaking C++ implementation details into JS.

Expected result: browser integration becomes useful beyond a smoke test.

---

## Example future layout

A future browser example may look like this:

```text
examples/browser_wasm/
  README.md
  CMakePresets.json or build script
  index.html
  main.js or main.ts
  city-life-core-loader.js
```

This is not present yet. It should be added only when a real WebAssembly build path exists.

---

## Roadmap alignment

### v1.x

Before `v2.0.0`, browser support should remain planning and feasibility work unless the minimal C ABI is enough for a smoke example.

Focus:

- keep the core portable;
- avoid browser-specific dependencies;
- identify ABI gaps;
- document the future adapter path.

### v2.x / v3.x

After `v2.0.0`, browser support can become more useful as deeper systems become stable enough to expose through foreign-language boundaries.

Focus:

- richer C ABI;
- JavaScript/TypeScript adapter design;
- validation and diagnostics for web tools;
- persistence and replay workflows suitable for browser-hosted tools or demos.

### v4.0.0+

For large-world and production-level simulations, browser support should focus on tooling, visualization, debugging, balancing and controlled client-side simulation rather than replacing server-authoritative backends.
