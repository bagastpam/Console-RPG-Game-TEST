/*
================================================================
PROGRAM: TEXT RPG GAME - LEGEND OF THE VOID (DELUXE EDITION)
================================================================
*/

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <limits>
#include <map>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

using namespace std;

class Game;

struct Equipment {
    string nama;
    string tipe;
    int attackBonus;
    int defenseBonus;
    int hpBonus;
    int harga;
    int levelRequirement;
};

struct Skill {
    string nama;
    string deskripsi;
    int mpCost;
    int damageMultiplier;
    int cooldown;
    int currentCooldown;
    string elemen;
};

struct Item {
    string nama;
    string deskripsi;
    string tipe;
    int efekHp;
    int efekMp;
    int efekAttack;
    int efekDefense;
    int harga;
    int quantity;
};

struct Quest {
    string nama;
    string deskripsi;
    string targetMusuh;
    int targetKill;
    int currentKill;
    int expReward;
    int goldReward;
    Item itemReward;
    bool completed;
    bool active;
};

struct Musuh {
    string nama;
    string tipe;
    int hp;
    int hpMax;
    int mp;
    int attack;
    int defense;
    int magic;
    int speed;
    int expReward;
    int goldReward;
    vector<Skill> skills;
    string dropItem;
    int dropChance;
};

struct Karakter {
    string nama;
    string kelas;
    int level;
    int exp;
    int expToNext;
    int hp;
    int hpMax;
    int mp;
    int mpMax;
    int attack;
    int defense;
    int magic;
    int speed;
    int critRate;
    int critDamage;
    int gold;
    int skillPoints;
    
