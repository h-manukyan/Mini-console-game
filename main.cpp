#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <limits>
#include "SamiClasses.h"

using namespace std;

// ANSI Color Codes
const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN    = "\033[36m";

// Clear Screen Utility
void ClearScreen() {
    cout << "\033[2J\033[1;1H";
}

// Single ENTER press listener that avoids buffer traps
void WaitForKeypress() {
    cin.clear();
    // Flush any leftover input in the stream buffer
    while (cin.rdbuf()->in_avail() > 0) {
        cin.get();
    }
    cin.get();
}

// Save Game State to File
void SaveGame(const Sami& player, int currentRound, int hpLvl, int dmgLvl, int critLvl) {
    ofstream file("savegame.txt");
    if (file.is_open()) {
        file << currentRound << "\n";
        file << player.GetGold() << "\n";
        file << player.GetMaxHP() << "\n";
        file << player.GetStat("Damage") << "\n";
        file << player.GetStat("CritRate") << "\n";
        file << hpLvl << "\n";
        file << dmgLvl << "\n";
        file << critLvl << "\n";
        file.close();
    }
}

// Load Game State from File
bool LoadGame(Sami& player, int& currentRound, int& hpLvl, int& dmgLvl, int& critLvl) {
    ifstream file("savegame.txt");
    if (!file.is_open()) return false;

    int gold, maxHP, dmg, crit;
    if (file >> currentRound >> gold >> maxHP >> dmg >> crit >> hpLvl >> dmgLvl >> critLvl) {
        player.SetMaxHP(maxHP);
        player.SetStat("Damage", dmg);
        player.SetStat("CritRate", crit);
        player.AddGold(gold);
        file.close();
        return true;
    }
    file.close();
    return false;
}

// Visual HP Bar
string RenderBar(int current, int maxVal, const string& color) {
    int totalBlocks = 15;
    float percentage = static_cast<float>(current) / maxVal;
    int filledBlocks = static_cast<int>(percentage * totalBlocks);
    if (filledBlocks < 0) filledBlocks = 0;

    string bar = color + "[";
    for (int i = 0; i < totalBlocks; ++i) {
        if (i < filledBlocks) bar += "█";
        else bar += "░";
    }
    bar += "] " + to_string(current) + "/" + to_string(maxVal) + RESET;
    return bar;
}

void DisplayDice(int val) {
    cout << CYAN;
    switch (val) {
        case 1: cout << "┌───────┐\n│       │\n│   ●   │\n│       │\n└───────┘\n"; break;
        case 2: cout << "┌───────┐\n│ ●     │\n│       │\n│     ● │\n└───────┘\n"; break;
        case 3: cout << "┌───────┐\n│ ●     │\n│   ●   │\n│     ● │\n└───────┘\n"; break;
        case 4: cout << "┌───────┐\n│ ●   ● │\n│       │\n│ ●   ● │\n└───────┘\n"; break;
        case 5: cout << "┌───────┐\n│ ●   ● │\n│   ●   │\n│ ●   ● │\n└───────┘\n"; break;
        case 6: cout << "┌───────┐\n│ ●   ● │\n│ ●   ● │\n│ ●   ● │\n└───────┘\n"; break;
    }
    cout << RESET;
}

int RollDice() {
    return (rand() % 6) + 1;
}

// Enemy Creator
Enemy CreateScaledEnemy(int round, int wave, bool isBoss) {
    float scaleFactor = pow(1.15f, round - 1) * pow(1.08f, wave - 1);

    if (isBoss) {
        int hp = static_cast<int>(250 * scaleFactor * 2.5f);
        int damage = static_cast<int>(20 * scaleFactor * 1.5f);
        int crit = min(40, 15 + round);
        int armor = min(5, 3 + (round / 5));
        return Enemy(hp, damage, crit, armor);
    } else {
        int hp = static_cast<int>(40 * scaleFactor);
        int damage = static_cast<int>(8 * scaleFactor);
        int crit = 10;
        int armor = (wave >= 4) ? 3 : 2;
        return Enemy(hp, damage, crit, armor);
    }
}

