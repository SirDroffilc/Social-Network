#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include "socialNetwork.h"

using namespace std;

SocialNetwork NETWORK;

const int USERNAME_LIMIT = 15;
const int PASSWORD_LIMIT = 50;

void home();
void authPage();

void postToFeed();

bool signUp();
bool logIn();
void addFriend();

bool isValidUsername(string username);
bool isValidPassword(string password);

int getInteger();
string getYesOrNo();
User getUserFriend();
string getContent();

int main()
{
    NETWORK.retrieve();
    system("pause");
    authPage();
    NETWORK.save();
    return 0;
}

void home()
{
    while (true)
    {
        system("cls");
        cout << "WELCOME, " << NETWORK.currentUser.username << "!\n\n";
        cout << "1. Go to Feed\n";
        cout << "2. See Friends\n";
        cout << "3. See All Users\n";
        cout << "4. Log Out\n\n";

        int action = getInteger();
        cin.ignore();

        switch (action)
        {
        case 1:
        {
            system("cls");
            NETWORK.generateFeed(NETWORK.currentUser.id);
            postToFeed();
            break;
        }
        case 2:
        {
            system("cls");
            NETWORK.printFriends(NETWORK.currentUser.id);
            system("pause");
            break;
        }
        case 3:
        {
            system("cls");
            NETWORK.printAllUsers();
            addFriend();
            system("pause");
            break;
        }
        case 4:
            return;
        default:
            continue;
        }
    }
}

void authPage()
{
    while (true)
    {
        system("cls");
        cout << "\n-- BASIC SOCIAL NETWORK IMPLEMENTATION --\n-- by 4rd\n\n";
        cout << "1. Sign Up\n";
        cout << "2. Log In\n";
        cout << "3. Exit Social Network\n\n";
        int action = getInteger();
        cin.ignore();

        switch (action)
        {
        case 1:
        {
            bool signUpSuccessful = signUp();
            if (signUpSuccessful)
            {
                NETWORK.isLoggedIn = true;
                home();
            }
            break;
        }

        case 2:
        {
            bool logInSuccessful = logIn();
            if (logInSuccessful)
            {
                NETWORK.isLoggedIn = true;
                home();
            }
            break;
        }

        case 3:
            return;

        default:
            continue;
        }
    }
}

void postToFeed()
{
    cout << "Share your thoughts? ";
    string answer = getYesOrNo();
    if (answer == "no")
        return;

    string content;
    cout << "What's on your mind? \n> ";
    content = getContent();
    NETWORK.createPost(NETWORK.currentUser.id, content);
}

User getUserFriend()
{
    const unordered_map<int, User> &allUsers = NETWORK.getAllUsers();

    string username;
    while (true)
    {
        cout << "Enter the name of the user you want to add: ";
        getline(cin, username);
        if (username == NETWORK.currentUser.username)
        {
            cout << "Got you. You can't add yourself.\n";
            continue;
        }
        for (auto itr = allUsers.begin(); itr != allUsers.end(); itr++)
        {
            if (username == itr->second.username)
                return itr->second;
        }
        cout << "Username not found.\n";
    }
}

void addFriend()
{
    cout << "Add a friend? ";
    string answer = getYesOrNo();

    if (answer == "yes")
    {
        User newFriend = getUserFriend();
        if (NETWORK.areFriends(NETWORK.currentUser.id, newFriend.id))
            cout << "You are already friends with " << newFriend.username << endl;
        else
        {
            NETWORK.addEdge(NETWORK.currentUser.id, newFriend.id);
            cout << "You are now friends with " << newFriend.username << "!\n";
        }
        return;
    }
    else
        return;
}

bool logIn()
{
    string username, password;
    system("cls");
    cout << "\n-- LOG IN -- \n";
    cout << "\nUsername: ";
    getline(cin, username);
    cout << "\nPassword: ";
    getline(cin, password);

    const unordered_map<int, User> &allUsers = NETWORK.getAllUsers();
    for (auto itr = allUsers.begin(); itr != allUsers.end(); itr++) // O(n)
    {
        if (username == itr->second.username && password == itr->second.password)
        {
            NETWORK.currentUser = itr->second;
            return true;
        }
    }

    cout << "Incorrect account detail/s.\n";
    system("pause");
    return false;
}

bool signUp()
{
    string username, password;
    system("cls");
    cout << "\n-- SIGN UP -- \n";
    cout << "\nUsername: ";
    getline(cin, username);
    if (!isValidUsername(username))
    {
        system("pause");
        return false;
    }

    cout << "\nPassword: ";
    getline(cin, password);
    if (!isValidPassword(password))
    {
        system("pause");
        return false;
    }

    NETWORK.currentUser = NETWORK.addUser(username, password);
    system("pause");
    return true;
}

bool isValidUsername(string username)
{
    const unordered_map<int, User> &allUsers = NETWORK.getAllUsers();
    if (username.length() < 3 || username.length() > USERNAME_LIMIT)
    {
        cout << "Username must be 3-" << USERNAME_LIMIT << " characters.\n";
        return false;
    }

    for (auto itr = allUsers.begin(); itr != allUsers.end(); itr++)
    {
        if (username == itr->second.username)
        {
            cout << "Username is already taken.\n";
            return false;
        }
    }
    return true;
}

bool isValidPassword(string password)
{
    if (password.length() < 8 || password.length() > PASSWORD_LIMIT)
    {
        cout << "Password must be 8-" << PASSWORD_LIMIT << " characters.\n";
        return false;
    }
    return true;
}

int getInteger()
{
    int action;
    while (true)
    {
        cout << "Action: ";
        if (cin >> action)
        {
            return action;
        }
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
        }
    }
}

string getYesOrNo()
{
    string answer;
    while (true)
    {
        cout << "(yes/no): ";
        getline(cin, answer);
        if (answer == "yes" || answer == "no")
            return answer;
        else
        {
            cout << "Please enter 'yes' or 'no' only.\n";
        }
    }
}

string getContent()
{
    string content;
    while (true)
    {
        getline(cin, content);
        if (content.length() > 5000 || content.length() < 1)
            cout << "Content should be 1-5000 characters\n";
        else
            break;
    }
    return content;
}