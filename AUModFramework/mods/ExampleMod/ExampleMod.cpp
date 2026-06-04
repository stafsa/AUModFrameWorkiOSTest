#include "../../API/ModAPI.hpp"
#include "../../API/Utilities.hpp"
#include <stdio.h>

class ExampleMod : public AUMFMod {
public:
    ModInfo GetInfo() const override {
        return {
            .id          = "com.aumf.examplemod",
            .name        = "ExampleMod",
            .version     = "1.0.0",
            .author      = "YourName",
            .description = "Demonstrates all AUMF hooks and APIs",
        };
    }

    void OnLoad() override {
        AUMF::Log("ExampleMod", "Loaded! Hello Among Us.");
    }

    void OnUnload() override {
        AUMF::Log("ExampleMod", "Unloaded.");
    }

    void OnGameStarted(const GameStartedEvent& e) override {
        AUMF::Log("ExampleMod", "Game started!");
        AUMF::ForEachPlayer([](PlayerControl* p) {
            std::string name = AUMF::GetPlayerName(p);
            std::string role = AUMF::RoleTypeName(p->GetData()->RoleType());
            printf("[ExampleMod] Player: %s | Role: %s | Dead: %s\n",
                   name.c_str(), role.c_str(), AUMF::IsDead(p) ? "yes" : "no");
        });
    }

    void OnGameEnded(const GameEndedEvent& e) override {
        AUMF::Log("ExampleMod", "Game ended.");
    }

    void OnPlayerSpawned(const PlayerSpawnedEvent& e) override {
        printf("[ExampleMod] Player spawned: %s\n", AUMF::GetPlayerName(e.player).c_str());
    }

    void OnPlayerDied(const PlayerDiedEvent& e) override {
        printf("[ExampleMod] Player died: %s\n", AUMF::GetPlayerName(e.player).c_str());
    }

    void OnMeetingStarted(const MeetingStartedEvent& e) override {
        AUMF::Log("ExampleMod", "Meeting started!");
        printf("[ExampleMod] Discussion timer: %.1f\n", e.hud->discussionTimer());
    }

    void OnMeetingEnded(const MeetingEndedEvent& e) override {
        AUMF::Log("ExampleMod", "Meeting ended.");
    }

    void OnChatMessage(ChatMessageEvent& e) override {
        printf("[ExampleMod] Chat from %s: %s\n",
               AUMF::GetPlayerName(e.sender).c_str(), e.message.c_str());

        // Simple mod command example
        if (e.message == "!players" && AUMF::IsLocalPlayer(e.sender)) {
            e.cancel = true;
            AUMF::ForEachPlayer([](PlayerControl* p) {
                printf("  -> %s (%s)\n",
                    AUMF::GetPlayerName(p).c_str(),
                    AUMF::RoleTypeName(p->GetData()->RoleType()).c_str());
            });
        }
    }

    void OnFixedUpdate(const FixedUpdateEvent& e) override {
        // Per-physics-frame logic goes here
    }

    void OnGuiDraw(const GuiDrawEvent& e) override {
        // Per-mod HUD overlay goes here
    }
};

AUMF_DECLARE_MOD(ExampleMod)