// Upgrades Menu
void ShowUpgradeMenu(Sami& player, int& hpLevel, int& dmgLevel, int& critLevel, int currentRound) {
    while (true) {
        ClearScreen();
        int hpCost = 50 * hpLevel;
        int dmgCost = 40 * dmgLevel;
        int critCost = 60 * critLevel;

        cout << BOLD << YELLOW << "===================================================================\n";
        cout << "                       🛠️  SAMI'S ARMORY (UPGRADES)                 \n";
        cout << "===================================================================\n" << RESET;
        cout << "  Gold Balance: " << BOLD << YELLOW << player.GetGold() << " 🪙" << RESET << "\n\n";

        cout << "  [1] Upgrade Max HP (+25 HP)    - Level " << hpLevel << " | Cost: " << GREEN << hpCost << " Gold" << RESET << "\n";
        cout << "      Current Max HP: " << player.GetMaxHP() << "\n\n";

        cout << "  [2] Upgrade Base Damage (+5)  - Level " << dmgLevel << " | Cost: " << GREEN << dmgCost << " Gold" << RESET << "\n";
        cout << "      Current Damage: " << player.GetStat("Damage") << "\n\n";

        cout << "  [3] Upgrade Crit Rate (+5%)   - Level " << critLevel << " | Cost: " << GREEN << critCost << " Gold" << RESET << "\n";
        cout << "      Current Crit Rate: " << player.GetStat("CritRate") << "%\n\n";

        cout << "  [4] Return to Raid Hub\n";
        cout << "===================================================================\n";
        cout << "Choose an option (1-4): ";

        int choice = 0;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) {
            if (player.SpendGold(hpCost)) {
                player.SetMaxHP(player.GetMaxHP() + 25);
                hpLevel++;
                SaveGame(player, currentRound, hpLevel, dmgLevel, critLevel);
                cout << GREEN << "\n✔️ Max HP Upgraded!" << RESET;
            } else cout << RED << "\n❌ Not enough gold!" << RESET;
        } else if (choice == 2) {
            if (player.SpendGold(dmgCost)) {
                player.SetStat("Damage", player.GetStat("Damage") + 5);
                dmgLevel++;
                SaveGame(player, currentRound, hpLevel, dmgLevel, critLevel);
                cout << GREEN << "\n✔️ Damage Upgraded!" << RESET;
            } else cout << RED << "\n❌ Not enough gold!" << RESET;
        } else if (choice == 3) {
            if (player.SpendGold(critCost)) {
                if (player.GetStat("CritRate") < 75) {
                    player.SetStat("CritRate", player.GetStat("CritRate") + 5);
                    critLevel++;
                    SaveGame(player, currentRound, hpLevel, dmgLevel, critLevel);
                    cout << GREEN << "\n✔️ Crit Rate Upgraded!" << RESET;
                } else cout << RED << "\n❌ Crit Rate is capped at 75%!" << RESET;
            } else cout << RED << "\n❌ Not enough gold!" << RESET;
        } else if (choice == 4) {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(800));
    }
}

