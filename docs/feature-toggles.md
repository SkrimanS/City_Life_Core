# Feature Toggles

Feature toggles declare whether a simulation feature is available to processors.

Processors list `required_feature_ids`. If any required feature is disabled or missing, the processor is skipped and the run report records a diagnostic.

Feature toggles are deliberately separate from downstream game settings. They describe core processing availability, not UI choices.

