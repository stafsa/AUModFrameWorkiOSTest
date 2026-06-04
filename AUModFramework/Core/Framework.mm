#import <Foundation/Foundation.h>
#include "IL2CPP.hpp"
#include "../Hooks/GameHooks.hpp"
#include "../ModLoader/ModLoader.hpp"
#include "../UI/ModMenu.mm"
#include <pthread.h>
#include <stdio.h>

// ═══════════════════════════════════════════════════════════════
//  AUMF Framework Entry Point
//  Injected via LiveContainer as a dylib
// ═══════════════════════════════════════════════════════════════

static void* AUMF_InitThread(void*) {
    // Wait for GameAssembly to be fully loaded
    usleep(2000000); // 2 second delay — adjust if hooks fail on slower devices

    printf("[AUMF] ══════════════════════════════════\n");
    printf("[AUMF]  Among Us Mod Framework v1.0.0\n");
    printf("[AUMF]  github.com/yourname/AUModFramework\n");
    printf("[AUMF] ══════════════════════════════════\n");

    // 1. Resolve GameAssembly base address
    IL2CPP::Init();
    if (IL2CPP::baseAddress == 0) {
        printf("[AUMF] ❌ FATAL: Could not find GameAssembly base address!\n");
        printf("[AUMF]    Make sure you are injecting into Among Us.\n");
        return nullptr;
    }
    printf("[AUMF] ✅ GameAssembly base: 0x%lx\n", IL2CPP::baseAddress);

    // 2. Scan and load mods from Documents/AUMF/Mods/
    ModLoader::ScanAndLoad();

    // 3. Install IL2CPP hooks
    GameHooks::InstallAll();

    // 4. Install the mod menu overlay (4-finger tap)
    [AUMFMenuOverlay install];

    printf("[AUMF] 🚀 Framework ready. %zu mod(s) loaded.\n",
           ModLoader::g_mods.size());
    return nullptr;
}

// ── dylib constructor — called automatically on injection ──────
__attribute__((constructor))
static void AUMF_Constructor() {
    pthread_t thread;
    pthread_create(&thread, nullptr, AUMF_InitThread, nullptr);
    pthread_detach(thread);
}

// ── dylib destructor ───────────────────────────────────────────
__attribute__((destructor))
static void AUMF_Destructor() {
    printf("[AUMF] Shutting down...\n");
    ModLoader::UnloadAll();
}
