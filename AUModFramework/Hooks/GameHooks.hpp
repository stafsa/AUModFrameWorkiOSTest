#pragma once
#include "../Core/HookEngine.hpp"
#include "../ModLoader/ModLoader.hpp"
#include "../include/AUClasses.hpp"

// ═══════════════════════════════════════════════════════════════
//  GameHooks — patches Among Us IL2CPP methods, fires mod events
//  All RVAs verified from dump.cs
// ═══════════════════════════════════════════════════════════════

namespace GameHooks {

// ─────────────────────────────────────────────
//  PlayerControl::OnGameStart  RVA: 0x1DCBFE8
// ─────────────────────────────────────────────
static void* orig_OnGameStart = nullptr;
static void OnGameStart_hook(PlayerControl* self) {
    if (self == PlayerControl::GetLocalPlayer())
        ModLoader::DispatchGameStarted({ GameState::Started });
    ((void(*)(PlayerControl*))orig_OnGameStart)(self);
}

// ─────────────────────────────────────────────
//  PlayerControl::OnGameEnd    RVA: 0x1DCC158
// ─────────────────────────────────────────────
static void* orig_OnGameEnd = nullptr;
static void OnGameEnd_hook(PlayerControl* self) {
    if (self == PlayerControl::GetLocalPlayer())
        ModLoader::DispatchGameEnded({ 0 });
    ((void(*)(PlayerControl*))orig_OnGameEnd)(self);
}

// ─────────────────────────────────────────────
//  PlayerControl::FixedUpdate   RVA: 0x1DCA924
// ─────────────────────────────────────────────
static void* orig_FixedUpdate = nullptr;
static void FixedUpdate_hook(PlayerControl* self) {
    ((void(*)(PlayerControl*))orig_FixedUpdate)(self);
    if (self == PlayerControl::GetLocalPlayer())
        ModLoader::DispatchFixedUpdate({ 0.02f }); // AU runs at 50Hz fixed
}

// ─────────────────────────────────────────────
//  PlayerControl::Die   RVA: 0x1DCD414
// ─────────────────────────────────────────────
static void* orig_Die = nullptr;
static void Die_hook(PlayerControl* self, DeathReason reason, bool assignGhostRole) {
    auto* info = self->GetData();
    ModLoader::DispatchPlayerDied({ self, info });
    ((void(*)(PlayerControl*, DeathReason, bool))orig_Die)(self, reason, assignGhostRole);
}

// ─────────────────────────────────────────────
//  PlayerControl::Revive   RVA: 0x1DCDC70
// ─────────────────────────────────────────────
static void* orig_Revive = nullptr;
static void Revive_hook(PlayerControl* self) {
    // Fire spawn event since revive essentially re-spawns player
    ModLoader::DispatchPlayerSpawned({ self });
    ((void(*)(PlayerControl*))orig_Revive)(self);
}

// ─────────────────────────────────────────────
//  MeetingHud::Start   RVA: 0x1D11374
// ─────────────────────────────────────────────
static void* orig_MeetingStart = nullptr;
static void MeetingStart_hook(MeetingHud* self) {
    ((void(*)(MeetingHud*))orig_MeetingStart)(self);
    ModLoader::DispatchMeetingStart({ self });
}

// ─────────────────────────────────────────────
//  MeetingHud::Close   RVA: 0x1D126DC
// ─────────────────────────────────────────────
static void* orig_MeetingClose = nullptr;
static void MeetingClose_hook(MeetingHud* self) {
    ModLoader::DispatchMeetingEnd({});
    ((void(*)(MeetingHud*))orig_MeetingClose)(self);
}

// ─────────────────────────────────────────────
//  ChatController::AddChat   RVA: 0x1CE4EE0
//  Intercept incoming chat messages
// ─────────────────────────────────────────────
static void* orig_AddChat = nullptr;
static void AddChat_hook(ChatController* self, PlayerControl* source,
                         Il2CppString* text, bool censor)
{
    std::string msg = IL2CPP::StringToStd(text);
    bool cancel = false;
    ChatMessageEvent e { source, msg, cancel };
    ModLoader::DispatchChatMessage(e);
    if (!cancel)
        ((void(*)(ChatController*, PlayerControl*, Il2CppString*, bool))orig_AddChat)
            (self, source, text, censor);
}

// ─────────────────────────────────────────────
//  ChatController::SendChat   RVA: 0x1CE34AC
//  Intercept outgoing chat (local player sends)
// ─────────────────────────────────────────────
static void* orig_SendChat = nullptr;
static void SendChat_hook(ChatController* self) {
    // Let mods observe send (cancelable via the AddChat hook above)
    ((void(*)(ChatController*))orig_SendChat)(self);
}

// ─────────────────────────────────────────────
//  Install all hooks
// ─────────────────────────────────────────────
inline void InstallAll() {
    printf("[AUMF] Installing hooks...\n");

    Hooks::Install("PlayerControl.OnGameStart",  0x1DCBFE8, (void*)OnGameStart_hook,  &orig_OnGameStart);
    Hooks::Install("PlayerControl.OnGameEnd",    0x1DCC158, (void*)OnGameEnd_hook,    &orig_OnGameEnd);
    Hooks::Install("PlayerControl.FixedUpdate",  0x1DCA924, (void*)FixedUpdate_hook,  &orig_FixedUpdate);
    Hooks::Install("PlayerControl.Die",          0x1DCD414, (void*)Die_hook,          &orig_Die);
    Hooks::Install("PlayerControl.Revive",       0x1DCDC70, (void*)Revive_hook,       &orig_Revive);
    Hooks::Install("MeetingHud.Start",           0x1D11374, (void*)MeetingStart_hook, &orig_MeetingStart);
    Hooks::Install("MeetingHud.Close",           0x1D126DC, (void*)MeetingClose_hook, &orig_MeetingClose);
    Hooks::Install("ChatController.AddChat",     0x1CE4EE0, (void*)AddChat_hook,      &orig_AddChat);
    Hooks::Install("ChatController.SendChat",    0x1CE34AC, (void*)SendChat_hook,     &orig_SendChat);

    Hooks::PrintAll();
}

} // namespace GameHooks
