# City Life Core 0.9.7 Release Notes / Заметки релиза

Status: **Unreleased**

---

## Русский

`0.9.7` — небольшой runtime workflow слой после `0.9.6`, сфокусированный на batch delivery для уже прибывших караванов.

### Добавлено

- Batch workflow helper:
  - `deliver_all_runtime_arrived_caravan_cargo_to_destinations(...)`
- Aggregated result type:
  - `RuntimeBulkCargoDeliveryResult`
- Batch delivery semantics:
  - доставляет cargo только у уже прибывших караванов;
  - игнорирует не прибывшие караваны;
  - игнорирует пустой cargo;
  - сохраняет deterministic fleet order в result;
  - возвращает количество доставленных караванов и total delivered amount.
- Workflow test coverage для batch delivery:
  - early batch delivery without arrivals;
  - two arrived caravans delivered in one call;
  - pending caravan remains untouched;
  - repeated empty batch delivery is safe.

### Изменено

- `simulation_runtime_workflow_tests` расширен batch arrival cargo delivery coverage.
- `CHANGELOG.md` backfilled for `0.9.3` through `0.9.6`.
- Version bumped to `0.9.7`.

### Важно

Default runtime tick semantics не менялись. Batch delivery остаётся явным workflow action и не делает auto-unload во время tick.

---

## English

`0.9.7` is a small runtime workflow layer after `0.9.6`, focused on batch delivery for already-arrived caravans.

### Added

- Batch workflow helper:
  - `deliver_all_runtime_arrived_caravan_cargo_to_destinations(...)`
- Aggregated result type:
  - `RuntimeBulkCargoDeliveryResult`
- Batch delivery semantics:
  - delivers cargo only from already-arrived caravans;
  - ignores non-arrived caravans;
  - ignores empty cargo;
  - preserves deterministic fleet order in the result;
  - reports delivered caravan count and total delivered amount.
- Workflow test coverage for batch delivery:
  - early batch delivery without arrivals;
  - two arrived caravans delivered in one call;
  - pending caravan remains untouched;
  - repeated empty batch delivery is safe.

### Changed

- Extended `simulation_runtime_workflow_tests` with batch arrival cargo delivery coverage.
- Backfilled `CHANGELOG.md` for `0.9.3` through `0.9.6`.
- Version bumped to `0.9.7`.

### Important

Default runtime tick semantics did not change. Batch delivery remains an explicit workflow action and does not auto-unload during tick.
