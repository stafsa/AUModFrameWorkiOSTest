#pragma once
#include <string>
#include <functional>
#include "../include/AUClasses.hpp"

// ═══════════════════════════════════════════════════════════════
//  AUMF Mod API  —  mirrors BepInEx's BasePlugin contract
//  Every mod inherits AUMFMod and implements the virtual methods
// ═══════════════════════════════════════════════════════════════

struct ModInfo {
    std::string id;          // unique reverse-domain e.g. "com.you.mymod"
    std::string name;        // display name
    std::string version;     // semver string "1.0.0"
    std::string author;
    std::string description;
};

// ─────────────────────────────────────────────
//  Event argument structs (passed to callbacks)
// ─────────────────────────────────────────────
struct PlayerSpawnedEvent  { PlayerControl* player; };
struct PlayerDiedEvent     { PlayerControl* player; NetworkedPlayerInfo* info; };
struct MeetingStartedEvent { MeetingHud* hud; };
struct MeetingEndedEvent   {};
struct ChatMessageEvent    { PlayerControl* sender; std::string message; bool& cancel; };
struct GameStartedEvent    { GameState state; };
struct GameEndedEvent      { int reason; };
struct FixedUpdateEvent    { float deltaTime; };
struct GuiDrawEvent        {}; // fired every frame for IMGUI-style overlay drawing

// ─────────────────────────────────────────────
//  Base class every mod extends
// ─────────────────────────────────────────────
class AUMFMod {
public:
    virtual ~AUMFMod() = default;

    // Called once when mod is loaded — register your hooks here
    virtual void OnLoad() {}

    // Called when mod is unloaded (framework is shutting down)
    virtual void OnUnload() {}

    // ── Game lifecycle ──
    virtual void OnGameStarted (const GameStartedEvent&)  {}
    virtual void OnGameEnded   (const GameEndedEvent&)    {}

    // ── Player events ──
    virtual void OnPlayerSpawned(const PlayerSpawnedEvent&) {}
    virtual void OnPlayerDied   (const PlayerDiedEvent&)    {}

    // ── Meeting events ──
    virtual void OnMeetingStarted(const MeetingStartedEvent&) {}
    virtual void OnMeetingEnded  (const MeetingEndedEvent&)   {}

    // ── Chat ──
    virtual void OnChatMessage(ChatMessageEvent&) {}

    // ── Per-frame ──
    virtual void OnFixedUpdate(const FixedUpdateEvent&) {}

    // ── UI overlay (called every frame inside GL/Metal context) ──
    virtual void OnGuiDraw(const GuiDrawEvent&) {}

    // ── Metadata ──
    virtual ModInfo GetInfo() const = 0;

    // Internal — set by loader
    bool enabled = true;
    std::string logTag() const { return "[" + GetInfo().name + "]"; }
};

// ─────────────────────────────────────────────
//  Macro to declare mod entry point
//  Put this in your mod's .cpp file:
//
//    AUMF_DECLARE_MOD(MyModClass)
// ─────────────────────────────────────────────
#define AUMF_DECLARE_MOD(ClassName)                          \
    extern "C" __attribute__((visibility("default")))        \
    AUMFMod* AUMF_CreateMod() { return new ClassName(); }    \
    extern "C" __attribute__((visibility("default")))        \
    void     AUMF_DestroyMod(AUMFMod* mod) { delete mod; }
