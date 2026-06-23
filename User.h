#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  BASE CLASS: User
//  Every account in the system is a "User".
//  Admin and RegularUser will INHERIT from this.
// ─────────────────────────────────────────────
class User {
protected:
    // 'protected' means these are hidden from outside code,
    // but derived classes (Admin, RegularUser) CAN access them.
    string username;
    string encryptedPassword;
    int failedAttempts;   // tracks wrong password tries
    bool isLocked;        // account gets locked after 3 failures

public:
    // Constructor: runs automatically when a User object is created
    User(string uname, string encPwd);

    // Getters: safe way to READ private/protected data
    string getUsername() const;
    string getEncryptedPassword() const;
    bool getIsLocked() const;
    int getFailedAttempts() const;

    // Setters: safe way to MODIFY data
    void incrementFailedAttempts();
    void resetFailedAttempts();
    void lockAccount();

    // 'virtual' means derived classes can OVERRIDE this method
    // This is POLYMORPHISM — each role can display itself differently
    virtual void displayRole() const;

    // Virtual destructor (good practice in base classes)
    virtual ~User() {}
};

#endif