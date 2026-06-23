// ════════════════════════════════════════════════
//  FileManager.cpp
// ════════════════════════════════════════════════
#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// ── Operator overload: how to print a FileRecord ──
ostream& operator<<(ostream& os, const FileRecord& r) {
    os << "  File:      " << r.filename << "\n"
       << "  Encrypted: " << (r.isEncrypted ? "Yes" : "No") << "\n"
       << "  Hash:      " << r.storedHash << "\n";
    return os;
}

// ── Constructor ──
FileManager::FileManager(Encryption* enc, string recFile)
    : cipher(enc), logger("logs.txt", true), recordFile(recFile) {
    loadRecords();
}

// ── Read entire file into a string ──
string FileManager::readFile(const string& filename) const {
    ifstream file(filename);
    if (!file.is_open()) return "";
    // Read the whole file at once using stream iterators
    return string((istreambuf_iterator<char>(file)),
                   istreambuf_iterator<char>());
}

// ── Write a string into a file ──
void FileManager::writeFile(const string& filename, const string& content) const {
    ofstream file(filename);
    file << content;
}

// ── Simple hash: sum of (char value * position) ──
// Not cryptographic — just enough to detect tampering.
// Real systems use SHA-256 etc., but this demonstrates the concept.
string FileManager::computeHash(const string& content) const {
    unsigned long hash = 0;
    for (size_t i = 0; i < content.size(); i++) {
        hash += (unsigned char)content[i] * (i + 1);
    }
    // Convert to hex string
    stringstream ss;
    ss << hex << hash;
    return ss.str();
}

// ── Find a record by filename ──
FileRecord* FileManager::findRecord(const string& filename) {
    for (FileRecord& r : records) {
        if (r.filename == filename) return &r;
    }
    return nullptr;
}

// ── Persist records to file ──
void FileManager::saveRecords() const {
    ofstream file(recordFile);
    for (const FileRecord& r : records) {
        file << r.filename << "|" << r.storedHash << "|" << r.isEncrypted << "\n";
    }
}

// ── Load records from file ──
void FileManager::loadRecords() {
    ifstream file(recordFile);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        FileRecord r;
        string encStr;
        getline(ss, r.filename, '|');
        getline(ss, r.storedHash, '|');
        getline(ss, encStr, '|');
        r.isEncrypted = (encStr == "1");
        records.push_back(r);
    }
}

// ── Encrypt a file ──
bool FileManager::encryptFile(const string& filename) {
    FileRecord* existing = findRecord(filename);
    if (existing && existing->isEncrypted) {
        cout << "  [!] File is already encrypted.\n";
        return false;
    }

    string content = readFile(filename);
    if (content.empty()) {
        // File doesn't exist — offer to create it
        cout << "  [!] File not found: " << filename << "\n";
        cout << "  [?] Would you like to create it? (1=Yes, 0=No): ";
        int create; cin >> create;
        if (create != 1) return false;

        // Read content line by line until user types END on its own line
        cout << "  [>] Enter file content (type END on a new line when done):\n";
        cin.ignore();
        string line, allContent;
        while (getline(cin, line)) {
            if (line == "END") break;
            allContent += line + "\n";
        }
        if (allContent.empty()) {
            cout << "  [!] No content entered. File not created.\n";
            return false;
        }
        writeFile(filename, allContent);
        cout << "  [+] File created: " << filename << "\n";
        content = allContent;
    }

    // Store hash of ORIGINAL content before encrypting
    string hash = computeHash(content);
    string encrypted = cipher->encrypt(content);  // POLYMORPHISM: works for XOR or Caesar
    writeFile(filename, encrypted);

    if (existing) {
        existing->storedHash = hash;
        existing->isEncrypted = true;
    } else {
        FileRecord r;
        r.filename = filename;
        r.storedHash = hash;
        r.isEncrypted = true;
        records.push_back(r);
    }

    saveRecords();
    logger.logInfo("File encrypted", filename);
    cout << "  [+] File encrypted successfully.\n";
    cout << "  [+] Integrity hash stored: " << hash << "\n";
    return true;
}