    vector<Skill> skills;
    vector<Item> inventory;
    Equipment weapon;
    Equipment armor;
    Equipment accessory;
    vector<Quest> quests;
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitEnter() {
    cout << "\nTekan Enter untuk melanjutkan...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int randomRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

string formatNumber(int num) {
    string str = to_string(num);
    int insertPosition = (int)str.length() - 3;
    while (insertPosition > 0) {
        str.insert(insertPosition, ".");
        insertPosition -= 3;
    }
    return str;
}

void delay(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

vector<Skill> getSkillsForClass(string kelas, int level) {
    vector<Skill> skills;
    
    if (kelas == "Warrior") {
        skills.push_back({"Slash", "Serangan fisik dasar", 0, 120, 0, 0, "physical"});
        if (level >= 5)  skills.push_back({"Power Strike", "Serangan kuat 200%", 15, 200, 2, 0, "physical"});
        if (level >= 10) skills.push_back({"Whirlwind", "Serangan area 150%", 25, 150, 3, 0, "physical"});
        if (level >= 15) skills.push_back({"Berserker", "Attack +50% 3 turn", 30, 0, 5, 0, "buff"});
        if (level >= 20) skills.push_back({"Mortal Blow", "Serangan fatal 300%", 40, 300, 4, 0, "physical"});
    } else if (kelas == "Mage") {
        skills.push_back({"Fireball", "Serangan api 130%", 10, 130, 0, 0, "fire"});
        if (level >= 5)  skills.push_back({"Ice Lance", "Serangan es 150%", 15, 150, 2, 0, "ice"});
        if (level >= 10) skills.push_back({"Thunder", "Serangan petir 180%", 20, 180, 2, 0, "thunder"});
        if (level >= 15) skills.push_back({"Meteor", "Serangan area 250%", 35, 250, 4, 0, "fire"});
        if (level >= 20) skills.push_back({"Ancient Magic", "Ultimate 400%", 50, 400, 5, 0, "magic"});
    } else if (kelas == "Archer") {
        skills.push_back({"Arrow Shot", "Serangan jarak jauh", 0, 115, 0, 0, "physical"});
        if (level >= 5)  skills.push_back({"Multi Arrow", "Serangan 3x 80%", 15, 80, 2, 0, "physical"});
        if (level >= 10) skills.push_back({"Piercing Arrow", "Abaikan 50% defense", 20, 170, 3, 0, "physical"});
        if (level >= 15) skills.push_back({"Rain of Arrows", "Serangan area 140%", 25, 140, 3, 0, "physical"});
        if (level >= 20) skills.push_back({"Sniper Mode", "Crit +50% 3 turn", 30, 0, 5, 0, "buff"});
    } else if (kelas == "Assassin") {
        skills.push_back({"Quick Strike", "Serangan cepat", 0, 110, 0, 0, "physical"});
        if (level >= 5)  skills.push_back({"Poison Stab", "Racun 3 turn", 12, 130, 2, 0, "poison"});
        if (level >= 10) skills.push_back({"Shadow Step", "Hindari & counter", 20, 0, 4, 0, "buff"});
        if (level >= 15) skills.push_back({"Assassinate", "Crit dmg +100%", 25, 200, 3, 0, "physical"});
        if (level >= 20) skills.push_back({"Death Mark", "Damage +100%", 35, 250, 5, 0, "physical"});
    }
    
    return skills;
}

Musuh generateMusuh(int playerLevel, string tipe = "normal") {
    Musuh musuh;

    // Nama musuh berdasarkan tier level
    vector<string> normalLow  = {"Goblin", "Tikus Raksasa", "Bandit"};
    vector<string> normalMid  = {"Orc", "Skeleton", "Harpy", "Dark Elf"};
    vector<string> normalHigh = {"Minotaur", "Serigala Iblis", "Golem Batu", "Prajurit Neraka"};
    vector<string> eliteMusuh = {"Orc Warlord", "Shadow Knight", "Ancient Treant",
                                  "Ice Wyvern", "Flame Demon", "Thunder Bird",
                                  "Dark Paladin", "Bone Dragon", "Void Stalker"};
    vector<string> bossMusuh  = {"Dragon Lord", "Lich King", "Demon Overlord",
                                  "Ancient Guardian", "Void Entity", "Chaos Titan",
                                  "Abyssal Hydra", "Fallen Archangel", "Death Emperor"};

    // Scaling eksponensial di level tinggi
    double scale = 1.0 + (playerLevel / 10.0) * (playerLevel / 10.0) * 0.5;

    if (tipe == "normal") {
        vector<string>& pool = (playerLevel < 8) ? normalLow
                             : (playerLevel < 18) ? normalMid
                             : normalHigh;
        musuh.nama = pool[randomRange(0, (int)pool.size() - 1)];

        musuh.hp      = (int)((60  + playerLevel * 18)  * scale) + randomRange(-10, 20);
        musuh.attack  = (int)((14  + playerLevel * 6)   * scale) + randomRange(-3, 6);
        musuh.defense = (int)((6   + playerLevel * 3.5) * scale) + randomRange(-2, 4);
        musuh.magic   = (int)((8   + playerLevel * 4)   * scale) + randomRange(-3, 5);
        musuh.speed   = (int)((10  + playerLevel * 2.5) * scale) + randomRange(-2, 4);
        musuh.expReward  = (int)((50  + playerLevel * 12) * scale);
        musuh.goldReward = (int)((35  + playerLevel * 8)  * scale);
        musuh.dropChance = 30;

        // Di level tinggi normal musuh punya skill dasar
        if (playerLevel >= 10)
            musuh.skills.push_back({"Heavy Blow", "Serangan keras", 0, 150, 2, 0, "physical"});
        if (playerLevel >= 20)
            musuh.skills.push_back({"Frenzy", "Serangan brutal", 0, 200, 3, 0, "physical"});

    } else if (tipe == "elite") {
        musuh.nama = eliteMusuh[randomRange(0, (int)eliteMusuh.size() - 1)];

        musuh.hp      = (int)((150 + playerLevel * 30)  * scale) + randomRange(-20, 35);
        musuh.attack  = (int)((28  + playerLevel * 9)   * scale) + randomRange(-5, 10);
        musuh.defense = (int)((15  + playerLevel * 5.5) * scale) + randomRange(-4, 8);
        musuh.magic   = (int)((18  + playerLevel * 7)   * scale) + randomRange(-5, 9);
        musuh.speed   = (int)((20  + playerLevel * 4)   * scale) + randomRange(-4, 7);
        musuh.expReward  = (int)((150 + playerLevel * 22) * scale);
        musuh.goldReward = (int)((120 + playerLevel * 16) * scale);
        musuh.dropChance = 55;

        musuh.skills.push_back({"Power Strike",  "Serangan kuat",     0,  190, 2, 0, "physical"});
        musuh.skills.push_back({"Armor Break",   "Melemahkan defense",0,  140, 3, 0, "physical"});
        if (playerLevel >= 15)
            musuh.skills.push_back({"Elemental Rage", "Serangan elemen", 0, 230, 3, 0, "fire"});

    } else { // boss
        musuh.nama = bossMusuh[randomRange(0, (int)bossMusuh.size() - 1)];

        musuh.hp      = (int)((400 + playerLevel * 60)  * scale) + randomRange(-30, 60);
        musuh.attack  = (int)((50  + playerLevel * 14)  * scale) + randomRange(-8, 15);
        musuh.defense = (int)((30  + playerLevel * 8)   * scale) + randomRange(-6, 12);
        musuh.magic   = (int)((35  + playerLevel * 11)  * scale) + randomRange(-8, 14);
        musuh.speed   = (int)((28  + playerLevel * 5)   * scale) + randomRange(-6, 10);
        musuh.expReward  = (int)((700  + playerLevel * 50) * scale);
        musuh.goldReward = (int)((500  + playerLevel * 35) * scale);
        musuh.dropChance = 100;

        musuh.skills.push_back({"Mighty Roar",   "Serangan area masif",  0,  220, 2, 0, "physical"});
        musuh.skills.push_back({"Dark Blast",     "Ledakan gelap",       20,  280, 3, 0, "dark"});
        musuh.skills.push_back({"Devastate",      "Serangan ultimate",    0,  350, 4, 0, "physical"});
        if (playerLevel >= 10)
            musuh.skills.push_back({"Soul Drain","Serap jiwa musuh",     0,  180, 2, 0, "dark"});
        if (playerLevel >= 20)
            musuh.skills.push_back({"Apocalypse", "Kehancuran total",    0,  450, 5, 0, "dark"});
    }

    musuh.hpMax = musuh.hp;
    musuh.mp    = 80 + playerLevel * 8;
    musuh.tipe  = tipe;
    
    vector<string> drops = {"Potion", "Elixir", "Material Shard", "Magic Crystal", "Rare Equipment"};
    musuh.dropItem = drops[randomRange(0, (int)drops.size() - 1)];
    
    return musuh;
}

Equipment getRandomEquipment(int playerLevel) {
    Equipment equip;
    
    vector<string> tipeList = {"weapon", "armor", "accessory"};
    vector<string> weaponNames = {"Iron Sword", "Steel Blade", "Dragon Fang", "Mystic Staff", "Shadow Dagger"};
    vector<string> armorNames = {"Leather Armor", "Iron Plate", "Mystic Robe", "Dragon Scale", "Shadow Cloak"};
    vector<string> accessoryNames = {"Ring of Power", "Amulet of Health", "Crystal of Magic", "Talisman of Speed", "Gem of Defense"};
    
    equip.tipe = tipeList[randomRange(0, 2)];
    
    if (equip.tipe == "weapon") {
        equip.nama = weaponNames[randomRange(0, (int)weaponNames.size() - 1)];
        equip.attackBonus = 10 + (playerLevel * 3) + randomRange(-5, 10);
        equip.defenseBonus = 0;
        equip.hpBonus = 0;
    } else if (equip.tipe == "armor") {
        equip.nama = armorNames[randomRange(0, (int)armorNames.size() - 1)];
        equip.attackBonus = 0;
        equip.defenseBonus = 8 + (playerLevel * 2) + randomRange(-4, 8);
        equip.hpBonus = 20 + (playerLevel * 5);
    } else {
        equip.nama = accessoryNames[randomRange(0, (int)accessoryNames.size() - 1)];
        equip.attackBonus = 5 + (playerLevel * 2);
        equip.defenseBonus = 5 + (playerLevel * 2);
        equip.hpBonus = 15 + (playerLevel * 3);
    }
    
    equip.harga = 100 + (playerLevel * 20);
    equip.levelRequirement = max(1, playerLevel - 2);
    
    return equip;
}

// ================== KARAKTER MANAGER ==================

class KarakterManager {
private:
    Karakter player;
    
public:
    Karakter initKarakter() {
        clearScreen();
        cout << "+----------------------------------------------------------+" << endl;
        cout << "|                 TEXT RPG GAME - DELUXE EDITION           |" << endl;
        cout << "|                     LEGEND OF THE VOID                   |" << endl;
        cout << "+----------------------------------------------------------+" << endl;
        
        cout << "\nMasukkan nama karaktermu: ";
        cin >> player.nama;
        
        cout << "\nPilih Kelas:" << endl;
        cout << "+------------------------------------------------+" << endl;
        cout << "| 1. WARRIOR  - HP tinggi, serangan fisik kuat   |" << endl;
        cout << "| 2. MAGE     - MP tinggi, sihir elementalis     |" << endl;
        cout << "| 3. ARCHER   - Critical tinggi, serangan jarak  |" << endl;
        cout << "| 4. ASSASSIN - Speed tinggi, damage mematikan   |" << endl;
        cout << "+------------------------------------------------+" << endl;
        
        int kelasChoice = 0;
        cout << "\nPilih (1-4): ";
        while (!(cin >> kelasChoice) || kelasChoice < 1 || kelasChoice > 4) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Pilihan tidak valid! Masukkan 1-4: ";
        }
        
        switch(kelasChoice) {
            case 1:
                player.kelas = "Warrior";
                player.hpMax = 150; player.mpMax = 50;
                player.attack = 25; player.defense = 15;
                player.magic = 5;   player.speed = 10;
                player.critRate = 10;
                break;
            case 2:
                player.kelas = "Mage";
                player.hpMax = 90;  player.mpMax = 120;
                player.attack = 10; player.defense = 8;
                player.magic = 28;  player.speed = 12;
                player.critRate = 15;
                break;
            case 3:
                player.kelas = "Archer";
                player.hpMax = 110; player.mpMax = 80;
                player.attack = 22; player.defense = 10;
                player.magic = 12;  player.speed = 18;
                player.critRate = 25;
                break;
            default:
                player.kelas = "Assassin";
                player.hpMax = 100; player.mpMax = 70;
                player.attack = 24; player.defense = 9;
                player.magic = 10;  player.speed = 25;
                player.critRate = 30;
                break;
        }
        
        player.level = 1;
        player.exp = 0;
        player.expToNext = 100;
        player.hp = player.hpMax;
        player.mp = player.mpMax;
        player.critDamage = 150;
        player.gold = 200;
        player.skillPoints = 1;
        
        Item potion = {"Potion", "Memulihkan 50 HP dan 30 MP", "consumable", 50, 30, 0, 0, 50, 3};
        Item elixir  = {"Elixir", "Memulihkan Full HP & MP",   "consumable", 9999, 9999, 0, 0, 200, 1};
        player.inventory.push_back(potion);
        player.inventory.push_back(elixir);
        
        player.skills = getSkillsForClass(player.kelas, player.level);
        
        player.weapon    = {"Wooden Sword", "weapon",    5, 0, 0,  50, 1};
        player.armor     = {"Leather Vest", "armor",     0, 3, 10, 40, 1};
        player.accessory = {"Old Ring",     "accessory", 1, 1, 5,  30, 1};
        
        Item questReward = {"Potion", "Hadiah Quest", "consumable", 50, 30, 0, 0, 0, 1};
        Quest starterQuest = {"Pertama Berburu", "Kalahkan 3 Goblin", "Goblin",
                               3, 0, 100, 100, questReward, false, true};
        player.quests.push_back(starterQuest);
        
        cout << "\nSelamat datang, " << player.nama << " si " << player.kelas << "!" << endl;
        waitEnter();
        
        return player;
    }
    
    void tampilkanStatus() {
        cout << "\n+----------------------------------------------+" << endl;
        cout << "|              STATUS KARAKTER                  |" << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "| Nama   : " << player.nama << endl;
        cout << "| Kelas  : " << player.kelas << endl;
        cout << "| Level  : " << player.level << " (" << player.exp << "/" << player.expToNext << " EXP)" << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "| HP     : " << player.hp << "/" << player.hpMax << endl;
        cout << "| MP     : " << player.mp << "/" << player.mpMax << endl;
        cout << "| Attack : " << player.attack << endl;
        cout << "| Defense: " << player.defense << endl;
        cout << "| Magic  : " << player.magic << endl;
        cout << "| Speed  : " << player.speed << endl;
        cout << "| Crit   : " << player.critRate << "% (" << player.critDamage << "% dmg)" << endl;
        cout << "| Gold   : " << formatNumber(player.gold) << " Gold" << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "| EQUIPMENT:" << endl;
        cout << "|   Weapon   : " << player.weapon.nama << " (+" << player.weapon.attackBonus << " ATK)" << endl;
        cout << "|   Armor    : " << player.armor.nama  << " (+" << player.armor.defenseBonus << " DEF, +" << player.armor.hpBonus << " HP)" << endl;
        cout << "|   Accessory: " << player.accessory.nama << " (+" << player.accessory.attackBonus << " ATK)" << endl;
        cout << "+----------------------------------------------+" << endl;
    }
    
    void levelUp() {
        while (player.exp >= player.expToNext) {
            player.exp -= player.expToNext;
            player.level++;
            player.expToNext = 100 + (player.level - 1) * 50;
            
            if (player.kelas == "Warrior") {
                player.hpMax += 25; player.attack += 8; player.defense += 5; player.mpMax += 5;
            } else if (player.kelas == "Mage") {
                player.hpMax += 15; player.mpMax += 20; player.magic += 10; player.attack += 3;
            } else if (player.kelas == "Archer") {
                player.hpMax += 18; player.attack += 7; player.speed += 3; player.critRate += 2;
            } else {
                player.hpMax += 16; player.attack += 8; player.speed += 5; player.critRate += 3;
            }
            
            player.hp = player.hpMax;
            player.mp = player.mpMax;
            player.skillPoints++;
            
            vector<Skill> newSkills = getSkillsForClass(player.kelas, player.level);
            for (auto& skill : newSkills) {
                bool exists = false;
                for (auto& existing : player.skills) {
                    if (existing.nama == skill.nama) { exists = true; break; }
                }
                if (!exists) player.skills.push_back(skill);
            }
            
            cout << "\n====== LEVEL UP! ======" << endl;
            cout << "Kamu sekarang level " << player.level << "!" << endl;
            cout << "Skill Points +1 (Total: " << player.skillPoints << ")" << endl;
            delay(1000);
        }
    }
    
    void upgradeSkill() {
        if (player.skillPoints <= 0) {
            cout << "\nTidak ada skill points! Naikkan level dulu!" << endl;
            return;
        }
        
        cout << "\n--- UPGRADE SKILL ---" << endl;
        cout << "Skill Points tersedia: " << player.skillPoints << endl;
        
        for (size_t i = 0; i < player.skills.size(); i++) {
            cout << i + 1 << ". " << player.skills[i].nama
                 << " - " << player.skills[i].deskripsi
                 << " (DMG: " << player.skills[i].damageMultiplier << "%)" << endl;
        }
        
        int choice = 0;
        cout << "\nPilih skill (0 untuk batal): ";
        cin >> choice;
        
        if (choice > 0 && choice <= (int)player.skills.size()) {
            player.skills[choice - 1].damageMultiplier += 20;
            player.skillPoints--;
            cout << player.skills[choice - 1].nama << " meningkat! Damage: "
                 << player.skills[choice - 1].damageMultiplier << "%" << endl;
        }
    }
    
    void gunakanItem() {
        if (player.inventory.empty()) {
            cout << "Inventory kosong!" << endl;
            return;
        }
        
        cout << "\n--- INVENTORY ---" << endl;
        for (size_t i = 0; i < player.inventory.size(); i++) {
            cout << i + 1 << ". " << player.inventory[i].nama
                 << " x" << player.inventory[i].quantity
                 << " - " << player.inventory[i].deskripsi << endl;
        }
        
        int pilih = 0;
        cout << "\nPilih item (0 untuk batal): ";
        cin >> pilih;
        
        if (pilih > 0 && pilih <= (int)player.inventory.size()) {
            Item& item = player.inventory[pilih - 1];
            if (item.quantity > 0) {
                if (item.efekHp > 0) {
                    int heal = min(item.efekHp, player.hpMax - player.hp);
                    player.hp += heal;
                    cout << "HP +" << heal << "! (HP: " << player.hp << "/" << player.hpMax << ")" << endl;
                }
                if (item.efekMp > 0) {
                    int restore = min(item.efekMp, player.mpMax - player.mp);
                    player.mp += restore;
                    cout << "MP +" << restore << "! (MP: " << player.mp << "/" << player.mpMax << ")" << endl;
                }
                if (item.efekAttack > 0) {
                    player.attack += item.efekAttack;
                    cout << "Attack +" << item.efekAttack << " (sementara)!" << endl;
                }
                item.quantity--;
                if (item.quantity <= 0)
                    player.inventory.erase(player.inventory.begin() + (pilih - 1));
            }
        }
    }
    
    void lihatQuest() {
        cout << "\n--- QUEST LOG ---" << endl;
        bool hasQuest = false;
        
        for (auto& quest : player.quests) {
            if (quest.active && !quest.completed) {
                hasQuest = true;
                cout << "\n[AKTIF] " << quest.nama << endl;
                cout << "  " << quest.deskripsi << endl;
                cout << "  Progress: " << quest.currentKill << "/" << quest.targetKill << " " << quest.targetMusuh << endl;
                cout << "  Reward: " << quest.expReward << " EXP, " << quest.goldReward << " Gold" << endl;
            } else if (quest.completed) {
                cout << "\n[SELESAI] " << quest.nama << endl;
            }
        }
        
        if (!hasQuest) cout << "\nTidak ada quest aktif." << endl;
    }
    
    void updateQuestProgress(string musuhName) {
        for (auto& quest : player.quests) {
            if (quest.active && !quest.completed && quest.targetMusuh == musuhName) {
                quest.currentKill++;
                cout << "Quest progress: " << quest.currentKill << "/" << quest.targetKill << endl;
                if (quest.currentKill >= quest.targetKill) {
                    quest.completed = true;
                    cout << "\n=== QUEST COMPLETED: " << quest.nama << "! ===" << endl;
                    cout << "Mendapat " << quest.expReward << " EXP!" << endl;
                    cout << "Mendapat " << quest.goldReward << " Gold!" << endl;
                    player.exp += quest.expReward;
                    player.gold += quest.goldReward;
                    
                    if (!quest.itemReward.nama.empty()) {
                        bool found = false;
                        for (auto& item : player.inventory) {
                            if (item.nama == quest.itemReward.nama) { item.quantity++; found = true; break; }
                        }
                        if (!found) player.inventory.push_back(quest.itemReward);
                        cout << "Mendapat " << quest.itemReward.nama << "!" << endl;
                    }
                    levelUp();
                }
            }
        }
    }
    
    void equipItem() {
        cout << "\n--- EQUIPMENT SHOP ---" << endl;
        Equipment newEquip = getRandomEquipment(player.level);
        
        cout << "Equipment baru ditemukan!" << endl;
        cout << "Nama  : " << newEquip.nama << endl;
        cout << "Tipe  : " << newEquip.tipe << endl;
        cout << "ATK+  : " << newEquip.attackBonus << endl;
        cout << "DEF+  : " << newEquip.defenseBonus << endl;
        cout << "HP+   : " << newEquip.hpBonus << endl;
        cout << "Harga : " << newEquip.harga << " Gold" << endl;
        cout << "Gold kamu: " << player.gold << endl;
        
        char wantBuy;
        cout << "\nBeli? (y/n): ";
        cin >> wantBuy;
        
        if (wantBuy == 'y' || wantBuy == 'Y') {
            if (player.gold >= newEquip.harga) {
                player.gold -= newEquip.harga;
                if (newEquip.tipe == "weapon") {
                    player.weapon = newEquip;
                    player.attack += newEquip.attackBonus;
                } else if (newEquip.tipe == "armor") {
                    player.armor = newEquip;
                    player.defense += newEquip.defenseBonus;
                    player.hpMax += newEquip.hpBonus;
                    player.hp = min(player.hp + newEquip.hpBonus, player.hpMax);
                } else {
                    player.accessory = newEquip;
                    player.attack += newEquip.attackBonus;
                    player.defense += newEquip.defenseBonus;
                    player.hpMax += newEquip.hpBonus;
                    player.hp = min(player.hp + newEquip.hpBonus, player.hpMax);
                }
                cout << "Equipment berhasil dipasang!" << endl;
            } else {
                cout << "Gold tidak cukup! (Butuh " << newEquip.harga << ", punya " << player.gold << ")" << endl;
            }
        }
    }
    
    Karakter& getPlayer() { return player; }
    
    void setPlayer(Karakter& p) { player = p; }
};

// ================== BATTLE SYSTEM ==================

class BattleSystem {
private:
    Karakter& player;
    Musuh musuh;
    bool playerTurn;
    int turnCount;
    bool escaped;
    
public:
    BattleSystem(Karakter& p) : player(p), playerTurn(true), turnCount(0), escaped(false) {}
    
    bool startBattle(Musuh enemy) {
        musuh = enemy;
        playerTurn = true;
        turnCount = 0;
        escaped = false;
        
        cout << "\n====== PERTARUNGAN DIMULAI ======" << endl;
        cout << "Kamu bertemu dengan " << musuh.nama;
        if (musuh.tipe == "elite") cout << " (ELITE)";
        if (musuh.tipe == "boss") cout << " (BOSS)";
        cout << "!" << endl;
        delay(1000);
        
        while (player.hp > 0 && musuh.hp > 0 && !escaped) {
            tampilkanStatusPertarungan();
            if (playerTurn) {
                playerAction();
            } else {
                enemyAction();
            }
            turnCount++;
            delay(500);
        }
        
        if (escaped) return false; // kabur
        
        if (player.hp <= 0) {
            cout << "\n====== GAME OVER ======" << endl;
            cout << "Kamu dikalahkan oleh " << musuh.nama << "!" << endl;
            cout << "Kamu kembali ke kota dengan HP tersisa..." << endl;
            player.hp = player.hpMax / 4; // Tidak langsung mati, beri HP 25%
            waitEnter();
            return false;
        } else {
            victoryReward();
            return true;
        }
    }
    
private:
    void tampilkanStatusPertarungan() {
        clearScreen();
        int hpBarLen = 20;
        int musuhHpFill = (musuh.hpMax > 0) ? (musuh.hp * hpBarLen / musuh.hpMax) : 0;
        int playerHpFill = (player.hpMax > 0) ? (player.hp * hpBarLen / player.hpMax) : 0;
        
        cout << "+----------------------------------------------+" << endl;
        cout << "|                  PERTARUNGAN                  |" << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "| MUSUH: " << musuh.nama;
        if (musuh.tipe == "elite") cout << " [ELITE]";
        if (musuh.tipe == "boss") cout << " [BOSS]";
        cout << endl;
        cout << "| HP: [";
        for (int i = 0; i < hpBarLen; i++) cout << (i < musuhHpFill ? "#" : "-");
        cout << "] " << musuh.hp << "/" << musuh.hpMax << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "| " << player.nama << " (" << player.kelas << ")" << endl;
        cout << "| HP: [";
        for (int i = 0; i < hpBarLen; i++) cout << (i < playerHpFill ? "#" : "-");
        cout << "] " << player.hp << "/" << player.hpMax << endl;
        cout << "| MP: " << player.mp << "/" << player.mpMax << endl;
        cout << "+----------------------------------------------+" << endl;
    }
    
    void playerAction() {
        cout << "\n--- AKSI KAMU (Turn " << turnCount + 1 << ") ---" << endl;
        cout << "1. Serang Normal" << endl;
        cout << "2. Gunakan Skill" << endl;
        cout << "3. Gunakan Item" << endl;
        cout << "4. Coba Kabur" << endl;
        
        int aksi = 0;
        cout << "\nPilih: ";
        while (!(cin >> aksi) || aksi < 1 || aksi > 4) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Pilihan tidak valid! (1-4): ";
        }
        
        switch(aksi) {
            case 1: normalAttack(); break;
            case 2: useSkill();     break;
            case 3: useItem();      break;
            case 4: tryRun();       break;
        }
    }
    
    void normalAttack() {
        int damage = calculateDamage(player.attack, musuh.defense);
        bool isCrit = (rand() % 100) < player.critRate;
        if (isCrit) {
            damage = damage * player.critDamage / 100;
            cout << "CRITICAL HIT!" << endl;
        }
        musuh.hp -= damage;
        if (musuh.hp < 0) musuh.hp = 0;
        cout << "Kamu menyerang " << musuh.nama << " -> " << damage << " damage!" << endl;
        playerTurn = false;
    }
    
    void useSkill() {
        if (player.skills.empty()) {
            cout << "Tidak ada skill!" << endl;
            normalAttack();
            return;
        }
        
        cout << "\n--- SKILL LIST ---" << endl;
        for (size_t i = 0; i < player.skills.size(); i++) {
            cout << i + 1 << ". " << player.skills[i].nama
                 << " (DMG:" << player.skills[i].damageMultiplier
                 << "%, MP:" << player.skills[i].mpCost << ")" << endl;
        }
        
        int choice = 0;
        cout << "Pilih skill (0=batal): ";
        cin >> choice;
        
        if (choice <= 0 || choice > (int)player.skills.size()) {
            normalAttack();
            return;
        }
        
        Skill& skill = player.skills[choice - 1];
        if (player.mp < skill.mpCost) {
            cout << "MP tidak cukup! (Butuh " << skill.mpCost << ", punya " << player.mp << ")" << endl;
            normalAttack();
            return;
        }
        
        player.mp -= skill.mpCost;
        int damage = calculateDamage(player.attack, musuh.defense);
        damage = damage * skill.damageMultiplier / 100;
        if (damage < 1) damage = 1;
        
        bool isCrit = (rand() % 100) < player.critRate;
        if (isCrit) {
            damage = damage * player.critDamage / 100;
            cout << "CRITICAL HIT!" << endl;
        }
        
        musuh.hp -= damage;
        if (musuh.hp < 0) musuh.hp = 0;
        cout << "Menggunakan " << skill.nama << "! -> " << damage << " damage!" << endl;
        
        if      (skill.elemen == "fire")    cout << "Musuh terbakar!" << endl;
        else if (skill.elemen == "ice")     cout << "Musuh melambat!" << endl;
        else if (skill.elemen == "thunder") cout << "Musuh tersengat listrik!" << endl;
        else if (skill.elemen == "poison")  cout << "Musuh terkena racun!" << endl;
        
        playerTurn = false;
    }
    
    void useItem() {
        if (player.inventory.empty()) {
            cout << "Inventory kosong!" << endl;
            normalAttack();
            return;
        }
        
        cout << "\n--- INVENTORY ---" << endl;
        for (size_t i = 0; i < player.inventory.size(); i++) {
            cout << i + 1 << ". " << player.inventory[i].nama
                 << " x" << player.inventory[i].quantity
                 << " - " << player.inventory[i].deskripsi << endl;
        }
        
        int choice = 0;
        cout << "Pilih item (0=batal): ";
        cin >> choice;
        
        if (choice <= 0 || choice > (int)player.inventory.size()) {
            normalAttack();
            return;
        }
        
        Item& item = player.inventory[choice - 1];
        if (item.quantity <= 0) {
            cout << "Item habis!" << endl;
            normalAttack();
            return;
        }
        
        if (item.efekHp > 0) {
            int heal = min(item.efekHp, player.hpMax - player.hp);
            player.hp += heal;
            cout << "HP +" << heal << "! (" << player.hp << "/" << player.hpMax << ")" << endl;
        }
        if (item.efekMp > 0) {
            int restore = min(item.efekMp, player.mpMax - player.mp);
            player.mp += restore;
            cout << "MP +" << restore << "!" << endl;
        }
        
        item.quantity--;
        if (item.quantity <= 0)
            player.inventory.erase(player.inventory.begin() + (choice - 1));
        
        playerTurn = false;
    }
    
    void tryRun() {
        int chance = 30 + player.speed - musuh.speed;
        chance = max(20, min(70, chance));
        
        if (randomRange(1, 100) <= chance) {
            cout << "Kamu berhasil kabur!" << endl;
            escaped = true;
        } else {
            cout << "Gagal kabur!" << endl;
            playerTurn = false;
        }
    }
    
    void enemyAction() {
        cout << "\n--- GILIRAN " << musuh.nama << " ---" << endl;
        
        int damage;
        if (!musuh.skills.empty() && randomRange(1, 100) > 60) {
            Skill& skill = musuh.skills[randomRange(0, (int)musuh.skills.size() - 1)];
            damage = calculateDamage(musuh.attack, player.defense);
            damage = damage * skill.damageMultiplier / 100;
            if (damage < 1) damage = 1;
            cout << musuh.nama << " menggunakan " << skill.nama << "! -> " << damage << " damage!" << endl;
        } else {
            damage = calculateDamage(musuh.attack, player.defense);
            cout << musuh.nama << " menyerang! -> " << damage << " damage!" << endl;
        }
        
        player.hp -= damage;
        if (player.hp < 0) player.hp = 0;
        playerTurn = true;
    }
    
    int calculateDamage(int attack, int defense) {
        int damage = attack - defense / 3;
        if (damage < 1) damage = 1;
        damage = randomRange(max(1, damage - 3), damage + 3);
        return damage;
    }
    
    void victoryReward() {
        cout << "\n====== KEMENANGAN! ======" << endl;
        cout << "Mendapat " << musuh.expReward << " EXP!" << endl;
        cout << "Mendapat " << musuh.goldReward << " Gold!" << endl;
        
        player.exp += musuh.expReward;
        player.gold += musuh.goldReward;
        
        if (randomRange(1, 100) <= musuh.dropChance) {
            bool found = false;
            for (auto& item : player.inventory) {
                if (item.nama == musuh.dropItem) { item.quantity++; found = true; break; }
            }
            if (!found) {
                Item drop = {musuh.dropItem, "Drop dari " + musuh.nama, "material", 0, 0, 0, 0, 50, 1};
                player.inventory.push_back(drop);
            }
            cout << "Mendapat " << musuh.dropItem << "!" << endl;
        }
        
        // Update quest
        for (auto& quest : player.quests) {
            if (quest.active && !quest.completed && quest.targetMusuh == musuh.nama) {
                quest.currentKill++;
                cout << "Quest [" << quest.nama << "]: " << quest.currentKill << "/" << quest.targetKill << endl;
                if (quest.currentKill >= quest.targetKill) {
                    quest.completed = true;
                    cout << "=== QUEST COMPLETED: " << quest.nama << "! ===" << endl;
                    player.exp += quest.expReward;
                    player.gold += quest.goldReward;
                    cout << "Bonus: +" << quest.expReward << " EXP, +" << quest.goldReward << " Gold!" << endl;
                }
            }
        }
        
        // Level up
        while (player.exp >= player.expToNext) {
            player.exp -= player.expToNext;
            player.level++;
            player.expToNext = 100 + (player.level - 1) * 50;
            
            if (player.kelas == "Warrior") {
                player.hpMax += 25; player.attack += 8; player.defense += 5; player.mpMax += 5;
            } else if (player.kelas == "Mage") {
                player.hpMax += 15; player.mpMax += 20; player.magic += 10; player.attack += 3;
            } else if (player.kelas == "Archer") {
                player.hpMax += 18; player.attack += 7; player.speed += 3; player.critRate += 2;
            } else {
                player.hpMax += 16; player.attack += 8; player.speed += 5; player.critRate += 3;
            }
            player.hp = player.hpMax;
            player.mp = player.mpMax;
            player.skillPoints++;
            
            // Unlock skill baru
            vector<Skill> newSkills = getSkillsForClass(player.kelas, player.level);
            for (auto& skill : newSkills) {
                bool exists = false;
                for (auto& existing : player.skills) {
                    if (existing.nama == skill.nama) { exists = true; break; }
                }
                if (!exists) {
                    player.skills.push_back(skill);
                    cout << "Skill baru terbuka: " << skill.nama << "!" << endl;
                }
            }
            
            cout << "\n====== LEVEL UP! ======" << endl;
            cout << "Sekarang level " << player.level << "! HP & MP penuh!" << endl;
        }
        
        waitEnter();
    }
};

// ================== GAME CLASS ==================

class Game {
private:
    KarakterManager karakterManager;
    bool isRunning;
    
public:
    Game() : isRunning(true) {}
    
