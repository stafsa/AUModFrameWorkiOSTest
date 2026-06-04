#pragma once
#include <string>
#include <vector>
#include <memory>
#include <dlfcn.h>
#include <dirent.h>
#include <stdio.h>
#import <Foundation/Foundation.h>
#include "../API/ModAPI.hpp"

// ═══════════════════════════════════════════════════════════════
//  ModLoader — discovers, loads and manages AUMFMod instances
//
//  On a non-jailbroken device (LiveContainer injection),
//  NSHomeDirectory() returns Among Us's own sandbox container:
//    /var/mobile/Containers/Data/Application/<UUID>/
//
//  Mods go in:
//    <AppContainer>/Documents/AUMF/Mods/*.dylib
//
//  Users can access this via Files app → On My iPhone →
//  Among Us → AUMF → Mods
// ═══════════════════════════════════════════════════════════════

namespace ModLoader {

using CreateModFn  = AUMFMod*(*)();
using DestroyModFn = void(*)(AUMFMod*);

struct LoadedMod {
    void*         handle   = nullptr;
    AUMFMod*      instance = nullptr;
    DestroyModFn  destroy  = nullptr;
    std::string   path;
};

inline std::vector<LoadedMod> g_mods;

// ── Path helpers ──────────────────────────────
inline std::string GetModsDirectory() {
    // NSHomeDirectory() inside a sandboxed app = the app's own container.
    // No jailbreak or special entitlements needed.
    NSString* home = NSHomeDirectory();
    const char* cHome = home ? [home UTF8String] : getenv("HOME");
    if (!cHome) cHome = ".";
    return std::string(cHome) + "/Documents/AUMF/Mods";
}

// Auto-create the mods directory tree if it doesn't exist yet.
// Runs inside Among Us's own sandbox → always writable.
inline void EnsureModsDirectory() {
    std::string dir = GetModsDirectory();
    NSString* nsDir = [NSString stringWithUTF8String:dir.c_str()];
    NSError* err = nil;
    [[NSFileManager defaultManager]
        createDirectoryAtPath:nsDir
   withIntermediateDirectories:YES
                   attributes:nil
                        error:&err];
    if (err)
        printf("[AUMF] ⚠️  Could not create mods dir: %s\n",
               [[err localizedDescription] UTF8String]);
    else
        printf("[AUMF] 📂 Mods directory: %s\n", dir.c_str());
}

// ── Load a single .dylib mod ──────────────────
inline bool LoadMod(const std::string& path) {
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        printf("[AUMF] ❌ dlopen failed for %s: %s\n", path.c_str(), dlerror());
        return false;
    }

    auto createFn  = (CreateModFn) dlsym(handle, "AUMF_CreateMod");
    auto destroyFn = (DestroyModFn)dlsym(handle, "AUMF_DestroyMod");

    if (!createFn || !destroyFn) {
        printf("[AUMF] ❌ Not a valid AUMF mod (missing entry points): %s\n", path.c_str());
        dlclose(handle);
        return false;
    }

    AUMFMod* mod = createFn();
    if (!mod) {
        printf("[AUMF] ❌ AUMF_CreateMod returned null: %s\n", path.c_str());
        dlclose(handle);
        return false;
    }

    ModInfo info = mod->GetInfo();
    printf("[AUMF] ✅ Loaded mod: %s v%s by %s\n",
           info.name.c_str(), info.version.c_str(), info.author.c_str());

    mod->OnLoad();
    g_mods.push_back({ handle, mod, destroyFn, path });
    return true;
}

// ── Scan mods directory and load all .dylib files ──
inline void ScanAndLoad() {
    EnsureModsDirectory(); // creates the folder if missing

    std::string dir = GetModsDirectory();
    DIR* d = opendir(dir.c_str());
    if (!d) {
        printf("[AUMF] Mods directory unreadable — no mods loaded.\n");
        return;
    }

    struct dirent* entry;
    int count = 0;
    while ((entry = readdir(d)) != nullptr) {
        std::string name = entry->d_name;
        if (name.size() > 6 && name.substr(name.size()-6) == ".dylib") {
            LoadMod(dir + "/" + name);
            count++;
        }
    }
    closedir(d);

    if (count == 0)
        printf("[AUMF] No mods found in Documents/AUMF/Mods/ — drop .dylib files there.\n");
    else
        printf("[AUMF] Mod loading complete. %zu mod(s) active.\n", g_mods.size());
}

// ── Unload all mods ──
inline void UnloadAll() {
    for (auto& m : g_mods) {
        m.instance->OnUnload();
        m.destroy(m.instance);
        dlclose(m.handle);
    }
    g_mods.clear();
}

// ── Event dispatchers ─────────────────────────
#define DISPATCH(event_type, ...)                          \
    for (auto& m : g_mods) {                               \
        if (m.instance && m.instance->enabled)             \
            m.instance->On##event_type(__VA_ARGS__);       \
    }

inline void DispatchGameStarted  (const GameStartedEvent& e)   { DISPATCH(GameStarted,   e) }
inline void DispatchGameEnded    (const GameEndedEvent& e)      { DISPATCH(GameEnded,     e) }
inline void DispatchPlayerSpawned(const PlayerSpawnedEvent& e)  { DISPATCH(PlayerSpawned, e) }
inline void DispatchPlayerDied   (const PlayerDiedEvent& e)     { DISPATCH(PlayerDied,    e) }
inline void DispatchMeetingStart (const MeetingStartedEvent& e) { DISPATCH(MeetingStarted,e) }
inline void DispatchMeetingEnd   (const MeetingEndedEvent& e)   { DISPATCH(MeetingEnded,  e) }
inline void DispatchChatMessage  (ChatMessageEvent& e)          { DISPATCH(ChatMessage,   e) }
inline void DispatchFixedUpdate  (const FixedUpdateEvent& e)    { DISPATCH(FixedUpdate,   e) }
inline void DispatchGuiDraw      (const GuiDrawEvent& e)        { DISPATCH(GuiDraw,       e) }

#undef DISPATCH

} // namespace ModLoader
