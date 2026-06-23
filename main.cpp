// ════════════════════════════════════════════════
//  main.cpp  —  Cyber Security Management System
//  Module 5: Final Main Menu
// ════════════════════════════════════════════════
#include <iostream>
#include <string>
#include "AuthSystem.h"
#include "Encryption.h"
#include "Logger.h"
#include "FileManager.h"
#include "AdminPanel.h"
using namespace std;

// ── UI Helpers ──
void clearScreen() {
    cout << "\033[2J\033[1;1H";  // ANSI escape: clear terminal
}

void printBanner() {
    cout << "\n";
    cout << "  ╔═══════════════════════════════════════════╗\n";
    cout << "  ║    CYBER SECURITY MANAGEMENT SYSTEM       ║\n";
    cout << "  ║         C++ OOP Project — 2025            ║\n";
    cout << "  ╚═══════════════════════════════════════════╝\n";
}

void printSessionBar() {
    string user = AdminPanel::getCurrentUser();
    if (!user.empty()) {
        string role = AdminPanel::isAdminLoggedIn() ? "Admin" : "User";
        cout << "  ┌─────────────────────────────────────────┐\n";
        cout << "  │  Logged in as: " << user
             << " [" << role << "]"
             << string(24 - user.size() - role.size(), ' ') << "│\n";
        cout << "  └─────────────────────────────────────────┘\n";
    } else {
        cout << "  ┌─────────────────────────────────────────┐\n";
        cout << "  │  Not logged in                          │\n";
        cout << "  └─────────────────────────────────────────┘\n";
    }
}

void printSeparator() {
    cout << "  ─────────────────────────────────────────────\n";
}

void welcomeScreen() {
    clearScreen();
    cout << "\n\n";
    cout << "  ╔═══════════════════════════════════════════╗\n";
    cout << "  ║                                           ║\n";
    cout << "  ║    CYBER SECURITY MANAGEMENT SYSTEM       ║\n";
    cout << "  ║         C++ OOP Project — 2025            ║\n";
    cout << "  ║                                           ║\n";
    cout << "  ║   Developed by:                           ║\n";
    cout << "  ║     Abdullah Usmani   — 2540013           ║\n";
    cout << "  ║     Muhammad Hammad  — 2540190            ║\n";
    cout << "  ║     Uzair Mansoor    — 2540028            ║\n";
    cout << "  ║                                           ║\n";
    cout << "  ╚═══════════════════════════════════════════╝\n";
    cout << "\n  Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void goodbyeScreen() {
    clearScreen();
    cout << "\n\n";
    cout << "  ╔═══════════════════════════════════════════╗\n";
    cout << "  ║                                           ║\n";
    cout << "  ║        Thank you for using CSMS!          ║\n";
    cout << "  ║         Session ended securely.           ║\n";
    cout << "  ║                                           ║\n";
    cout << "  ╚═══════════════════════════════════════════╝\n\n";
}

void checkPasswordStrength() {
    string pwd;
    cout << "\n  Enter password to check: ";
    cin >> pwd;

    int score = 0;
    bool hasUpper = false, hasLower = false,
         hasDigit = false, hasSpecial = false;

    for (char c : pwd) {
        if (isupper(c))  hasUpper   = true;
        if (islower(c))  hasLower   = true;
        if (isdigit(c))  hasDigit   = true;
        if (ispunct(c))  hasSpecial = true;
    }

    if (pwd.length() >= 8)  score++;
    if (pwd.length() >= 12) score++;
    if (hasUpper)   score++;
    if (hasLower)   score++;
    if (hasDigit)   score++;
    if (hasSpecial) score++;

    cout << "\n  ── Password Strength Report ──\n";
    cout << "  Length 8+:        " << (pwd.length() >= 8  ? "✓" : "✗") << "\n";
    cout << "  Length 12+:       " << (pwd.length() >= 12 ? "✓" : "✗") << "\n";
    cout << "  Uppercase letter: " << (hasUpper   ? "✓" : "✗") << "\n";
    cout << "  Lowercase letter: " << (hasLower   ? "✓" : "✗") << "\n";
    cout << "  Number:           " << (hasDigit   ? "✓" : "✗") << "\n";
    cout << "  Special char:     " << (hasSpecial ? "✓" : "✗") << "\n";
    printSeparator();

    if      (score <= 2) cout << "  Rating: WEAK     — please choose a stronger password.\n";
    else if (score <= 4) cout << "  Rating: MODERATE — consider adding more variety.\n";
    else                 cout << "  Rating: STRONG   — great password!\n";
}

