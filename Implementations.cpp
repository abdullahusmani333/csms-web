// ════════════════════════════════════════════════
//  User.cpp  —  Base class implementation
// ════════════════════════════════════════════════
#include "User.h"
#include <iostream>
using namespace std;

User::User(string uname, string encPwd)
    : username(uname), encryptedPassword(encPwd),
      failedAttempts(0), isLocked(false) {}

string User::getUsername() const { return username; }
string User::getEncryptedPassword() const { return encryptedPassword; }
bool   User::getIsLocked() const { return isLocked; }
int    User::getFailedAttempts() const { return failedAttempts; }

void User::incrementFailedAttempts() {
    failedAttempts++;
    if (failedAttempts >= 3) {
        isLocked = true;  // lock after 3 failed tries
    }
}

void User::resetFailedAttempts() {
    failedAttempts = 0;
    isLocked = false;
}

void User::lockAccount() {
    isLocked = true;
}

void User::displayRole() const {
    cout << "[User] " << username << endl;
}


// ════════════════════════════════════════════════
//  DerivedUsers.cpp  —  RegularUser & Admin
// ════════════════════════════════════════════════
#include "Derivedusers.h"
#include <iostream>
using namespace std;

// ── RegularUser ──
RegularUser::RegularUser(string uname, string encPwd)
    : User(uname, encPwd) {}   // pass args up to base class constructor

void RegularUser::displayRole() const {
    cout << "[Regular User] " << username << endl;
}

// ── Admin ──
Admin::Admin(string uname, string encPwd)
    : User(uname, encPwd), canDeleteUsers(true) {}

void Admin::displayRole() const {
    cout << "[Admin] " << username << " (Full privileges)" << endl;
}

bool Admin::getCanDeleteUsers() const { return canDeleteUsers; }


// ════════════════════════════════════════════════
//  Encryption.cpp  —  XORCipher & CaesarCipher
// ════════════════════════════════════════════════
#include "Encryption.h"

// ── XORCipher ──
XORCipher::XORCipher(char k) : key(k) {}

string XORCipher::encrypt(const string& text) const {
    string result = text;
    for (char& c : result) {
        c = c ^ key;   // XOR each character with the key
    }
    return result;
}

// XOR is its own inverse — decrypt is identical to encrypt
string XORCipher::decrypt(const string& text) const {
    return encrypt(text);
}

// ── CaesarCipher ──
CaesarCipher::CaesarCipher(int s) : shift(s) {}

string CaesarCipher::encrypt(const string& text) const {
    string result = text;
    for (char& c : result) {
        if (isalpha(c)) {
            // Stay within alphabet bounds (a-z or A-Z)
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + shift) % 26 + base;
        }
    }
    return result;
}

string CaesarCipher::decrypt(const string& text) const {
    string result = text;
    for (char& c : result) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base - shift + 26) % 26 + base;  // reverse the shift
        }
    }
    return result;
}


// ════════════════════════════════════════════════
//  AuthSystem.cpp  —  Core authentication logic
// ════════════════════════════════════════════════
#include "AuthSystem.h"
#include "Derivedusers.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

AuthSystem::AuthSystem(Encryption* enc, string filename)
    : cipher(enc), dbFile(filename), logger("logs.txt", true) {
    // logger("logs.txt", true) — COMPOSITION in action:
    // Logger is initialized right here alongside cipher and dbFile.
    // AuthSystem owns and controls its Logger.
    loadUsers();
    logger.logInfo("System started");
}

AuthSystem::~AuthSystem() {
    // Free each User object from memory
    for (User* u : users) delete u;
    // Note: cipher is created outside and deleted by the caller
}

// ── Find a user by username (private helper) ──
User* AuthSystem::findUser(const string& username) {
    for (User* u : users) {
        if (u->getUsername() == username) return u;
    }
    return nullptr;  // not found
}

