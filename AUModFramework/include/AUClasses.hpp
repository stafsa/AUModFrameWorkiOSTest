#pragma once

enum class GameState : int32_t {
    NotStarted = 0,
    Lobby      = 1,
    Started    = 3,
    Ended      = 4
};

#include "../Core/IL2CPP.hpp"

// ═══════════════════════════════════════════════════════════════
//  Among Us IL2CPP Bindings — offsets from dump.cs (latest AU iOS)
//  RVA values used directly as offsets from GameAssembly base
// ═══════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────
//  Enums (from dump.cs)
// ─────────────────────────────────────────────
enum class RoleTypes : int16_t {
    Crewmate      = 0,
    Impostor      = 1,
    Scientist     = 2,
    Engineer      = 3,
    GuardianAngel = 4,
    Shapeshifter  = 5,
    Noisemaker    = 6,
    Phantom       = 7,
    Tracker       = 8,
};

enum class DeathReason : int32_t {
    Kill      = 0,
    Exile     = 1,
    Disconnect= 2,
    Error     = 3,
    Exile2    = 4,
    Sabotage  = 5,
};

enum class MurderResultFlags : int32_t {
    Succeeded        = 1,
    FailedError      = 2,
    FailedProtected  = 4,
};

enum class SystemTypes : int32_t {
    Hallway=0, Storage=1, Reactor=2, UpperEngine=3,
    Security=4, Electrical=5, MedBay=6,
};

enum class DisconnectReasons : int32_t {
    ExitGame=0, GameFull=1, GameStarted=2, GameNotFound=3,
    ServerClosed=4, Destroy=16, Error=17,
};

// ─────────────────────────────────────────────
//  Forward declarations
// ─────────────────────────────────────────────
struct PlayerControl;
struct PlayerPhysics;
struct NetworkedPlayerInfo;
struct GameData;
struct AmongUsClient;
struct MeetingHud;
struct ChatController;

// ─────────────────────────────────────────────
//  PlayerPhysics
//  Speed @ 0x50, GhostSpeed @ 0x54
// ─────────────────────────────────────────────
struct PlayerPhysics : Il2CppObject {
    // Field offsets (from dump.cs)
    static constexpr uintptr_t OFF_Speed        = 0x50;
    static constexpr uintptr_t OFF_GhostSpeed   = 0x54;

    // Method RVAs (from dump.cs)
    static constexpr uintptr_t MTH_TrueSpeed    = 0x1DDF2C4; // get_TrueSpeed
    static constexpr uintptr_t MTH_FixedUpdate  = 0x1DDF6B8;
    static constexpr uintptr_t MTH_SetNormVel   = 0x1DD4D38; // SetNormalizedVelocity

    float getSpeed()       { return *reinterpret_cast<float*>((uintptr_t)this + OFF_Speed); }
    float getGhostSpeed()  { return *reinterpret_cast<float*>((uintptr_t)this + OFF_GhostSpeed); }
    void  setSpeed(float v){ *reinterpret_cast<float*>((uintptr_t)this + OFF_Speed) = v; }
    void  setGhostSpeed(float v){ *reinterpret_cast<float*>((uintptr_t)this + OFF_GhostSpeed) = v; }
};

// ─────────────────────────────────────────────
//  NetworkedPlayerInfo
//  PlayerId@0x35, ClientId@0x38, RoleType@0x50
//  IsDead@0x78, WasEjected@0x79, Tasks@0x70
// ─────────────────────────────────────────────
struct NetworkedPlayerInfo : Il2CppObject {
    static constexpr uintptr_t OFF_PlayerId      = 0x35;
    static constexpr uintptr_t OFF_ClientId      = 0x38;
    static constexpr uintptr_t OFF_RoleType      = 0x50;
    static constexpr uintptr_t OFF_Disconnected  = 0x64;
    static constexpr uintptr_t OFF_Tasks         = 0x70;
    static constexpr uintptr_t OFF_IsDead        = 0x78;
    static constexpr uintptr_t OFF_WasEjected    = 0x79;
    static constexpr uintptr_t OFF_Object        = 0x80; // _object (PlayerControl*)

