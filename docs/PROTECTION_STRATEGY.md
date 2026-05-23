# Project Protection Strategy / Стратегия защиты проекта

Status: **1.0.0 protection strategy / стратегия защиты 1.0.0**

This document describes practical ways to protect City Life Core as a public SDK/release project. It is not legal advice. The final license and commercial/legal model must be chosen by the project owner.

Этот документ описывает практические способы защиты City Life Core как публичного SDK/release проекта. Это не юридическая консультация. Финальную лицензию и коммерческо-правовую модель должен выбрать владелец проекта.

---

## Core principle / Главный принцип

No technical measure can fully prevent someone from copying code they can read. Protection must be layered:

Технически невозможно полностью запретить копирование кода, который доступен для чтения. Защита должна быть многоуровневой:

1. **License / Лицензия** — what others are legally allowed to do.
2. **Copyright ownership / Авторские права** — who owns the code and contributions.
3. **Trademark/name protection / Защита имени** — who can use the project name and brand.
4. **Release provenance / Происхождение релизов** — how users verify official builds.
5. **Contributor policy / Политика вкладов** — how external code enters the project.
6. **Closed core, source-available core, or dual licensing / Закрытое ядро, source-available ядро или dual licensing** — optional commercial protection.
7. **Official extension points / Официальные точки расширения** — how users modify behavior without replacing the official core.

---

## 1. License decision / Решение по лицензии

A public 1.0.0 release must not ship without a root `LICENSE` file.

Публичный 1.0.0 release нельзя выпускать без файла `LICENSE` в корне репозитория.

### Main options

| Model | What it protects | Tradeoff |
| --- | --- | --- |
| Proprietary / source-available | Maximum control; copying/reuse can be restricted. | Harder for open community adoption. |
| Dual license | Open/free under one license, commercial under another. | Requires clear commercial terms. |
| Copyleft, e.g. GPL/AGPL family | Forks/derivatives must follow copyleft obligations. | Some commercial/game studios may avoid it. |
| Permissive, e.g. MIT/Apache/BSD family | Easy adoption. | Others can reuse/fork with fewer restrictions. |

### Owner decision required

Before 1.0.0, the owner must decide:

- Is City Life Core open source, source-available, or proprietary?
- Can third parties fork it?
- Can third parties use it commercially?
- Must modified versions publish changes?
- Can hosted/server use avoid publishing changes?
- Is a commercial license planned?

---

## 2. Copyright and contribution ownership

Recommended before accepting external contributions:

- Keep `CONTRIBUTING.md` current.
- Decide whether contributors need a CLA/DCO.
- Require contributors to confirm they have the right to submit code.
- Keep commit history and authorship clear.
- Avoid accepting copied code from unknown sources.

Possible policies:

- **DCO**: contributors certify origin through Signed-off-by.
- **CLA**: contributors grant broader rights to the project owner.
- **No external contributions before license/contribution policy is final**: simplest until the legal model is chosen.

---

## 3. Name, brand, and official distribution

Even if code is open, the project name can be protected separately.

Recommended policy:

- Define official project name: `City Life Core`.
- Define official namespace: `clc` / `CityLifeCore::core`.
- Define official repository and release channels.
- State that forks may not imply they are official builds.
- Consider trademark registration if the project becomes commercial/public enough.

Add later if needed:

- `TRADEMARK_POLICY.md` or a section in a future branding document.

---

## 4. Release provenance and tamper resistance

To protect users from unofficial or modified builds, official releases should be verifiable.

Recommended for 1.0.0:

- Git tag `v1.0.0`.
- Signed tag or signed release commit if available.
- SDK ZIP checksums.
- Release manifest listing files and hashes.
- Changelog/release notes tied to exact commit SHA.
- Installed/unpacked SDK consumer validation recorded in the manifest.

Recommended later:

- SLSA/provenance attestation.
- Reproducible build notes.
- CI-generated artifacts only.
- Package manager publishing only from protected workflows.

---

## 5. Branch and repository protection

Recommended repository settings before public 1.0.0:

- Protect `main`.
- Require PR review before merge.
- Require CI green before merge.
- Disable force push to protected branches.
- Require signed tags for releases or document signing policy.
- Use CODEOWNERS for critical areas:
  - `include/clc/**`
  - `src/clc/**`
  - `cmake/**`
  - release-governance docs;
  - `CMakeLists.txt`.

---

## 6. Public SDK boundary protection

The easiest way to lose control of a public SDK is to accidentally make internals stable.

Protection measures:

- Keep `docs/PUBLIC_API_STATUS.md` up to date.
- Mark experimental/specialized headers clearly.
- Avoid promising C++ binary ABI compatibility before it is ready.
- Keep the C ABI scope explicit in `docs/C_ABI.md`.
- Add deprecation policy before removing stable APIs.

---

## 7. Future protected distribution model

City Life Core 1.0.0 is prepared as a source-first SDK while the public API stabilizes and real integrations are validated.

Later, the owner may switch to a more protected model without blocking legitimate customization. Possible direction:

- keep the **official core** closed or source-available under restricted redistribution terms;
- publish public headers, SDK docs, and binary packages;
- expose official extension points for mods/plugins/data packs;
- keep data definitions, scenario packs, and runtime configuration modifiable;
- allow game developers to extend behavior through documented APIs instead of modifying core internals;
- sign official SDK packages;
- reject the claim that modified core forks are official unless explicitly approved.

This model protects the identity and integrity of the core while still allowing modification through supported boundaries.

Для будущих версий можно перейти к модели: закрытое или source-available ядро, но с официальными точками расширения. Цель — не запретить моддинг, а не дать людям выдавать изменённое ядро за официальное.

Recommended architecture work before that transition:

- define plugin/mod API boundaries;
- separate stable public SDK from internal engine internals;
- document data-pack schemas;
- add signed package/release manifest workflow;
- define what modifications are allowed without calling the build official;
- define what modifications require a separate fork name.

---

## 8. Anti-copy expectations

What is realistic:

- You can legally restrict copying/reuse with license terms.
- You can protect the official name and official release channel.
- You can prove what the official version is through signed tags/checksums.
- You can require modifications to be published if using a suitable copyleft license.
- You can forbid modified third-party builds from claiming official status.

What is not realistic:

- Preventing all copying of public source code.
- Preventing someone from writing a similar project from scratch.
- Preventing forks if the chosen license allows forks.
- Enforcing license terms automatically without legal/process follow-up.

---

## 9. Minimum protection checklist before 1.0.0

- [ ] Choose license model.
- [ ] Add root `LICENSE`.
- [ ] Add license note to README.
- [ ] Add license note to package docs.
- [ ] Decide whether external contributions are accepted.
- [ ] Keep `CONTRIBUTING.md` aligned with the chosen model.
- [ ] Decide DCO/CLA policy if external contributions are accepted.
- [ ] Protect `main` branch.
- [ ] Require CI before merge.
- [ ] Use signed tags or document release signing policy.
- [ ] Generate release checksums.
- [ ] Fill release manifest.
- [ ] Define official release channel.
- [ ] Document that unofficial forks/builds are not official City Life Core releases.

---

## Recommended near-term policy

For the current 1.0.0 preparation phase, the safest non-final policy is:

- do not publish 1.0.0 until a license is chosen;
- do not accept outside code unless contribution terms are clear;
- treat `v/1.0-rc-prep` as release-preparation work only;
- release `1.0.0` only with LICENSE, checksums, release notes, security policy, verification guide, public API status and completed manifest;
- evaluate protected/source-available/commercial distribution later if the owner chooses it.
