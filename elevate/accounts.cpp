// ============================================================
//  accounts.cpp
//  Account registration, login, and session management.
//  Passwords are never stored as plain text - only their hash.
// ============================================================

#include "accounts.h"
#include "utils.h"
using namespace std;

Account accounts[MAX_ACCOUNTS];
int     totalAccounts = 0;
int     loggedInIndex = -1;

bool registerAccount(const string& username, const string& password) {
    if (totalAccounts >= MAX_ACCOUNTS) return false;
    if (username.size() < 3 || password.size() < 4) return false;

    // Check for duplicate username using string comparison
    string userLower = username;
    for (char& c : userLower) c = (char)tolower(c);

    for (int i = 0; i < totalAccounts; i++) {
        string existing = accounts[i].username;
        for (char& c : existing) c = (char)tolower(c);
        if (existing == userLower) return false;
    }

    accounts[totalAccounts].username     = username;
    accounts[totalAccounts].passwordHash = hashPassword(password);
    accounts[totalAccounts].isAdmin      = (totalAccounts == 0);
    totalAccounts++;
    return true;
}

bool loginAccount(const string& username, const string& password) {
    string userLower = username;
    for (char& c : userLower) c = (char)tolower(c);

    for (int i = 0; i < totalAccounts; i++) {
        string existing = accounts[i].username;
        for (char& c : existing) c = (char)tolower(c);

        if (existing == userLower) {
            if (accounts[i].passwordHash == hashPassword(password)) {
                loggedInIndex = i;
                return true;
            }
            return false;
        }
    }
    return false;
}

void   logoutAccount()        { loggedInIndex = -1; }
bool   isLoggedIn()           { return loggedInIndex >= 0; }
bool   isCurrentUserAdmin()   { return loggedInIndex >= 0 && accounts[loggedInIndex].isAdmin; }
string getCurrentUsername()   { return loggedInIndex >= 0 ? accounts[loggedInIndex].username : "Guest"; }
