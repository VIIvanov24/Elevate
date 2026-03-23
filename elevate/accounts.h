#pragma once
#include <string>
using namespace std;

const int MAX_ACCOUNTS = 100;

struct Account {
    string username;
    string passwordHash;
    bool   isAdmin;
};

extern Account accounts[MAX_ACCOUNTS];
extern int     totalAccounts;
extern int     loggedInIndex;

bool   registerAccount(const string& username, const string& password);
bool   loginAccount(const string& username, const string& password);
void   logoutAccount();
string getCurrentUsername();
bool   isLoggedIn();
bool   isCurrentUserAdmin();
