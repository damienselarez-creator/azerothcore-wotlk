# Companion stack: 2026-10-01

This assembly preserves the deployed companion changes on official AzerothCore
and incorporates the upstream updates reviewed on 2026-10-01. It is a source
assembly, not a second game server. Deployment to the existing server is a
separate operation, including backup, installation, database updates and restart.

## Pinned sources

| Component | Revision |
| --- | --- |
| Official AzerothCore baseline | `8ab7f044801fbf4e9dd5e3828a43c8eb33b95bd6` |
| Playerbots companion fork | `e341bff26c7aee360e2a8d605840d92559659459` |
| PBC companion fork | `00d5ea48f52e730ff60910f71f8ed35c4ea3a17c` |
| PBC upstream merged | `c5874c3a459ddfb56487640c106b0de17e58f45f` |

Use `git submodule update --init --recursive`, without `--remote`. The gitlinks
record the exact module revisions. Do not substitute upstream Playerbots master
or test-staging for the adapted module.

## Preserved behavior

- Core: read-only `LootTemplate::HasNonQuestItem` for companion professions,
  from damienselarez-creator/azerothcore-wotlk commit
  `11b11e6d0c1467fdf70f5e753b971507bce4c2cb`.
- Playerbots: generic upstream hooks, headless sessions and companion-only
  population guards; opt-in town errands, training and profession handling.
  Compatibility adaptations derive from upstream commits `56a0f3830173`,
  `d8ac0d4fa259`, `cc54f8f2934f` and `1e5add887878`, credited in the module commit.
- PBC: automatic adventure memory, journal recovery, character-specific biography
  and psychology, Winifred's knowledge corpus, and cpp-httplib 0.58.0.
- PBC upstream: UTF-8 filesystem paths and removal of the active-development notice.

Runtime configuration, credentials, character cards, databases, extracted client
data, memories and journals are not part of this source snapshot. Preserve them
on the existing server. In particular, retain the existing companion-only profile
and disabled automatic random population. Corpus character GUIDs must match the
existing database; the Winifred corpus currently targets GUID 3.

## Build without running a server

```sh
git submodule update --init --recursive
cmake -S . -B build-companion \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX="$PWD/artifacts-companion" \
  -DSCRIPTS=static -DMODULES=static -DTOOLS_BUILD=none -DBUILD_TESTING=OFF
cmake --build build-companion --parallel 6
```

These commands only compile. They do not install binaries, start a game server,
connect to the game databases, or import SQL. The October core update contains
11 new world-database migration files relative to the deployed September 28 core;
apply them only as part of the agreed deployment to the existing server.

## Standalone regression suites

```sh
for suite in selfbot condensation adventure lore history mutations quest_reactions recovery foundation; do
  cmake -S "modules/mod-pbc/tests/$suite" -B "build-tests/$suite" || exit
  cmake --build "build-tests/$suite" --parallel 2 || exit
  ctest --test-dir "build-tests/$suite" --output-on-failure || exit
done
```

On Debian 13 / GCC 14 / MySQL client 8.4.11, all 17 existing standalone tests
passed. The history fixture now covers the adventure journal hook independently
of dialogue SQL availability. HTTP tests use temporary loopback fixtures; they
require no WoW server, game database, API key or external model call.

Core and Playerbots C++ codestyle checks passed. SQL codestyle passed against the
existing local refs with the checker's network fetch omitted. The imported SQL
files are unchanged upstream files; no database migration was executed here.
Compilation does not establish gameplay correctness. No new in-game validation
or Windows/macOS build is claimed for this assembly.
