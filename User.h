#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

//  BASE CLASS: User
//  Every account in the system is a "User".
//  Admin and RegularUser will INHERIT from this.
class User {
protected:
    string username;
    string encryptedPassword;
    int failedAttempts;   // tracks wrong password tries
    bool isLocked;        // account gets locked after 3 failures

public:
    User(string uname, string encPwd);

    string getUsername() const;
    string getEncryptedPassword() const;
    bool getIsLocked() const;
    int getFailedAttempts() const;

    void incrementFailedAttempts();
    void resetFailedAttempts();
    void lockAccount();

    virtual void displayRole() const;

    virtual ~User() {}
};

#endif
