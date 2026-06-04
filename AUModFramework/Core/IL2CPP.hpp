#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <functional>
#include <dlfcn.h>
#include <mach-o/dyld.h>

// ─────────────────────────────────────────────
//  IL2CPP Type Aliases
// ─────────────────────────────────────────────
struct Il2CppObject { void* klass; void* monitor; };
struct Il2CppString { Il2CppObject obj; int32_t length; uint16_t chars[1]; };
struct Il2CppArray  { Il2CppObject obj; void* bounds; uint32_t max_length; void* vector[1]; };
struct Il2CppClass;
struct Il2CppDomain;
struct Il2CppAssembly;
struct Il2CppImage;
struct Il2CppMethod;
struct Il2CppFieldInfo;

// ─────────────────────────────────────────────
//  il2cpp_api function pointers
// ─────────────────────────────────────────────
namespace IL2CPP {

// Base address of the Among Us binary (GameAssembly.dylib)
inline uintptr_t baseAddress = 0;

inline void Init() {
    for (uint32_t i = 0; i < _dyld_image_count(); i++) {
        std::string name = _dyld_get_image_name(i);
        if (name.find("GameAssembly") != std::string::npos ||
            name.find("UnityFramework") != std::string::npos) {
            baseAddress = (uintptr_t)_dyld_get_image_vmaddr_slide(i);
            // Try absolute base
            if (baseAddress == 0)
                baseAddress = (uintptr_t)_dyld_get_image_header(i);
            break;
        }
    }
}

// Resolve a function at a static offset from the binary base
template<typename Fn>
inline Fn ResolveOffset(uintptr_t offset) {
    return reinterpret_cast<Fn>(baseAddress + offset);
}

// Resolve via exported symbol name (fallback)
template<typename Fn>
inline Fn ResolveSymbol(const char* symbol) {
    void* handle = dlopen(nullptr, RTLD_NOW);
    return reinterpret_cast<Fn>(dlsym(handle, symbol));
}

// ─────────────────────────────────────────────
//  String helpers
// ─────────────────────────────────────────────
inline std::string StringToStd(Il2CppString* str) {
    if (!str) return "";
    std::string result;
    for (int i = 0; i < str->length; i++)
        result += (char)str->chars[i];
    return result;
}

inline Il2CppString* NewString(const char* text) {
    // il2cpp_string_new offset — replace with value from dump
    using il2cpp_string_new_t = Il2CppString*(*)(const char*);
    static auto fn = ResolveOffset<il2cpp_string_new_t>(0x0 /* TODO: il2cpp_string_new offset */);
    return fn(text);
}

} // namespace IL2CPP
