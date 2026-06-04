# Content Pipeline

`ContentPackManifest` describes content pack metadata for the platform core:

- content pack id;
- schema version;
- required module ids;
- compatible profile ids;
- migration ids.

`validate_content_pack_manifest` checks module/profile references against the platform registry. The core does not own asset importers, visual editors, online stores or project-file formats.