    void run() {
        srand((unsigned int)time(0));
        
        while (isRunning) {
            clearScreen();
            tampilkanMenuUtama();
            
            int choice = 0;
            cout << "\nPilih: ";
            while (!(cin >> choice) || choice < 1 || choice > 3) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Pilihan tidak valid (1-3): ";
            }
            
            switch(choice) {
                case 1: startNewGame(); break;
                case 2: loadGame();     break;
                case 3:
                    isRunning = false;
                    cout << "\nTerima kasih telah bermain! Sampai jumpa!" << endl;
                    break;
            }
        }
    }
    
private:
    void tampilkanMenuUtama() {
        cout << "+----------------------------------------------+" << endl;
        cout << "|              TEXT RPG GAME                   |" << endl;
        cout << "|           LEGEND OF THE VOID                 |" << endl;
        cout << "|              DELUXE EDITION                  |" << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "|                                              |" << endl;
        cout << "|              1. NEW GAME                     |" << endl;
        cout << "|              2. LOAD GAME                    |" << endl;
        cout << "|              3. EXIT                         |" << endl;
        cout << "|                                              |" << endl;
        cout << "+----------------------------------------------+" << endl;
    }
    
    void startNewGame() {
        Karakter player = karakterManager.initKarakter();
        mainGameLoop(player);
    }
    
