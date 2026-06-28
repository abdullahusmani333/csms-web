#ifndef AUTHSYSTEM_H
#define AUTHSYSTEM_H

#include "User.h"
#include "Encryption.h"
#include "Logger.h"
#include <vector>
using namespace std;

//  CLASS: AuthSystem
//  The brain of the authentication module.
class AuthSystem {
private:
    vector<User*> users;       // list of all registered users (pointers for polymorphism)
    Encryption* cipher;        // the encryption method to use (XOR or Caesar)
    string dbFile;             // filename used as our "database"
    Logger logger;             

    User* findUser(const string& username);
    void saveUsers();           // writes all users to file
    void loadUsers();           // reads all users from file on startup

public:
    AuthSystem(Encryption* enc, string filename = "users.txt");

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
