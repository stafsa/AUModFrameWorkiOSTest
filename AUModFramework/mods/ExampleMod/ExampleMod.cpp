#include "../../API/ModAPI.hpp"
#include "../../API/Utilities.hpp"
#include <vector>
#include <string>

class ExampleMod : public AUMFMod {
public:
    ModInfo GetInfo() const override {
        return {
            .id          = "com.aumf.tabletmod",
            .name        = "Tablet Mod",
            .version     = "1.4.0",
            .author      = "YourName",
            .description = "Tablet panel with lobby button support (ImGui-free)",
        };
    }

private:
    bool showTablet = false;
    std::vector<std::string> logs;

    void AddLog(const std::string& msg) {
        logs.push_back(msg);
        if (logs.size() > 100) logs.erase(logs.begin());
    }

public:
    void OnLoad() override {
        AddLog("Tablet Mod loaded successfully!");
        AUMF::Log(GetInfo().name, "Ready. Type !tablet in chat to open.");
    }

    void OnUnload() override {
        AddLog("Tablet Mod unloaded.");
    }

    void OnGameStarted(const GameStartedEvent& e) override {
        AddLog("Game Started!");
    }

    void OnPlayerSpawned(const PlayerSpawnedEvent& e) override {
        AddLog("Player Spawned: " + AUMF::GetPlayerName(e.player));
    }

    void OnPlayerDied(const PlayerDiedEvent& e) override {
        AddLog("Player Died: " + AUMF::GetPlayerName(e.player));
    }

    void OnMeetingStarted(const MeetingStartedEvent& e) override {
        AddLog("Meeting Started!");
    }

    void OnChatMessage(ChatMessageEvent& e) override {
        std::string logLine = AUMF::GetPlayerName(e.sender) + ": " + e.message;
        AddLog(logLine);

        // Tablet toggle command
        if (e.message == "!tablet" && AUMF::IsLocalPlayer(e.sender)) {
            e.cancel = true;
            showTablet = !showTablet;
            AUMF::Log("TabletMod", showTablet ? "Tablet opened" : "Tablet closed");
        }

        // Players list command
        if (e.message == "!players" && AUMF::IsLocalPlayer(e.sender)) {
            e.cancel = true;
            AddLog("--- Player List ---");
            AUMF::ForEachPlayer([&](PlayerControl* p) {
                std::string name = AUMF::GetPlayerName(p);
                std::string role = AUMF::RoleTypeName(p->GetData()->RoleType());
                bool dead = AUMF::IsDead(p);
                AddLog("  " + name + " | " + role + (dead ? " [DEAD]" : ""));
            });
        }
    }

    void OnGuiDraw(const GuiDrawEvent& e) override {
        // TODO: Simple white square button will be added once ImGui is integrated
        // For now we use chat command !tablet
    }
};

AUMF_DECLARE_MOD(ExampleMod)
