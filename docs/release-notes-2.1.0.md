# Release Notes 2.1.0

Added the source-level `ActionAuthority` API for server-authoritative actor/session action review before runtime dispatch.

Rejected authority checks do not mutate runtime state and produce audit-friendly diagnostics.
