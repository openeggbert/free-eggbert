# Free Eggbert — Technical Documentation

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Relationship to Planet Blupi](#2-relationship-to-planet-blupi)
3. [Repository Structure](#3-repository-structure)
4. [Build System](#4-build-system)
5. [Compile-Time Flags](#5-compile-time-flags)
6. [Game Constants and Dimensions](#6-game-constants-and-dimensions)
7. [Image Channels](#7-image-channels)
8. [Action Codes (ACTION_*)](#8-action-codes-action_)
9. [Object Types (TYPE_*)](#9-object-types-type_)
10. [Sound Effects (SOUND_*)](#10-sound-effects-sound_)
11. [Security Power-Ups (SEC_*)](#11-security-power-ups-sec_)
12. [Key Input Flags](#12-key-input-flags)
13. [Game Phase System (WM_PHASE_*)](#13-game-phase-system-wm_phase_)
14. [World and Data Structures](#14-world-and-data-structures)
15. [CDecor Class](#15-cdecor-class)
16. [Audio System](#16-audio-system)
17. [Network / Multiplayer](#17-network--multiplayer)
18. [Game Asset Files](#18-game-asset-files)
    - [DATA directory](#181-data-directory)
    - [IMAGE08 directory](#182-image08-directory)
    - [IMAGE16 directory](#183-image16-directory)
    - [SOUND directory](#184-sound-directory)
19. [Source File Overview](#19-source-file-overview)
20. [Decompilation Notes](#20-decompilation-notes)
21. [Web / Emscripten Build](#21-web--emscripten-build)
22. [Android](#22-android)
23. [Cheat Codes](#23-cheat-codes)
24. [Known Issues and TODO](#24-known-issues-and-todo)

---

## 1. Project Overview

**Free Eggbert** is an open-source reconstruction of **Speedy Eggbert 2** (also known as **Speedy Blupi II**), a side-scrolling platform game originally developed for Windows using DirectX 3.

The project is based on decompiled and reverse-engineered source code. The original developers never released the source. This project uses:

- **Ghidra** — binary analysis and decompilation
- **IDA** — binary disassembly and analysis
- **ILSpy** — inspection of the Windows Phone 2013 port of Speedy Blupi (which preserved demangled symbol names)

Current status: gameplay is partially functional but the game is still defective and many features are incomplete or inaccurate. It is a work-in-progress reverse-engineering and preservation project.

**License:** GPLv3

**Main executable:** `SPEEDY_BLUPI_WINDOWS`

**Primary language:** C++20

**Original platform:** Win32, DirectX 3

**Cross-platform backend:** Free Direct (DirectDraw → SDL3), Free API (Win32 → cross-platform)

---

## 2. Relationship to Planet Blupi

Free Eggbert and Planet Blupi share common ancestry — the original Speedy Eggbert 2 / Speedy Blupi II was a fork/sequel of the Planet Blupi game engine. However, they are **separate projects** and must not be confused:

| Aspect | Planet Blupi | Free Eggbert |
|---|---|---|
| Game genre | Isometric RTS/puzzle | Side-scrolling platformer |
| World grid | 200×200 cells (2D isometric) | 100×100 cells (side-scroll) |
| Cell size | 60×30 px isometric | flat grid, objects 64×64 px |
| Player characters | Up to 100 Blupi units (`Blupi[100]`) | Single player (`m_blupi*` fields) |
| Multiplayer | 6 players (MAXBLUPI=100 slots) | Up to 4 network players (MAXNETPLAYER=4) |
| Fog of war | Yes (15-pattern quadrant fog) | No fog system |
| Audio | DirectSound / MCI | DirectSound or BASS library |
| Source origin | Official open-source release | Decompiled via Ghidra/IDA/ILSpy |
| Header extension | `.h` | `.hpp` |
| Repository | `planetblupi/` | `free-eggbert/` |

The shared codebase heritage means many class and function names, data formats, and constants are similar or identical — but the games and their source trees are maintained independently.

---

## 3. Repository Structure

```
free-eggbert/
├── include/            # All header files (.hpp)
│   ├── def.hpp         # Global constants, enums, compile flags
│   ├── decor.hpp       # World/game-state structures and CDecor class
│   ├── pixmap.hpp      # CPixmap — image surface manager
│   ├── sound.hpp       # CSound — audio playback
│   ├── event.hpp       # CEvent — input and game loop
│   ├── button.hpp      # CButton — UI buttons
│   ├── jauge.hpp       # CJauge — progress/gauge bars
│   ├── menu.hpp        # CMenu — menu system
│   ├── text.hpp        # Text rendering
│   ├── wave.hpp        # WAV file loader
│   ├── ddutil.hpp      # DirectDraw utility functions
│   ├── misc.hpp        # Miscellaneous helpers
│   ├── movie.hpp       # Video playback (AVI/movies)
│   ├── network.hpp     # CNetwork — DirectPlay networking
│   ├── obstacle.hpp    # Obstacle/collision data
│   ├── dectables.hpp   # Decor table declarations
│   ├── pixtables.hpp   # Pixmap table declarations
│   ├── texttables.hpp  # Text table declarations
│   └── web_persistence.hpp  # Emscripten IndexedDB persistence
├── src/                # All C++ source files (.cpp)
│   ├── blupi.cpp       # WinMain, main game loop, window management
│   ├── decor.cpp       # CDecor core: world init, rendering, step logic
│   ├── decblupi.cpp    # CDecor: Blupi character movement and logic
│   ├── decblock.cpp    # CDecor: block/case interaction logic
│   ├── decmove.cpp     # CDecor: moving object logic
│   ├── decnet.cpp      # CDecor: network game synchronization
│   ├── decdesign.cpp   # CDecor: level editor / design mode
│   ├── decio.cpp       # CDecor: save/load game state (I/O)
│   ├── dectables.cpp   # Static lookup tables for decor
│   ├── event.cpp       # CEvent: input handling and phase dispatch
│   ├── pixmap.cpp      # CPixmap: DirectDraw surface management
│   ├── pixtables.cpp   # Static lookup tables for pixmap
│   ├── sound.cpp       # CSound: DirectSound audio
│   ├── soundbass.cpp   # CSound: BASS library audio (alternative)
│   ├── wave.cpp        # WAV file loading
│   ├── network.cpp     # CNetwork: DirectPlay multiplayer
│   ├── button.cpp      # CButton: UI button rendering
│   ├── jauge.cpp       # CJauge: gauge bar rendering
│   ├── menu.cpp        # CMenu: menu rendering
│   ├── text.cpp        # Text rendering
│   ├── texttables.cpp  # Static text tables
│   ├── movie.cpp       # AVI video playback
│   ├── misc.cpp        # Miscellaneous helpers
│   ├── obstacle.cpp    # Obstacle/collision data
│   ├── ddutil.cpp      # DirectDraw utilities
│   └── web_persistence.cpp  # Emscripten IDBFS persistence
├── gamefiles/          # Game assets (not in git — must be supplied by user)
│   ├── DATA/           # Level worlds, demos, config (~110 .blp files)
│   ├── IMAGE08/        # 8-bit palette images (~68 .blp files)
│   ├── IMAGE16/        # 16-bit true-color images (~55 .blp files)
│   └── SOUND/          # Music and sound effect files (~103 files)
├── dxsdk3/             # DirectX 3 SDK (git submodule)
├── third_party/        # Vendored SDL (git submodules)
│   ├── SDL/
│   ├── SDL_image/
│   └── SDL_mixer/
├── bass/               # BASS audio library
├── cmake/              # CMake helper modules
├── android/            # Android build support
├── msvc5/              # Legacy MSVC 5 project files
├── resource/           # Windows resource files
├── util/               # Utility scripts
├── CMakeLists.txt      # Main CMake build file
├── README.md           # Quick-start and build guide
├── TODO.md             # Development checklist
├── LICENSE             # GPLv3
└── ANDROID.md          # Android build notes
```

---

## 4. Build System

### CMake (cross-platform, recommended)

The canonical cross-platform build uses the **Free Direct** backend (SDL3-based replacement for DirectDraw/DirectSound).

**Clone and initialize submodules:**
```bash
git submodule update --init --recursive
```

**Configure and build:**
```bash
cmake -S . -B build -DSPEEDY_BLUPI_BACKEND=FREEDIRECT
cmake --build build -j
```

**Clean rebuild:**
```bash
rm -rf build
git submodule update --init --recursive
cmake -S . -B build -DSPEEDY_BLUPI_BACKEND=FREEDIRECT
cmake --build build -j
```

**System SDL override (advanced):**
```bash
cmake -S . -B build -DSPEEDY_BLUPI_BACKEND=FREEDIRECT -DFREE_USE_SYSTEM_SDL=ON
```

**Dependency boundaries:**
- `SPEEDY_BLUPI_WINDOWS` links only `free-api` and `free-direct`
- SDL include directories and libraries stay private to `free-api` / `free-direct`
- No game source file should include SDL directly

### Visual Studio 2022 (Windows, native DirectX)

1. Open `Speedy Eggbert 2 Source.sln` in Visual Studio 2022
2. Set platform to `x86`, debugger to `Win32`
3. In Project Properties → C/C++ → Command Line → Additional Options, add: `/wd4700 /wd4703`
4. Set Platform Toolset to `Visual Studio 2022 (v143)`
5. Build solution

The `/wd4700 /wd4703` flags suppress warnings about potentially uninitialized local variables — these are widespread in the decompiled code.

### CMake compiler options (non-MSVC)

```cmake
-fpermissive
-fms-extensions
-fPIC
-w
-Wno-narrowing
-Wno-int-to-pointer-cast
-g
-O0
```

These permissive options are necessary because decompiled code often uses constructs that modern C++ compilers reject by default.

---

## 5. Compile-Time Flags

All flags are defined in `include/def.hpp`:

| Flag | Default | Description |
|---|---|---|
| `_DEMO` | `FALSE` | Enable demo mode (limited content) |
| `_EGAMES` | `FALSE` | Enable eGames retail version behavior |
| `_SE` | `FALSE` | eGames Special Edition variant |
| `_INTRO` | `FALSE` | Show intro images at startup |
| `_CD` | `FALSE` | Require the game CD to be present |
| `_BYE` | `!_EGAMES` | Show exit/goodbye image (like `_INTRO`) |
| `_LEGACY` | `FALSE` | Keep broken/quirky legacy code (compatibility mode) |
| `_DREAM` | `TRUE` | Allow Dream blocks when loading levels |
| `_BASS` | `FALSE` | Use BASS audio library instead of DirectMusic |

`_DREAM=TRUE` is the current default to support Dream-mode levels. Setting `_LEGACY=TRUE` restores original bugs and quirks from the binary.

---

## 6. Game Constants and Dimensions

Defined in `include/def.hpp`:

### Window and rendering

| Constant | Value | Description |
|---|---|---|
| `LXIMAGE` | 640 | Game window width (pixels) |
| `LYIMAGE` | 480 | Game window height (pixels) |
| `POSDRAWX` | 0 | Draw origin X |
| `POSDRAWY` | 0 | Draw origin Y |

### World grid

| Constant | Value | Description |
|---|---|---|
| `MAXCELX` | 100 | Max world cells horizontally |
| `MAXCELY` | 100 | Max world cells vertically |

### Sprite dimensions

| Constant | Value | Description |
|---|---|---|
| `DIMOBJX` | 64 | Object sprite width (px) |
| `DIMOBJY` | 64 | Object sprite height (px) |
| `DIMBLUPIX` | 60 | Blupi character sprite width (px) |
| `DIMBLUPIY` | 60 | Blupi character sprite height (px) |
| `DIMEXPLOX` | 128 | Explosion sprite width (px) |
| `DIMEXPLOY` | 128 | Explosion sprite height (px) |
| `DIMBUTTONX` | 40 | UI button width (px) |
| `DIMBUTTONY` | 40 | UI button height (px) |
| `DIMJAUGEX` | 124 | Gauge bar width (px) |
| `DIMJAUGEY` | 22 | Gauge bar height (px) |
| `DIMTEXTX` | 16 | Max character sprite width (px) |
| `DIMTEXTY` | 16 | Max character sprite height (px) |
| `DIMLITTLEX` | 16 | Small character sprite width (px) |
| `DIMLITTLEY` | 12 | Small character sprite height (px) |

### Multiplayer and game limits

| Constant | Value | Description |
|---|---|---|
| `MAXGAMER` | 8 | Maximum total gamers |
| `MAXNETPLAYER` | 4 | Maximum network players |
| `MAXTEAM` | 4 | Maximum teams |
| `MAXNETMESSAGE` | 20 | Max pending network messages |
| `MAXSESSION` | 100 | Max DirectPlay sessions |
| `MAXMOVEOBJECT` | 200 | Max simultaneously active moving objects |
| `MAXNOTIF` | 5 | Max simultaneous notifications |
| `MAXFIFOPOS` | 10 | FIFO position queue size |
| `MAXCHAT` | 6 | Max chat messages visible |
| `MAXSAVE` | 6 | Max save game slots |
| `MAXDEMO` | 9999 | Max demo frames |
| `MAXINDEX` | 20 | Max index entries |
| `MAXMESSAGEPERPACKET` | 5 | Max messages per network packet |
| `MAXBUTTON` | 40 | Max UI buttons |

### Scrolling

| Constant | Value | Description |
|---|---|---|
| `SCROLL_SPEED` | 8 | Camera scroll speed (px/tick) |
| `SCROLL_MARGX` | 80 | Horizontal scroll trigger margin (px) |
| `SCROLL_MARGY` | 40 | Vertical scroll trigger margin (px) |

### Character offsets

| Constant | Value | Description |
|---|---|---|
| `BLUPIFLOOR` | 2 | Floor contact offset (px) |
| `BLUPIOFFY` | 6 | Y offset with floor |
| `BLUPISURF` | 12 | Water surface offset (px) |
| `BLUPISUSPEND` | 12 | Bar-hanging Y offset (px) |
| `OVERHEIGHT` | 80 | Hovercraft flight height (px) |
| `MAXQUART` | 441 | Quarter cell count (21×21 grid) |

---

## 7. Image Channels

The `CPixmap` class manages multiple DirectDraw surfaces simultaneously, each identified by a channel index. Channels are defined as an enum in `include/def.hpp`:

| Enum | Value | Used for |
|---|---|---|
| `CHBACK` | 0 | Background / level terrain |
| `CHOBJECT` | 1 | Foreground objects and pickups |
| `CHBLUPI` | 2 | Player character sprites (default) |
| `CHDECOR` | 3 | Decorative overlays |
| `CHBUTTON` | 4 | UI buttons |
| `CHJAUGE` | 5 | Gauge/progress bar sprites |
| `CHTEXT` | 6 | Normal-size text characters |
| `CHLITTLE` | 7 | Small text characters |
| `CHMAP` | 8 | Minimap sprites |
| `CHEXPLO` | 9 | Explosion sprites |
| `CHELEMENT` | 10 | Element/particle effects |
| `CHBLUPI1` | 11 | Player 1 character variant |
| `CHBLUPI2` | 12 | Player 2 character variant |
| `CHBLUPI3` | 13 | Player 3 character variant |
| `CHTEMP` | 14 | Temporary surface (scratch space) |
| `CHMAX` | 14 | Alias for CHTEMP (total = 15 channels) |

In the `IMAGE08` (8-bit) and `IMAGE16` (16-bit) asset directories, each image file corresponds to a specific channel. See [Section 18](#18-game-asset-files) for the file-to-channel mapping.

---

## 8. Action Codes (ACTION_*)

Character animation states are selected by `ACTION_*` constants defined in `include/def.hpp`. There are 87 action codes (1–87):

### Movement
| Code | Value | Description |
|---|---|---|
| `ACTION_STOP` | 1 | Standing still |
| `ACTION_MARCH` | 2 | Walking |
| `ACTION_TURN` | 3 | Turning around |
| `ACTION_JUMP` | 4 | Beginning to jump |
| `ACTION_AIR` | 5 | In the air (mid-jump) |
| `ACTION_DOWN` | 6 | Looking down |
| `ACTION_UP` | 7 | Looking up |
| `ACTION_VERTIGO` | 8 | "Whoops" — edge vertigo |
| `ACTION_RECEDE` | 9 | Backing away from edge |
| `ACTION_ADVANCE` | 10 | Walking forward away from edge |
| `ACTION_STOPMARCH` | 60 | Stopped from walking |
| `ACTION_STOPJUMP` | 61 | Stopped from jumping |
| `ACTION_STOPJUMPh` | 62 | Stopped from jump (variant) |
| `ACTION_TELEPORTE` | 74 | Teleporting |

### Helicopter
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPHELICO` | 15 | Hovering (helicopter stopped) |
| `ACTION_MARCHHELICO` | 16 | Flying (helicopter moving) |
| `ACTION_TURNHELICO` | 17 | Turning in helicopter |
| `ACTION_HELICOGLU` | 58 | Glued while in helicopter |
| `ACTION_TURNAIR` | 59 | Turning in mid-air |

### Swimming
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPNAGE` | 18 | Stopped in deep water |
| `ACTION_MARCHNAGE` | 19 | Swimming in deep water |
| `ACTION_TURNNAGE` | 20 | Turning in deep water |
| `ACTION_STOPSURF` | 21 | Stopped at water surface |
| `ACTION_MARCHSURF` | 22 | Swimming at surface |
| `ACTION_TURNSURF` | 23 | Turning at surface |
| `ACTION_DROWN` | 24 | Drowning |

### Jeep
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPJEEP` | 25 | Jeep stopped |
| `ACTION_MARCHJEEP` | 26 | Jeep moving |
| `ACTION_TURNJEEP` | 27 | Jeep turning |

### Crate pushing/pulling
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPPOP` | 28 | Stopped while pulling crate |
| `ACTION_POP` | 29 | Pulling crate |
| `ACTION_PUSH` | 14 | Pushing a crate |
| `ACTION_OUF3` | 47 | Stop moving crate ("gahh!") |

### Bar hanging
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPSUSPEND` | 31 | Stopped on bars |
| `ACTION_MARCHSUSPEND` | 32 | Moving on bars |
| `ACTION_TURNSUSPEND` | 33 | Turning on bars |
| `ACTION_JUMPSUSPEND` | 34 | Pulling up from bars |

### Skateboard
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPSKATE` | 37 | Skateboard stopped |
| `ACTION_MARCHSKATE` | 38 | Skateboarding |
| `ACTION_TURNSKATE` | 39 | Turning on skateboard |
| `ACTION_JUMPSKATE` | 40 | Jumping on skateboard |
| `ACTION_AIRSKATE` | 41 | In air on skateboard |
| `ACTION_TAKESKATE` | 42 | Picking up skateboard |
| `ACTION_DEPOSESKATE` | 43 | Dropping skateboard |
| `ACTION_SLOWDOWNSKATE` | 85 | Slowing down on skateboard |

### Tank
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPTANK` | 50 | Tank stopped |
| `ACTION_MARCHTANK` | 51 | Tank moving |
| `ACTION_TURNTANK` | 52 | Tank turning |
| `ACTION_FIRETANK` | 53 | Tank firing |

### Hovercraft
| Code | Value | Description |
|---|---|---|
| `ACTION_STOPOVER` | 67 | Hovercraft stopped |
| `ACTION_MARCHOVER` | 68 | Hovercraft moving |
| `ACTION_TURNOVER` | 69 | Hovercraft turning |

### Power-up actions
| Code | Value | Description |
|---|---|---|
| `ACTION_SUCETTE` | 49 | Eating lollipop (power boost) |
| `ACTION_GLU` | 54 | Glued (trapped by glue ball) |
| `ACTION_DRINK` | 55 | Drinking |
| `ACTION_CHARGE` | 56 | Charging up |
| `ACTION_ELECTRO` | 57 | Electrocuted |
| `ACTION_BALLOON` | 66 | Stung by wasp (swells up) |
| `ACTION_HIDE` | 35 | Invisible (cloud power-up) |
| `ACTION_ECRASE` | 73 (MARCHÉCRASÉ) | Flattened/squished |
| `ACTION_STOPECRASE` | 72 | Stopped while flattened |

### Reactions and expressions
| Code | Value | Description |
|---|---|---|
| `ACTION_WIN` | 13 | Victory animation |
| `ACTION_BYE` | 30 | "au au" farewell |
| `ACTION_JUMPAIE` | 36 | Headache ("aie!") |
| `ACTION_OUF1a` | 44 | Relief expression 1a |
| `ACTION_OUF1b` | 45 | Relief expression 1b |
| `ACTION_OUF2` | 46 | Relief expression 2 |
| `ACTION_OUF4` | 48 | Relief expression 4 |
| `ACTION_OUF5` | 65 | Relief expression 5 |
| `ACTION_MOCKERY` | 63 | Mockery animation |
| `ACTION_MOCKERYi` | 64 | Mockery variant i |
| `ACTION_MOCKERYp` | 83 | Mockery variant p |
| `ACTION_NON` | 84 | Can't place personal bomb |

### Dynamite
| Code | Value | Description |
|---|---|---|
| `ACTION_TAKEDYNAMITE` | 86 | Picking up dynamite |
| `ACTION_PUTDYNAMITE` | 87 | Placing dynamite |

### Utility / unused
| Code | Value | Description |
|---|---|---|
| `ACTION_CLEAR1` | 11 | Clear animation 1 |
| `ACTION_SET` | 12 | Set state |
| `ACTION_CLEAR2`–`ACTION_CLEAR8` | 75–81 | Unused clear animations |
| `ACTION_SWITCH` | 82 | Switch action |
| `ACTION_RECEDEq` | 70 | Recede quick |
| `ACTION_ADVANCEq` | 71 | Advance quick |

---

## 9. Object Types (TYPE_*)

Moving game objects (enemies, items, effects, vehicles) are identified by `TYPE_*` constants (1–203). These are tracked in `MoveObject` structs and managed by `CDecor`.

### Lifts and elevators
| Type | Value | Description |
|---|---|---|
| `TYPE_ASCENSEUR` | 1 | Standard lift |
| `TYPE_ASCENSEURs` | 47 | Conveyor belt moving right |
| `TYPE_ASCENSEURsi` | 48 | Conveyor belt moving left |

### Enemies
| Type | Value | Description |
|---|---|---|
| `TYPE_POISSON` | 17 | Fish (aquatic enemy) |
| `TYPE_OISEAU` | 20 | Bird (aerial enemy) |
| `TYPE_GUEPE` | 44 | Wasp |
| `TYPE_TENTACULE` | 53 | Slime tentacle effect |
| `TYPE_CREATURE` | 54 | Moving slime creature |

### Bombs and explosions
| Type | Value | Description |
|---|---|---|
| `TYPE_BOMBEDOWN` | 2 | Floor bomb |
| `TYPE_BOMBEUP` | 3 | Hanging bomb |
| `TYPE_BOMBEMOVE` | 16 | Moving bomb |
| `TYPE_BOMBEFOLLOW1` | 96 | Homing bomb (stopped) |
| `TYPE_BOMBEFOLLOW2` | 97 | Homing bomb (moving) |
| `TYPE_DYNAMITE` | 55 | Dynamite stick |
| `TYPE_DYNAMITEf` | 56 | Lit dynamite |
| `TYPE_BOMBEPERSO1` | 200 | Player 1 personal bomb (yellow) |
| `TYPE_BOMBEPERSO2` | 201 | Player 2 personal bomb (orange) |
| `TYPE_BOMBEPERSO3` | 202 | Player 3 personal bomb (blue) |
| `TYPE_BOMBEPERSO4` | 203 | Player 4 personal bomb (green) |
| `TYPE_EXPLO1`–`TYPE_EXPLO4` | 8–11 | Explosion effects 1–4 |
| `TYPE_EXPLO5`–`TYPE_EXPLO10` | 90–95 | Explosion effects 5–10 |
| `TYPE_SPLOUTCH1`–`TYPE_SPLOUTCH3` | 98–100 | Splat particle effects |

### Collectibles and keys
| Type | Value | Description |
|---|---|---|
| `TYPE_TRESOR` | 5 | Treasure (collectible) |
| `TYPE_EGG` | 6 | Egg collectible |
| `TYPE_CLE` | 21 | Gold key |
| `TYPE_CLE1` | 49 | Red key |
| `TYPE_CLE2` | 50 | Green key |
| `TYPE_CLE3` | 51 | Blue key |

### Doors and goals
| Type | Value | Description |
|---|---|---|
| `TYPE_DOOR` | 22 | Door |
| `TYPE_GOAL` | 7 | Goal/exit marker |
| `TYPE_BRIDGE` | 52 | Fragile bridge |

### Vehicles
| Type | Value | Description |
|---|---|---|
| `TYPE_HELICO` | 13 | Helicopter |
| `TYPE_BLUPIHELICO` | 32 | Blupi inside helicopter |
| `TYPE_JEEP` | 19 | Jeep |
| `TYPE_TANK` | 28 | Tank |
| `TYPE_BLUPITANK` | 33 | Blupi inside tank |
| `TYPE_OVER` | 46 | Hovercraft |
| `TYPE_BULLDOZER` | 4 | Bulldozer |

### Interactive items and pick-ups
| Type | Value | Description |
|---|---|---|
| `TYPE_CAISSE` | 12 | Crate (pushable/pullable box) |
| `TYPE_SKATE` | 24 | Skateboard |
| `TYPE_DRINK` | 30 | Drinkable item |
| `TYPE_CHARGE` | 31 | Charging device |

### Power-up effects
| Type | Value | Description |
|---|---|---|
| `TYPE_SHIELD` | 25 | Shield power-up |
| `TYPE_POWER` | 26 | Lollipop power-up |
| `TYPE_MAGICTRACK` | 27 | Sparkle trail |
| `TYPE_TRESORTRACK` | 39 | Treasure sparkle trail |
| `TYPE_SHIELDTRACK` | 57 | Shield particle trail |
| `TYPE_HIDETRACK` | 58 | Invisibility particle trail |

### Water effects
| Type | Value | Description |
|---|---|---|
| `TYPE_PLOUF` | 14 | Big water splash |
| `TYPE_TIPLOUF` | 35 | Small water splash |
| `TYPE_BLUP` | 15 | Underwater bubble |
| `TYPE_GLU` | 34 | Glue blob |

### Hazards
| Type | Value | Description |
|---|---|---|
| `TYPE_BALLE` | 23 | Glue ball projectile |
| `TYPE_BULLET` | 29 | Tank bullet |
| `TYPE_POLLUTION` | 36 | Pollution/toxic zone |
| `TYPE_ELECTRO` | 38 | Shocked Blupi effect |
| `TYPE_CLEAR` | 37 | Burned Blupi effect |

### Special effects
| Type | Value | Description |
|---|---|---|
| `TYPE_INVERT` | 40 | Controls inversion (active) |
| `TYPE_INVERTSTART` | 41 | Controls inversion start |
| `TYPE_INVERTSTOP` | 42 | Controls inversion end |
| `TYPE_INVERTSPIN` | 43 | Controls inversion spin |
| `TYPE_TOMATES` | 18 | Tomatoes (unused) |

---

## 10. Sound Effects (SOUND_*)

93 sound effects (indices 0–92) plus a special movie sound marker (99). Defined in `include/def.hpp`.

### UI and menu
| Code | Value | Description |
|---|---|---|
| `SOUND_CLICK` | 0 | Button click |
| `SOUND_ERROR` | 27 | Error / invalid action |

### Movement
| Code | Value | Description |
|---|---|---|
| `SOUND_JUMP0` | 22 | Low jump |
| `SOUND_JUMP1` | 1 | Medium jump |
| `SOUND_JUMP2` | 2 | High jump |
| `SOUND_JUMPEND` | 3 | Land on stone |
| `SOUND_JUMPTOC` | 4 | Hit stone |
| `SOUND_TURN` | 5 | Turn around |
| `SOUND_FALL` | 8 | Falling |
| `SOUND_NEW` | 9 | Respawn |

### Footstep variants (surface type)
| Code | Value | Surface |
|---|---|---|
| `SOUND_JUMPENDb` / `SOUND_JUMPTOCb` | 78 / 79 | Wood |
| `SOUND_JUMPENDm` / `SOUND_JUMPTOCm` | 80 / 81 | Metal |
| `SOUND_JUMPENDg` / `SOUND_JUMPTOCg` | 82 / 83 | Cave |
| `SOUND_JUMPENDo` / `SOUND_JUMPTOCo` | 84 / 85 | Slime |
| `SOUND_JUMPENDk` / `SOUND_JUMPTOCk` | 86 / 87 | Plastic |
| `SOUND_JUMPENDf` / `SOUND_JUMPTOCf` | 88 / 89 | Cheese |
| `SOUND_JUMPENDh` / `SOUND_JUMPTOCh` | 90 / 91 | Grass |

### Character reactions
| Code | Value | Description |
|---|---|---|
| `SOUND_VERTIGO` | 6 | Vertigo "whoops" |
| `SOUND_DOWN` | 7 | Look down |
| `SOUND_BYE` | 32 | "au au" farewell |
| `SOUND_JUMPAIE` | 40 | Headache "aie!" |
| `SOUND_OUF1` | 46 | Exit water "phew" |
| `SOUND_OUF2` | 47 | Scared "huaaaahhhhh!" |
| `SOUND_OUF3` | 48 | Stop moving crate "gahh!" |
| `SOUND_OUF4` | 49 | Shriek / heartbeat |
| `SOUND_MOCKERY` | 65 | Mockery taunt |
| `SOUND_PATIENT` | 37 | Patience expression |

### Helicopter
| Code | Value | Description |
|---|---|---|
| `SOUND_HELICOSTART` | 15 | Helicopter starting |
| `SOUND_HELICOHIGH` | 16 | Helicopter in flight (high RPM) |
| `SOUND_HELICOSTOP` | 17 | Helicopter stopping |
| `SOUND_HELICOLOW` | 18 | Helicopter low speed |

### Jeep
| Code | Value | Description |
|---|---|---|
| `SOUND_JEEPSTART` | 28 | Jeep starting |
| `SOUND_JEEPHIGH` | 29 | Jeep at speed |
| `SOUND_JEEPSTOP` | 30 | Jeep stopping |
| `SOUND_JEEPLOW` | 31 | Jeep slow |

### Water
| Code | Value | Description |
|---|---|---|
| `SOUND_PLOUF` | 23 | Big water splash |
| `SOUND_BLUP` | 24 | Underwater bubble |
| `SOUND_SURF` | 25 | Inhale at surface |
| `SOUND_DROWN` | 26 | Drowning |
| `SOUND_TIPLOUF` | 64 | Small water splash |

### Crate and bar interactions
| Code | Value | Description |
|---|---|---|
| `SOUND_PUSH` | 38 | Pushing crate |
| `SOUND_POP` | 39 | Pulling crate |
| `SOUND_RESSORT` | 41 | Spring bounce |
| `SOUND_SUSPENDTOC` | 34 | Grab bar |
| `SOUND_SUSPENDJUMP` | 35 | Pull up from bar |
| `SOUND_SINGE` | 36 | Singed |

### Power-ups
| Code | Value | Description |
|---|---|---|
| `SOUND_STARTSHIELD` | 42 | Shield activates |
| `SOUND_STOPSHIELD` | 43 | Shield deactivates |
| `SOUND_STARTPOWER` | 44 | Lollipop activates |
| `SOUND_STOPPOWER` | 45 | Lollipop deactivates |
| `SOUND_SUCETTE` | 50 | Lollipop lick sound |
| `SOUND_STARTHIDE` | 62 | Cloud invisibility starts |
| `SOUND_STOPCLOUD` | 56 | Cloud ends |
| `SOUND_STARTCLOUD` | 55 | Cloud begins |
| `SOUND_DRINK` | 57 | Drinking |
| `SOUND_CHARGE` | 58 | Charging up |
| `SOUND_ELECTRO` | 59 | Electrocution |

### Combat and bombs
| Code | Value | Description |
|---|---|---|
| `SOUND_BOUM` | 10 | Explosion |
| `SOUND_FIREOK` | 52 | Tank fire successful |
| `SOUND_FIREKO` | 53 | Tank fire failed (ratchet) |
| `SOUND_GLU` | 51 | Glued sound |
| `SOUND_TAKEGLU` | 54 | Picking up glue |
| `SOUND_BLITZ` | 69 | Lightning buzz |
| `SOUND_ECRASE` | 70 | Squished |

### Collectibles and goals
| Code | Value | Description |
|---|---|---|
| `SOUND_TRESOR` | 11 | Treasure collected |
| `SOUND_EGG` | 12 | Egg collected |
| `SOUND_LASTTRESOR` | 19 | Last treasure collected |
| `SOUND_ENDKO` | 13 | Level lost |
| `SOUND_ENDOK` | 14 | Level won |
| `SOUND_ANGEL` | 74 | Angel sound "uie uie" |

### Doors, keys and level mechanics
| Code | Value | Description |
|---|---|---|
| `SOUND_DOOR` | 33 | Door creak |
| `SOUND_TELEPORTE` | 71 | Teleport |
| `SOUND_BRIDGE1` | 72 | Bridge falling |
| `SOUND_BRIDGE2` | 73 | Bridge reappearing |
| `SOUND_SCIE` | 75 | Saw cutting |
| `SOUND_SWITCHOFF` | 76 | Switch turned off |
| `SOUND_SWITCHON` | 77 | Switch turned on |

### Multiplayer personal bombs
| Code | Value | Description |
|---|---|---|
| `SOUND_PERSOTAKE` | 60 | Picking up personal bomb ("wow!") |
| `SOUND_PERSOPOSE` | 61 | Placing personal bomb (evil laugh) |

### Inversion
| Code | Value | Description |
|---|---|---|
| `SOUND_INVERTSTART` | 66 | Controls inversion begins |
| `SOUND_INVERTSTOP` | 67 | Controls inversion ends |

### Hovercraft and misc
| Code | Value | Description |
|---|---|---|
| `SOUND_OVERSTOP` | 68 | Hovercraft stall |
| `SOUND_UP` | 20 | Partially unused (L+R+Down press) |
| `SOUND_LOOKUP` | 21 | Looking up |
| `SOUND_FOLLOW` | 92 | Homing bomb "be-be-beep" |
| `SOUND_MOVIE` | 99 | Movie/cutscene marker (not a real sample) |

---

## 11. Security Power-Ups (SEC_*)

Active security/power-up states for the player character, defined as an enum in `include/def.hpp`:

| Enum | Value | Description |
|---|---|---|
| `SEC_SHIELD` | 1 | Shield active (invulnerability) |
| `SEC_POWER` | 2 | Power-up lollipop active |
| `SEC_CLOUD` | 3 | Cloud invisibility active |
| `SEC_HIDE` | 4 | Hidden state active |

---

## 12. Key Input Flags

Bitmask constants for key/input state in `include/def.hpp`:

| Flag | Value | Description |
|---|---|---|
| `KEY_NONE` | 0 | No key pressed |
| `KEY_LEFT` | 1 | Left arrow |
| `KEY_RIGHT` | 2 | Right arrow |
| `KEY_UP` | 4 | Up arrow |
| `KEY_DOWN` | 8 | Down arrow |
| `KEY_JUMP` | 16 | Jump button |
| `KEY_FIRE` | 32 | Fire/action button |

---

## 13. Game Phase System (WM_PHASE_*)

Game phases control which screen or mode is active. They are dispatched as Windows messages (`WM_USER + N`) and handled by `CEvent`. All phases are defined as an enum in `include/def.hpp`:

### Core gameplay
| Phase | Description |
|---|---|
| `WM_PHASE_INIT` | Initialization |
| `WM_PHASE_PLAY` | Normal gameplay |
| `WM_PHASE_PLAYTEST` | Test play (from editor) |
| `WM_PHASE_DOPLAY` | Start single-player game |
| `WM_PHASE_BUILD` | Level editor / design mode |
| `WM_PHASE_DEMO` | Demo playback |
| `WM_PHASE_STOP` | Escape key pause menu during gameplay |

### Win / lose
| Phase | Description |
|---|---|
| `WM_PHASE_WIN` | Level won |
| `WM_PHASE_LOST` | Level lost |
| `WM_PHASE_WINMOVIE` | Win cutscene |
| `WM_PHASE_LASTWIN` | Last win (Planet Blupi vestige) |
| `WM_PHASE_LOSTMULTI` | Multiplayer: lost |
| `WM_PHASE_WINMULTI` | Multiplayer: won |
| `WM_PHASE_WINMOVIEMULTI` | Multiplayer: win cutscene |
| `WM_PHASE_WINDESIGN` | Editor test: won |
| `WM_PHASE_LOSTDESIGN` | Editor test: lost |
| `WM_PHASE_WINMOVIEDESIGN` | Editor test: win cutscene |

### Menus and UI
| Phase | Description |
|---|---|
| `WM_PHASE_SETUP` | Settings screen |
| `WM_PHASE_SETUPp` | Settings (alternate) |
| `WM_PHASE_MUSIC` | Music selection |
| `WM_PHASE_REGION` | Region selection |
| `WM_PHASE_INSERT` | Insert (CD check) |
| `WM_PHASE_INFO` | Info/statistics screen |
| `WM_PHASE_HELP` | Help screen |
| `WM_PHASE_BYE` | Exit/goodbye |
| `WM_PHASE_INTRO1` | Intro animation 1 |
| `WM_PHASE_INTRO2` | Intro animation 2 |
| `WM_PHASE_PLAYMOVIE` | Play intro movie |
| `WM_PHASE_TESTCD` | CD presence test |
| `WM_PHASE_MANUEL` | Manual/instructions |
| `WM_PHASE_PRIVATE` | Private/cheat mode |

### Gamer profile management
| Phase | Description |
|---|---|
| `WM_PHASE_GAMER` | Gamer selection |
| `WM_PHASE_NAMEGAMER` | Name a gamer |
| `WM_PHASE_CLEARGAMER` | Clear a gamer profile |
| `WM_PHASE_DONAMEGAMER` | Execute gamer naming |
| `WM_PHASE_DOCLEARGAMER` | Execute gamer profile clear |

### Level editor I/O
| Phase | Description |
|---|---|
| `WM_PHASE_NAMEDESIGN` | Name a design level |
| `WM_PHASE_WRITEDESIGN` | Export design to `c:\user` |
| `WM_PHASE_READDESIGN` | Import design from `c:\user` |
| `WM_PHASE_CLEARDESIGN` | Clear design |
| `WM_PHASE_DONAMEDESIGN` | Execute design naming |
| `WM_PHASE_DOWRITEDESIGN` | Execute design write |
| `WM_PHASE_DOREADDESIGN` | Execute design read |
| `WM_PHASE_DOCLEARDESIGN` | Execute design clear |
| `WM_PHASE_QUITPLAYTEST` | Exit test play |
| `WM_PHASE_QUITPLAY` | Exit play |

### Save/Load
| Phase | Description |
|---|---|
| `WM_PHASE_GWRITE` | Save game |
| `WM_PHASE_GREADp` | Load game (prompt) |
| `WM_PHASE_GREAD` | Load game (execute) |

### Multiplayer (DirectPlay)
| Phase | Description |
|---|---|
| `WM_PHASE_SERVICE` | Select DirectPlay service |
| `WM_PHASE_DP_DOSERVICE` | Execute service selection |
| `WM_PHASE_DP_CANCELSERVICE` | Cancel service |
| `WM_PHASE_SESSION` | Session browser |
| `WM_PHASE_DP_JOIN` | Join session |
| `WM_PHASE_DP_CREATELOBBY` | Create lobby |
| `WM_PHASE_DP_REFRESH` | Refresh session list |
| `WM_PHASE_DP_CANCELSESSION` | Cancel session |
| `WM_PHASE_MULTI` | Multiplayer lobby |
| `WM_PHASE_DP_STARTMULTI` | Start multiplayer |
| `WM_PHASE_DP_CANCELMULTI` | Cancel multiplayer |
| `WM_PHASE_CREATE` | Create session |
| `WM_PHASE_DP_DOCREATE` | Execute session creation |
| `WM_PHASE_DP_CANCELCREATE` | Cancel session creation |

---

## 14. World and Data Structures

Defined in `include/decor.hpp`.

### `Cellule` struct
The world is stored as a 100×100 grid of `Cellule` values:

```cpp
typedef struct {
    short icon;   // sprite index into the background/decor sheet
} Cellule;
```

The `decor[100][100]` and `bigDecor[100][100]` arrays in `DescSave` use `short` directly (equivalent to the icon field).

### `MoveObject` struct
Tracks up to `MAXMOVEOBJECT` (200) simultaneously active moving game objects (enemies, bombs, vehicles, effects):

| Field | Type | Description |
|---|---|---|
| `type` | short | Object type (`TYPE_*` constant) |
| `stepAdvance` | short | Steps when advancing |
| `stepRecede` | short | Steps when receding |
| `timeStopStart` | short | Time stopped at start position |
| `timeStopEnd` | short | Time stopped at end position |
| `posStart` | POINT | Starting position |
| `posEnd` | POINT | Target/end position |
| `posCurrent` | POINT | Current position |
| `step` | short | Current animation step |
| `time` | short | Timer |
| `phase` | short | Current motion phase |
| `channel` | short | Sprite channel |
| `icon` | short | Sprite icon index |

### `NetMessage` struct
A compact network event message:

| Field | Type | Description |
|---|---|---|
| `type` | char | Message type (`MESS_*` constant) |
| `data1` | char | First data byte |
| `data2` | short | Second data word |
| `data3` | short | Third data word |
| `data4` | short | Fourth data word |

### `NetPacket` struct
A complete network synchronization packet sent each game tick:

| Field | Type | Description |
|---|---|---|
| `size` | char | Packet size |
| `type` | char | Packet type (`PK_*` constant) |
| `keyPress` | short | Current key input flags |
| `time` | int | Simulation time |
| `blupiPosX/Y` | short | Player position |
| `blupiIcon` | short | Current sprite icon |
| `blupiSec` | short | Secondary icon |
| `blupiChannel` | char | Sprite channel |
| `blupiTransport` | char | Vehicle transport mode |
| `nbMessages` | char | Number of messages in packet |
| `messages[]` | NetMessage[5] | Piggy-backed game events |

### `DescFile` struct
Level file header (stored in `.blp` world files):

| Field | Type | Description |
|---|---|---|
| `majRev` / `minRev` | short | File format version |
| `posDecor` | POINT | Camera/scroll position |
| `dimDecor` | POINT | World dimensions |
| `world` | short | World/chapter number |
| `music` | short | Background music track |
| `region` | short | Visual region/theme |
| `blupiPos[4]` | POINT[4] | Player start positions (up to 4) |
| `blupiDir[4]` | int[4] | Player start directions |
| `name[100]` | char | Level name string |

### `DescSave` struct (formerly `DescLevel`)
Complete serialized game state (~56KB). This is what gets written to save-game slots. Key fields:

| Field | Type | Description |
|---|---|---|
| `decor[100][100]` | short | Main decor grid (terrain icons) |
| `bigDecor[100][100]` | short | Background decor layer |
| `balleTraj[100][13]` | char | Glue ball trajectory data |
| `moveTraj[100][13]` | char | Moving object trajectory data |
| `moveObject[200]` | MoveObject | All active moving objects |
| `posDecor` | POINT | Current camera position |
| `dimDecor` | POINT | World dimensions |
| `phase` | int | Current game phase |
| `music` | int | Current music track |
| `region` | int | Current visual region |
| `time` | int | Game timer |
| `missionTitle[100]` | char | Mission name |
| `blupiPos` | POINT | Player position |
| `blupiAction` | int | Current action code |
| `blupiDir` | int | Facing direction |
| `blupiChannel` | int | Sprite channel |
| `blupiTransport` | int | Vehicle mode (0=none, 1=helicopter, 2=jeep, 3=tank) |
| `blupiShield` | BOOL | Shield active |
| `blupiPower` | BOOL | Lollipop power active |
| `blupiCloud` | BOOL | Cloud invisibility active |
| `blupiHide` | BOOL | Hidden |
| `blupiNage` | BOOL | Swimming |
| `blupiSurf` | BOOL | At water surface |
| `blupiSuspend` | BOOL | Hanging from bar |
| `blupiSkate` | BOOL | On skateboard |
| `blupiHelico` | BOOL | In helicopter |
| `blupiJeep` | BOOL | In jeep |
| `blupiTank` | BOOL | In tank |
| `blupiOver` | BOOL | In hovercraft |
| `blupiInvert` | BOOL | Controls inverted |
| `blupiDynamite` | int | Dynamite count |
| `blupiCle` | int | Gold key count |
| `blupiPerso` | int | Active player character |
| `nbVies` | int | Remaining lives |
| `nbTresor` | int | Treasures collected |
| `totalTresor` | int | Total treasures in level |
| `goalPhase` | int | Current goal/objective phase |
| `doors[200]` | char | Door state array |
| `bCheatDoors` | BOOL | Cheat: all doors open |
| `bSuperBlupi` | BOOL | Cheat: invincible player |
| `bDrawSecret` | BOOL | Cheat: show secret objects |
| `scrollPoint` | POINT | Scroll position |
| `jaugeHide[2]` | BOOL | Gauge visibility per slot |
| `jaugeType[2]` | BOOL | Gauge type per slot |
| `jaugeLevel[2]` | BOOL | Gauge level per slot |

---

## 15. CDecor Class

`CDecor` (`include/decor.hpp`, `src/decor*.cpp`) is the central game engine class. It manages the entire game world, all moving objects, the player character, and all game logic.

### Source file responsibilities

| File | Responsibility |
|---|---|
| `src/decor.cpp` | Initialization, rendering, main step loop, region/music |
| `src/decblupi.cpp` | Player movement, jumping, vehicle logic, physics |
| `src/decblock.cpp` | Block/crate interaction, push/pull, door logic |
| `src/decmove.cpp` | Moving object (enemy/bomb/effect) step logic |
| `src/decnet.cpp` | Network game synchronization and event dispatch |
| `src/decdesign.cpp` | Level editor — place/remove tiles and objects |
| `src/decio.cpp` | Save/load game state to `.blp` files |
| `src/dectables.cpp` | Static lookup tables for decor |

### Key public methods

```cpp
void Create(HWND hWnd, CSound*, CPixmap*, CNetwork*);
BOOL LoadImages();
void InitGamer();
void InitDecor();
void PlayPrepare(BOOL bTest);
void BuildPrepare();
int  IsTerminated();
void MoveStep();           // advance world simulation one tick
void Build(RECT rect);     // render the world
void SetInput(int keys);   // feed key/joystick input
int  GetRegion();
void SetRegion(int region);
int  GetMusic();
void SetMusic(int music);
BOOL GetSuperBlupi();
void SetSuperBlupi(BOOL);
BOOL GetDrawSecret();
void SetDrawSecret(BOOL);
void CheatAction(int cheat);
```

### Player state

The player state is stored directly in `CDecor` as individual member fields (`m_blupiPos`, `m_blupiAction`, `m_blupiDir`, etc.) rather than in a separate struct. This differs from Planet Blupi which uses a `Blupi[100]` array for up to 100 simultaneous characters.

In multiplayer, up to 4 players are tracked via the `blupiStartPos[4]` / `blupiStartDir[4]` fields in `DescSave`, and each player has their own `NetPacket` stream.

---

## 16. Audio System

The audio system in Free Eggbert supports two backends, switchable via the `_BASS` compile flag:

### DirectSound backend (default, `_BASS=FALSE`)
- Implemented in `src/sound.cpp`
- Uses `CSound` class wrapping DirectSound
- Loads `.blp` sound files from `gamefiles/SOUND/`
- Supports 3D positional audio relative to player position

### BASS library backend (`_BASS=TRUE`)
- Implemented in `src/soundbass.cpp`
- Uses BASS and BASSMIDI libraries for audio playback
- Located in `bass/` subdirectory
- MIDI music support via BASSMIDI
- Can be enabled by changing `#define _BASS` in `def.hpp`

### Music
- 10 background music tracks: `MUSIC000.blp` – `MUSIC009.blp`
- Music track is selected per-level (stored in `DescFile::music`)
- Music stops and changes when transitioning between regions

### Sound effects
- 93 SFX files: `SOUND000.blp` – `SOUND092.blp`
- Positional audio: sounds play louder when the player is nearby
- Vehicle sounds (helicopter, jeep) loop and change pitch based on state

---

## 17. Network / Multiplayer

Free Eggbert supports up to 4 simultaneous network players via **DirectPlay** (DirectX 3).

### Architecture
- `CNetwork` class wraps DirectPlay session management
- `CDecor::decnet.cpp` handles in-game synchronization
- Each player sends a `NetPacket` every game tick containing:
  - Current key input
  - Player position and animation state
  - Up to 5 game events (`NetMessage` structs)

### Session lifecycle phases
1. `WM_PHASE_SERVICE` — select DirectPlay transport (IPX, TCP/IP, modem)
2. `WM_PHASE_SESSION` — browse or create a game session
3. `WM_PHASE_MULTI` — pre-game lobby (wait for players)
4. `WM_PHASE_PLAY` — synchronized gameplay

### Packet types (PK_*)
| Constant | Value | Description |
|---|---|---|
| `PK_LEAVE` | 8 | Player left the session |
| `PK_LOST` | 9 | Player lost (game over) |
| `PK_DIE` | 10 | Player died |
| `PK_PAUSE` | 12 | Game pause toggled |

### Game events (MESS_*)
Key message types piggy-backed in packets:

| Constant | Value | Description |
|---|---|---|
| `MESS_RESUME` | 0 | Resume game |
| `MESS_PAUSE` | 1 | Pause game |
| `MESS_LOBBY` | 4 | Return to lobby |
| `MESS_START` | 10 | Start game |
| `MESS_OBJECTSTART` | 20 | Spawn object |
| `MESS_OBJECTDELETE` | 21 | Remove object |
| `MESS_MODIFDECOR` | 30 | Modify decor cell |
| `MESS_PLAYSOUND` | 40 | Play sound effect |
| `MESS_STOPSOUND` | 41 | Stop sound effect |
| `MESS_ASCENSEURSTART` | 50 | Start elevator |
| `MESS_ASCENSEUREND` | 51 | Stop elevator |
| `MESS_STOPCLOUD` | 60 | Stop cloud effect |

### Personal bombs
Each player in multiplayer has their own colored bomb type:
- Player 1: `TYPE_BOMBEPERSO1` (yellow)
- Player 2: `TYPE_BOMBEPERSO2` (orange)
- Player 3: `TYPE_BOMBEPERSO3` (blue)
- Player 4: `TYPE_BOMBEPERSO4` (green)

---

## 18. Game Asset Files

All assets use the `.blp` binary format inherited from Planet Blupi. Assets must be obtained from an original copy of Speedy Eggbert 2.

### 18.1 DATA directory

Contains ~110 files: world levels, demo recordings, config, and metadata.

**Configuration:**
- `config.def` — text-format user settings (FullScreen, color depth, mouse type, speed, timer)

**Level worlds** follow a chapter-based numbering scheme (`worldXXX.blp`):

| Range | Chapter | Count |
|---|---|---|
| `world001` | Tutorial/intro | 1 |
| `world010`–`world025` | Chapter 1 | 6 |
| `world030`–`world046` | Chapter 2 | 7 |
| `world050`–`world058` | Chapter 3 | 9 |
| `world060`–`world066` | Chapter 4 | 7 |
| `world070`–`world075` | Chapter 5 | 6 |
| `world080`–`world084` | Chapter 6 | 5 |
| `world090`–`world095` | Chapter 7 | 6 |
| `world100`–`world107` | Chapter 8 | 8 |
| `world110`–`world115` | Chapter 9 | 6 |
| `world120`–`world125` | Chapter 10 | 6 |
| `world199` | Special | 1 |
| `world201`–`world212` | Bonus/extra | 12 |
| `world300`–`world309` | Special/secret | 10 |

The pattern `worldNXX` uses the first digit for chapter (1–9, 0=special, 3=secret, 2=bonus).

**Demo recordings:**
- `demo300.blp`–`demo309.blp` — 10 prerecorded demo playbacks

**Other DATA files:**
- `bench.blp` — benchmark reference data
- `time.blp` — timing/speed reference data
- `info001.blp` — level info / statistics

### 18.2 IMAGE08 directory

8-bit palette (256-color) images. Used when `TrueColorBack=8` or `TrueColorDecor=8` in config. Contains ~68 `.blp` files.

**Character sprites:**
| File | Description |
|---|---|
| `blupi000.blp` | Default player character sprites |
| `blupi001.blp` | Player character variant 1 |
| `blupi002.blp` | Player character variant 2 |
| `blupi003.blp` | Player character variant 3 |

**Terrain / decor sheets** (32 sheets, one per world region/theme):
| File range | Description |
|---|---|
| `decor000.blp`–`decor031.blp` | Background terrain sheets for each region |

**Other sprites:**
| File | Channel | Description |
|---|---|---|
| `element.blp` | `CHELEMENT` | Particle and element effects |
| `explo.blp` | `CHEXPLO` | Explosion sprites |
| `object.blp` | `CHOBJECT` | Foreground objects (pickups, items) |
| `jauge.blp` | `CHJAUGE` | Health/gauge bar sprites |
| `map.blp` | `CHMAP` | Minimap sprites |

**UI / screen images:**
| File | Description |
|---|---|
| `button.blp` | UI buttons |
| `mouse.blp` | Mouse cursor sprites |
| `text.blp` | Normal text characters |
| `littletxt.blp` | Small text characters |
| Various `screen*.blp` | Menu and intro background screens |

### 18.3 IMAGE16 directory

16-bit true-color images. Used when `TrueColorBack=16` or `TrueColorDecor=16` in config (default). Contains ~55 `.blp` files.

The IMAGE16 set mirrors IMAGE08 but omits `jauge.blp` and `map.blp` (these remain 8-bit). All character and decor sheets exist in 16-bit versions for higher visual quality.

### 18.4 SOUND directory

Contains ~103 files: music tracks and sound effects.

**Music tracks (10):**
| File | Description |
|---|---|
| `MUSIC000.blp` | Background music track 0 |
| `MUSIC001.blp` | Background music track 1 |
| … | … |
| `MUSIC009.blp` | Background music track 9 |

**Sound effects (93, `SOUND_CLICK`=0 through `SOUND_FOLLOW`=92):**

Files are named `SOUND000.blp` through `SOUND092.blp` but the actual filenames on disk have mixed case (e.g., `Sound027.blp`). The index directly maps to the `SOUND_*` constant value.

---

## 19. Source File Overview

### Files tentatively complete (needs testing)
- `blupi.cpp` — WinMain, application entry, window message loop
- `button.cpp` — UI button rendering and hit-testing
- `jauge.cpp` — Progress/gauge bar rendering
- `menu.cpp` — Menu rendering and navigation
- `movie.cpp` — AVI/cutscene video playback
- `network.cpp` — DirectPlay session and lobby management
- `pixmap.cpp` — DirectDraw surface allocation, blitting, sprite drawing
- `sound.cpp` — DirectSound initialization and playback
- `wave.cpp` — WAV file parsing and loading

### Files requiring the most work (in priority order)
1. `event.cpp` — Input processing and phase dispatch (most complex)
2. `decblupi.cpp` — Player physics, movement, all vehicle logic
3. `decio.cpp` — Save/load serialization, file format compatibility
4. `decdesign.cpp` — Level editor interaction
5. `decblock.cpp` — Crate, door, and block interaction
6. `decmove.cpp` — All enemy and moving object AI
7. `decnet.cpp` — Network synchronization and event replay
8. `decor.cpp` — Core world render and init loop
9. `misc.cpp` — Miscellaneous helpers

---

## 20. Decompilation Notes

### Source of symbol names
Many function and variable names come from the **2013 Windows Phone version of Speedy Blupi**, inspected using **ILSpy**. This version retained demangled C++ symbol names. Credit to **Ч.У.Ш** from the 4PDA forum for archiving this obscure port.

### Known decompilation bugs

**Double `CPixmap::Create()` call:** The decompiled code calls `CPixmap::Create()` twice during initialization. This is likely a decompilation artifact — the second call was probably not present in the original code.

**Parameter swap bug:** At least one function has parameters in the wrong order due to incorrect decompilation of calling conventions. This produces wrong behavior when the function is called.

**Unsafe `delete this`:** Some decompiled code contains `delete this` patterns copied directly from the decompiled binary. These are inherently unsafe in C++ and represent either decompilation artifacts or original code patterns that need careful handling.

**Uninitialized variables:** The original binary was compiled with optimizations that reuse stack space aggressively. Decompiled code has many apparently uninitialized variables that were actually initialized by the original compiler's register allocation. The `/wd4700 /wd4703` MSVC flags suppress these warnings.

**`_LEGACY` flag:** When `_LEGACY=TRUE`, the codebase retains intentionally broken behaviors from the original decompiled code. Setting it to `FALSE` (default) enables fixes for confirmed bugs.

### Comparison against Planet Blupi
Where behavior is ambiguous in the decompiled output, the **Planet Blupi source code** (the official open-source release of the original engine) is used as a reference. The two codebases share common heritage and many algorithms are nearly identical.

---

## 21. Web / Emscripten Build

Free Eggbert can be compiled to WebAssembly using Emscripten.

### Build steps

```bash
source /path/to/emsdk/emsdk_env.sh
emcmake cmake -S . -B cmake-build-web -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-web -j
emrun cmake-build-web/bin/SPEEDY_BLUPI_WINDOWS.html
```

Game assets from `gamefiles/` (DATA, IMAGE08, IMAGE16, SOUND) are preloaded into the Emscripten virtual filesystem at build time — no manual copying required.

### Persistent saves

Save data is stored in **IndexedDB** via Emscripten's **IDBFS** virtual filesystem, mounted at `/save`. Data persists across page reloads. Clearing browser site data resets to preloaded defaults.

**Export/import from the browser console:**
```js
// Flush /save to IndexedDB
Module.ccall('FreeEggbert_ExportPersistentData', null, [], []);

// Restore /save from IndexedDB
Module.ccall('FreeEggbert_ImportPersistentData', null, [], []);
```

These functions are defined in `src/web_persistence.cpp` and exported via `EMSCRIPTEN_KEEPALIVE`. On non-Emscripten builds, the file is empty.

---

## 22. Android

Android build support is present under `android/`. See `ANDROID.md` for current instructions and status.

---

## 23. Cheat Codes

Three cheat codes are tracked as boolean flags in `DescSave` and accessible via `CDecor::CheatAction()`:

| Flag | Cheat code | Effect |
|---|---|---|
| `bCheatDoors` | `opendoors` | Opens all doors in the level |
| `bSuperBlupi` | `megablupi` | Player becomes invincible |
| `bDrawSecret` | `showsecret` | Reveals hidden/secret objects |

---

## 24. Known Issues and TODO

### Build verification needed
- Clean checkout build with `git submodule update --init --recursive && cmake -S . -B build -DSPEEDY_BLUPI_BACKEND=FREEDIRECT && cmake --build build -j`
- Confirm default Linux build requires no system SDL packages
- Verify `FREE_USE_SYSTEM_SDL=ON` override still works
- Confirm `free-api` and `free-direct` link SDL as `PRIVATE`

### CMake cleanup
- Change `target_include_directories` for `SPEEDY_BLUPI_WINDOWS` from `PUBLIC` to `PRIVATE`
- Verify `/permissive-` MSVC flag is appropriate for decompiled code

### Gameplay
- Many movement and physics behaviors in `decblupi.cpp` need verification against original
- Network synchronization in `decnet.cpp` is incomplete
- Level editor in `decdesign.cpp` has known missing features
- Save/load in `decio.cpp` may have version compatibility gaps

### Documentation goals (from README)
- [ ] Add Doxygen documentation to all headers and source files
- [ ] Document all `TYPE_*`, `ACTION_*`, and `SOUND_*` constants in context
- [ ] Document `.blp` binary file format specification

### Platform goals (from README)
- [ ] Improve accuracy of decompiled source code
- [ ] Restore missing or defective gameplay behavior
- [ ] Add support for Free Direct via CMake (in progress)
- [ ] Port to additional platforms beyond Windows
- [ ] Investigate Android support
- [ ] Investigate web browser support