    // Method RVAs
    static constexpr uintptr_t MTH_get_PlayerName  = 0x203FA1C;
    static constexpr uintptr_t MTH_set_PlayerName  = 0x204047C;
    static constexpr uintptr_t MTH_get_Object      = 0x203D9F4;

    uint8_t  PlayerId()     { return *reinterpret_cast<uint8_t*> ((uintptr_t)this + OFF_PlayerId); }
    RoleTypes RoleType()    { return *reinterpret_cast<RoleTypes*>((uintptr_t)this + OFF_RoleType); }
    bool     IsDead()       { return *reinterpret_cast<bool*>    ((uintptr_t)this + OFF_IsDead); }
    bool     WasEjected()   { return *reinterpret_cast<bool*>    ((uintptr_t)this + OFF_WasEjected); }
    bool     Disconnected() { return *reinterpret_cast<bool*>    ((uintptr_t)this + OFF_Disconnected); }

    Il2CppString* GetPlayerName() {
        using fn_t = Il2CppString*(*)(NetworkedPlayerInfo*);
        return IL2CPP::ResolveOffset<fn_t>(MTH_get_PlayerName)(this);
    }
    void SetPlayerName(Il2CppString* name) {
        using fn_t = void(*)(NetworkedPlayerInfo*, Il2CppString*);
        IL2CPP::ResolveOffset<fn_t>(MTH_set_PlayerName)(this, name);
    }
    PlayerControl* GetObject() {
        using fn_t = PlayerControl*(*)(NetworkedPlayerInfo*);
        return IL2CPP::ResolveOffset<fn_t>(MTH_get_Object)(this);
    }
};

// ─────────────────────────────────────────────
//  PlayerControl
//  PlayerId@0x35, moveable@0x4C, inVent@0x60
//  shapeshifting@0x66, MyPhysics@0xD0
// ─────────────────────────────────────────────
struct PlayerControl : Il2CppObject {
    // Static fields
    static constexpr uintptr_t STATIC_LocalPlayer    = 0x0;  // PlayerControl.LocalPlayer (static)
    static constexpr uintptr_t STATIC_AllPlayers     = 0x8;  // PlayerControl.AllPlayerControls (static)

    // Instance field offsets
    static constexpr uintptr_t OFF_PlayerId          = 0x35;
    static constexpr uintptr_t OFF_moveable          = 0x4C;
    static constexpr uintptr_t OFF_cosmetics         = 0x50;
    static constexpr uintptr_t OFF_inVent            = 0x60;
    static constexpr uintptr_t OFF_walkingToVent     = 0x61;
    static constexpr uintptr_t OFF_shapeshifting     = 0x66;
    static constexpr uintptr_t OFF_isKilling         = 0x68;
    static constexpr uintptr_t OFF_CachedPlayerData  = 0x70;
    static constexpr uintptr_t OFF_killTimer         = 0xB0;
    static constexpr uintptr_t OFF_MyPhysics         = 0xD0;
    static constexpr uintptr_t OFF_NetTransform      = 0xD8;
    static constexpr uintptr_t OFF_myTasks           = 0xF8;
    static constexpr uintptr_t OFF_isDummy           = 0x110;

