#ifndef SOCIAL_NETWORK_H
#define SOCIAL_NETWORK_H

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct User // vertex
{
    int id;
    string username;
    string password;

    User() : id(-1), username("default"), password("default") {}
    User(int _id, const string &name, const string &pass) : id(_id), username(name), password(pass) {}
};

struct Edge // "friendship" between two users
{
    int firstId;
    int secondId;

    Edge() : firstId(-1), secondId(-1) {}
    Edge(int id1, int id2) : firstId(id1), secondId(id2) {}
};

struct Post
{
    int userId;
    string content;
    string datetime;

    Post() : userId(-1), content("default"), datetime(getCurrentDateTime()) {}
    Post(int id, string _content) : userId(id), content(_content), datetime(getCurrentDateTime()) {}

private:
    string getCurrentDateTime()
    {
        auto now = chrono::system_clock::now();
        time_t now_datetime = chrono::system_clock::to_time_t(now);
        tm *local_time = localtime(&now_datetime);

        stringstream ss;
        ss << put_time(local_time, "%Y-%m-%d %H:%M");
        return ss.str();
    }
};

class SocialNetwork // Graph Representation
{
private:
    unordered_map<int, User> vertices;      // <userId, User>
    unordered_map<int, vector<Edge>> edges; // <userId, vector of edges>, adjacent list
    unordered_map<int, vector<Post>> posts;

    int userIdCounter = 0;

public:
    SocialNetwork() = default;
    User currentUser;
    bool isLoggedIn = false;

    int generateUserId()
    {
        return ++userIdCounter;
    }

    User &addUser(const string &username, const string &password) // O(1)
    {
        int newId = generateUserId();
        User newUser(newId, username, password);
        vertices[newId] = newUser;
        return vertices[newId];
    }

    void addEdge(const string &username1, const string &username2) // adding edge using strings O(n)
    {
        int id1 = -1;
        int id2 = -1;
        for (auto itr = vertices.begin(); itr != vertices.end(); itr++) // O(n)
        {
            if (itr->second.username == username1)
                id1 = itr->first;
            if (itr->second.username == username2)
                id2 = itr->first;
        }
        if (id1 == -1 || id2 == -1)
            cout << "\nUsername not found.\n";
        else
            addEdge(id1, id2);
    }

    void addEdge(int id1, int id2) // adding edge using id O(1)
    {
        if (vertices.find(id1) == vertices.end() && vertices.find(id2) == vertices.end()) // checking is O(1) since it's an unordered map
        {
            cout << "\nOne or both users do not exist.\n";
            return;
        }

        edges[id1].emplace_back(id1, id2); // 0(1), emplace_back creates the instance/object on the spot
        edges[id2].emplace_back(id2, id1);
    }

    vector<int> findAdjacentUsers(int userId) // find friends, O(n)
    {
        vector<int> adjacentUsers;
        if (vertices.find(userId) == vertices.end())
        {
            std::cout << "\nUser does not exist.\n";
            return adjacentUsers;
        }

        int size = edges[userId].size();
        // cout << endl
        //      << vertices[userId].username << "'s Friends \n\n";
        for (int i = 0; i < size; i++)
        {
            int friendId = edges[userId][i].secondId;
            // cout << i + 1 << ". " << vertices[friendId].username << endl;
            adjacentUsers.push_back(friendId);
        }
        // cout << endl;
        return adjacentUsers;
    }

    void printFriends(int userId) // same as findAdjacentUsers() but for printing
    {
        if (vertices.find(userId) == vertices.end())
        {
            std::cout << "\nUser does not exist.\n";
            return;
        }
        int size = edges[userId].size();
        cout << "\n-- " << vertices[userId].username << "'s Friends --\n\n";
        for (int i = 0; i < size; i++)
        {
            int friendId = edges[userId][i].secondId;
            cout << i + 1 << ". " << vertices[friendId].username << endl;
        }
        cout << endl;
        return;
    }

    bool areFriends(int id1, int id2) // check if friends, O(n)
    {
        if (vertices.find(id1) == vertices.end() && vertices.find(id2) == vertices.end())
        {
            cout << "\nOne or both users do not exist.\n";
            return false;
        }

        int size = edges[id1].size();
        for (int i = 0; i < size; i++)
        {
            if (edges[id1][i].secondId == id2)
                return true;
        }
        return false;
    }