    void loadGame() {
        ifstream file("savegame.txt");
        if (!file.is_open()) {
            cout << "File save tidak ditemukan! Mulai New Game dulu." << endl;
            waitEnter();
            return;
        }

        Karakter p;
        p.weapon    = {"", "weapon",    0, 0, 0, 0, 1};
        p.armor     = {"", "armor",     0, 0, 0, 0, 1};
        p.accessory = {"", "accessory", 0, 0, 0, 0, 1};

        // Baca nama & kelas (string, pakai getline)
        getline(file, p.nama);
        getline(file, p.kelas);

        // Baca stats angka satu per satu
        file >> p.level >> p.exp >> p.expToNext
             >> p.hp >> p.hpMax
             >> p.mp >> p.mpMax
             >> p.attack >> p.defense >> p.magic >> p.speed
             >> p.critRate >> p.critDamage
             >> p.gold >> p.skillPoints;
        file.ignore();

        // Baca equipment: nama (getline) lalu bonus (angka)
        getline(file, p.weapon.nama);
        file >> p.weapon.attackBonus >> p.weapon.defenseBonus >> p.weapon.hpBonus;
        file.ignore();

        getline(file, p.armor.nama);
        file >> p.armor.attackBonus >> p.armor.defenseBonus >> p.armor.hpBonus;
        file.ignore();

        getline(file, p.accessory.nama);
        file >> p.accessory.attackBonus >> p.accessory.defenseBonus >> p.accessory.hpBonus;
        file.ignore();

        // Baca inventory
        int invSize = 0;
        file >> invSize;
        file.ignore();
        for (int i = 0; i < invSize; i++) {
            Item item = {"", "", "consumable", 0, 0, 0, 0, 0, 0};
            getline(file, item.nama);
            file >> item.quantity;
            file.ignore();
            p.inventory.push_back(item);
        }

        // Baca skills yang sudah diupgrade
        int skillSize = 0;
        file >> skillSize;
        file.ignore();

        // Load base skills dulu, lalu patch damage multiplier dari file
        p.skills = getSkillsForClass(p.kelas, p.level);

        for (int i = 0; i < skillSize; i++) {
            string sNama;
            int sDmg, sMp;
            getline(file, sNama);
            file >> sDmg >> sMp;
            file.ignore();
            // Cari skill yang namanya cocok lalu update multiplier-nya
            for (auto& sk : p.skills) {
                if (sk.nama == sNama) {
                    sk.damageMultiplier = sDmg;
                    sk.mpCost = sMp;
                    break;
                }
            }
        }

        file.close();

        if (p.nama.empty() || p.kelas.empty()) {
            cout << "File save rusak atau tidak valid!" << endl;
            waitEnter();
            return;
        }

        karakterManager.setPlayer(p);
        clearScreen();
        cout << "+----------------------------------------------+" << endl;
        cout << "|              GAME LOADED!                    |" << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "| Nama   : " << p.nama 		<<			"   |"<< endl;
        cout << "| Kelas  : " << p.kelas <<					"   |"<< endl;
        cout << "| Level  : " << p.level <<					"   |"<< endl;
        cout << "| HP     : " << p.hp << "/" <<p.hpMax <<	   "|"<< endl;
        cout << "| Gold   : " << formatNumber(p.gold) <<      " |"<< endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "\nSelamat datang kembali, " << p.nama << "!" << endl;
        waitEnter();
        mainGameLoop(p);
    }
    
