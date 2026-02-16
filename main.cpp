#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// --- Constants ---
const int MAX_ITEMS = 500;
const int MAX_USERS = 100;

// --- Helper Functions ---
string extractValue(string s) {
    size_t first = s.find("'");
    size_t last = s.rfind("'");
    if (first != string::npos && last != string::npos && last > first) {
        return s.substr(first + 1, last - first - 1);
    }
    return s;
}


class Item {
private:
    int id; 
    string title;
    string genre;
    string type;
    double totalRating;
    int ratingCount;

public:
    // Constructors
    Item() {
        id = 0;
        title = "";
        genre = "";
        type = "";
        totalRating = 0.0;
        ratingCount = 0;
    }

    // Setters
    void setId(int i) {
        id = i;
    }
    void setTitle(string t) {
        title = t;
    }
    void setGenre(string g) {
        genre = g;
    }
    void setType(string t) {
        type = t;
    }

    // Getters
    string getTitle() const {
        return title;
    }
    string getGenre() const {
        return genre;
    }
    string getType() const {
        return type;
    }
    int getId() const {
        return id;
    }
    
    void addRating(double r) {
        totalRating += r;
        ratingCount++;
    }

    double getAverageRating() const {
        if (ratingCount == 0) return 0.0;
        return totalRating / ratingCount;
    }

    friend ostream& operator<<(ostream& os, const Item& item);
};

ostream& operator<<(ostream& os, const Item& item) {
    os << "[" << item.type << "] " << item.title << " (Rating: " << item.getAverageRating() << ")";
    return os;
}


class Person {
protected:
    string name;
    string city;

public:
    Person() {
        name = "";
        city = "";
    }
    Person(string n, string c) {
        name = n;
        city = c;
    }
    
    string getName() const {
        return name;
    }
    string getCity() const {
        return city;
    }
};

class User : public Person {
private:
    int id;
    string ageGroup;

public:
    User() {
        person();
        id = 0;
        agegroup = "";
    }
    User(int i, string n, string c, string a) : Person(n, c), id(i), ageGroup(a) {}

    int getId() const { return id; }

    bool operator==(const User& other) const {
        return this->name == other.name;
    }
};

class RecommendationSystem {
private:
    Item items[MAX_ITEMS];
    int itemCount;
    User users[MAX_USERS];
    int userCount;

public:
    RecommendationSystem() {
        itemCount = 0;
        userCount = 0;
    }

