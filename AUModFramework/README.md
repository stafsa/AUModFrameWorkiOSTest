# AUModFramework (AUMF)

A full modding framework for Among Us iOS — inspired by BepInEx, built for LiveContainer injection.

## Architecture

```
AUModFramework/
├── Core/
│   ├── IL2CPP.hpp       — base address resolution, string helpers
│   ├── HookEngine.hpp   — Dobby-based method patching + AUFM_HOOK macro
│   └── Framework.mm     — dylib constructor, init thread, entry point
├── include/
│   └── AUClasses.hpp    — All AU IL2CPP classes with REAL offsets from dump.cs
├── API/
│   ├── ModAPI.hpp       — AUMFMod base class, event structs, AUMF_DECLARE_MOD
│   └── Utilities.hpp    — Helper functions for mod authors
├── Hooks/
│   └── GameHooks.hpp    — IL2CPP hooks that fire framework events
├── ModLoader/
│   └── ModLoader.hpp    — dylib scanner, loader, event dispatcher
├── UI/
│   └── ModMenu.mm       — UIKit in-game overlay (4-finger tap)
└── mods/
    └── ExampleMod/
        └── ExampleMod.cpp
```

## Offsets (from dump.cs — latest AU iOS)

All offsets are verified from the attached `dump.cs`. Key RVAs:

| Method | RVA |
|--------|-----|
| PlayerControl::OnGameStart | 0x1DCBFE8 |
| PlayerControl::OnGameEnd | 0x1DCC158 |
| PlayerControl::FixedUpdate | 0x1DCA924 |
| PlayerControl::Die | 0x1DCD414 |
| PlayerControl::Revive | 0x1DCDC70 |
| PlayerControl::SetName | 0x1DCF178 |
| PlayerControl::SetColor | 0x1DCF5D4 |
| PlayerControl::MurderPlayer | 0x1DD23C0 |
| PlayerControl::RpcMurderPlayer | 0x1DD1A8C |
| PlayerControl::RpcSetName | 0x1DD7E0C |
| PlayerControl::RpcSetColor | 0x1DCF448 |
| PlayerControl::Shapeshift | 0x1DD4690 |
| PlayerControl::CheckMurder | 0x1DD1678 |
| PlayerControl::StartMeeting | 0x1DD0A78 |
| PlayerControl::GetTruePosition | 0x1DCBD80 |
| MeetingHud::Start | 0x1D11374 |
| MeetingHud::Close | 0x1D126DC |
| ChatController::AddChat | 0x1CE4EE0 |
| ChatController::SendChat | 0x1CE34AC |
| GameData::GetPlayerById | 0x203E610 |
| NetworkedPlayerInfo::get_PlayerName | 0x203FA1C |
| PlayerPhysics::get_TrueSpeed | 0x1DDF2C4 |

## Building

### Requirements
- macOS + Xcode with iOS SDK
- [Dobby](https://github.com/jmpews/Dobby) built for arm64-ios

### Steps
```bash
git clone https://github.com/jmpews/Dobby && cd Dobby
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake \
         -DPLATFORM=OS64 -DDEPLOYMENT_TARGET=14.0
make -j4
cd ../../AUModFramework
export DOBBY_DIR=../Dobby/build
make all
```

## Writing a Mod

```cpp
#include "API/ModAPI.hpp"
#include "API/Utilities.hpp"

class MyMod : public AUMFMod {
public:
    ModInfo GetInfo() const override {
        return { "com.you.mymod", "MyMod", "1.0.0", "You", "Does stuff" };
    }

    void OnLoad() override {
        AUMF::Log("MyMod", "Hello!");
    }

    void OnGameStarted(const GameStartedEvent& e) override {
        // Set speed to 2x on game start
        AUMF::SetLocalPlayerSpeed(5.0f);
    }

    void OnChatMessage(ChatMessageEvent& e) override {
        if (e.message == "!hello") {
            e.cancel = true;
            AUMF::Log("MyMod", "Hello from chat command!");
        }
    }
};

AUMF_DECLARE_MOD(MyMod)
```

Compile to `.dylib` and drop into `~/Documents/AUMF/Mods/`.

## Injection (LiveContainer)

1. Build `AUModFramework.dylib`
2. In LiveContainer, add it as an injected dylib for Among Us
3. Launch Among Us — framework loads automatically
4. Use **4-finger tap** to open the mod menu in-game

## Events Available

| Event | When it fires |
|-------|--------------|
| `OnLoad` | Mod dylib loaded |
| `OnGameStarted` | Game phase transitions to Started |
| `OnGameEnded` | Game ends or you disconnect |
| `OnPlayerSpawned` | A player spawns / revives |
| `OnPlayerDied` | A player dies |
| `OnMeetingStarted` | Emergency/body meeting begins |
| `OnMeetingEnded` | Meeting closes |
| `OnChatMessage` | Any chat message received (cancelable) |
| `OnFixedUpdate` | Every physics frame (~50Hz) |
| `OnGuiDraw` | Every render frame (for overlays) |