    void mainGameLoop(Karakter player) {
        bool exploring = true;
        
        while (exploring && player.hp > 0) {
            clearScreen();
            karakterManager.setPlayer(player);
            karakterManager.tampilkanStatus();
            
            cout << "\n+--------------------------------------------+" << endl;
            cout << "|                 PETUALANGAN                  |" << endl;
            cout << "+----------------------------------------------+" << endl;
            cout << "|  1. Cari Musuh                               |" << endl;
            cout << "|  2. Rest Area (Full HP/MP)                   |" << endl;
            cout << "|  3. Pergi ke Toko                            |" << endl;
            cout << "|  4. Inventory & Items                        |" << endl;
            cout << "|  5. Quest Log                                |" << endl;
            cout << "|  6. Upgrade Skill                            |" << endl;
            cout << "|  7. Equipment Shop                           |" << endl;
            cout << "|  8. Dungeon Mode (Elite & Boss)              |" << endl;
            cout << "|  9. Save Game                                |" << endl;
            cout << "|  10. Kembali ke Menu Utama                   |" << endl;
            cout << "+----------------------------------------------+" << endl;
            
            int aksi = 0;
            cout << "\nPilih aksi: ";
            cin >> aksi;
            
            // Sinkronisasi player ke manager
            karakterManager.setPlayer(player);
            
            switch(aksi) {
                case 1: {
                    string tipe = "normal";
                    if (randomRange(1, 100) <= 20) tipe = "elite";
                    Musuh musuh = generateMusuh(player.level, tipe);
                    cout << "\nKamu menemukan " << musuh.nama;
                    if (musuh.tipe == "elite") cout << " (ELITE)";
                    cout << "!" << endl;
                    waitEnter();
                    
                    BattleSystem battle(player);
                    battle.startBattle(musuh);
                    break;
                }
                case 2:
                    player.hp = player.hpMax;
                    player.mp = player.mpMax;
                    cout << "\nKamu beristirahat... HP & MP pulih sepenuhnya!" << endl;
                    waitEnter();
                    break;
                case 3:
                    toko(player);
                    break;
                case 4:
                    karakterManager.setPlayer(player);
                    karakterManager.gunakanItem();
                    player = karakterManager.getPlayer();
                    waitEnter();
                    break;
                case 5:
                    karakterManager.setPlayer(player);
                    karakterManager.lihatQuest();
                    waitEnter();
                    break;
                case 6:
                    karakterManager.setPlayer(player);
                    karakterManager.upgradeSkill();
                    player = karakterManager.getPlayer();
                    waitEnter();
                    break;
                case 7:
                    karakterManager.setPlayer(player);
                    karakterManager.equipItem();
                    player = karakterManager.getPlayer();
                    waitEnter();
                    break;
                case 8:
                    dungeonMode(player);
                    break;
                case 9:
                    saveGame(player);
                    waitEnter();
                    break;
                case 10:
                    exploring = false;
                    break;
                default:
                    cout << "Aksi tidak valid! (1-10)" << endl;
                    waitEnter();
            }
        }
    }
    
