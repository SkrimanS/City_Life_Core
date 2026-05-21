# City Life Core 0.9.3 Release Notes / Заметки релиза

Status: **Unreleased**

---

## Русский

`0.9.3` — небольшой release-readiness слой после `0.9.2`, сфокусированный на CMake packaging и внешнем подключении SDK.

### Добавлено

- CMake install/export package flow для C++ SDK.
- Установка публичных headers из `include/`.
- Установка library target `city_life_core`.
- Export target namespace `CityLifeCore::`.
- Imported target для внешних проектов: `CityLifeCore::core`.
- Генерация `CityLifeCoreConfig.cmake`.
- Генерация `CityLifeCoreConfigVersion.cmake`.
- Package config template: `cmake/CityLifeCoreConfig.cmake.in`.
- Документация packaging flow: `docs/PACKAGING.md`.

### Изменено

- README теперь содержит install/find_package пример.
- README packaging status обновлён: CMake install/export package flow добавлен, binary release ещё впереди.

### Не входит в этот слой

- C ABI.
- CPack artifacts.
- Binary release layout для всех платформ.
- Shared/static policy.
- ABI compatibility policy.

---

## English

`0.9.3` is a small release-readiness layer after `0.9.2`, focused on CMake packaging and external SDK consumption.

### Added

- CMake install/export package flow for the C++ SDK.
- Public header installation from `include/`.
- Library target installation for `city_life_core`.
- Export target namespace `CityLifeCore::`.
- Imported target for external projects: `CityLifeCore::core`.
- Generated `CityLifeCoreConfig.cmake`.
- Generated `CityLifeCoreConfigVersion.cmake`.
- Package config template: `cmake/CityLifeCoreConfig.cmake.in`.
- Packaging flow documentation: `docs/PACKAGING.md`.

### Changed

- README now includes install/find_package usage.
- README packaging status now says the CMake install/export package flow exists, while binary release is still pending.

### Not included in this layer

- C ABI.
- CPack artifacts.
- Binary release layout for all platforms.
- Shared/static policy.
- ABI compatibility policy.
