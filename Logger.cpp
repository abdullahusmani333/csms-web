//  Logger.cpp  —  Logger implementation
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

Logger::Logger(string filename, bool printToConsole)
    : logFile(filename), consoleOutput(printToConsole) {}

// ── Converts enum to readable label ──
string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO:     return "INFO    ";
        case LogLevel::WARNING:  return "WARNING ";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN ";
    }
}

// ── Gets current date and time ──
string Logger::getCurrentTimestamp() const {
    time_t now = time(0);        // gets current time as seconds since 1970
    tm* ltm = localtime(&now);   // converts to local time struct

    // Builds a formatted string: YYYY-MM-DD HH:MM:SS
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buffer);
}

// ── Core log method ──
void Logger::log(LogLevel level, const string& event, const string& username) {
    // Builds the log entry string
    string entry = "[" + getCurrentTimestamp() + "] "
                 + "[" + levelToString(level) + "] ";

    if (!username.empty())
        entry += "<" + username + "> ";

    entry += event;

    // Writes to file (append mode — doesn't overwrite old logs)
    ofstream file(logFile, ios::app);
    if (file.is_open())
        file << entry << "\n";

    // Also prints to console if enabled
    if (consoleOutput)
        cout << "  LOG: " << entry << "\n";
}

// ── Convenience wrappers ──
void Logger::logInfo(const string& event, const string& username) {
    log(LogLevel::INFO, event, username);
}

void Logger::logWarning(const string& event, const string& username) {
    log(LogLevel::WARNING, event, username);
}

void Logger::logCritical(const string& event, const string& username) {
    log(LogLevel::CRITICAL, event, username);
}

// ── Display all logs ──
void Logger::displayLogs() const {
    ifstream file(logFile);
    if (!file.is_open()) {
        cout << "  [!] No log file found.\n";
        return;
    }

    cout << "\n  ── Activity Logs ──\n";
    string line;
    while (getline(file, line))
        cout << "  " << line << "\n";
}

// ── Clear logs ──
void Logger::clearLogs() {
    ofstream file(logFile, ios::trunc);  // trunc = wipe the file
    cout << "  [+] Logs cleared.\n";
}