    void toko(Karakter& player) {
        bool inShop = true;
        while (inShop) {
            clearScreen();
            cout << "+----------------------------------------------+" << endl;
            cout << "|               TOKO PEDAGANG                   |" << endl;
            cout << "+----------------------------------------------+" << endl;
            cout << "Gold kamu: " << formatNumber(player.gold) << " Gold" << endl;
            cout << "\nDaftar Item:" << endl;
            cout << "1. Potion       (HP+50, MP+30)  - 50 Gold" << endl;
            cout << "2. Elixir       (Full HP/MP)    - 200 Gold" << endl;
            cout << "3. Strength Pot (+20 ATK)       - 150 Gold" << endl;
            cout << "4. Magic Crystal(+15 MAG)       - 120 Gold" << endl;
            cout << "5. Lucky Charm  (+5% CRIT)      - 180 Gold" << endl;
            cout << "6. Keluar" << endl;
            
            int pilih = 0;
            cout << "\nPilih: ";
            cin >> pilih;
            
            auto addOrStack = [&](Item newItem) {
                for (auto& item : player.inventory) {
                    if (item.nama == newItem.nama) { item.quantity++; return; }
                }
                player.inventory.push_back(newItem);
            };
            
            switch(pilih) {
                case 1:
                    if (player.gold >= 50) {
                        addOrStack({"Potion", "HP+50, MP+30", "consumable", 50, 30, 0, 0, 50, 1});
                        player.gold -= 50;
                        cout << "Membeli Potion! Gold tersisa: " << player.gold << endl;
                    } else cout << "Gold tidak cukup!" << endl;
                    break;
                case 2:
                    if (player.gold >= 200) {
                        addOrStack({"Elixir", "Full HP & MP", "consumable", 9999, 9999, 0, 0, 200, 1});
                        player.gold -= 200;
                        cout << "Membeli Elixir!" << endl;
                    } else cout << "Gold tidak cukup!" << endl;
                    break;
                case 3:
                    if (player.gold >= 150) {
                        player.attack += 20; player.gold -= 150;
                        cout << "Attack meningkat +20! (Total: " << player.attack << ")" << endl;
                    } else cout << "Gold tidak cukup!" << endl;
                    break;
                case 4:
                    if (player.gold >= 120) {
                        player.magic += 15; player.gold -= 120;
                        cout << "Magic meningkat +15! (Total: " << player.magic << ")" << endl;
                    } else cout << "Gold tidak cukup!" << endl;
                    break;
                case 5:
                    if (player.gold >= 180) {
                        player.critRate += 5; player.gold -= 180;
                        cout << "Critical Rate +5%! (Total: " << player.critRate << "%)" << endl;
                    } else cout << "Gold tidak cukup!" << endl;
                    break;
                case 6:
                    inShop = false;
                    break;
                default:
                    cout << "Pilihan tidak valid!" << endl;
            }
            if (pilih != 6) waitEnter();
        }
    }
    
