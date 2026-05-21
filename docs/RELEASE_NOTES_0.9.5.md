# City Life Core 0.9.5 Release Notes / Заметки релиза

Status: **Unreleased**

---

## Русский

`0.9.5` — небольшой persistence/replay hardening слой после `0.9.4`, сфокусированный на сохранении, загрузке и deterministic replay для explicit cargo delivery.

### Добавлено

- Новый persistence test:
  - `clc_runtime_cargo_delivery_persistence_tests`
- Новый файл теста:
  - `tests/runtime_cargo_delivery_persistence_tests.cpp`
- Roundtrip coverage для already-delivered cargo state:
  - cargo доставлен в destination settlement storage до save/load;
  - destination storage сохраняется и загружается;
  - caravan cargo остаётся empty после load;
  - semantic runtime equivalence подтверждает состояние после load.
- Negative drift coverage:
  - drift по delivered destination storage;
  - drift по redelivered caravan cargo.

### Изменено

- `clc_runtime_replay_persistence_tests` теперь покрывает explicit cargo delivery после midpoint load:
  - caravan загружает 40 grain;
  - contract fulfillment забирает 30 grain;
  - оставшиеся 10 grain доставляются в `hillford` через `deliver_runtime_arrived_caravan_cargo_to_destination(...)`;
  - post-load replay equivalence проверяет доставленное storage состояние.
- README обновлён до версии `0.9.5` и отмечает cargo delivery replay coverage.

### Важно

Default runtime tick по-прежнему не делает auto-unload. `0.9.5` усиливает persistence/replay guarantees вокруг explicit delivery workflow, не меняя runtime tick semantics.

---

## English

`0.9.5` is a small persistence/replay hardening layer after `0.9.4`, focused on save/load and deterministic replay for explicit cargo delivery.

### Added

- New persistence test:
  - `clc_runtime_cargo_delivery_persistence_tests`
- New test file:
  - `tests/runtime_cargo_delivery_persistence_tests.cpp`
- Roundtrip coverage for already-delivered cargo state:
  - cargo is delivered into destination settlement storage before save/load;
  - destination storage is saved and loaded;
  - caravan cargo remains empty after load;
  - semantic runtime equivalence confirms loaded state.
- Negative drift coverage:
  - drift in delivered destination storage;
  - drift in redelivered caravan cargo.

### Changed

- `clc_runtime_replay_persistence_tests` now covers explicit cargo delivery after midpoint load:
  - caravan loads 40 grain;
  - contract fulfillment consumes 30 grain;
  - remaining 10 grain is delivered to `hillford` via `deliver_runtime_arrived_caravan_cargo_to_destination(...)`;
  - post-load replay equivalence validates delivered storage state.
- README now reports version `0.9.5` and notes cargo delivery replay coverage.

### Important

Default runtime tick still does not auto-unload cargo. `0.9.5` strengthens persistence/replay guarantees around the explicit delivery workflow without changing runtime tick semantics.
