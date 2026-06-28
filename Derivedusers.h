#ifndef DERIVED_USERS_H
#define DERIVED_USERS_H

#include "User.h"

//  DERIVED CLASS: RegularUser
//  Inherits everything from User.
//  Overrides displayRole() to say "Regular User".

class RegularUser : public User {
public:
    RegularUser(string uname, string encPwd);
    void displayRole() const override;  // POLYMORPHISM in action
};


//  DERIVED CLASS: Admin
//  Also inherits from User.
//  Has extra privilege: canDeleteUsers
class Admin : public User {
private:
    bool canDeleteUsers;  // extra data only Admin has

public:
    Admin(string uname, string encPwd);
    void displayRole() const override;  // POLYMORPHISM in action
    bool getCanDeleteUsers() const;
};

#endif