    void dungeonMode(Karakter& player) {
        clearScreen();
        cout << "+----------------------------------------------+" << endl;
        cout << "|               DUNGEON MODE                    |" << endl;
        cout << "|               [HARD MODE]                     |" << endl;
        cout << "+----------------------------------------------+" << endl;
        cout << "\nKamu memasuki dungeon berbahaya!" << endl;
        cout << "5 lantai, lantai terakhir ada BOSS!" << endl;
        cout << "Hadiahnya besar tapi risikonya tinggi!" << endl;
        
        char confirm;
        cout << "\nYakin masuk? (y/n): ";
        cin >> confirm;
        
        if (confirm != 'y' && confirm != 'Y') return;
        
        int floor = 1;
        while (player.hp > 0 && floor <= 5) {
            cout << "\n=== Lantai " << floor << "/5 ===" << endl;
            
            string tipe;
            if (floor == 5) tipe = "boss";
            else if (randomRange(1, 100) <= 40) tipe = "elite";
            else tipe = "normal";
            
            Musuh musuh = generateMusuh(player.level + floor, tipe);
            cout << "Muncul: " << musuh.nama;
            if (musuh.tipe == "elite") cout << " [ELITE]";
            if (musuh.tipe == "boss") cout << " [BOSS]";
            cout << "!" << endl;
            waitEnter();
            
            BattleSystem battle(player);
            bool won = battle.startBattle(musuh);
            
            if (!won) {
                if (player.hp <= 0) {
                    cout << "\nKamu terpaksa keluar dari dungeon..." << endl;
                } else {
                    cout << "\nKabur dari dungeon..." << endl;
                }
                break;
            }
            
            cout << "\nBerhasil melewati lantai " << floor << "!" << endl;
            // Heal sedikit antar lantai
            player.hp = min(player.hpMax, player.hp + player.hpMax / 5);
            cout << "HP dipulihkan sedikit: " << player.hp << "/" << player.hpMax << endl;
            floor++;
            if (floor <= 5) waitEnter();
        }
        
        if (player.hp > 0 && floor > 5) {
            cout << "\n====== SELAMAT! DUNGEON SELESAI! ======" << endl;
            int bonusGold = 1000 + (player.level * 100);
            int bonusExp  = 500  + (player.level * 50);
            player.gold += bonusGold;
            player.exp  += bonusExp;
            cout << "Bonus: +" << bonusGold << " Gold, +" << bonusExp << " EXP!" << endl;
            
            // Level up jika perlu
            while (player.exp >= player.expToNext) {
                player.exp -= player.expToNext;
                player.level++;
                player.expToNext = 100 + (player.level - 1) * 50;
                cout << "LEVEL UP! Sekarang level " << player.level << "!" << endl;
            }
        }
        waitEnter();
    }
    
