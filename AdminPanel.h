#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include "AuthSystem.h"
#include "FileManager.h"
#include "Logger.h"
#include <string>
using namespace std;

//  CLASS: AdminPanel
class AdminPanel {
private:
    AuthSystem& auth;        // reference to the main auth system
    FileManager& fm;         // reference to the file manager
    Logger logger;           // own logger for admin actions

    // ── STATIC MEMBER ──
    // Stores the username of whoever is currently logged in.
    static string currentUser;
    static bool currentUserIsAdmin;

public:
    AdminPanel(AuthSystem& authRef, FileManager& fmRef);

    // ── STATIC METHODS ──
    // Used by AuthSystem to update session state on login/logout.
    static void setSession(const string& username, bool isAdmin);
    static void clearSession();
    static string getCurrentUser();
    static bool isAdminLoggedIn();

    // Admin-only features (will check isAdminLoggedIn() before proceeding)
    void manageUsers();
    void viewLogs();
    void viewTrackedFiles();
    void showPanel();    // the admin panel menu
};

#endif
