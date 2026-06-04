# Diagnostics API

The unified diagnostics surface is composed of validation reports and stable digest strings.

`make_platform_diagnostics_report` aggregates:

- platform registry query diagnostics;
- content pipeline validation diagnostics;
- runtime diagnostic counts;
- economy diagnostic counts;
- action diagnostic counts;
- replay diagnostic counts;
- migration diagnostic counts.

Use digest strings for logs, CI artifacts, editor panels and server smoke tests.
