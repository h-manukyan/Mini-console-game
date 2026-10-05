# ⚔️ Sami's Raid Arena

A modern, turn-based C++ console RPG featuring dice-roll combat mechanics, incremental monster scaling, an armory upgrade system, and persistent save data.

---

## 🌟 Key Features

* **🎲 Dice-Based Combat:** Attacks are resolved using a simulated 6-sided dice roll. Foes feature an **Armor Defense** stat—rolling below their required threshold results in a missed attack!
* **🌊 Wave-Based Raid System:** 
  * Standard Rounds consist of **6 waves** of scaling monsters.
  * Every 5th Round increases the challenge to **10 waves**.
  * Every 10th Round is a dedicated **Grand Boss Battle**.
* **🛠️ Armory & Incremental Upgrades:** Earn Gold from defeating monsters and completing rounds. Spend Gold in the shop to permanently upgrade **Max HP**, **Base Damage**, and **Critical Hit Rate**.
* **📈 Exponential Enemy Scaling:** Foes dynamically scale in Health, Damage, and Armor across rounds and waves using exponential growth formulas.
* **💾 Automatic Save/Load System:** Game state (Gold, Round progress, and Upgrades) automatically persists to `savegame.txt` after every wave, upgrade, or exit.
* **🎨 Clean Terminal UI:** Formatted with ANSI colors, graphical HP bars, visual ASCII dice, and custom HUD banners.

---

## 📂 Project Structure

```text
├── SamiClasses.h    # Base Entity, Enemy, and Sami player class definitions
├── main.cpp         # Game loop, battle engine, UI renderer, and save system
├── savegame.txt     # Auto-generated save file storing player state
└── README.md        # Project documentation
