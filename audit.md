# Free Eggbert / Speedy Blupi 2 — Decompilation Audit & Long-Term Plan

**Date:** 2026-07-08
**Author:** Prepared by Claude Code, at the request of Robert Vokac (project owner)
**Scope:** `free-eggbert` repository (this repo) + the Ghidra project
`speedy-blupi-2_robertvokac/speedy-blupi-2` on the local library drive
(`drive.openeggbert.com`), plus the wider `openeggbert` GitHub organization
that supports it.

**Purpose of this document:** a single reference that (a) inventories
exactly what exists today across the source repo and the Ghidra project,
(b) analyzes what is missing to call the decompilation "done" in two
senses — *behaviorally correct* and *buildable as every historical
variant of the game from one codebase* — and (c) lays out a workflow,
sustainable over an arbitrarily long horizon (the user has explicitly
framed this as "even if it takes 40 more years of my life, 1000 hours a
year"), for using AI coding agents (Claude Code, ChatGPT, and similar) as
a force multiplier on that work without the effort collapsing under its
own weight after the first few months.

This is a working document, not a one-time report. It is meant to be
re-read, edited, and kept current as the project evolves — see
[Section 9](#9-knowledge-persistence-so-progress-survives-decades-and-model-resets)
for why that matters more than almost anything else here.

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Current State Inventory](#2-current-state-inventory)
3. [Gap Analysis](#3-gap-analysis)
4. [Goal 1 — Behaviorally Faithful Decompilation](#4-goal-1--behaviorally-faithful-decompilation)
5. [Goal 2 — One Codebase, Many Variants via Build Macros](#5-goal-2--one-codebase-many-variants-via-build-macros)
6. [Long-Term Roadmap (Horizons, Not a 40-Year Task List)](#6-long-term-roadmap-horizons-not-a-40-year-task-list)
7. [AI-Assisted Workflow — Claude Code, ChatGPT, and Friends](#7-ai-assisted-workflow--claude-code-chatgpt-and-friends)
8. [Tooling Stack](#8-tooling-stack)
9. [Knowledge Persistence — So Progress Survives Decades and Model Resets](#9-knowledge-persistence-so-progress-survives-decades-and-model-resets)
10. [Concrete Next Actions (First 90 Days)](#10-concrete-next-actions-first-90-days)
11. [Risks and Limitations](#11-risks-and-limitations)
12. [Ecosystem Map — the Wider `openeggbert` Organization](#12-ecosystem-map--the-wider-openeggbert-organization)
13. [Appendix A — Ghidra Project Binary Inventory](#appendix-a--ghidra-project-binary-inventory)
14. [Appendix B — Source File Status Table](#appendix-b--source-file-status-table)

---

## 1. Executive Summary

**The good news: this project is much further along than "starting a
decompilation from scratch."** Three things are already in place that a
typical reverse-engineering effort spends its first year building:

1. A **partially working, buildable, playable C++ reconstruction**
   (`free-eggbert`, ~31,700 lines across `src/` + `include/`) that already
   runs on Windows (native DirectX and via a compatibility shim), Linux,
   Web (Emscripten), and has early Android support.
2. A **mature Ghidra project** with roughly **20 imported binaries**
   spanning the entire *Blupi* lineage (Planet Blupi demos and full
   version, Speedy Blupi demo/1.0/2.2, four Speedy Eggbert 1.x variants,
   Speedy Eggbert 2, a Russian localization, French/"Home" variants, and
   the Windows Phone .NET port DLLs), **Version Tracking correlation
   sessions** between several of these binaries, and **captured debugger
   traces** from live runs (including of the DirectPlay multiplayer
   service and a MIDI synth helper process). This is exactly the kind of
   cross-version correlation data that turns "guess what this function
   does" into "confirm what this function does by diffing five variants
   of it."
3. A **wider `openeggbert` organization** of ~25 sibling repositories
   (`free-api`, `free-direct`, `sharp-runtime`, `planetblupi`, asset/tool
   repos, etc.) that already contains a proven, working convention for
   long-horizon AI-agent-driven development (`CLAUDE.md` / `NEXT.md` /
   `plan.md`) — it simply hasn't been applied to `free-eggbert` yet. See
   [Section 12](#12-ecosystem-map--the-wider-openeggbert-organization).

**The two goals stated for this audit, restated precisely:**

- **Goal 1:** Finish decompiling the game so it is fully playable and
  behaviorally identical to the original, not just "close enough."
- **Goal 2:** Make the *same* decompiled source tree buildable, via
  compile-time configuration (macros/CMake options), as any of the
  historical `.exe` variants — Speedy Blupi (demo, 1.0, 2.2), Speedy
  Eggbert (1.0, 1.1, demo, SE/Valueware, 2) — rather than needing a
  separate fork or hand edit per variant.

**The honest framing of "40 years / 40,000 hours":** a literal
year-by-year task list for four decades is not a meaningful engineering
artifact — nobody can predict 2050's tooling, and the plan would be
stale within a year regardless. What *is* meaningful, and what this
document provides, is (a) a **process** that scales indefinitely without
degrading — because progress is stored outside any single AI
conversation, in the repository itself — and (b) a **phased roadmap in
Horizons** (Section 6) with the first 1-2 Horizons planned concretely and
later Horizons deliberately left as a lighter-touch strategy to be
re-planned as they approach. Time budget (1000 h/year) is treated as a
*throughput constraint* that should shape scope per Horizon, not as a
literal Gantt chart.

---

## 2. Current State Inventory

### 2.1 `free-eggbert` repository (this repo)

| Aspect | State |
|---|---|
| Language | C++20 |
| Size | ~31,700 lines across 24 `.cpp` files + 19 `.hpp` headers |
| License | GPLv3 |
| Build systems | CMake (primary; Free Direct/SDL3 backend), Visual Studio 2022 `.sln`/`.vcxproj` (native DirectX 3), legacy MSVC5 `.dsp` (`msvc5/sb2decomp`) |
| Platforms working today | Windows (native DirectX, and via `free-api`/`free-direct` shim), Linux (SDL3-backed `FREEDIRECT` backend), Web/WASM (Emscripten, with IndexedDB persistence), Android (early) |
| Dependencies | `dxsdk3` (DirectX 3 SDK, submodule), `third_party/SDL`, `SDL_image`, `SDL_mixer` (submodules, vendored by default), `bass`/`bassmidi` (optional alt audio backend), sibling repos `free-api` and `free-direct` (`add_subdirectory(../free-api)`, `add_subdirectory(../free-direct)` — **not vendored, must exist as sibling checkouts**) |
| Existing regression tests | A `test_*` binary suite already exists in the build tree (`test_eggbert_loop`, `test_planetblupi_loop`, `test_gdi_regressions`, `test_winuser_regressions`, `test_mci_sequences`, `test_joystick_regressions`, `test_loadstring_regressions`, `test_timer_regressions`, `test_file_regressions`, `test_resources`, `test_header_compile`, `test_input_pipeline`, `test_mci_avivideo_regressions`, `test_timeb`, `test_file_paths`) — these are built via `free-api`'s own `CMakeLists.txt` (`add_subdirectory(../free-api FREE_API)`), not from sources inside `free-eggbert`. **This is a real, working regression-test culture already in place one repo over — it should be extended into `free-eggbert` itself, see Section 7.4.** |
| Symbol-name provenance | Many demangled function/variable names come from ILSpy-inspecting `WindowsPhoneSpeedyBlupi.dll`, the 2013 Windows Phone C#/.NET port of Speedy Blupi, archived by forum user "Ч.У.Ш" (4PDA). This port is structurally closer to source-level C++ than raw Ghidra pseudocode and should be treated as a first-class reference, not a footnote. |
| Original decompilation authorship | This codebase did **not** start with Robert Vokac. `git log --reverse` shows the project began as `HMVocaloid/Speedy-Eggbert-2-Source-Code-Decomp` on GitHub, with early contributions from a collaborator using the handle `jummy` (whose initials also appear as "JUMMY WAS HERE" in `util/extract_*.py`, the scripts that pull static tables directly out of `BLUPI.exe` by hardcoded file offset). Robert Vokac's `free-eggbert` is a continuation/fork of that community effort. **Action:** the original `HMVocaloid` repo (issues, any wiki, alternate branches) has not been re-examined in this audit and may hold additional notes worth mining. |
| Known compile-time variant flags (already exist, just not build-matrixed) | `include/def.hpp`: `_DEMO`, `_EGAMES`, `_SE`, `_INTRO`, `_CD`, `_BYE`, `_LEGACY`, `_DREAM`, `_BASS` — all plain `#define`s that must be hand-edited and the whole project rebuilt to switch variant. This is the exact mechanism Goal 2 needs to replace with a CMake-level, per-target configuration (Section 5). |
| Known decompilation artifacts/bugs (already documented in README / prior `DOC.md`) | Double `CPixmap::Create()` call during init (likely spurious); at least one function with swapped parameter order from miscompiled calling-convention recovery; `delete this` patterns copied literally from decompiler output; widespread apparently-uninitialized locals that are artifacts of the original compiler's aggressive stack-slot reuse (suppressed today via MSVC `/wd4700 /wd4703`) — the `_LEGACY` flag exists specifically to let you toggle "keep the historical bug" vs "apply the confirmed fix." |
| Files flagged as needing the most work (from README, in priority order) | `event.cpp` (5,878 lines — input/phase dispatch, by far the largest and most tangled file), `decblupi.cpp` (4,395 lines — player physics/vehicles), `decio.cpp`, `decdesign.cpp`, `decblock.cpp`, `decmove.cpp`, `decnet.cpp`, `decor.cpp`, `misc.cpp` |
| Files considered tentatively complete (still need verification) | `blupi.cpp`, `button.cpp`, `jauge.cpp`, `menu.cpp`, `movie.cpp`, `network.cpp`, `pixmap.cpp`, `sound.cpp`, `wave.cpp` |
| Explicit `TODO`/uncertain markers found in source (2026-07-08 grep) | 11 inline markers, concentrated in `decblock.cpp`, `decdesign.cpp`, and `event.cpp` (e.g. `decblock.cpp:16: //TODO: rewrite this like a human`) — a small, honest number; the *actual* gap is almost certainly larger and simply undocumented (see Section 3). |
| Working-tree note | `DOC.md` (a prior comprehensive technical reference, added in commit `dae5652`) is currently **deleted in the working tree but not committed**. Its content overlaps substantially with Sections 2, 3 and Appendix B of this document — this audit effectively supersedes and extends it. Recommend deciding whether to restore, delete for good, or fold its remaining unique content (the full `TYPE_*`/`ACTION_*`/`SOUND_*` enumerations) into a `docs/` reference page; not touched by this audit. |

### 2.2 Reference asset library (`drive.openeggbert.com`)

Beyond the Ghidra project (Section 2.3) and the source repo, the local
library drive holds substantial primary and secondary source material
that is directly usable for decompilation verification work:

- `Blupi/Games/Speedy_Blupi_(Windows)/` — `Bugs/`, `Covers/`, `Images/`,
  `Level/`, `Levels/`, `Mods/`, `Other/`, `Proposal_of_new_features/`,
  `Screens/`, `spritesheets/`, `Troubleshooting/`: community-collected
  bug reports, level dumps, sprite sheets and mods (e.g.
  `restoring_dream_blocks_in_speedy_blupi_2/Eggbert2_customobjgfx.exe`,
  `floppy_mod/sb2_1.4M.exe`) — several of these mods are themselves
  patched binaries, i.e. more variants to eventually correlate.
- `Blupi/Games/Speedy_Blupi_(Windows_Phone)/` — resources and
  screenshots for the C#/.NET port that is the source of many symbol
  names (Section 2.1).
- `Blupi/Games/Planet_Blupi/` — the predecessor engine, `pblupi-e.exe`.
- `other/` — Blupi comics (`BLUP_COMICS_by_Daniel_Roux*.zip`), the
  *Blupi at Home* and *Blupimania* spin-off screenshots/binaries,
  `speedydgVoodoo.zip` (a Voodoo/3dfx compatibility patch — evidence of
  yet another binary variant), `vr1tovr2_v2.2.zip` (a version-upgrade
  patch, useful for understanding exactly what changed between two
  shipped builds), and a cropped promotional animation.

None of this has been exhaustively catalogued function-by-function; it
should be treated as a **priority-ordered backlog of ground truth**, not
as something to process all at once.

### 2.3 Ghidra project inventory (`speedy-blupi-2`)

The project directory (`Speedy Blupi 2.rep`) is a genuine, non-trivial
Ghidra project, not an empty shell. Reading its internal index directly
(`idata/~index.dat`) recovers the full catalog of everything ever
imported, including items that no longer show up anywhere else on disk.
See **Appendix A** for the full table. Highlights:

- **~11 `Program` entries** (importable, disassemblable binaries): the
  main target `SpeedyBlupi2_v2.2_EN.exe`, plus `SpeedyBlupiDemo_v0.8_EN`,
  `SpeedyBlupi_v1.0_EN`, `SpeedyEggbert_v1.0_EN`, `SpeedyEggbert_v1.1_EN`,
  `SpeedyEggbertDemo_v1.0_EN`, `SpeedyEggbertSEValueware_v1.0_EN`,
  `SpeedyEggbert2_v1.0_EN`, `ReaktivniyEgbert_v1.1_RU` (a Russian
  localization — "Reaktivniy Egbert" ≈ "Speedy Eggbert"),
  `PlanetBlupi_v1.7_EN`, `PlanetBlupiDemo_v1.6_EN`,
  `PlanetBlupiDemo_v1.8_EN`, and `BLUPIF.EXE` / `BLUPIH.EXE` (French /
  another regional or "Home" build — needs confirmation).
- **Seven named Version Tracking correlation sessions** already created
  between specific binary pairs, e.g. `SB2 v2.2 to SE2 v1.0`,
  `SB2 v2.2 to PB v1.7`, `PB v1.7 to PB v1.6 demo`, `PB v1.7 to SB2 v2.2`,
  `SB2 v2.2 to SB v1.0`, `SB v1.0 to SE v1.0`, `SB v1.0 to RE v1.1`,
  `SB2 v2.2 to FwB`, `SB2 v2.2 to BaH` (the latter almost certainly
  correlates against `blupi-home-demo.exe`, "Blupi at Home", found
  separately in `other/blupi_at_home/`). **This is the single most
  underused asset in the whole project** — Ghidra's Version Tracking
  feature exists precisely to propagate confirmed function names, types
  and even matched-decompiled semantics from one binary to a related one.
  If even one of these ~11 programs has been analyzed to high confidence,
  that confidence can be pushed into the others automatically instead of
  re-derived by hand.
- **Debugger trace captures** ("New Traces" folder) from live dynamic
  analysis sessions dated July 2024, including traces of
  `dplaysvr.exe` (the DirectPlay multiplayer service — relevant to
  finishing `decnet.cpp`/`network.cpp`) and `VirtualMIDISynth-synth.exe`
  (relevant to the music/MIDI path). This confirms dynamic
  analysis/debugging (not just static disassembly) has already been used
  at least once and the harness for it exists.
- **The DirectX 3/5-era SDK headers** (`include/d3d.h`, `ddraw.h`,
  `dinput.h`, `dmusicc.h`, etc., dated 1999) are imported into the Ghidra
  project's data-type archive — the same SDK vendored as the
  `dxsdk3` submodule in `free-eggbert` — so Ghidra's decompiler already
  has accurate struct/COM-interface layouts for DirectDraw/DirectSound/
  DirectInput/DirectMusic calls rather than guessing them.
- System DLLs imported for import resolution: `ddraw.dll`, `ddrawex.dll`,
  `dinput.dll`, `dinput8.dll`, `dmusic.dll`, `dplay.dll`, `dplayx.dll`,
  `dsound.dll`, `gdi32.dll`, `kernel32.dll`, `ntdll.dll`, `user32.dll`,
  `winmm.dll`.
- `util/extract_phasetable.py`, `extract_pixtables.py`,
  `extract_tableblupi.py`, `extract_tablemirror.py` in `free-eggbert`
  read hardcoded byte offsets directly out of `BLUPI.exe` ("speedy blupi
  2.2 english ver" per their own comments) to recover static data tables
  (phase table, icon tables, mirror table). **These offsets are specific
  to that one exact binary** — they will silently read garbage (or crash)
  against any other variant, which is exactly the failure mode Goal 2
  needs to design away from (Section 5).

---

## 3. Gap Analysis

Cross-referencing the README's own "files requiring the most attention"
list, the 11 inline `TODO` markers, and the DOC.md "Known Issues"
section against what a from-scratch audit would expect to find, three
categories of gap emerge — worth keeping separate because they need
different tools and different AI-assistance strategies:

### 3.1 Behavioral gaps (code exists, may be wrong)
Most of `event.cpp`, `decblupi.cpp`, `decnet.cpp`, `decdesign.cpp`,
`decio.cpp`. The code compiles and mostly runs, but fidelity to the
original binary's exact behavior is unverified — the only way to close
this gap is **systematic diffing against ground truth** (Ghidra
pseudocode of the matching function in `SpeedyBlupi2_v2.2_EN.exe`, cross-
checked via the Version Tracking sessions against the other ~10
variants, and — where genuinely ambiguous — against Planet Blupi's
released source per the project's own stated convention). Note that
`planetblupi` is not just archived text to read — it is a real sibling
repository (~41,270 lines of C++, GPLv3 lineage via Daniel Roux/Epsitec
SA) that is itself buildable and runnable today via the same
`free-api`/`free-direct` stack `free-eggbert` uses (Section 12.1).
Prefer building and running it side-by-side over relying on memory of
what its source does.

### 3.2 Known decompiler artifacts (code exists, is *known* to be an
artifact of decompilation, not of the original design)
Double `CPixmap::Create()`, the parameter-swap bug, `delete this`
patterns, and the large class of "uninitialized" variables that are
really register-allocation reuse from the original optimizing compiler.
These need a human/AI reviewer who understands *why* Ghidra produces this
pattern (stack frame reuse under `/O2`-class optimization, C++ vtable
thunks, etc.) rather than a naive "fix the warning" pass — naive fixes
here are exactly where an AI assistant without ground-truth grounding
will confidently introduce new bugs. The book the project owner has
already purchased (*Software Reverse Engineering: Navigating x86 Windows
Applications with Ghidra*) directly targets this skill.

### 3.3 Structural gap: one binary vs. the whole family
Everything in the repo today targets *one* binary
(`SpeedyBlupi2_v2.2_EN.exe`) even though the Ghidra project already has
~10 siblings analyzed. The `util/extract_*.py` hardcoded-offset scripts
are the sharpest example of this: they are correct for exactly one file
and will not generalize. Goal 2 (Section 5) exists to close this gap
deliberately, not as an afterthought once Goal 1 is "done" — doing it
concurrently is cheaper, because every variant-specific quirk discovered
along the way informs the behavioral-fidelity work in 3.1.

### 3.4 What is *not* a gap (don't rebuild what already works)
The cross-platform build (CMake/SDL3, Emscripten/Web with IndexedDB
persistence, early Android), the `free-api`/`free-direct` compatibility
shim architecture, and the existing `test_*` regression binaries are
functioning infrastructure. AI-assisted effort should route around
these, not through them.

---

## 4. Goal 1 — Behaviorally Faithful Decompilation

The target is not "the game runs" (already true) but "the game's
observable behavior — physics, AI, save format, network protocol,
editor — is provably the same as the original binary's, function by
function, with the confidence level tracked explicitly" (see Appendix B
and Section 9 for how "provably" is operationalized without requiring
literal byte-identical recompilation everywhere).

Recommended working definition of "done" per function, in increasing
order of confidence — use this exact ladder in the knowledge-persistence
ledger (Section 9):

1. **Not started** — untouched decompiler output or missing entirely.
2. **Drafted** — compiles, runs, "looks plausible," not checked against
   the binary.
3. **Behavior-verified** — manually or automatically compared against
   the original `.exe`'s observed behavior (gameplay trace, save-file
   round-trip, network packet capture) for representative inputs and
   found equivalent.
4. **Structurally verified** — the C++ has been diffed line-by-line
   against Ghidra's decompilation of the *same* function across all
   correlated variants (via Version Tracking) and against the Windows
   Phone C# port where available, with no unexplained divergence.
5. **Byte-matched** — recompiling this translation unit with a period-
   correct toolchain reproduces the original machine code exactly (or
   the diff is fully explained, e.g. a different register allocation
   with identical semantics). This is the gold standard used by mature
   decompilation communities (Nintendo 64/GameCube-era "matching decomps"
   like Super Mario 64, Ocarina of Time, Paper Mario; and — much closer
   to this project's actual era and toolchain — the **LEGO Island
   (`isledecomp`) project**, which built tooling (`reccmp` and friends)
   specifically for *MSVC-compiled, DirectX-era, C++ Win32* games. That
   project is worth studying directly as the closest known precedent —
   verify its current state and license before adopting any of its
   tooling, but the *methodology* (function-level binary diffing against
   a period MSVC build) transfers almost directly to Speedy Blupi 2.

Levels 1-3 are achievable now, with the current toolchain, and should be
the near-term focus. Level 5 is a multi-year stretch goal gated on
recovering (or closely approximating) the original build environment —
`msvc5/sb2decomp` in this repo already hints at MSVC 5/6 being the
original toolchain, consistent with the DirectX 3 SDK date (1999).

---

## 5. Goal 2 — One Codebase, Many Variants via Build Macros

### 5.1 What exists today
Variant selection is a single, flat set of `#define`s in `include/def.hpp`
(`_DEMO`, `_EGAMES`, `_SE`, `_INTRO`, `_CD`, `_BYE`, `_LEGACY`, `_DREAM`,
`_BASS`) that must be hand-edited before each rebuild. This works for
"one developer building one variant at a time" but cannot express "build
all N known variants in one CI run" or "add a 12th variant without
touching shared code."

### 5.2 What Goal 2 actually requires
Given the Ghidra inventory in Appendix A, the known variants that
plausibly belong on this ladder are at minimum:

| Variant family | Binaries (from Ghidra project) | Notes |
|---|---|---|
| Speedy Blupi | `SpeedyBlupiDemo_v0.8_EN`, `SpeedyBlupi_v1.0_EN`, `SpeedyBlupi2_v2.2_EN` | `SpeedyBlupi2_v2.2_EN` is the current decompilation target |
| Speedy Eggbert (1) | `SpeedyEggbert_v1.0_EN`, `SpeedyEggbert_v1.1_EN`, `SpeedyEggbertDemo_v1.0_EN`, `SpeedyEggbertSEValueware_v1.0_EN` | `_SE`/`_EGAMES` flags already exist for exactly this family |
| Speedy Eggbert 2 | `SpeedyEggbert2_v1.0_EN` | English rename/re-release of Speedy Blupi 2 |
| Localizations | `ReaktivniyEgbert_v1.1_RU`, `BLUPIF.EXE`/`BLUPIH.EXE` | Confirm exact French/Home mapping before building targets for these |
| Mods (lower priority) | `Eggbert2_customobjgfx.exe`, `sb2_1.4M.exe` (floppy mod) | Real historical binaries but derivative — treat as stretch targets, not core matrix |

Planet Blupi variants are a related but **separate engine/repo**
(`planetblupi`, see Section 12) and should stay out of `free-eggbert`'s
build matrix even though they share Ghidra Version Tracking correlations
useful for cross-checking.

### 5.3 Recommended architecture
Move from "one `#define` set, hand-edited" to a **CMake variant matrix**:

1. Introduce a single **variant descriptor** (e.g.
   `cmake/variants.cmake` or a small JSON/CMake data file) listing each
   known target with: display name, the `_DEMO`/`_EGAMES`/`_SE`/`_INTRO`/
   `_CD`/`_BYE`/`_LEGACY`/`_DREAM` values it needs, which `gamefiles/`
   asset set it expects, and (once known) which of the hardcoded-offset
   tables in `util/extract_*.py` need re-extracting for that binary.
2. Replace the raw `#define`s in `def.hpp` with values driven by
   `target_compile_definitions(<target> PRIVATE _DEMO=$<...>)` per
   CMake target, so `cmake --build . --target speedy_blupi_2_2_en` and
   `--target speedy_eggbert_demo_1_0_en` can coexist from one configure,
   one source tree, zero manual edits.
3. Retire the hardcoded byte-offset extraction scripts as a manual,
   per-release, run-once step; replace them with either (a) values
   already recovered into the decompiled source as named constants/
   tables (preferred, since then they're variant-agnostic and don't need
   `BLUPI.exe` at build time at all), or (b) a small offset-table
   *per variant*, keyed by the same variant descriptor, if a table
   genuinely cannot be recovered as source.
4. Add one CI matrix job per variant target (build + smoke-run against
   that variant's `gamefiles/`, once available) so a regression in one
   variant is caught immediately instead of silently accumulating.

This should be sequenced *early* (see Section 10), not after Goal 1 is
"finished" — most of the behavioral-fidelity work in Section 4 will
naturally surface variant-specific branches (`_EGAMES`/`_SE`/`_CD` gate
real behavioral differences, not cosmetic ones), and it is far cheaper to
express those as data in the variant descriptor from the start than to
retrofit them later.

---

## 6. Long-Term Roadmap (Horizons, Not a 40-Year Task List)

Framed against a 1000 h/year budget, decades-scale but re-planned as it
approaches:

**Horizon 1 — Foundation (roughly 0–2 years, ~1000–2000 h):**
Behavior-verify (Section 4, level 3) the highest-priority files in
priority order (`event.cpp`, `decblupi.cpp`, `decio.cpp`, `decdesign.cpp`,
`decblock.cpp`, `decmove.cpp`, `decnet.cpp`, `decor.cpp`, `misc.cpp`).
Stand up the knowledge-persistence ledger and `CLAUDE.md`/`NEXT.md`
convention (Section 9) *first*, before deep behavioral work, so nothing
done in this Horizon is lost to context resets. Land the CMake variant
matrix (Section 5) for at least the Speedy Blupi and Speedy Eggbert
English variants. Stand up an automated gameplay-replay regression
harness (Section 7.4) using the existing `demo3xx.blp` recordings.

**Horizon 2 — Structural verification & remaining variants (roughly
2–5 years, ~2000–3000 h):** Extend behavior-verification to
structural-verification (level 4) across the full file list. Extend the
variant matrix to the localizations and remaining variants once their
exact identity is confirmed. Begin recovering the original build
environment (MSVC 5/6 + DirectX 3 SDK) well enough to attempt level-5
byte-matching on a handful of small, self-contained functions as a proof
of concept — this is the point at which studying `isledecomp`/`reccmp`-
style tooling in depth pays off.

**Horizon 3 — Byte-matching & preservation depth (roughly 5–15 years):**
Scale byte-matching as far as it is economical to go (it will likely
never be 100% for a C++ binary of this size and age — MSVC C++ ABI
quirks, inlining, and vtable layout make this categorically harder than
the C-only 90s console decomps that pioneered the technique). Deepen
platform ports (Android, further web work) on top of a codebase that is
by now trustworthy enough to port confidently.

**Horizon 4 — Maintenance & handoff (15–40 years):** By this point the
practical goal shifts from "verify more" to "keep it building, keep it
documented, keep the org's institutional knowledge (Section 9) legible to
whoever — human or AI — picks it up next," including this user
personally at a very different life stage, or a successor maintainer. Do
not attempt to plan this Horizon's task list now; re-derive it when
Horizon 3 is closing.

**The actual lesson from framing it this way:** the binding constraint on
a project like this is not calendar time, it is whether **institutional
knowledge compounds or resets**. A plan that gets 90% of the way to
"done" in year 3 and then has to be re-discovered from scratch in year 4
because nothing was written down is worse than a plan that moves slower
but never re-derives the same fact twice. Section 9 is the single most
important section in this document for that reason.

---

## 7. AI-Assisted Workflow — Claude Code, ChatGPT, and Friends

This section is the direct answer to "how do I use AI so it actually
helps with the decompilation, even across tens of thousands of hours."
The short version: **AI is extremely good at the parts of this task that
are "read a lot, compare carefully, write it up clearly" and extremely
dangerous, unsupervised, at the parts that are "assert what x86 assembly
means without checking."** Structure the workflow so the first kind of
task is delegated freely and the second kind is always checked against
ground truth (Ghidra's own disassembly/decompilation, or actual program
behavior) before being trusted.

### 7.1 What AI is reliably good at here
- **Reading and explaining Ghidra pseudocode.** Given raw decompiler
  output for a function plus the surrounding data types (already
  present, given the imported DirectX headers), an LLM is good at
  proposing idiomatic, readable C++ that preserves semantics — variable
  naming, restructuring `goto`-heavy control flow into loops, spotting
  where a pattern matches a known idiom (e.g. a state machine over
  `WM_PHASE_*`).
- **Cross-referencing across large amounts of text** — e.g. "does this
  function's Ghidra pseudocode in `SpeedyBlupi2_v2.2_EN.exe` match the
  corresponding function surfaced by the `SB2 v2.2 to SB v1.0` Version
  Tracking session in `SpeedyBlupi_v1.0_EN.exe`, and does either match
  the corresponding class in the Windows Phone C# port?" This is
  exactly the kind of multi-source diffing that benefits from an agent
  that can hold several files in context simultaneously and is patient
  about it.
- **Mechanical, high-volume, low-risk sweeps**: generating Doxygen
  comment scaffolding, auditing every `TODO`/uncertain marker and
  classifying it, writing the CMake variant-matrix boilerplate from a
  descriptor, drafting the first pass of a new regression test from an
  existing demo replay.
- **Writing and maintaining the knowledge-persistence ledger itself**
  (Section 9) — this is a text-generation and organization task AI
  agents excel at, and it is the highest-leverage use of AI time in the
  entire project because of the compounding effect described in
  Section 6.

### 7.2 What AI is *not* reliably good at, unsupervised
- **Asserting x86 calling-convention or optimization semantics from
  memory.** LLMs will confidently describe register-allocation or
  stack-layout behavior that sounds plausible and is wrong for this
  specific compiler/optimization level. Ground every such claim in
  Ghidra's actual disassembly of the actual binary, not in the model's
  prior knowledge of "how MSVC usually does X." This is precisely why
  the project owner studying *Software Reverse Engineering: Navigating
  x86 Windows Applications with Ghidra* is high-value personal
  investment that AI cannot substitute for — it builds the judgment
  needed to tell when the AI's explanation of a decompiled function is
  actually consistent with the disassembly versus merely fluent-sounding.
- **"Fixing" apparently-uninitialized variables or `delete this`
  patterns by local reasoning.** As noted in Section 3.2, several of
  these are known artifacts with specific, non-obvious root causes. An
  AI asked to "clean up this warning" without that context will produce
  confident, plausible, wrong code. Always frame these tasks as
  "explain why Ghidra produced this pattern here, citing the actual
  disassembly," not "fix this."
- **Silent scope creep on a 32,000-line, decades-long project.** Without
  an explicit ledger of what's already verified (Section 9), an AI
  session has no way to know whether it's re-deriving a fact already
  established two years ago or discovering something new. This is a
  process failure, not a model failure, and the fix is process
  (Section 9), not a better prompt.

### 7.3 Concretely, how to run sessions
- **Use Claude Code (or an equivalent agentic coding CLI) as the primary
  driver**, not a plain chat interface, precisely because this task
  needs to *read* Ghidra exports, source files, and reference binaries
  concurrently, and to *write* both code and the persistent ledger as it
  goes — a chat window that can't touch the filesystem forces constant
  manual copy-paste and loses the audit trail.
- **Investigate a Ghidra↔agent bridge** so the agent can query the live
  Ghidra project directly (list functions, pull a specific function's
  decompilation, pull xrefs, rename/retype a symbol) instead of a human
  manually exporting pseudocode and pasting it in. Community bridges for
  this exist in the Ghidra ecosystem (commonly implemented as a Ghidra
  script/plugin exposing an HTTP or MCP-style interface, sometimes paired
  with Ghidrathon for Python scripting inside Ghidra itself) — evaluate
  current options against this Ghidra version and this project's
  security posture before adopting one; do not assume any specific named
  tool is still maintained without checking.
- **For large mechanical sweeps** (e.g., "classify every one of the 11
  `TODO` markers plus every undocumented function in `event.cpp` by
  confidence level," or "diff this function across all 6 correlated
  variants and report divergences"), use a multi-agent fan-out
  (parallel review agents, one per file or per variant-pair, each
  reporting structured findings) rather than one long serial
  conversation — the task is naturally parallel and the volume would
  otherwise blow past any single context window.
- **Keep session scope narrow and let the ledger carry continuity.** A
  session that opens by reading `NEXT.md` (today's queue) and
  `docs/decomp-status/<file>.md` (this file's confirmed facts), does one
  bounded unit of work, and closes by updating both, is far more durable
  across a 40-year horizon than a session that tries to hold the whole
  project's state in its own context.

### 7.4 High-value harness: gameplay-replay regression testing
The game already ships **prerecorded demo playback files**
(`demo300.blp`–`demo309.blp`) and a fully-specified save-state struct
(`DescSave`, ~56 KB, covering world grid, all moving objects, player
state, doors, cheats). This is, almost by accident, exactly the raw
material a "matching decompilation" project needs for behavior-level
regression testing:

1. Run each demo recording through the *original* binary (under Wine,
   DOSBox-adjacent tooling, or a VM/emulator as needed — several original
   binaries are already in the Ghidra project and library) and capture
   `DescSave` state at fixed intervals or on level-end.
2. Run the same recording through `free-eggbert`'s own simulation and
   capture the same state.
3. Diff the two. Any divergence is a concrete, reproducible bug report
   pointing at a specific tick and a specific field — vastly more
   actionable than "movement feels slightly off."

This directly extends the existing `test_planetblupi_loop` /
`test_eggbert_loop` regression tests already present via `free-api`, so
it is additive to proven infrastructure rather than a new invention. It
is one of the highest-leverage single investments available for
Section 4's behavior-verification work, because once built it runs
unattended and produces an ever-growing, self-checking confidence set —
exactly the kind of artifact that keeps paying off across a multi-decade
horizon instead of being redone by hand each session.

---

## 8. Tooling Stack

| Tool | Role today | Recommended role going forward |
|---|---|---|
| **Ghidra** | Primary static analysis, Version Tracking across variants, live debugger traces | Keep as the system of record for "ground truth about the binary." Consider a scripted/agent-queryable interface (Section 7.3) so AI sessions can pull decompilation directly instead of manual copy-paste. |
| **IDA** | Used per README, role not detailed further here | Useful as a cross-check against Ghidra's decompiler on the hardest functions — different decompilers sometimes disagree in instructive ways. |
| **ILSpy** | Inspecting `WindowsPhoneSpeedyBlupi.dll` for demangled names/structure | Continue treating the WP8 C# port as a first-class reference implementation, not just a naming source — its control flow is much closer to source-level than raw x86 pseudocode. |
| **Visual Studio 2022** | Native Win32/DirectX build target | Keep for the "does it work on real Windows/DirectX 3" check. |
| **Legacy MSVC 5/6** (`msvc5/sb2decomp`) | Present but apparently not actively used for builds today | Revive deliberately once byte-matching (Section 4 level 5) becomes a near-term goal — matching the *exact* original compiler is a hard requirement for that level, not a nice-to-have. |
| **CMake + SDL3** | Cross-platform build (Linux/Web/Android), `FREEDIRECT` backend | Extend with the variant matrix (Section 5) and a CI job per variant. |
| **Emscripten** | Web/WASM build with IndexedDB persistence | Working; low priority for further audit attention. |
| **`isledecomp`/`reccmp`-style matching-decomp tooling** (external project, LEGO Island) | Not currently used | Evaluate directly as the closest known precedent for *MSVC, DirectX-era, C++* byte-matching decompilation — verify current maintenance status and license before depending on it; the methodology is the main asset even if the tooling itself needs adaptation. |
| **`planetblupi`** (sibling repo) | Buildable, runnable predecessor engine (Section 12.1) | Use as a live behavioral reference wherever `free-eggbert`'s decompiled behavior is ambiguous — build and run it, don't just read it. |
| **`tiled-blupi`** (sibling repo) | Tiled-editor extensions + a documentation site for the `.blp` format family (`blp-format.html`, `map-properties.html`, `editing-workflow.html`) | **Check this before re-deriving `.blp` structure by hand** — it may already document parts of what `decio.cpp`, `dectables.cpp`, and the `util/extract_*.py` offset scripts currently reverse-engineer from scratch. |
| **`speedyblupi-data`** (sibling repo) | Canonical original asset/cover archive, GPLv3 | Authoritative source for assets and release/version metadata across the whole variant family (Section 5.2). |
| **Claude Code / ChatGPT / equivalent agentic AI** | Ad hoc | Formalize per Section 7 and 9 — narrow-scope sessions, ledger-driven continuity, multi-agent fan-out for parallel mechanical work, always grounded in Ghidra/binary ground truth for anything asserting low-level semantics. |
| **The purchased book** (*Software Reverse Engineering: Navigating x86 Windows Applications with Ghidra*) + existing x86 references | Not yet studied (per user) | Pair reading with practice: after each chapter, apply its specific technique to one currently-unresolved function in `event.cpp` or `decblupi.cpp` and record the outcome in the ledger (Section 9) — this both cements the skill and produces project progress simultaneously, rather than reading in isolation from the work. |

---

## 9. Knowledge Persistence — So Progress Survives Decades and Model Resets

This is the mechanism that makes the "40,000 hours" framing survivable
at all: **an AI conversation's context is not where project knowledge
should live.** It must live in the repository, as plain text, so that any
future session — this year, or in 2050, run by this user or by whoever
inherits the project — can reconstruct full context in minutes instead
of re-deriving it.

The `openeggbert` organization already has a proven convention for this
in `free-direct` and `sharp-runtime` (see Section 12 for specifics once
confirmed) — the recommendation here is to **apply that same convention
to `free-eggbert`**, not invent a new one:

- **`CLAUDE.md`** (repo root) — stable, slowly-changing context: what
  this project is, the architecture (`free-api`/`free-direct` dependency
  boundary, variant matrix once built, where assets live), conventions
  (e.g. "always check the Version Tracking session before asserting a
  function's behavior differs across variants"), and pointers to this
  audit document and the status ledger below. This is what a brand-new
  AI session reads first.
- **`NEXT.md`** (repo root) — the current, short-lived task queue: what
  is being worked on *right now* and what's queued next. Gets rewritten
  constantly; not a history.
- **`plan.md`** (repo root or `docs/`) — the longer-horizon roadmap
  (essentially a living version of Section 6), re-derived/updated at
  Horizon boundaries rather than every session.
- **`docs/decomp-status/<source-file>.md`** (new, recommended) — one file
  per source file in `src/`/`include/`, listing every non-trivial
  function with its current confidence level on the ladder from
  Section 4 (Not started → Drafted → Behavior-verified →
  Structurally-verified → Byte-matched), the date/session it was last
  touched, and any known-artifact notes (Section 3.2) so nobody
  "fixes" a documented artifact by accident. This is the single
  ledger every session should read before touching a file and update
  before finishing.
- **`docs/variant-matrix.md`** (new, recommended) — human-readable
  companion to the CMake variant descriptor (Section 5.3): which
  binaries map to which build target, what's confirmed vs. guessed
  (e.g. the exact identity of `BLUPIF.EXE`/`BLUPIH.EXE` and the "FwB"
  Version Tracking session target).

**The habit to build, more than any specific file format:** end every
substantial AI-assisted session by updating the ledger before ending the
session, even if the code change itself feels too small to bother
documenting. The ledger is worthless if it silently falls out of date;
it is the entire point of this section.

---

## 10. Concrete Next Actions (First 90 Days)

Ordered so each step makes the next one cheaper, not by file-priority
alone:

1. **Stand up the ledger first** (Section 9): add `CLAUDE.md`, `NEXT.md`,
   `docs/decomp-status/` (seed it from the README's existing "most
   attention" / "tentatively complete" lists — that's a free first
   draft), and `docs/variant-matrix.md`. Decide what to do with the
   deleted-but-uncommitted `DOC.md` (restore, fold into `docs/`, or
   discard) as part of this pass.
2. **Confirm the ambiguous Ghidra inventory entries**: what exactly
   `BLUPIF.EXE`/`BLUPIH.EXE` are, what "FwB" in the `SB2 v2.2 to FwB`
   Version Tracking session refers to, and what `Telefou.xch` and
   `WORLD199.BLP` (both individually imported into the Ghidra project)
   were imported for. Cheap to resolve now, confusing to leave open.
   At the same time, **read through `tiled-blupi`'s `.blp` format
   documentation** (`blp-format.html`, `map-properties.html`,
   `editing-workflow.html`) — some of the ground this audit assumes still
   needs reverse-engineering (Section 3.3, the `util/extract_*.py`
   hardcoded offsets) may already be written up there.
3. **Pick one already-"tentatively complete" file** (e.g. `wave.cpp` or
   `jauge.cpp` — small, self-contained) and run it through the full
   confidence ladder (Section 4) end to end, including a first attempt at
   Ghidra Version-Tracking cross-checking against at least one other
   variant. The goal is to prove the *pipeline* end-to-end on a cheap
   example before spending real effort on `event.cpp`.
4. **Build the gameplay-replay regression harness** (Section 7.4) using
   `demo300.blp` first — even a partial version (compare a handful of
   `DescSave` fields at level-end, original binary run manually once to
   capture a baseline) is enough to start catching behavioral
   regressions automatically.
5. **Prototype the CMake variant matrix** (Section 5.3) for exactly two
   targets (`SpeedyBlupi2_v2.2_EN` full and demo) to validate the
   mechanism before generalizing to all ~10.
6. **Only then** resume systematic behavior-verification of `event.cpp`
   and `decblupi.cpp`, now backed by the ledger, the regression harness,
   and the variant matrix — so that work compounds instead of evaporating
   the moment the session ends.

---

## 11. Risks and Limitations

- **C++ makes byte-matching categorically harder than the C-only 90s
  console decomp scene this methodology is borrowed from.** Vtable
  layout, name mangling, inlining, and MSVC-specific ABI quirks mean
  100% byte-matching may never be fully economical for a binary this
  size. Treat structural-verification (Section 4, level 4) as the
  realistic default target and byte-matching as a stretch goal for
  specific, high-value, self-contained functions rather than a blanket
  requirement.
- **Provenance and legality**: this is a preservation/interoperability
  project built on decompiling a commercial binary whose source was
  never released, using assets that "must be obtained from an original
  copy" per the existing README. Keep the project's existing framing
  (research/preservation/compatibility, GPLv3 for the *reconstructed
  code* only, assets not redistributed) explicit in the ledger so it
  isn't accidentally eroded as the project scales across decades and
  contributors.
- **AI hallucination on low-level semantics** (Section 7.2) is the
  single largest correctness risk introduced by leaning on AI more
  heavily — mitigate structurally (always ground in Ghidra/binary
  output), not by trusting the model more as it becomes more fluent.
- **Tool rot over a 40-year horizon is a certainty, not a risk.** Ghidra,
  Claude Code, and every other named tool in this document will change
  or be replaced multiple times before this project could plausibly be
  "done." This is exactly why Section 9's plain-text, tool-agnostic
  ledger — not a Ghidra project file, not a chat history — is the
  actual long-term asset.
- **Scope discipline**: the `openeggbert` org's ~25 sibling repos
  (Section 12) show a real risk of effort diffusing sideways into
  adjacent tooling projects. That may well be the right call for the
  org as a whole, but within `free-eggbert` specifically, prefer finishing
  the decompilation of the currently-in-scope binaries before absorbing
  more.

---

## 12. Ecosystem Map — the Wider `openeggbert` Organization

`free-eggbert` is one repo in a ~25-repo personal organization
(`/rv/data/development/github.com/openeggbert/`) that has been under
active, AI-assisted development for years. Understanding this
ecosystem matters for two reasons: (a) several sibling repos are direct
dependencies or references for the decompilation work, and (b) the
organization has already converged on a working long-horizon AI-agent
workflow that simply hasn't been applied to `free-eggbert` yet.

### 12.1 Repo map

| Repo | Purpose | Relevance to `free-eggbert` decompilation |
|---|---|---|
| **`free-api`** | MIT, C++20 reimplementation of a minimal ~1998 Win32 API subset | **Direct build dependency** (`add_subdirectory(../free-api)`) — lets decompiled Win32 calls compile without real Windows |
| **`free-direct`** | MIT, C++20 reimplementation of a narrow DirectX 3 (2D) subset on SDL3 | **Direct build dependency** — the backend `free-eggbert` links for DirectDraw/DirectSound/DirectPlay on non-Windows targets |
| **`planetblupi`** | Buildable, portable copy of Epsitec's original 1997 *Planet Blupi* source (built via `free-api`/`free-direct`), ~41,270 lines of C++, GPLv3 lineage (Daniel Roux / Epsitec SA) | **The most important reference in the whole ecosystem for Goal 1.** This is not just "text to consult" — it is a *live, compilable, runnable* predecessor engine. Where `free-eggbert`'s decompiled behavior is ambiguous, build and run `planetblupi` itself rather than relying on memory of what its source does. |
| **`sharp-runtime`** / `sharp-runtime_work` | C++23 reimplementation of a .NET subset (`System::*`) supporting the CNA/`mobile-eggbert` chain | Not a direct dependency of `free-eggbert`, but see 12.2 — its process convention is the one to copy |
| **`mobile-eggbert`** (+ `-legacy`, `-libgdx`) | Port chain: WP8 XNA Speedy Blupi → ILSpy-decompiled C# → MonoGame → C++/CNA → LibGDX | This is the *origin* of the `WindowsPhoneSpeedyBlupi.dll` symbol names `free-eggbert` already borrows (Section 2.1) — treat as the same reference lineage, not a separate concern |
| **`speedyblupi-data`** | Canonical original asset archive (`originals/`, `backgrounds/`, `covers/` incl. `egames_scans`, an `analysis.fods` spreadsheet), GPLv3 | Authoritative source for `gamefiles/`-equivalent assets and box/cover metadata across releases |
| **`tiled-blupi`** | Tiled map-editor extensions for the Blupi `.blp`-family formats, with per-game folders for "Speedy Blupi" and "Speedy Blupi II" plus a documentation site (`blp-format.html`, `map-properties.html`, `editing-workflow.html`) | **The closest thing that already exists to a written `.blp` format specification.** Read this before doing further work on `decio.cpp`/`dectables.cpp`/the `util/extract_*.py` offset scripts — some of that reverse-engineering may already be documented here. |
| **`BlupiEdit`** | Level/scene editor (CMake + web build); has its own `plan.md` but no confirmed README/CLAUDE.md yet | Likely relevant to level-editor parity (`decdesign.cpp`) — worth a dedicated follow-up look, not yet fully characterized |
| **`galaxy-eggbert`**, `xna4-spec`, `cna*`, `easy-3d`, `easy-gl`, `fake-gl`, `mesh-craft`, `mesh-world`, `simple-3d`, `sprite-utils`, `rd4-to-gltf` | Broader OpenEggbert 3D/graphics framework and tooling (XNA-style engine, GL wrappers, scene formats, asset conversion) | Downstream/parallel creative and tooling work; not relevant to the Win32/DirectX3 decompilation itself — explicitly out of scope per the scope-discipline risk in Section 11 |
| **`openeggbert`** | Org-level meta repository/README | Context only |

### 12.2 The AI-agent workflow convention (already proven, just not here yet)

`free-direct` and `sharp-runtime` both use the same convention for
surviving long, multi-session, multi-year AI-assisted development — this
is the concrete pattern Section 9 recommends adopting verbatim in
`free-eggbert`:

- **`CLAUDE.md`** — a stable "project charter" that rarely changes:
  mission, explicit scope/non-scope boundaries, subsystem policies,
  coding conventions, and an explicit "Safety Rules for Claude Code"
  section (invariants an agent must never violate — e.g. dependency
  direction, what must never be committed, what always needs human
  sign-off).
- **`plan.md`** — the long-horizon roadmap. In `free-direct` this is a
  literal numbered **Phase 0…18** plan with a short "how to read this
  plan" preamble at the top. In `sharp-runtime`, planning has moved a
  step further: it's now **database-driven** — a git-ignored
  `plan.sqlite3` with `task`/`ticket` tables is the actual live plan, and
  `plan.md`/`plan_namespaces.md` are historical/pointer documents with
  ready-made SQL status queries. For `free-eggbert`'s scale (a big but
  finite, well-enumerable function/file list — see Appendix B), a plain
  `plan.md` is almost certainly sufficient to start; graduate to a
  database only if the per-function tracking in
  `docs/decomp-status/` (Section 9) grows unwieldy as flat files.
- **`NEXT.md`** — the living handoff/checkpoint document, with a
  consistent skeleton across repos: **(1) project summary, (2) current
  status — build/tests/tools, (3) recent changes, (4) current blocker,
  (5) known bugs, (6) architecture notes, (7) useful commands, (8) next
  smallest tasks, (9) do-not-do-yet, (10) resume prompt.** The top of the
  file always carries a dated "RESUME HERE" section. **This file, not
  conversation history, is the actual cross-session/cross-year memory
  mechanism** — a brand-new Claude Code session with zero prior context
  reads `CLAUDE.md` + `NEXT.md` and is productive within minutes.
- **`prompt.md`** (seen in `sharp-runtime`) — instructions for running a
  *fully autonomous* loop: at the start of every fresh context, read
  `CLAUDE.md` + `NEXT.md`, pull the next queued item, do it (classify /
  implement / test / commit), update the tracking record, and don't stop
  to ask permission except for genuinely ambiguous architecture
  decisions or destructive/irreversible actions. This is the concrete
  mechanism that lets a 1000-h/year, multi-year effort actually run
  as a long series of mostly-unattended agent sessions rather than
  requiring constant hands-on driving — directly applicable to Horizon 1
  in Section 6 once the ledger (Section 9) exists to make it safe.

Adopting this exact four-file convention in `free-eggbert` (it currently
has none of them — only `README.md`, `TODO.md`, and a deleted-but-
uncommitted `DOC.md`) is one of the highest-leverage, lowest-risk actions
available, precisely because it is already proven elsewhere in this
user's own workflow rather than a novel process to validate from
scratch.

---

## Appendix A — Ghidra Project Binary Inventory

Recovered directly from `Speedy Blupi 2.rep/idata/~index.dat` on
2026-07-08. "Item" is the internal Ghidra catalog ID; useful if
cross-referencing against the project's own `.prp` metadata files later.

| Item | Name | Type |
|---|---|---|
| `00000000` | `SpeedyBlupi2_v2.2_EN.exe` | Program (current main decompilation target) |
| `00000010` | `SpeedyBlupiDemo_v0.8_EN.exe` | Program |
| `00000011` | `SpeedyBlupi_v1.0_EN.exe` | Program |
| `00000012` | `SpeedyEggbert_v1.1_EN.exe` | Program |
| `00000013` | `SpeedyEggbertDemo_v1.0_EN.exe` | Program |
| `00000014` | `SpeedyEggbert_v1.0_EN.exe` | Program |
| `00000015` | `SpeedyEggbertSEValueware_v1.0_EN.exe` | Program |
| `00000016` | `SpeedyEggbert2_v1.0_EN.exe` | Program |
| `00000017` | `ReaktivniyEgbert_v1.1_RU.exe` | Program (Russian localization) |
| `00000018` | `PlanetBlupiDemo_v1.8_EN.exe` | Program |
| `00000019` | `PlanetBlupiDemo_v1.6_EN.exe` | Program |
| `0000001a` | `PlanetBlupi_v1.7_EN.exe` | Program |
| `00000035` | `BLUPIH.EXE` | Program (identity to confirm — likely "Home" variant) |
| `00000036` | `BLUPIF.EXE` | Program (identity to confirm — likely French) |
| `0000001d` | `WindowsPhoneSpeedyBlupi.dll` | .NET assembly (source of many symbol names via ILSpy) |
| `0000001e` | `WindowsPhoneSpeedyBlupi.resources.dll` | .NET resources |
| `00000008`–`0000000f` | `ddraw.dll`, `dplay.dll`, `dplayx.dll`, `kernel32.dll`, `dsound.dll`, `gdi32.dll`, `user32.dll`, `winmm.dll` | System DLLs (import resolution only) |
| `00000026` | `WORLD199.BLP` | Imported level/data file (reason for standalone import unconfirmed) |
| `00000027` | `Telefou.xch` | Imported file, purpose unconfirmed |
| `0000001b` | `SB2 v2.2 to SE2 v1.0` | Version Tracking session |
| `0000001c` | `SB2 v2.2 to PB v1.7` | Version Tracking session |
| `00000021` | `PB v1.7 to PB v1.6 demo` | Version Tracking session |
| `00000022` | `PB v1.7 to SB2 v2.2` | Version Tracking session |
| `00000023` | `SB2 v2.2 to SB v1.0` | Version Tracking session |
| `00000024` | `SB v1.0 to SE v1.0` | Version Tracking session |
| `00000025` | `SB v1.0 to RE v1.1` | Version Tracking session |
| `00000037` | `SB2 v2.2 to FwB` | Version Tracking session (target "FwB" unconfirmed — possibly another Blupi-franchise title) |
| `00000038` | `SB2 v2.2 to BaH` | Version Tracking session (likely vs. "Blupi at Home", `other/blupi_at_home/blupi-home-demo.exe`) |
| "New Traces" folder, IDs `00000004`–`00000034` (partial) | Debugger trace captures dated 2023-03 and 2024-07, including traces of `dplaysvr.exe` and `VirtualMIDISynth-synth.exe` and several raw memory images (`image00000000_00400000`) | Dynamic analysis sessions |

**Not yet done, recommended before relying on this table further:** open
each `Program` entry in Ghidra and record its analysis completeness
(has auto-analysis run? are functions named? is there a matching
`ProgramUserData` entry with meaningful annotations, or is it an empty
import?) in `docs/variant-matrix.md` per Section 9.

---

## Appendix B — Source File Status Table

Seeded from the README's own classification (not independently
re-verified line-by-line in this pass — that is exactly the job of
`docs/decomp-status/` going forward, see Section 9).

| File | Lines | README status | Notes |
|---|---:|---|---|
| `src/event.cpp` | 5,878 | Needs most work (#1 priority) | Largest file; input handling + `WM_PHASE_*` dispatch; 6 of the repo's 11 inline TODOs live here |
| `src/decblupi.cpp` | 4,395 | Needs most work (#2) | Player physics, all vehicle logic |
| `src/pixmap.cpp` | 1,922 | Tentatively complete | DirectDraw surface management |
| `src/decor.cpp` | 2,044 | Needs most work (#8) | Core world init/render/step loop |
| `src/decmove.cpp` | 2,311 | Needs most work (#6) | Enemy/moving-object AI |
| `src/blupi.cpp` | 953 | Tentatively complete | `WinMain`, window/message loop |
| `src/sound.cpp` | 779 | Tentatively complete | DirectSound backend |
| `src/soundbass.cpp` | 720 | (BASS alt. backend, not separately classified in README) | Enabled via `_BASS` flag |
| `src/decblock.cpp` | 804 | Needs most work (#5) | Crate/door/block interaction; has 2 explicit TODOs |
| `include/dectables.hpp` / `src/dectables.cpp` | 238 / 691 | Data tables (not separately classified) | Static lookup tables |
| `src/decdesign.cpp` | 662 | Needs most work (#4) | Level editor logic; has 1 explicit TODO |
| `src/button.cpp` | 393 | Tentatively complete | UI buttons |
| `src/decio.cpp` | 391 | Needs most work (#3) | Save/load `.blp` serialization |
| `src/misc.cpp` | 358 | Needs most work (#9) | Misc helpers |
| `src/network.cpp` | 356 | Tentatively complete | DirectPlay session/lobby |
| `src/ddutil.cpp` | 344 | (Utility, not separately classified) | DirectDraw helpers |
| `src/text.cpp` | 334 | (Not separately classified) | Text rendering |
| `src/movie.cpp` | 301 | Tentatively complete | AVI/cutscene playback |
| `src/web_persistence.cpp` | 317 | (New, Emscripten-only) | IndexedDB persistence, no Windows-original equivalent to verify against |
| `src/wave.cpp` | 272 | Tentatively complete | WAV loading |
| `src/decnet.cpp` | 467 | Needs most work (#7) | Network game sync; has 1 explicit TODO |
| `src/menu.cpp` | 160 | Tentatively complete | Menu rendering |
| `src/jauge.cpp` | 147 | Tentatively complete | Gauge/progress bars |

*(Header-only files and very small utility files omitted from this table
for brevity; `docs/decomp-status/` should still track them individually
once created.)*