    // Method RVAs
    static constexpr uintptr_t MTH_get_CanMove       = 0x1DC92F0;
    static constexpr uintptr_t MTH_get_Data          = 0x1DC8BB4;
    static constexpr uintptr_t MTH_SetKillTimer      = 0x1DC99B4;
    static constexpr uintptr_t MTH_FixedUpdate       = 0x1DCA924;
    static constexpr uintptr_t MTH_OnGameStart       = 0x1DCBFE8;
    static constexpr uintptr_t MTH_OnGameEnd         = 0x1DCC158;
    static constexpr uintptr_t MTH_GetTruePosition   = 0x1DCBD80;
    static constexpr uintptr_t MTH_Die               = 0x1DCD414;
    static constexpr uintptr_t MTH_Revive            = 0x1DCDC70;
    static constexpr uintptr_t MTH_CoSetRole         = 0x1DCEC8C;
    static constexpr uintptr_t MTH_Exiled            = 0x1DCED3C;
    static constexpr uintptr_t MTH_SetName           = 0x1DCF178;
    static constexpr uintptr_t MTH_SetColor          = 0x1DCF5D4;
    static constexpr uintptr_t MTH_StartMeeting      = 0x1DD0A78;
    static constexpr uintptr_t MTH_ResetForMeeting   = 0x1DD0CCC;
    static constexpr uintptr_t MTH_CheckMurder       = 0x1DD1678;
    static constexpr uintptr_t MTH_MurderPlayer      = 0x1DD23C0;
    static constexpr uintptr_t MTH_CheckProtect      = 0x1DD303C;
    static constexpr uintptr_t MTH_ProtectPlayer     = 0x1DD3450;
    static constexpr uintptr_t MTH_Shapeshift        = 0x1DD4690;
    static constexpr uintptr_t MTH_RpcMurderPlayer   = 0x1DD1A8C;
    static constexpr uintptr_t MTH_RpcSetColor       = 0x1DCF448;
    static constexpr uintptr_t MTH_RpcSetName        = 0x1DD7E0C;
    static constexpr uintptr_t MTH_RpcStartMeeting   = 0x1DD01CC;

    // ── Static accessors ──
    static PlayerControl* GetLocalPlayer() {
        // LocalPlayer is a static field on the class, stored in BSS
        // We access it via the Il2Cpp class static fields table
        // Use GameData's resolved instance as alternative
        return *reinterpret_cast<PlayerControl**>(IL2CPP::baseAddress + STATIC_LocalPlayer);
    }
    static Il2CppArray* GetAllPlayers() {
        return *reinterpret_cast<Il2CppArray**>(IL2CPP::baseAddress + STATIC_AllPlayers);
    }

    // ── Field accessors ──
    uint8_t   PlayerId()     { return *reinterpret_cast<uint8_t*>((uintptr_t)this + OFF_PlayerId); }
    bool      moveable()     { return *reinterpret_cast<bool*>   ((uintptr_t)this + OFF_moveable); }
    bool      inVent()       { return *reinterpret_cast<bool*>   ((uintptr_t)this + OFF_inVent); }
    bool      shapeshifting(){ return *reinterpret_cast<bool*>   ((uintptr_t)this + OFF_shapeshifting); }
    float     killTimer()    { return *reinterpret_cast<float*>  ((uintptr_t)this + OFF_killTimer); }
    bool      isDummy()      { return *reinterpret_cast<bool*>   ((uintptr_t)this + OFF_isDummy); }

    PlayerPhysics* MyPhysics() {
        return *reinterpret_cast<PlayerPhysics**>((uintptr_t)this + OFF_MyPhysics);
    }
    NetworkedPlayerInfo* CachedData() {
        return *reinterpret_cast<NetworkedPlayerInfo**>((uintptr_t)this + OFF_CachedPlayerData);
    }

    void setMoveable(bool v)  { *reinterpret_cast<bool*>((uintptr_t)this + OFF_moveable) = v; }
    void setKillTimer(float v){ *reinterpret_cast<float*>((uintptr_t)this + OFF_killTimer) = v; }

