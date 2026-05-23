# Verifying Official Releases

Status: **1.0.0 release verification guide**

This guide helps users distinguish official City Life Core releases from forks, modified archives, fake packages or malicious builds.

## Verify source and archives

- Prefer the official repository release page and tags.
- Compare archive checksums against published `SHA256SUMS.txt` when provided.
- Rebuild from source when a downstream package cannot be trusted.
- Review `LICENSE` and release notes before redistribution.

## Local validation

Run the manual validation script when CI logs or artifacts are unavailable:

```bash
bash scripts/manual_release_validation.sh
```

Windows PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/manual_release_validation.ps1
```

## Related documents

- [Release notes](release-notes-1.0.0.md)
- [Release manifest](release-manifest-1.0.0.md)
- [CI artifact review](ci-artifact-review.md)
