# Migration v3 to v4

The v4.0.0 platform foundation keeps v3 large-world systems and adds platform metadata around them.

Migration guidance:

- keep existing runtime and regional APIs;
- map game use cases to genre profiles;
- describe content packs with `ContentPackManifest`;
- validate module dependencies before enabling profile-specific workflows;
- use platform diagnostics to capture registry, content, runtime, replay and migration status.
