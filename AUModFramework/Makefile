# ═══════════════════════════════════════════════════════════════
#  AUMF Makefile — builds the framework dylib + ExampleMod dylib
#  Requirements:
#    - Xcode + iOS SDK (on macOS)
#    - Dobby (https://github.com/jmpews/Dobby) — for hooking
#    - Theos (optional, for .deb packaging)
#
#  Usage:
#    make framework     → builds AUModFramework.dylib
#    make examplemod    → builds ExampleMod.dylib
#    make all           → builds both
#    make clean         → remove build artifacts
# ═══════════════════════════════════════════════════════════════

# ── Configuration ───────────────────────────────────────────────
IOS_SDK      ?= $(shell xcrun --sdk iphoneos --show-sdk-path 2>/dev/null)
ARCH         ?= arm64
MIN_IOS      ?= 14.0
CC           := $(shell xcrun -sdk iphoneos -find clang)
CXX          := $(shell xcrun -sdk iphoneos -find clang++)

# Dobby — clone and build from https://github.com/jmpews/Dobby
# Then set DOBBY_DIR to your Dobby build output
DOBBY_DIR    ?= $(HOME)/Dobby/build

# ── Flags ───────────────────────────────────────────────────────
COMMON_FLAGS := \
    -arch $(ARCH) \
    -isysroot $(IOS_SDK) \
    -miphoneos-version-min=$(MIN_IOS) \
    -fobjc-arc \
    -fvisibility=hidden \
    -O2 \
    -std=c++17 \
    -I$(DOBBY_DIR)/include \
    -DNDEBUG

LDFLAGS := \
    -arch $(ARCH) \
    -isysroot $(IOS_SDK) \
    -miphoneos-version-min=$(MIN_IOS) \
    -dynamiclib \
    -framework Foundation \
    -framework UIKit \
    -lobjc \
    -L$(DOBBY_DIR) -ldobby \
    -Wl,-rpath,@loader_path

# ── Targets ─────────────────────────────────────────────────────
.PHONY: all framework examplemod clean

all: framework examplemod

# Framework dylib (the loader + hooks)
framework: build/AUModFramework.dylib

FRAMEWORK_SRCS := Core/Framework.mm

build/AUModFramework.dylib: $(FRAMEWORK_SRCS) | build
	$(CXX) $(COMMON_FLAGS) $(LDFLAGS) \
	    -install_name @rpath/AUModFramework.dylib \
	    -o $@ $^
	$(shell xcrun -sdk iphoneos -find codesign_allocate) 2>/dev/null || true
	@echo "✅ Framework built: $@"

# ExampleMod dylib
examplemod: build/ExampleMod.dylib

build/ExampleMod.dylib: mods/ExampleMod/ExampleMod.cpp | build
	$(CXX) $(COMMON_FLAGS) $(LDFLAGS) \
	    -install_name @rpath/ExampleMod.dylib \
	    -o $@ $^
	@echo "✅ ExampleMod built: $@"

build:
	@mkdir -p build

clean:
	@rm -rf build/
	@echo "Cleaned."

# ── Install to device via SSH (optional) ────────────────────────
DEVICE_IP ?= 192.168.1.x

install: all
	ssh root@$(DEVICE_IP) "mkdir -p ~/Documents/AUMF/Mods"
	scp build/AUModFramework.dylib root@$(DEVICE_IP):~/Documents/AUMF/
	scp build/ExampleMod.dylib     root@$(DEVICE_IP):~/Documents/AUMF/Mods/
	@echo "✅ Installed to device."
