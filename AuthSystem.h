#ifndef AUTHSYSTEM_H
#define AUTHSYSTEM_H

#include "User.h"
#include "Encryption.h"
#include "Logger.h"
#include <vector>
using namespace std;

// ─────────────────────────────────────────────
//  CLASS: AuthSystem
//  The brain of the authentication module.
//  Handles: registration, login, lockouts,
//  saving/loading users from a file.
//
//  COMPOSITION: AuthSystem HAS-A Logger.
//  Logger is a member object — not a parent class.
// ─────────────────────────────────────────────
class AuthSystem {
private:
    vector<User*> users;       // list of all registered users (pointers for polymorphism)
    Encryption* cipher;        // the encryption method to use (XOR or Caesar)
    string dbFile;             // filename used as our "database"
    Logger logger;             // COMPOSITION: Logger lives inside AuthSystem

    // Internal helpers (private — not for outside use)
    User* findUser(const string& username);
    void saveUsers();           // write all users to file
    void loadUsers();           // read all users from file on startup

public:
    // Constructor: sets up the system with a chosen cipher and file
    AuthSystem(Encryption* enc, string filename = "users.txt");

    // Destructor: frees memory (important for pointers!)
    ~AuthSystem();

    // Core features
    bool registerUser(const string& username, const string& password, bool isAdmin = false);
    int loginUser(const string& username, const string& password);
    void displayAllUsers() const;   // Admin feature

    // Password strength check
    static bool isStrongPassword(const string& password);

    // Logger access (so main.cpp can display logs without touching Logger directly)
    void displayLogs() const;
};

#endif