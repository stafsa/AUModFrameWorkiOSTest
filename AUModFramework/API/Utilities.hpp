#pragma once
#include "../include/AUClasses.hpp"
#include <string>
#include <vector>
#include <functional>

// ═══════════════════════════════════════════════════════════════
//  AUMF Utilities — helper functions for mod authors
// ═══════════════════════════════════════════════════════════════

namespace AUMF {

// ─────────────────────────────────────────────
//  Player helpers
// ─────────────────────────────────────────────

// Get the local player (the one you're playing as)
inline PlayerControl* GetLocalPlayer() {
    return PlayerControl::GetLocalPlayer();
}

// Iterate all active players, calling fn(PlayerControl*) for each
inline void ForEachPlayer(std::function<void(PlayerControl*)> fn) {
    auto* list = PlayerControl::GetAllPlayers();
    if (!list) return;
    uint32_t count = list->max_length;
    for (uint32_t i = 0; i < count; i++) {
        auto* player = reinterpret_cast<PlayerControl*>(list->vector[i]);
        if (player) fn(player);
    }
}

// Get all players as a std::vector
inline std::vector<PlayerControl*> GetAllPlayers() {
    std::vector<PlayerControl*> result;
    ForEachPlayer([&](PlayerControl* p){ result.push_back(p); });
    return result;
}

// Get player name as std::string
inline std::string GetPlayerName(PlayerControl* player) {
    if (!player) return "";
    auto* data = player->GetData();
    if (!data) return "";
    return IL2CPP::StringToStd(data->GetPlayerName());
}

// Check if a player is the impostor
inline bool IsImpostor(PlayerControl* player) {
    if (!player) return false;
    auto* data = player->GetData();
    if (!data) return false;
    RoleTypes role = data->RoleType();
    return role == RoleTypes::Impostor || role == RoleTypes::Shapeshifter;
}

// Check if a player is dead
inline bool IsDead(PlayerControl* player) {
    if (!player) return false;
    auto* data = player->GetData();
    return data ? data->IsDead() : false;
}

// Check if a player is local player
inline bool IsLocalPlayer(PlayerControl* player) {
    return player && player == GetLocalPlayer();
}

// ─────────────────────────────────────────────
//  Role helpers
// ─────────────────────────────────────────────

inline std::string RoleTypeName(RoleTypes role) {
    switch (role) {
        case RoleTypes::Crewmate:      return "Crewmate";
        case RoleTypes::Impostor:      return "Impostor";
        case RoleTypes::Scientist:     return "Scientist";
        case RoleTypes::Engineer:      return "Engineer";
        case RoleTypes::GuardianAngel: return "GuardianAngel";
        case RoleTypes::Shapeshifter:  return "Shapeshifter";
        case RoleTypes::Noisemaker:    return "Noisemaker";
        case RoleTypes::Phantom:       return "Phantom";
        case RoleTypes::Tracker:       return "Tracker";
        default: return "Unknown";
    }
}

// ─────────────────────────────────────────────
//  Game state helpers
// ─────────────────────────────────────────────

inline bool IsInGame() {
    auto* client = AmongUsClient::Instance();
    return client != nullptr; // expand with GameState check if needed
}

inline bool IsMeetingActive() {
    return MeetingHud::Instance() != nullptr;
}

// ─────────────────────────────────────────────
//  Movement helpers
// ─────────────────────────────────────────────

inline void SetLocalPlayerSpeed(float speed) {
    auto* local = GetLocalPlayer();
    if (!local) return;
    auto* phys = local->MyPhysics();
    if (!phys) return;
    phys->setSpeed(speed);
    phys->setGhostSpeed(speed);
}

inline float GetLocalPlayerSpeed() {
    auto* local = GetLocalPlayer();
    if (!local) return 0.0f;
    auto* phys = local->MyPhysics();
    return phys ? phys->getSpeed() : 0.0f;
}

// ─────────────────────────────────────────────
//  Chat helpers
// ─────────────────────────────────────────────

// Send a chat message as the local player
// Note: ChatController is a MonoBehaviour — find it via FindObjectOfType
// For now, uses the AddChat approach via modloader event dispatch
inline void SendSystemChatMessage(const std::string& msg) {
    // We can call AddChat on any ChatController instance found at runtime
    // This helper is best used from within a hook where you have the instance
    printf("[AUMF] SysChat: %s\n", msg.c_str());
}

// ─────────────────────────────────────────────
//  Logging
// ─────────────────────────────────────────────

inline void Log(const std::string& tag, const std::string& msg) {
    printf("[AUMF][%s] %s\n", tag.c_str(), msg.c_str());
}

} // namespace AUMF
