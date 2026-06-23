#ifndef LOGGER_H
#define LOGGER_H

#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  ENUM: LogLevel
//  Defines the "severity" of a log entry.
//  INFO = normal activity
//  WARNING = suspicious (e.g. wrong password)
//  CRITICAL = serious (e.g. account locked)
// ─────────────────────────────────────────────
enum class LogLevel {
    INFO,
    WARNING,
    CRITICAL
};


// ─────────────────────────────────────────────
//  CLASS: Logger
//
//  NEW OOP CONCEPT: Composition
//  AuthSystem will have a Logger as a member.
//  "AuthSystem HAS-A Logger" (not inherits from)
//
//  This is different from inheritance:
//  Inheritance = "IS-A" (Admin IS-A User)
//  Composition = "HAS-A" (AuthSystem HAS-A Logger)
// ─────────────────────────────────────────────
class Logger {
private:
    string logFile;       // file where logs are saved
    bool consoleOutput;   // whether to also print to screen

    // Converts LogLevel enum to a readable string
    string levelToString(LogLevel level) const;

    // Gets the current date & time as a string
    string getCurrentTimestamp() const;

public:
    // Constructor
    Logger(string filename = "logs.txt", bool printToConsole = true);

    // Core logging method — all other methods call this
    void log(LogLevel level, const string& event, const string& username = "");

    // Convenience wrappers for cleaner code in AuthSystem
    void logInfo(const string& event, const string& username = "");
    void logWarning(const string& event, const string& username = "");
    void logCritical(const string& event, const string& username = "");

    // Display all logs (Admin feature)
    void displayLogs() const;

    // Clear the log file
    void clearLogs();
};

#endif