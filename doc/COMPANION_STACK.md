# Companion stack architecture

This fork is maintained as a private persistent-companion stack, not as a simulated
population server.

## Component boundaries

- **AzerothCore** owns the game world, database layer and the smallest possible set
  of core APIs required by the companion stack.
- **mod-playerbots** owns companion gameplay: combat AI, manual same-account login,
  inventory, professions, errands, quest surveying and controlled autonomous cycles.
- **mod-pbc** owns character continuity: personality, dialogue, documentary knowledge,
  lived-adventure memory, relationships and LLM integration.

Gameplay state must not be inferred from generated dialogue. Documentary knowledge
must not be stored as lived memory unless it was actually experienced or exchanged
in game.

## Population invariant

The production profile is companion-only.

When `AiPlayerbot.CompanionOnly = 1`:

- random-bot autologin is disabled;
- RandomBot/AddClass account generation is bypassed entirely;
- automatic alt-bot login is disabled;
- cross-account, guild and trusted-account bot control is disabled;
- companions are admitted manually from the player's own account, subject to the
  configured companion restrictions.

The distributed Playerbot configuration also defaults RandomBots and the AddClass
pool to zero. Population simulation must therefore be an explicit opt-in.

## Reproducible stack

The core repository pins both modules as git submodules:

- `modules/mod-playerbots`
- `modules/mod-pbc`

A core commit therefore identifies the exact companion-engine and narrative-layer
revisions that belong together. Do not update a module independently in production
without updating and testing the core pin.

## Core-delta policy

Prefer module changes over AzerothCore changes. A core patch is justified only when
the required capability cannot be expressed through a stable module/script API.

Existing examples include checked transaction persistence and read-only game-data
queries required by persistent companion systems. Server-specific content should be
moved out of the core into dedicated modules when practical.

Upstream AzerothCore synchronization must happen on an integration branch and be
compiled before promotion. Do not resolve Playerbot/core conflicts by blindly taking
either side: scripting hooks, sessions, movement, database APIs and entity layouts
are compatibility boundaries.

## Persistence

PBC durable state must live outside disposable build/source directories in
production. On Linux, use dedicated writable paths such as:

```ini
PBC.HistoryJournalPath = "/var/lib/azerothcore/pbc-journal"
PBC.AdventurePath = "/var/lib/azerothcore/pbc-adventures"
```

Database and PBC journal/adventure backups must be kept as a coherent set.

## Reference platform

Debian/Linux is the reference build and production platform for this fork.
Windows remains a compatibility target where practical, but Linux behavior and
reproducible server deployment take precedence.
