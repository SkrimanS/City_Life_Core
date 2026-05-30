# Data Packs

City Life Core data packs use the `.clcd` text format and are loaded with `clc::data::DataPackLoader`.

Data packs are external input. They are validated more strictly than direct source-level C++ construction through `clc::data::DataRegistry` so editor exports, mods and tool-generated data fail early with clear diagnostics.

## Schema

Current schema:

```text
schema_version=0.2.2
```

The loader rejects missing or unsupported `schema_version` values.

## Sections

Supported sections:

```text
[resource]
[currency]
[profession]
[building]
[settlement]
```

Unknown sections are rejected. Duplicate keys inside one section are rejected.

## Resource definitions

Required fields:

```text
[resource]
id=grain
display_name=Grain
category=food
base_value=10
```

Validation rules:

- `id` must not be empty;
- `display_name` must not be empty;
- `base_value` is required in data packs;
- `base_value` must be a non-negative integer;
- `base_value` must be greater than zero.

`category` may be empty, but produces a warning.

## Currency definitions

Example:

```text
[currency]
id=coin
display_name=Coin
fractional_digits=2
```

Validation rules:

- `id` must not be empty;
- `display_name` must not be empty;
- `fractional_digits` must be a non-negative integer when present;
- `fractional_digits` must be less than or equal to 8.

## Profession definitions

Example:

```text
[profession]
id=farmer
display_name=Farmer
category=production
```

Validation rules:

- `id` must not be empty;
- `display_name` must not be empty;
- empty `category` produces a warning.

## Building definitions

Required fields:

```text
[building]
id=farm
display_name=Farm
category=production
worker_slots=8
required_profession_id=farmer
input_resource_ids=wood
output_resource_ids=grain
```

Validation rules:

- `id` must not be empty;
- `display_name` must not be empty;
- `worker_slots` is required in data packs;
- `worker_slots` must be a non-negative integer;
- `worker_slots` must be greater than zero;
- `required_profession_id`, when present, must reference a known profession;
- each `input_resource_ids` value must reference a known resource;
- each `output_resource_ids` value must reference a known resource.

`category` may be empty, but produces a warning.

## Settlement definitions

Example:

```text
[settlement]
id=riverwatch
display_name=Riverwatch
starting_population=120
```

Validation rules:

- `id` must not be empty;
- `display_name` must not be empty;
- `starting_population` must be a non-negative integer when present;
- zero `starting_population` produces a warning.

## Reference validation

`DataPackLoader::load_string`, `load_file` and `load_directory` run reference validation after registering parseable records. This means a building can be parsed but the final load result can still be invalid if it references a missing profession or resource.

## Compatibility note

Direct C++ API construction through `DataRegistry::add(...)` remains source-compatible for optional numeric defaults and may emit warnings instead of errors for zero values. Data packs are stricter because they represent external serialized input and should fail early when required gameplay fields are missing or unusable.

## Example

See:

```text
examples/custom_data_registry.cpp
examples/validation_hardening.cpp
```
