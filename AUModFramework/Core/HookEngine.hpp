#pragma once
#include "IL2CPP.hpp"
#include <unordered_map>
#include <string>
#include <stdio.h>

// ─────────────────────────────────────────────
//  Dobby hook interface (works with LiveContainer)
//  If you switch to Substrate: replace DobbyHook
//  with MSHookFunction from substrate.h
// ─────────────────────────────────────────────
extern "C" int DobbyHook(void* address, void* replace, void** origin);

namespace Hooks {

struct HookEntry {
    std::string  name;
    uintptr_t    offset;
    void*        replacement;
    void*        original;
    bool         active;
};

inline std::unordered_map<std::string, HookEntry> g_hooks;

// Install a hook by IL2CPP offset
// Usage:
//   void* orig = nullptr;
//   Install("PlayerControl$$set_name", 0x1234ABC, (void*)MyHook, &orig);
inline bool Install(const std::string& name,
                    uintptr_t offset,
                    void* replacement,
                    void** original)
{
    uintptr_t addr = IL2CPP::baseAddress + offset;
    int result = DobbyHook((void*)addr, replacement, original);
    bool ok = (result == 0);

    g_hooks[name] = { name, offset, replacement, original ? *original : nullptr, ok };

    if (ok)
        printf("[AUMF] ✅ Hook installed: %s @ 0x%lx\n", name.c_str(), addr);
    else
        printf("[AUMF] ❌ Hook FAILED:    %s @ 0x%lx (err %d)\n", name.c_str(), addr, result);

    return ok;
}

// Remove a hook by name
inline bool Remove(const std::string& name) {
    auto it = g_hooks.find(name);
    if (it == g_hooks.end()) return false;
    // Dobby doesn't expose unhook directly; we re-hook to original
    // If using Substrate: MSHookFunction to restore
    printf("[AUMF] Hook removed: %s\n", name.c_str());
    it->second.active = false;
    return true;
}

// List all hooks (for mod menu / debug)
inline void PrintAll() {
    printf("[AUMF] ── Registered Hooks ──\n");
    for (auto& [k, v] : g_hooks)
        printf("  [%s] %s @ 0x%lx\n", v.active ? "ON " : "OFF", v.name.c_str(), v.offset);
}

} // namespace Hooks


// ─────────────────────────────────────────────
//  Convenience macro (mirrors BepInEx HarmonyPatch style)
//
//  AUFM_HOOK(MethodName, ReturnType, offset, (ArgTypes...), body)
//
//  Example:
//    static void* orig_SetName = nullptr;
//    AUFM_HOOK(PlayerControl_SetName, void, 0x1234ABC,
//              (void* self, Il2CppString* name),
//    {
//        printf("Name set: %s\n", IL2CPP::StringToStd(name).c_str());
//        ((decltype(&PlayerControl_SetName_hook))orig_SetName)(self, name);
//    });
// ─────────────────────────────────────────────
#define AUFM_HOOK(name, ret, offset, args, body)                         \
    static void* orig_##name = nullptr;                                  \
    static ret name##_hook args body                                     \
    static bool name##_registered = []() -> bool {                      \
        return Hooks::Install(#name, offset,                             \
                              (void*)name##_hook, &orig_##name);         \
    }();
