#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include "AuthSystem.h"
#include "FileManager.h"
#include "Logger.h"
#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  CLASS: AdminPanel
//
//  NEW OOP CONCEPT: Static Members
//  A static member belongs to the CLASS itself,
//  not to any individual object.
//
//  Example: AdminPanel::currentUser is ONE shared
//  variable across the entire program — not a
//  separate copy per object.
//
//  This is perfect for tracking "who is logged in"
//  since there's only ever one active session.
// ─────────────────────────────────────────────
class AdminPanel {
private:
    AuthSystem& auth;        // reference to the main auth system
    FileManager& fm;         // reference to the file manager
    Logger logger;           // own logger for admin actions

    // ── STATIC MEMBER ──
    // Shared across ALL AdminPanel instances (and the whole program).
    // Stores the username of whoever is currently logged in.
    static string currentUser;
    static bool currentUserIsAdmin;

public:
    AdminPanel(AuthSystem& authRef, FileManager& fmRef);

    // ── STATIC METHODS ──
    // Can be called without creating an AdminPanel object.
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