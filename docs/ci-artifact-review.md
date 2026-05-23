# CI and Artifact Review

Status: **1.0.0 manual release gate**

Use this guide when reviewing CI results, benchmark output, SDK ZIP archives and release artifacts.

## Review checklist

- Confirm configure/build jobs complete for the supported toolchains.
- Confirm tests pass or documented failures are explicitly accepted.
- Review benchmark artifacts for obvious regressions.
- Download SDK ZIP artifacts and inspect the install layout.
- Verify checksums before publishing or redistributing archives.
- Confirm installed C++ and C ABI consumers can build against the unpacked package.

## Related documents

- [Release verification](verifying-releases.md)
- [Release manifest](release-manifest-1.0.0.md)
- [SDK ZIP package](sdk-zip-package.md)
