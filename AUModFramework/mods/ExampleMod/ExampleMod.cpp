#include "../../API/ModAPI.hpp"
#include "../../API/Utilities.hpp"
#include <vector>
#include <string>
#include <imgui.h>

class ExampleMod : public AUMFMod {
public:
    ModInfo GetInfo() const override {
        return {
            .id          = "com.aumf.tabletmod",
            .name        = "Tablet Mod",
            .version     = "1.3.0",
            .author      = "Stafsa",
            .description = "A mod that adds a tablet to the game.",
        };
    }

private:
    bool showTablet = false;
    std::vector<std::string> logs;

    void AddLog(const std::string& msg) {
        logs.push_back(msg);
        if (logs.size() > 80) logs.erase(logs.begin());
    }

public:
    void OnLoad() override {
        AddLog("Tablet Mod loaded");
    }

    void OnGuiDraw(const GuiDrawEvent& e) override {
        if (AUMF::IsInLobby() || true) {
            ImGui::SetNextWindowPos(ImVec2(280, 920), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(60, 60));
            
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.9f)); 
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 1.0f, 1.0f));

            if (ImGui::Button("##TabletBtn", ImVec2(55, 55))) {
                showTablet = !showTablet;
            }

            ImGui::PopStyleColor(4);

            ImGui::SetCursorPos(ImVec2(295, 945));
            ImGui::TextColored(ImVec4(0,0,0,1), "📱");
        }

        if (showTablet) {
            ImGui::SetNextWindowSize(ImVec2(650, 550), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Tablet", &showTablet, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
                
                ImGui::Text("Tablet");
                ImGui::Separator();

                if (ImGui::Button("Clean the logs")) logs.clear();
                ImGui::SameLine();
                if (ImGui::Button("Close")) showTablet = false;

                ImGui::Separator();
                ImGui::Text("Aktive Players:");

                AUMF::ForEachPlayer([&](PlayerControl* p) {
                    std::string name = AUMF::GetPlayerName(p);
                    std::string role = AUMF::RoleTypeName(p->GetData()->RoleType());
                    bool dead = AUMF::IsDead(p);
                    ImGui::Text("%s  |  %s %s", name.c_str(), role.c_str(), dead ? "[D]" : "");
                });

                ImGui::Separator();
                ImGui::Text("Logs:");
                ImGui::BeginChild("LogArea", ImVec2(0, 300), true);
                for (const auto& log : logs) {
                    ImGui::TextWrapped("%s", log.c_str());
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }
    }

    // Diğer event'lere log ekleyelim
    void OnGameStarted(const GameStartedEvent& e) override { AddLog("The game has started."); }
    void OnPlayerSpawned(const PlayerSpawnedEvent& e) override { AddLog("Spawn: " + AUMF::GetPlayerName(e.player)); }
    void OnPlayerDied(const PlayerDiedEvent& e) override { AddLog("Died: " + AUMF::GetPlayerName(e.player)); }
    void OnChatMessage(ChatMessageEvent& e) override {
        AddLog(AUMF::GetPlayerName(e.sender) + ": " + e.message);
    }
};

AUMF_DECLARE_MOD(ExampleMod)