    void createPost(int userId, string content)
    {
        if (vertices.find(userId) == vertices.end())
        {
            cout << "\nUser does not exist\n";
            return;
        }
        posts[userId].emplace(posts[userId].begin(), userId, content);
    }

    void generateFeed(int userId)
    {
        vector<int> friends = findAdjacentUsers(userId);
        cout << "\n-- SOCIAL NETWORK FEED --\n\n";

        for (int i = 0; i < posts[userId].size(); i++)
        {
            cout << vertices[userId].username << endl
                 << posts[userId][i].datetime << endl
                 << posts[userId][i].content << endl
                 << endl;
        }

        for (int i = 0; i < friends.size(); i++)
        {
            int friendId = friends[i];
            int totalPosts = posts[friendId].size();

            for (int j = 0; j < totalPosts; j++)
            {
                cout << vertices[friendId].username << endl
                     << posts[friendId][j].datetime << endl
                     << posts[friendId][j].content << endl
                     << endl;
            }
        }
    }

    void printAllUsers()
    {
        cout << "\n-- All Social Network Users -- \n\n";

        int i = 1;
        for (auto itr = vertices.begin(); itr != vertices.end(); itr++)
        {
            cout << i << ". " << itr->second.username << endl;
            i++;
        }
        cout << endl;
    }

    void save()
    {
        ofstream usersFile, edgesFile, postsFile;

        usersFile.open("usersDB.txt");
        for (auto itr = vertices.begin(); itr != vertices.end(); itr++)
        {
            usersFile << itr->first << endl
                      << itr->second.username << endl
                      << itr->second.password << endl;
        }
        usersFile.close();

        edgesFile.open("edgesDB.txt");
        for (auto itr = edges.begin(); itr != edges.end(); itr++)
        {
            int vectorSize = itr->second.size();
            for (int i = 0; i < vectorSize; i++)
            {
                int id1 = itr->second[i].firstId;
                int id2 = itr->second[i].secondId;
                edgesFile << id1 << " " << id2 << endl;
            }
        }
        edgesFile.close();

        postsFile.open("postsDB.txt");
        for (auto itr = posts.begin(); itr != posts.end(); itr++)
        {
            int vectorSize = itr->second.size();
            for (int i = 0; i < vectorSize; i++)
            {
                int id = itr->second[i].userId;
                string content = itr->second[i].content;
                string datetime = itr->second[i].datetime;

                postsFile << id << endl
                          << content << endl
                          << datetime << endl;
            }
        }
        postsFile.close();
    }

    void retrieve()
    {
        ifstream usersFile, edgesFile, postsFile;

        usersFile.open("usersDB.txt");
        if (!usersFile.is_open())
        {
            cerr << "Error: Could not open file usersDB.txt" << endl;
            cout << "Ignore this message if this is the first time running the program.\n";
            return;
        }
        string userId, username, password;
        while (getline(usersFile, userId))
        {
            getline(usersFile, username);
            getline(usersFile, password);

            vertices.emplace(stoi(userId), User(stoi(userId), username, password));
        }
        usersFile.close();

        edgesFile.open("edgesDB.txt");
        if (!edgesFile.is_open())
        {
            cerr << "Error: Could ont open file edgesDB.txt" << endl;
            cout << "Ignore this message if this is the first time running the program.\n";
            return;
        }
        int id1, id2;
        while (edgesFile >> id1 >> id2)
        {
            edges[id1].push_back(Edge(id1, id2));
        }
        edgesFile.close();

        postsFile.open("postsDB.txt");
        if (!postsFile.is_open())
        {
            cerr << "Error: Could ont open file postsDB.txt" << endl;
            cout << "Ignore this message if this is the first time running the program.\n";
            return;
        }
        int id;
        string content, datetime;
        while (postsFile >> id)
        {
            postsFile.ignore();
            getline(postsFile, content);
            getline(postsFile, datetime);

            posts[id].push_back(Post(id, content));
        }
        postsFile.close();
        cout << "All data from files successfully retrieved.\n";
    }

    // Getter Methods
    const unordered_map<int, User> &getAllUsers() // returning a const reference. const ensures that the vertices won't be modified outside the class. The & reference makes the returned value an alias to the original object, it is an address underneath the hood but it is NOT to be used like a pointer.
    {
        return vertices;
    }
};

#endif