// ── Menus ──
void showGuestMenu() {
    cout << "\n  [Account]\n";
    cout << "  1. Register\n";
    cout << "  2. Login\n";
    cout << "  3. Check Password Strength\n";
    cout << "  0. Exit\n";
}

void showUserMenu() {
    cout << "\n  [Account]\n";
    cout << "  1. Logout\n";
    cout << "  2. Check Password Strength\n";
    cout << "\n  [File Manager]\n";
    cout << "  3. Encrypt a File\n";
    cout << "  4. Decrypt a File\n";
    cout << "  5. Check File Integrity\n";
    cout << "  6. List Tracked Files\n";
    cout << "  7. View File Contents\n";
    cout << "  8. Edit File Contents\n";
    cout << "  0. Exit\n";
}

void showAdminMenu() {
    cout << "\n  [Account]\n";
    cout << "  1. Logout\n";
    cout << "  2. Check Password Strength\n";
    cout << "\n  [File Manager]\n";
    cout << "  3. Encrypt a File\n";
    cout << "  4. Decrypt a File\n";
    cout << "  5. Check File Integrity\n";
    cout << "  6. List Tracked Files\n";
    cout << "  7. View File Contents\n";
    cout << "  8. Edit File Contents\n";
    cout << "\n  [Admin Panel]\n";
    cout << "  9. Open Admin Panel\n";
    cout << "  0. Exit\n";
}

int main() {
    welcomeScreen();

    Encryption* cipher = new XORCipher('#');
    AuthSystem auth(cipher, "users.txt");
    FileManager fm(cipher);
    AdminPanel admin(auth, fm);

    int choice;
    do {
        clearScreen();
        printBanner();
        printSessionBar();
        printSeparator();

        bool loggedIn = !AdminPanel::getCurrentUser().empty();
        bool isAdmin  = AdminPanel::isAdminLoggedIn();

        // Show the right menu based on session state
        if (!loggedIn)       showGuestMenu();
        else if (isAdmin)    showAdminMenu();
        else                 showUserMenu();

        cout << "\n  Choice: ";
        cin >> choice;

        // ── Guest actions ──
        if (!loggedIn) {
            if (choice == 1) {
                string uname, pwd; int role;
                cout << "\n  Username: "; cin >> uname;
                cout << "  Password: "; cin >> pwd;
                cout << "  Register as Admin? (1=Yes, 0=No): "; cin >> role;
                auth.registerUser(uname, pwd, role == 1);

            } else if (choice == 2) {
                string uname, pwd;
                cout << "\n  Username: "; cin >> uname;
                cout << "  Password: "; cin >> pwd;
                int result = auth.loginUser(uname, pwd);
                if (result == 1) AdminPanel::setSession(uname, false);
                if (result == 2) AdminPanel::setSession(uname, true);

            } else if (choice == 3) {
                checkPasswordStrength();
            }

        // ── Logged-in actions (shared by user & admin) ──
        } else {
            if (choice == 1) {
                cout << "  [+] Logged out: " << AdminPanel::getCurrentUser() << "\n";
                AdminPanel::clearSession();

            } else if (choice == 2) {
                checkPasswordStrength();

            } else if (choice == 3) {
                string fname;
                cout << "\n  Filename to encrypt: "; cin >> fname;
                fm.encryptFile(fname);

            } else if (choice == 4) {
                string fname;
                cout << "\n  Filename to decrypt: "; cin >> fname;
                fm.decryptFile(fname);

            } else if (choice == 5) {
                string fname;
                cout << "\n  Filename to check: "; cin >> fname;
                fm.checkIntegrity(fname);

            } else if (choice == 6) {
                fm.listFiles();

            } else if (choice == 7) {
                string fname;
                cout << "\n  Filename to view: "; cin >> fname;
                fm.viewFile(fname);

            } else if (choice == 8) {
                string fname;
                cout << "\n  Filename to edit: "; cin >> fname;
                fm.editFile(fname);

            } else if (choice == 9 && isAdmin) {
                admin.showPanel();
            }
        }

        if (choice != 0) {
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
        }

    } while (choice != 0);

    AdminPanel::clearSession();
    goodbyeScreen();
    delete cipher;
    return 0;
}