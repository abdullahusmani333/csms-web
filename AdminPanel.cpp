// ════════════════════════════════════════════════
//  AdminPanel.cpp
// ════════════════════════════════════════════════
#include "AdminPanel.h"
#include <iostream>
using namespace std;

// ── Static member definitions ──
// Static members must be defined OUTSIDE the class body.
// This is where the actual memory is allocated for them.
string AdminPanel::currentUser = "";
bool AdminPanel::currentUserIsAdmin = false;

// ── Constructor ──
AdminPanel::AdminPanel(AuthSystem& authRef, FileManager& fmRef)
    : auth(authRef), fm(fmRef), logger("logs.txt", true) {}

// ── Static: set session after login ──
void AdminPanel::setSession(const string& username, bool isAdmin) {
    currentUser = username;
    currentUserIsAdmin = isAdmin;
}

// ── Static: clear session on logout ──
void AdminPanel::clearSession() {
    currentUser = "";
    currentUserIsAdmin = false;
}

// ── Static: get current logged-in user ──
string AdminPanel::getCurrentUser() {
    return currentUser;
}

// ── Static: check if an admin is logged in ──
bool AdminPanel::isAdminLoggedIn() {
    return currentUserIsAdmin && !currentUser.empty();
}

// ── Guard: prints error and returns false if not admin ──
// Used at the top of every admin-only method
static bool requireAdmin() {
    if (!AdminPanel::isAdminLoggedIn()) {
        cout << "  [X] Access denied. Please log in as Admin first.\n";
        return false;
    }
    return true;
}

// ── Manage Users ──
void AdminPanel::manageUsers() {
    if (!requireAdmin()) return;
    cout << "\n  ── User Management (Admin: " << currentUser << ") ──\n";
    auth.displayAllUsers();
}

// ── View Logs ──
void AdminPanel::viewLogs() {
    if (!requireAdmin()) return;
    cout << "\n  ── System Logs (Admin: " << currentUser << ") ──\n";
    auth.displayLogs();
}

// ── View Tracked Files ──
void AdminPanel::viewTrackedFiles() {
    if (!requireAdmin()) return;
    cout << "\n  ── Tracked Files (Admin: " << currentUser << ") ──\n";
    fm.listFiles();
}

// ── Admin Panel Menu ──
void AdminPanel::showPanel() {
    if (!requireAdmin()) return;

    int choice;
    do {
        cout << "\n  ╔══════════════════════════════════╗\n";
        cout << "    Admin Panel [" << currentUser << "]\n";
        cout << "  ╚══════════════════════════════════╝\n";
        cout << "  1. Manage Users\n";
        cout << "  2. View System Logs\n";
        cout << "  3. View Tracked Files\n";
        cout << "  0. Back to Main Menu\n";
        cout << "  Choice: ";
        cin >> choice;

        if (choice == 1) manageUsers();
        else if (choice == 2) viewLogs();
        else if (choice == 3) viewTrackedFiles();

    } while (choice != 0);

    logger.logInfo("Admin panel session ended", currentUser);
}