// Single Battle execution
bool ExecuteBattle(Sami& player, Enemy& monster, string monsterTitle, int round, int wave, int totalWaves) {
    int maxMonsterHP = monster.GetStat("Health");

    while (player.IsAlive() && monster.IsAlive()) {
        ClearScreen();
        cout << BOLD << MAGENTA << "═══════════════════════════════════════════════════════════════════\n" << RESET;
        cout << BOLD << CYAN << "  ROUND " << round << " | WAVE " << wave << "/" << totalWaves << RESET
             << " | Gold: " << YELLOW << player.GetGold() << " 🪙" << RESET << "\n";
        cout << BOLD << MAGENTA << "───────────────────────────────────────────────────────────────────\n" << RESET;
        cout << BOLD << GREEN << "  [PLAYER] SAMI" << RESET << "\n";
        cout << "  HP:     " << RenderBar(player.GetStat("Health"), player.GetMaxHP(), GREEN) << "\n";
        cout << "  Damage: " << player.GetStat("Damage") << " | Crit: " << player.GetStat("CritRate") << "%\n";
        cout << BOLD << MAGENTA << "───────────────────────────────────────────────────────────────────\n" << RESET;
        cout << BOLD << RED << "  [FOE] " << monsterTitle << RESET << "\n";
        cout << "  HP:     " << RenderBar(monster.GetStat("Health"), maxMonsterHP, RED) << "\n";
        cout << "  Armor:  " << BOLD << YELLOW << monster.GetStat("MinRollToHit") << "+" << RESET << " Roll Required | Damage: " << monster.GetStat("Damage") << "\n";
        cout << BOLD << MAGENTA << "═══════════════════════════════════════════════════════════════════\n\n" << RESET;

        cout << BOLD << "Press [ENTER] to roll the dice and attack! " << RESET;
        WaitForKeypress();

        // --- PLAYER TURN ---
        cout << YELLOW << "\n🎲 Rolling..." << RESET << "\n";
        this_thread::sleep_for(chrono::milliseconds(300));

        int pRoll = RollDice();
        DisplayDice(pRoll);

        int minReq = monster.GetStat("MinRollToHit");
        if (pRoll >= minReq) {
            bool isCrit = ((rand() % 100) + 1) <= player.GetStat("CritRate");
            int dmg = isCrit ? static_cast<int>(player.GetStat("Damage") * 1.5f) : player.GetStat("Damage");

            monster.TakeDamage(dmg);
            cout << GREEN << "💥 HIT! Rolled " << pRoll << " (" << minReq << "+ needed) -> Dealt " << dmg << " DMG!";
            if (isCrit) cout << BOLD << RED << " 🔥 CRIT!" << RESET;
            cout << "\n" << RESET;
        } else {
            cout << RED << "🛡️ MISSED! Rolled " << pRoll << " (Needed " << minReq << "+)!\n" << RESET;
        }

        if (!monster.IsAlive()) break;

        // --- ENEMY TURN ---
        cout << "\n" << RED << "⚔️ " << monsterTitle << " counters..." << RESET << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));

        int eRoll = RollDice();
        if (eRoll >= player.GetStat("MinRollToHit")) {
            bool isCrit = ((rand() % 100) + 1) <= monster.GetStat("CritRate");
            int dmg = isCrit ? static_cast<int>(monster.GetStat("Damage") * 1.5f) : monster.GetStat("Damage");

            player.TakeDamage(dmg);
            cout << RED << "💥 " << monsterTitle << " hit you for " << dmg << " DMG!\n" << RESET;
        } else {
            cout << GREEN << "💨 " << monsterTitle << " missed!\n" << RESET;
        }

        this_thread::sleep_for(chrono::milliseconds(800));
    }

    return player.IsAlive();
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    Sami player(100, 50, 20, 15, 1);
    int currentRound = 1;
    int hpLvl = 1, dmgLvl = 1, critLvl = 1;

    // Load progress automatically if savegame.txt exists
    if (LoadGame(player, currentRound, hpLvl, dmgLvl, critLvl)) {
        cout << GREEN << "Loaded saved game progress successfully!\n" << RESET;
        this_thread::sleep_for(chrono::milliseconds(800));
    }

    while (true) {
        ClearScreen();
        cout << BOLD << YELLOW;
        cout << "===================================================================\n";
        cout << "                  ⚔️   SAMI'S RAID HUB   ⚔️                        \n";
        cout << "===================================================================\n" << RESET;
        cout << "  Current Round: " << BOLD << CYAN << currentRound << RESET << "\n";
        cout << "  Gold: " << BOLD << YELLOW << player.GetGold() << " 🪙" << RESET << "\n";
        cout << "  Stats: " << GREEN << player.GetMaxHP() << " HP" << RESET
             << " | " << RED << player.GetStat("Damage") << " DMG" << RESET
             << " | " << CYAN << player.GetStat("CritRate") << "% CRIT" << RESET << "\n\n";

        cout << "  [1] Start Raid Round " << currentRound << "\n";
        cout << "  [2] Enter Armory (Upgrades)\n";
        cout << "  [3] Exit Game\n";
        cout << "===================================================================\n";
        cout << "Select (1-3): ";

        int option = 0;
        if (!(cin >> option)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (option == 2) {
            ShowUpgradeMenu(player, hpLvl, dmgLvl, critLvl, currentRound);
        }
        else if (option == 3) {
            SaveGame(player, currentRound, hpLvl, dmgLvl, critLvl);
            cout << GREEN << "\nProgress saved! Thanks for playing!\n" << RESET;
            break;
        }
        else if (option == 1) {
            int totalWaves = 6;
            if (currentRound % 5 == 0 && currentRound % 10 != 0) totalWaves = 10;
            else if (currentRound % 10 == 0) totalWaves = 1;

            bool roundPassed = true;
            player.HealFull();

            for (int wave = 1; wave <= totalWaves; ++wave) {
                bool isBoss = (currentRound % 10 == 0) || (wave == totalWaves && totalWaves == 10);
                string title = isBoss ? ("👹 GRAND BOSS (R" + to_string(currentRound) + ")")
                                      : ("👾 Monster #" + to_string(wave));

                Enemy foe = CreateScaledEnemy(currentRound, wave, isBoss);

                bool win = ExecuteBattle(player, foe, title, currentRound, wave, totalWaves);

                if (!win) {
                    roundPassed = false;
                    break;
                }

                int reward = static_cast<int>((isBoss ? 150 : 20) * pow(1.18f, currentRound - 1));
                player.AddGold(reward);
                SaveGame(player, currentRound, hpLvl, dmgLvl, critLvl);

                ClearScreen();
                cout << BOLD << GREEN << "\n✨ WAVE " << wave << " CLEARED! ✨\n" << RESET;
                cout << "Earned: " << YELLOW << reward << " Gold 🪙" << RESET << "\n";
                cout << "Current HP: " << player.GetStat("Health") << "/" << player.GetMaxHP() << "\n\n";

                if (wave < totalWaves) {
                    cout << "Press [ENTER] to advance to Wave " << (wave + 1) << "...";
                    WaitForKeypress();
                }
            }

            if (roundPassed) {
                int roundBonus = static_cast<int>(100 * pow(1.2f, currentRound - 1));
                player.AddGold(roundBonus);
                currentRound++;
                SaveGame(player, currentRound, hpLvl, dmgLvl, critLvl);

                ClearScreen();
                cout << BOLD << YELLOW << "\n🎉🎉 ROUND " << (currentRound - 1) << " CLEARED! 🎉🎉\n" << RESET;
                cout << "Round Completion Bonus: " << YELLOW << roundBonus << " Gold 🪙\n" << RESET;
                cout << "\nPress [ENTER] to return to Hub...";
                WaitForKeypress();
            } else {
                SaveGame(player, currentRound, hpLvl, dmgLvl, critLvl);
                ClearScreen();
                cout << BOLD << RED << "\n💀 YOU DIED IN RAID ROUND " << currentRound << "! 💀\n" << RESET;
                cout << "You kept your earned Gold, but must retry Round " << currentRound << ".\n";
                cout << "\nPress [ENTER] to return to Hub...";
                WaitForKeypress();
            }
        }
    }

    return 0;
}