    void saveGame(Karakter& player) {
        ofstream file("savegame.txt");
        if (!file.is_open()) {
            cout << "Gagal menyimpan game!" << endl;
            return;
        }

        // Baris 1-2: nama & kelas
        file << player.nama << "\n";
        file << player.kelas << "\n";
        // Baris 3-17: stats angka
        file << player.level      << "\n";
        file << player.exp        << "\n";
        file << player.expToNext  << "\n";
        file << player.hp         << "\n";
        file << player.hpMax      << "\n";
        file << player.mp         << "\n";
        file << player.mpMax      << "\n";
        file << player.attack     << "\n";
        file << player.defense    << "\n";
        file << player.magic      << "\n";
        file << player.speed      << "\n";
        file << player.critRate   << "\n";
        file << player.critDamage << "\n";
        file << player.gold       << "\n";
        file << player.skillPoints<< "\n";
        // Equipment: nama dulu, baru bonus
        file << player.weapon.nama         << "\n";
        file << player.weapon.attackBonus  << "\n";
        file << player.weapon.defenseBonus << "\n";
        file << player.weapon.hpBonus      << "\n";
        file << player.armor.nama          << "\n";
        file << player.armor.attackBonus   << "\n";
        file << player.armor.defenseBonus  << "\n";
        file << player.armor.hpBonus       << "\n";
        file << player.accessory.nama         << "\n";
        file << player.accessory.attackBonus  << "\n";
        file << player.accessory.defenseBonus << "\n";
        file << player.accessory.hpBonus      << "\n";
        // Inventory
        file << player.inventory.size() << "\n";
        for (auto& item : player.inventory) {
            file << item.nama     << "\n";
            file << item.quantity << "\n";
        }

        // Simpan skills beserta damage multiplier yang sudah diupgrade
        file << player.skills.size() << "\n";
        for (auto& skill : player.skills) {
            file << skill.nama             << "\n";
            file << skill.damageMultiplier << "\n";
            file << skill.mpCost           << "\n";
        }

        file.close();
        cout << "\nGame berhasil disimpan! (Level " << player.level << ")" << endl;
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
