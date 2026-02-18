#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

const int MAX_ITEMS = 500;
const int MAX_USERS = 100;

// Helper Function

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
    int ID;
    string Title;
    string Genre;
    string Type;
    double totalRating;
    int ratingCount;

public:

// Constructor
// Parametrised
    Item() {
        ID = 0;
        Title = "";
        Genre = "";
        Type = "";
        totalRating = 0.0;
        ratingCount = 0;
    }
// Setters

    void setID(int i) {
        ID = i; 
    }
    void setTitle(string t) {
        Title = t; 
    }
    void setGenre(string g) {
        Genre = g; 
    }
    void setType(string t)  {
        Type = t; 
    }

// Getters

    string getTitle() const {
        return Title; 
    }
    string getGenre() const {
        return Genre; 
    }
    string getType()  const {
        return Type;
    }
    int getId() const {
        return ID; 
    }

// Rating Logic

    void addRating(double r) {
        totalRating += r;
        ratingCount++;
    }

    double getAverageRating() const {
        if (ratingCount == 0) 
            return 0.0;
        return totalRating / ratingCount;
    }

    friend ostream& operator<<(ostream& os, const Item& item);
};

ostream& operator<<(ostream& os, const Item& item) {
    os << "[" << item.Type << "] " << item.Title
       << " (Rating: " << item.getAverageRating() << ")";
    return os;
}

// Base/Parent Class

class Person {
protected:
    string Name;
    string City;

public:

// Constructors
// Default
    Person(){
        Name = "";
        City = "";
    }

// Parametrised

    Person(string n, string c){
        Name = n;
        City = c;
    }


// Getters
    string getName() const { 
        return Name;
    }
    string getCity() const {
        return City;
    }

// virtual function allows subclasses to provide their own version of display()
    virtual void display() const {
        cout << "Person: " << Name << " from " << City << endl;
    }

    virtual ~Person() {}
};


// Derived/Child Class

class User : public Person {
private:
    int    Id;
    string AgeGroup;

public:

// Constructors
// Default
    User(){
        Person();
        Id = 0;
        AgeGroup = "";
    }

// Parametrised
    User(int id, string n, string c, string a){
            Person(n, c);
            Id = id;
            AgeGroup = a;
        }

// Getter
    int getId() const { 
        return Id; 
    }

// overrides Person::display() is called automatically when using a Person* pointer
    void display() const override {
        cout << "User #" << Id << ": " << Name << " | City: " << City << " | Age Group: " << AgeGroup << endl;
    }

// Making sure only identical objects are considered same
    bool operator==(const User& other) const {
        return this->Name == other.Name;
    }
};

// POLYMORPHISM: abstract base class with a pure virtual function
// Any class that inherits from this must implement its own recommend()
class BaseRecommender {
public:
    virtual void recommend() = 0;
    virtual ~BaseRecommender() {}
};

class GenreRecommender : public BaseRecommender {
private:
    Item*  items;     // Points to the items array in RecommendationSystem
    int    itemCount;
    string genre;

public:

// Constructors
// Parametrised
    GenreRecommender(Item* itemsPtr, int count, string g)
        : items(itemsPtr), itemCount(count), genre(g) {
            items = itemsptr;
            itemCount = count;
            genre = g;
        }

// Overrides BaseRecommender::recommend() for genre-based recommendations
    void recommend() override {
        cout << "\n-- Genre Recommendations: " << genre << " --" << endl;
        bool found = false;
        for (int i = 0; i < itemCount; i++) {
            if (items[i].getGenre() == genre) {
                cout << " - " << items[i] << endl;
                found = true;
            }
        }
        if (!found) cout << "No items found for this genre." << endl;
    }
};

class PersonalRecommender : public BaseRecommender {
private:
    Item* items;    // POINTER
    int   itemCount;
    User* user;     // POINTER

public:

// Constructor
// Parametrised
    PersonalRecommender(Item* itemsPtr, int count, User* u) {
            items = itemsPtr;
            itemcount = count;
            user = u;
        }

// Overrides BaseRecommender::recommend() for personalized recommendations
    void recommend() override {
        cout << "\n-- Personalized Recommendations for "
             << user->getName() << " --" << endl;
        cout << "Because you are from " << user->getCity()
             << ", here are our top picks:" << endl;

        Item tempItems[MAX_ITEMS];
        for (int i = 0; i < itemCount; i++) tempItems[i] = items[i];

        for (int i = 0; i < itemCount; i++) {
            for (int j = 0; j < itemCount - 1; j++) {
                if (tempItems[j].getAverageRating() < tempItems[j+1].getAverageRating()) {
                    Item temp      = tempItems[j];
                    tempItems[j]   = tempItems[j+1];
                    tempItems[j+1] = temp;
                }
            }
        }

        for (int i = 0; i < 3 && i < itemCount; i++) {
            cout << (i+1) << ". " << tempItems[i] << endl;
        }
    }
};