    // ── Method callers ──
    NetworkedPlayerInfo* GetData() {
        using fn_t = NetworkedPlayerInfo*(*)(PlayerControl*);
        return IL2CPP::ResolveOffset<fn_t>(MTH_get_Data)(this);
    }
    bool CanMove() {
        using fn_t = bool(*)(PlayerControl*);
        return IL2CPP::ResolveOffset<fn_t>(MTH_get_CanMove)(this);
    }
    void SetName(Il2CppString* name) {
        using fn_t = void(*)(PlayerControl*, Il2CppString*);
        IL2CPP::ResolveOffset<fn_t>(MTH_SetName)(this, name);
    }
    void SetColor(int colorId) {
        using fn_t = void(*)(PlayerControl*, int);
        IL2CPP::ResolveOffset<fn_t>(MTH_SetColor)(this, colorId);
    }
    void Die(DeathReason reason, bool assignGhostRole) {
        using fn_t = void(*)(PlayerControl*, DeathReason, bool);
        IL2CPP::ResolveOffset<fn_t>(MTH_Die)(this, reason, assignGhostRole);
    }
    void Revive() {
        using fn_t = void(*)(PlayerControl*);
        IL2CPP::ResolveOffset<fn_t>(MTH_Revive)(this);
    }
    void MurderPlayer(PlayerControl* target, MurderResultFlags flags) {
        using fn_t = void(*)(PlayerControl*, PlayerControl*, MurderResultFlags);
        IL2CPP::ResolveOffset<fn_t>(MTH_MurderPlayer)(this, target, flags);
    }
    void RpcMurderPlayer(PlayerControl* target, bool didSucceed) {
        using fn_t = void(*)(PlayerControl*, PlayerControl*, bool);
        IL2CPP::ResolveOffset<fn_t>(MTH_RpcMurderPlayer)(this, target, didSucceed);
    }
    void RpcSetName(Il2CppString* name) {
        using fn_t = void(*)(PlayerControl*, Il2CppString*);
        IL2CPP::ResolveOffset<fn_t>(MTH_RpcSetName)(this, name);
    }
    void RpcSetColor(uint8_t colorId) {
        using fn_t = void(*)(PlayerControl*, uint8_t);
        IL2CPP::ResolveOffset<fn_t>(MTH_RpcSetColor)(this, colorId);
    }
    void Shapeshift(PlayerControl* target, bool animate) {
        using fn_t = void(*)(PlayerControl*, PlayerControl*, bool);
        IL2CPP::ResolveOffset<fn_t>(MTH_Shapeshift)(this, target, animate);
    }
    void Exiled() {
        using fn_t = void(*)(PlayerControl*);
        IL2CPP::ResolveOffset<fn_t>(MTH_Exiled)(this);
    }
    void CheckMurder(PlayerControl* target) {
        using fn_t = void(*)(PlayerControl*, PlayerControl*);
        IL2CPP::ResolveOffset<fn_t>(MTH_CheckMurder)(this, target);
    }
    void StartMeeting(NetworkedPlayerInfo* target) {
        using fn_t = void(*)(PlayerControl*, NetworkedPlayerInfo*);
        IL2CPP::ResolveOffset<fn_t>(MTH_StartMeeting)(this, target);
    }
    // SetKillTimer via direct field write (more reliable)
    void SetKillTimerValue(float t) { setKillTimer(t); }
};

// ─────────────────────────────────────────────
//  GameData
//  Instance@static+0x0, AllPlayers@0x20
// ─────────────────────────────────────────────
struct GameData : Il2CppObject {
    static constexpr uintptr_t STATIC_Instance   = 0x0;  // static field offset in class statics

    static constexpr uintptr_t OFF_AllPlayers    = 0x20;
    static constexpr uintptr_t OFF_TotalTasks    = 0x30;
    static constexpr uintptr_t OFF_CompletedTasks= 0x34;

    static constexpr uintptr_t MTH_GetPlayerById = 0x203E610;
    static constexpr uintptr_t MTH_GetPlayerCount= 0x203DC68;
    static constexpr uintptr_t MTH_OnMeetingStart= 0x203DDD4;
    static constexpr uintptr_t MTH_OnGameEnd     = 0x203DEC0;

    // GameData.Instance is a static field — access via il2cpp class statics
    // In practice we resolve it after IL2CPP::Init() using the known pattern
    static GameData* Instance() {
        // Static field: GameData.Instance stores the singleton pointer
        // Located in the class static fields table, at static offset 0x0
        return *reinterpret_cast<GameData**>(IL2CPP::baseAddress + STATIC_Instance);
    }

    Il2CppArray* AllPlayers() {
        return *reinterpret_cast<Il2CppArray**>((uintptr_t)this + OFF_AllPlayers);
    }
    int TotalTasks()     { return *reinterpret_cast<int*>((uintptr_t)this + OFF_TotalTasks); }
    int CompletedTasks() { return *reinterpret_cast<int*>((uintptr_t)this + OFF_CompletedTasks); }

