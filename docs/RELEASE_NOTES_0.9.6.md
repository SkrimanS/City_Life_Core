# City Life Core 0.9.6 Release Notes / Заметки релиза

Status: **Unreleased**

---

## Русский

`0.9.6` — небольшой event-log diagnostics слой после `0.9.5`, сфокусированный на roundtrip coverage для события explicit cargo delivery.

### Добавлено

- Event-log roundtrip coverage для `runtime.caravan.cargo_delivered` после midpoint save/load.
- Проверка deterministic payload для cargo delivery event:
  - `caravan_id->destination_settlement_id:total=N`
- Проверка analysis counters после replay:
  - `caravan_cargo_delivered_events`
  - `contract_fulfilled_events`
- Negative drift coverage для event-log checksum при изменённом cargo delivery payload.

### Изменено

- `clc_runtime_event_log_roundtrip_tests` теперь покрывает explicit cargo delivery event после загрузки runtime и contract fulfillment.
- README обновлён до версии `0.9.6` и отмечает cargo delivery roundtrip diagnostics.

### Важно

Runtime tick semantics не менялись. Cargo delivery event появляется только после явного `deliver_runtime_arrived_caravan_cargo_to_destination(...)` и явного append через event helper.

---

## English

`0.9.6` is a small event-log diagnostics layer after `0.9.5`, focused on roundtrip coverage for the explicit cargo delivery event.

### Added

- Event-log roundtrip coverage for `runtime.caravan.cargo_delivered` after midpoint save/load.
- Deterministic payload validation for cargo delivery events:
  - `caravan_id->destination_settlement_id:total=N`
- Analysis counter checks after replay:
  - `caravan_cargo_delivered_events`
  - `contract_fulfilled_events`
- Negative drift coverage for event-log checksum when the cargo delivery payload changes.

### Changed

- `clc_runtime_event_log_roundtrip_tests` now covers explicit cargo delivery event emission after runtime load and contract fulfillment.
- README now reports version `0.9.6` and notes cargo delivery roundtrip diagnostics.

### Important

Runtime tick semantics did not change. Cargo delivery events are emitted only after explicit `deliver_runtime_arrived_caravan_cargo_to_destination(...)` and explicit event-helper append.
