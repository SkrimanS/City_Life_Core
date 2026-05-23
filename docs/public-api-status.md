# Public API Status

Version: **1.0.0**

This document classifies the installed API surface for the 1.0.0 line.

## Stable for 1.x source use

- `clc/CityLifeCore.hpp`
- version and time utilities
- data registry and validation APIs
- settlement, route, caravan and runtime simulation APIs
- economy primitives: market, wallet, trade and ledger
- persistence and save/load validation helpers

## Minimal C interface

The C interface is intentionally small. It covers version/time utilities, an opaque world handle, basic state access, simple tick advancement and read-only world event inspection.

See [C interface](c-abi.md).

## Compatibility expectations

- Source compatibility is prioritized for the 1.x line.
- Binary compatibility is not the primary distribution contract unless a downstream package defines it separately.
- Prefer rebuilding consumers against the installed SDK package.

See [Compatibility](compatibility.md) and [Build and linking policy](build-and-linking-policy.md).