    NetworkedPlayerInfo* GetPlayerById(uint8_t id) {
        using fn_t = NetworkedPlayerInfo*(*)(GameData*, uint8_t);
        return IL2CPP::ResolveOffset<fn_t>(MTH_GetPlayerById)(this, id);
    }
};

// ─────────────────────────────────────────────
//  AmongUsClient
//  Instance@static+0x0
// ─────────────────────────────────────────────
struct AmongUsClient : Il2CppObject {
    static constexpr uintptr_t STATIC_Instance   = 0x0;

    static constexpr uintptr_t OFF_discoverState     = 0x158;
    static constexpr uintptr_t OFF_MenuTarget        = 0x174;

    static constexpr uintptr_t MTH_ExitGame          = 0x1D46AAC; // approximate — search near class block

    static AmongUsClient* Instance() {
        return *reinterpret_cast<AmongUsClient**>(IL2CPP::baseAddress + STATIC_Instance);
    }
    void ExitGame(DisconnectReasons reason) {
        using fn_t = void(*)(AmongUsClient*, DisconnectReasons);
        IL2CPP::ResolveOffset<fn_t>(MTH_ExitGame)(this, reason);
    }
};

// ─────────────────────────────────────────────
//  MeetingHud
//  Instance@static+0x0, state@0xE0
//  discussionTimer@0x110, reporterId@0x114
// ─────────────────────────────────────────────
struct MeetingHud : Il2CppObject {
    static constexpr uintptr_t STATIC_Instance      = 0x0;

    static constexpr uintptr_t OFF_state            = 0xE0;
    static constexpr uintptr_t OFF_discussionTimer  = 0x110;
    static constexpr uintptr_t OFF_reporterId       = 0x114;
    static constexpr uintptr_t OFF_amDead           = 0x115;
    static constexpr uintptr_t OFF_exiledPlayer     = 0xF8;

    // Methods
    static constexpr uintptr_t MTH_get_CurrentState = 0x1D112F8;
    static constexpr uintptr_t MTH_Start            = 0x1D11374;
    static constexpr uintptr_t MTH_Close            = 0x1D126DC;
    static constexpr uintptr_t MTH_CastVote         = 0x1D12318; // approximate — check near class

    static MeetingHud* Instance() {
        return *reinterpret_cast<MeetingHud**>(IL2CPP::baseAddress + STATIC_Instance);
    }
    float discussionTimer() { return *reinterpret_cast<float*>((uintptr_t)this + OFF_discussionTimer); }
    void  setDiscussionTimer(float v){ *reinterpret_cast<float*>((uintptr_t)this + OFF_discussionTimer) = v; }
    uint8_t reporterId()    { return *reinterpret_cast<uint8_t*>((uintptr_t)this + OFF_reporterId); }

    void CastVote(int srcId, int suspectId) {
        using fn_t = void(*)(MeetingHud*, int, int);
        IL2CPP::ResolveOffset<fn_t>(MTH_CastVote)(this, srcId, suspectId);
    }
    void Close() {
        using fn_t = void(*)(MeetingHud*);
        IL2CPP::ResolveOffset<fn_t>(MTH_Close)(this);
    }
};

// ─────────────────────────────────────────────
//  ChatController
//  SendChat @ 0x1CE34AC
//  AddChat  @ 0x1CE4EE0
// ─────────────────────────────────────────────
struct ChatController : Il2CppObject {
    static constexpr uintptr_t MTH_SendChat     = 0x1CE34AC;
    static constexpr uintptr_t MTH_AddChat      = 0x1CE4EE0;
    static constexpr uintptr_t MTH_AddChatNote  = 0x1CE459C;
    static constexpr uintptr_t MTH_SendFreeChat = 0x1CE38C4;

    void SendChat() {
        using fn_t = void(*)(ChatController*);
        IL2CPP::ResolveOffset<fn_t>(MTH_SendChat)(this);
    }
    void AddChat(PlayerControl* source, Il2CppString* text, bool censor) {
        using fn_t = void(*)(ChatController*, PlayerControl*, Il2CppString*, bool);
        IL2CPP::ResolveOffset<fn_t>(MTH_AddChat)(this, source, text, censor);
    }
};