// ── Register a new user ──
bool AuthSystem::registerUser(const string& username, const string& password, bool isAdmin) {
    if (findUser(username)) {
        cout << "  [!] Username already exists.\n";
        logger.logWarning("Registration failed - username taken", username);
        return false;
    }
    if (!isStrongPassword(password)) {
        cout << "  [!] Weak password! Must be 8+ chars with letters and digits.\n";
        logger.logWarning("Registration failed - weak password", username);
        return false;
    }

    string encPwd = cipher->encrypt(password);

    User* newUser;
    if (isAdmin)
        newUser = new Admin(username, encPwd);
    else
        newUser = new RegularUser(username, encPwd);

    users.push_back(newUser);
    saveUsers();

    string role = isAdmin ? "Admin" : "RegularUser";
    logger.logInfo("New " + role + " registered", username);
    cout << "  [+] User '" << username << "' registered successfully.\n";
    return true;
}

// ── Login ──
int AuthSystem::loginUser(const string& username, const string& password) {
    User* user = findUser(username);

    if (!user) {
        cout << "  [!] Username not found.\n";
        logger.logWarning("Login failed - username not found", username);
        return 0;
    }
    if (user->getIsLocked()) {
        cout << "  [X] Account is LOCKED after too many failed attempts.\n";
        logger.logCritical("Login attempt on locked account", username);
        return 0;
    }

    string encPwd = cipher->encrypt(password);
    if (encPwd == user->getEncryptedPassword()) {
        user->resetFailedAttempts();
        logger.logInfo("Login successful", username);
        cout << "  [✓] Login successful! Welcome, " << username << ".\n";
        user->displayRole();
        // Return 2 for admin, 1 for regular user
        bool isAdmin = (dynamic_cast<Admin*>(user) != nullptr);
        return isAdmin ? 2 : 1;
    } else {
        user->incrementFailedAttempts();
        int remaining = 3 - user->getFailedAttempts();
        if (user->getIsLocked()) {
            cout << "  [X] Wrong password. Account is now LOCKED.\n";
            logger.logCritical("Account locked after repeated failures", username);
        } else {
            cout << "  [!] Wrong password. " << remaining << " attempt(s) remaining.\n";
            logger.logWarning("Failed login attempt (" + to_string(user->getFailedAttempts()) + "/3)", username);
        }
        saveUsers();
        return 0;
    }
}

// ── Display all users (Admin feature) ──
void AuthSystem::displayAllUsers() const {
    cout << "\n  ── Registered Users ──\n";
    for (User* u : users) {
        u->displayRole();
        cout << "     Locked: " << (u->getIsLocked() ? "Yes" : "No") << "\n";
    }
}

// ── Display logs (delegates to Logger) ──
void AuthSystem::displayLogs() const {
    logger.displayLogs();
}

// ── Password strength checker ──
bool AuthSystem::isStrongPassword(const string& password) {
    if (password.length() < 8) return false;
    bool hasLetter = false, hasDigit = false;
    for (char c : password) {
        if (isalpha(c)) hasLetter = true;
        if (isdigit(c)) hasDigit = true;
    }
    return hasLetter && hasDigit;
}

// ── Save users to file ──
void AuthSystem::saveUsers() {
    ofstream file(dbFile);
    for (User* u : users) {
        // Format: username|encryptedPassword|isAdmin|failedAttempts|isLocked
        bool isAdmin = (dynamic_cast<Admin*>(u) != nullptr);
        file << u->getUsername() << "|"
             << u->getEncryptedPassword() << "|"
             << isAdmin << "|"
             << u->getFailedAttempts() << "|"
             << u->getIsLocked() << "\n";
    }
}

// ── Load users from file ──
void AuthSystem::loadUsers() {
    ifstream file(dbFile);
    if (!file.is_open()) return;  // no file yet, that's fine

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string uname, encPwd, adminStr, attemptsStr, lockedStr;
        getline(ss, uname, '|');
        getline(ss, encPwd, '|');
        getline(ss, adminStr, '|');
        getline(ss, attemptsStr, '|');
        getline(ss, lockedStr, '|');

        User* u;
        if (adminStr == "1")
            u = new Admin(uname, encPwd);
        else
            u = new RegularUser(uname, encPwd);

        for (int i = 0; i < stoi(attemptsStr); i++)
            u->incrementFailedAttempts();
        if (lockedStr == "1")
            u->lockAccount();

        users.push_back(u);
    }
}