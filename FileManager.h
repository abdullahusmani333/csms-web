#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Encryption.h"
#include "Logger.h"
#include <string>
#include <vector>
using namespace std;

//  STRUCT: FileRecord
struct FileRecord {
    string filename;
    string storedHash;      // hash taken BEFORE encryption (to verify integrity later)
    bool isEncrypted;

    // Operator Overloading 
    friend ostream& operator<<(ostream& os, const FileRecord& r);
};


//  CLASS: FileManager
//  COMPOSITION: FileManager HAS-A Logger
//                            HAS-A Encryption*
//
//  It REUSES our existing Encryption classes
class FileManager {
private:
    Encryption* cipher;         // reusing our existing encryption (polymorphism!)
    Logger logger;              // composition: FileManager has its own logger
    vector<FileRecord> records; // tracks all files managed by this system
    string recordFile;          // file where we persist the records list

    // Internal helpers
    string readFile(const string& filename) const;
    void writeFile(const string& filename, const string& content) const;
    string computeHash(const string& content) const;  // simple hash for integrity
    FileRecord* findRecord(const string& filename);
    void saveRecords() const;
    void loadRecords();

public:
    FileManager(Encryption* enc, string recFile = "filerecords.txt");

    // Core features
    bool encryptFile(const string& filename);
    bool decryptFile(const string& filename);
    bool checkIntegrity(const string& filename);
    void listFiles() const;
    void viewFile(const string& filename);
    void editFile(const string& filename);
};

#endif