    void loadData() {
        ifstream file("Sample Database.sql");
        if (!file.is_open()) {
            cout << "Error: Could not open Database" << endl;
            return;
        }

        string line;
        int mode = 0;

        while (getline(file, line)) {
            if (line.find("INSERT INTO Users") != string::npos) {
                mode = 1;
                continue;
            } else if (line.find("INSERT INTO Items") != string::npos) {
                mode = 2;
                continue;
            } else if (line.find("INSERT INTO Interactions") != string::npos) {
                mode = 3;
                continue;
            }

            if (mode == 1) {
                 size_t startPos = line.find("(");
                 size_t endPos = line.find(")");
                 if (startPos != string::npos && endPos != string::npos) {
                    string content = line.substr(startPos + 1, endPos - startPos - 1);

                    size_t c1 = content.find(",");
                    size_t c2 = content.find(",", c1 + 1);
                    if (c1 != string::npos && c2 != string::npos) {
                        string u = extractValue(content.substr(0, c1));
                        string c = extractValue(content.substr(c1 + 1, c2 - c1 - 1));
                        string a = extractValue(content.substr(c2 + 1));
                        
                        if (userCount < MAX_USERS) {
                            users[userCount] = User(userCount + 1, u, c, a);
                            userCount++;
                        }
                    }
                 }
            }

            else if (mode == 2) { // Items
                size_t startPos = line.find("(");
                size_t endPos = line.find(")");
                if (startPos != string::npos && endPos != string::npos) {
                    string content = line.substr(startPos + 1, endPos - startPos - 1);
                    size_t c1 = content.find(",");
                    size_t c2 = content.find(",", c1 + 1);
                    if (c1 != string::npos && c2 != string::npos) {
                        string t = extractValue(content.substr(0, c1));
                        string g = extractValue(content.substr(c1 + 1, c2 - c1 - 1));
                        string Ty = extractValue(content.substr(c2 + 1));
                        
                        if (itemCount < MAX_ITEMS) {
                            items[itemCount].setId(itemCount + 1); 
                            items[itemCount].setTitle(t);
                            items[itemCount].setGenre(g);
                            items[itemCount].setType(Ty);
                            itemCount++; 
                        }
                    }
                }
            }

            else if (mode == 3) { // Interactions
                size_t pos = 0;
                while ((pos = line.find("(", pos)) != string::npos) {
                    size_t end = line.find(")", pos);
                    if (end == string::npos) break;
                    string content = line.substr(pos + 1, end - pos - 1);
                    stringstream ss(content);
                    string val1, val2, val3;
                    getline(ss, val1, ','); 
                    getline(ss, val2, ','); 
                    getline(ss, val3, ','); 
                    if (!val2.empty() && !val3.empty()) {
                        int i_id = stoi(val2);
                        double rate = stod(val3);
                        if (i_id > 0 && i_id <= itemCount) {
                            items[i_id - 1].addRating(rate);
                        }
                    }
                    pos = end + 1;
                }
            }

            if (line.find(";") != string::npos) mode = 0; 
        }
        file.close();
        cout << "System Loaded: " << itemCount << " Items and " << userCount << " Users." << endl;
    }

    User* login(string username) {
        User temp(0, username, "", "");
        for(int i=0; i<userCount; i++) {
            if (users[i] == temp) {
                return &users[i];
            }
        }
        return NULL;
    }


    void recommend(string genre) {
        cout << "Recommendations for Genre: " << genre << endl;
        bool found = false;
        for (int i = 0; i < itemCount; i++) {
            if (items[i].getGenre() == genre) {
                // Using operator<<
                cout << " - " << items[i] << endl;
                found = true;
            }
        }
        if (!found) cout << "No items found." << endl;
    }


    void recommend(User* user) {
        cout << "\nPersonalized Recommendations for " << user->getName() << " ---" << endl;
        cout << "Because you are from " << user->getCity() << "..." << endl;
        showTopPicks();
    }

    void showTopPicks() {
        Item tempItems[MAX_ITEMS]; 
        int count = itemCount;
        for(int i=0; i<count; i++) tempItems[i] = items[i];
        
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < count - 1; j++) {
                if (tempItems[j].getAverageRating() < tempItems[j+1].getAverageRating()) {
                    Item temp = tempItems[j];
                    tempItems[j] = tempItems[j+1];
                    tempItems[j+1] = temp;
                }
            }
        }
        
        for (int i = 0; i < 3 && i < count; i++) {
            cout << (i+1) << ". " << tempItems[i] << endl;
        }
    }
};


int main() {
    cout << "-- Movie Recommendation System --" << endl;
    RecommendationSystem sys;
    sys.loadData();
    
    cout << "\n-- Login --" << endl;
    string username;
    cout << "Enter Username (e.g., Ahmed_Khan): ";
    getline(cin, username);

    User* currentUser = sys.login(username);

    if (currentUser != NULL) {
        cout << "Login Successful! Welcome, " << currentUser->getName() << "." << endl;
        
        // Use Overloaded Function 2
        sys.recommend(currentUser);

        // Interactive Genre Search
        cout << "\nWould you like to browse by genre? (yes/no): ";
        string choice;
        getline(cin, choice);
        if (choice == "Y" ) {
             cout << "Enter Genre: ";
             string g;
             getline(cin, g);
             // Use Overloaded Function 1
             sys.recommend(g);
        }

    } else {
        cout << "User not found! Please register or check spelling." << endl;
    }

    return 0;
}

