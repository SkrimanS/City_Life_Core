# Multiplayer Economy and Trade Safety

Status: **v2.4.0 internal milestone / source-level C++ API**

`MultiplayerEconomySafety.hpp` adds no-mutation safety reviews for economy operations before a host mutates wallets, storage, contracts or ledgers.

Covered reviews:

- buy resource;
- sell resource;
- contract reward.

The review checks actor id, settlement existence, quantity, wallet coins, stored resource amount and duplicate ledger references where possible.

It does not implement payment systems, marketplace services, auction houses, account wallets or UI.
