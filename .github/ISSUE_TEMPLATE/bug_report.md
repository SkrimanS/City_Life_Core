---
name: Bug report
about: Report a reproducible problem in City Life Core
title: "bug: "
labels: ["bug"]
assignees: ""
---

## Summary

Describe the bug clearly and briefly.

## Affected area

Select all that apply:

- [ ] Core simulation
- [ ] Runtime / ticks
- [ ] Registry / validation
- [ ] Settlements
- [ ] Resources / storage
- [ ] Routes / caravans
- [ ] Economy / market / ledger
- [ ] Factions / ownership
- [ ] Contracts
- [ ] Persistence / save-load
- [ ] Replay / determinism
- [ ] C ABI
- [ ] C ABI expansion / foreign-language binding
- [ ] C# / Unity integration
- [ ] Browser / WebAssembly integration
- [ ] Integration targets / platform support
- [ ] CMake / packaging
- [ ] Documentation
- [ ] Other

## Version / branch

- Git ref, branch, or commit:
- Public release tag, if applicable:
- Internal milestone branch, if applicable:
- OS:
- Compiler:
- CMake version:
- Unity version, if applicable:
- Browser / runtime, if applicable:

## Steps to reproduce

1.
2.
3.

## Expected behavior

What should happen?

## Actual behavior

What happens instead?

## Minimal reproduction

Paste a minimal code sample, test case, save file excerpt, data definition, Unity script, C ABI call sequence, or command sequence.

```cpp
// minimal reproduction here
```

## Logs / output

```text
paste relevant output here
```

For Unity or browser integration issues, include the exact loading or binding error if available.

## Regression?

- [ ] This worked before
- [ ] This is new behavior
- [ ] Unknown

Last known good commit/ref, if known:

## Impact

Describe how serious this is:

- [ ] Blocks build
- [ ] Blocks tests
- [ ] Breaks runtime behavior
- [ ] Breaks public API usage
- [ ] Breaks C ABI / foreign-language binding
- [ ] Breaks Unity/C# integration
- [ ] Breaks Browser/WASM integration
- [ ] Breaks package/install flow
- [ ] Documentation mismatch
- [ ] Minor issue

## Additional context

Add anything else that helps explain the problem.