// ── Decrypt a file ──
bool FileManager::decryptFile(const string& filename) {
    FileRecord* record = findRecord(filename);
    if (!record || !record->isEncrypted) {
        cout << "  [!] File is not encrypted or not tracked.\n";
        return false;
    }

    string content = readFile(filename);
    if (content.empty()) {
        cout << "  [!] File not found: " << filename << "\n";
        return false;
    }

    string decrypted = cipher->decrypt(content);
    writeFile(filename, decrypted);
    record->isEncrypted = false;
    saveRecords();

    logger.logInfo("File decrypted", filename);
    cout << "  [+] File decrypted successfully.\n";
    return true;
}

// ── Check file integrity ──
bool FileManager::checkIntegrity(const string& filename) {
    FileRecord* record = findRecord(filename);
    if (!record) {
        cout << "  [!] No record found for: " << filename << "\n";
        return false;
    }

    // To check integrity we need the plain content.
    // If encrypted, decrypt temporarily in memory (don't write to disk).
    string content = readFile(filename);
    if (content.empty()) {
        cout << "  [!] File not found: " << filename << "\n";
        return false;
    }

    string plainContent = record->isEncrypted ? cipher->decrypt(content) : content;
    string currentHash = computeHash(plainContent);

    if (currentHash == record->storedHash) {
        cout << "  [✓] Integrity check PASSED — file is unchanged.\n";
        logger.logInfo("Integrity check passed", filename);
        return true;
    } else {
        cout << "  [X] Integrity check FAILED — file may have been tampered with!\n";
        cout << "      Expected: " << record->storedHash << "\n";
        cout << "      Found:    " << currentHash << "\n";
        logger.logCritical("Integrity check FAILED - possible tampering", filename);
        return false;
    }
}

// ── List all tracked files ──
void FileManager::listFiles() const {
    if (records.empty()) {
        cout << "  [!] No files tracked yet.\n";
        return;
    }
    cout << "\n  ── Tracked Files ──\n";
    for (const FileRecord& r : records) {
        cout << r;  // uses our overloaded << operator
        cout << "  ──────────────────\n";
    }
}

// ── View file contents ──
void FileManager::viewFile(const string& filename) {
    FileRecord* record = findRecord(filename);
    string content = readFile(filename);

    if (content.empty()) {
        cout << "  [!] File not found: " << filename << "\n";
        logger.logWarning("View failed - file not found", filename);
        return;
    }

    // If encrypted, decrypt in memory only — file on disk stays encrypted
    string display = (record && record->isEncrypted)
                     ? cipher->decrypt(content)
                     : content;

    cout << "\n  ── Contents of " << filename << " ──\n";
    cout << display;
    cout << "  ────────────────────────────────\n";
    logger.logInfo("File viewed", filename);
}

// ── Edit file contents ──
void FileManager::editFile(const string& filename) {
    FileRecord* record = findRecord(filename);
    string content = readFile(filename);

    if (content.empty()) {
        cout << "  [!] File not found: " << filename << "\n";
        return;
    }

    // Show current contents first (decrypted if needed)
    string current = (record && record->isEncrypted)
                     ? cipher->decrypt(content)
                     : content;

    cout << "\n  ── Current contents of " << filename << " ──\n";
    cout << current;
    cout << "  ────────────────────────────────\n";

    // Get new content
    cout << "  [>] Enter new content (type END on a new line when done):\n";
    cin.ignore();
    string line, newContent;
    while (getline(cin, line)) {
        if (line == "END") break;
        newContent += line + "\n";
    }

    if (newContent.empty()) {
        cout << "  [!] No content entered. File unchanged.\n";
        return;
    }

    // If file was encrypted, re-encrypt the new content
    if (record && record->isEncrypted) {
        string newHash = computeHash(newContent);
        string encrypted = cipher->encrypt(newContent);
        writeFile(filename, encrypted);
        record->storedHash = newHash;  // update hash for integrity checks
        cout << "  [+] File updated and re-encrypted.\n";
    } else {
        writeFile(filename, newContent);
        if (record) record->storedHash = computeHash(newContent);
        cout << "  [+] File updated.\n";
    }

    saveRecords();
    logger.logInfo("File edited", filename);
}