class RecommendationSystem {
private:
    Item* items;    // POINTER
    int   itemCount;

    User users[MAX_USERS];
    int  userCount;

public:
    RecommendationSystem() {
        itemCount = 0;
        userCount = 0;
        items = new Item[MAX_ITEMS];    // POINTER
    }

    ~RecommendationSystem() {
        delete[] items;                 // POINTER
    }




// ----------------------------------------------------------------------------------------------
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
                mode = 1; continue;
            } else if (line.find("INSERT INTO Items") != string::npos) {
                mode = 2; continue;
            } else if (line.find("INSERT INTO Interactions") != string::npos) {
                mode = 3; continue;
            }

            if (mode == 1) {
                size_t startPos = line.find("(");
                size_t endPos   = line.find(")");
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
            else if (mode == 2) {
                size_t startPos = line.find("(");
                size_t endPos   = line.find(")");
                if (startPos != string::npos && endPos != string::npos) {
                    string content = line.substr(startPos + 1, endPos - startPos - 1);
                    size_t c1 = content.find(",");
                    size_t c2 = content.find(",", c1 + 1);
                    if (c1 != string::npos && c2 != string::npos) {
                        string t  = extractValue(content.substr(0, c1));
                        string g  = extractValue(content.substr(c1 + 1, c2 - c1 - 1));
                        string Ty = extractValue(content.substr(c2 + 1));
                        if (itemCount < MAX_ITEMS) {
                            items[itemCount].setID(itemCount + 1);
                            items[itemCount].setTitle(t);
                            items[itemCount].setGenre(g);
                            items[itemCount].setType(Ty);
                            itemCount++;
                        }
                    }
                }
            } 
            else if (mode == 3) {
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
                        int    i_id = stoi(val2);
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
        cout << "System Loaded: " << itemCount << " Items and "
             << userCount << " Users." << endl;
    }


// --------------------------------------------------------------------------------------------------

    User* login(string username) {
        User temp(0, username, "", "");
        for (int i = 0; i < userCount; i++) {
            if (users[i] == temp)
                return &users[i];
        }
        return NULL;
    }


// Accepts a BaseRecommender* so it works with any recommender type
    void runRecommender(BaseRecommender* recommender) {
        recommender->recommend();
    }


// Getters
    Item* getItems()     { 
        return items; 
    }
    int getItemCount() {
        return itemCount; 
    }
};

int main() {
    cout << "-- Movie Recommendation System --" << endl;

    RecommendationSystem user_chachu;
    user_chachu.loadData();

    cout << "\n-- Login --" << endl;
    string username;
    cout << "Enter Username (e.g., Ahmed_Khan): ";
    getline(cin, username);

    User* currentUser = user_chachu.login(username);    // POINTER

    if (currentUser != NULL) {
        cout << "Login Successful!" << endl;


// Person* pointing to a User object calls User::display(), not Person::display()
        Person* personPtr = currentUser;
        personPtr->display();



// BaseRecommender* points to a PersonalRecommender object
        BaseRecommender* recommender = new PersonalRecommender(user_chachu.getItems(), user_chachu.getItemCount(), currentUser);



        user_chachu.runRecommender(recommender);
        delete recommender;

        cout << "\nWould you like to browse by genre? (yes/no): ";
        string choice;
        getline(cin, choice);
        if (choice.empty() || choice == "yes" || choice == "y" || choice == "Y") {
            cout << "Enter Genre: ";
            string g;
            getline(cin, g);

// Same BaseRecommender* now points to a GenreRecommender object
            recommender = new GenreRecommender(user_chachu.getItems(), user_chachu.getItemCount(), g);
            sys.runRecommender(recommender);
            delete recommender;
        }

    } 
    else {
        cout << "User not found, Please check spelling." << endl;
    }

    return 0;

}
