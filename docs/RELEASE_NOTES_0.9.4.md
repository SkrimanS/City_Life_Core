# City Life Core 0.9.4 Release Notes / Заметки релиза

Status: **Unreleased**

---

## Русский

`0.9.4` — небольшой runtime arrival consequences слой после `0.9.3`, сфокусированный на явной доставке cargo прибывшего каравана в storage поселения назначения и диагностике этого события.

### Добавлено

- Явный workflow helper:
  - `deliver_runtime_arrived_caravan_cargo_to_destination(...)`
- Result-типы для доставки cargo:
  - `RuntimeCargoDeliveryEntry`
  - `RuntimeCaravanCargoDeliveryResult`
- Доставка всего оставшегося cargo прибывшего caravan в destination settlement storage.
- Rollback safety при ошибках destination storage mutation.
- Runtime event type:
  - `runtime.caravan.cargo_delivered`
- Runtime event summary counter:
  - `cargo_events`
- Runtime event analysis counter:
  - `caravan_cargo_delivered_events`
- Helper для записи события:
  - `append_runtime_caravan_cargo_delivery_event(...)`

### Изменено

- Public API documentation обновлена под explicit cargo delivery.
- README обновлён до версии `0.9.4` и описывает explicit cargo delivery.
- Workflow tests покрывают early delivery rejection, delivery after arrival, destination storage mutation, empty repeated delivery, and missing caravan rejection.
- Runtime event tests покрывают cargo delivery event emission, deterministic payload, validation, and analysis.

### Важно

Default runtime tick не делает auto-unload. Это намеренно: contract fulfillment from arrived caravan cargo остаётся явным и детерминированным, а cargo delivery в settlement storage вызывается отдельным workflow helper.

---

## English

`0.9.4` is a small runtime arrival consequences layer after `0.9.3`, focused on explicit delivery of arrived caravan cargo into destination settlement storage and diagnostics for that event.

### Added

- Explicit workflow helper:
  - `deliver_runtime_arrived_caravan_cargo_to_destination(...)`
- Cargo delivery result types:
  - `RuntimeCargoDeliveryEntry`
  - `RuntimeCaravanCargoDeliveryResult`
- Delivery of all remaining arrived caravan cargo into destination settlement storage.
- Rollback safety for destination storage mutation failures.
- Runtime event type:
  - `runtime.caravan.cargo_delivered`
- Runtime event summary counter:
  - `cargo_events`
- Runtime event analysis counter:
  - `caravan_cargo_delivered_events`
- Event append helper:
  - `append_runtime_caravan_cargo_delivery_event(...)`

### Changed

- Public API documentation now describes explicit cargo delivery.
- README now reports version `0.9.4` and describes explicit cargo delivery.
- Workflow tests cover early delivery rejection, delivery after arrival, destination storage mutation, empty repeated delivery, and missing caravan rejection.
- Runtime event tests cover cargo delivery event emission, deterministic payload, validation, and analysis.

### Important

Default runtime tick does not auto-unload cargo. This is intentional: contract fulfillment from arrived caravan cargo remains explicit and deterministic, while cargo delivery into settlement storage is invoked through a separate workflow helper.
