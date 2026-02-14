#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;








const int MAX_ITEMS = 500; 

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
    int id; // Internal ID (index + 1)
    string title;
    string genre;
    string type;
    double totalRating;
    int ratingCount;

public:
    // Default Constructor
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
    string getTitle() {
        return title;
    }
    string getGenre() {
        return genre;
    }
    string getType() {
        return type;
    }
    int getId() {
        return id;
    }
    

    void addRating(double r) {
        totalRating += r;
        ratingCount++;
    }

    double getAverageRating() {
        if (ratingCount == 0) return 0.0;
        return totalRating / ratingCount;
    }
};











class RecommendationEngine {
private:
    Item items[MAX_ITEMS];
    int itemCount;

public:
    RecommendationEngine() {
        itemCount = 0;
    }

    void loadData() {
        ifstream file("Sample Database.sql");
        if (!file.is_open()) {
            cout << "Error: Could not retrieve from Database" << endl;
            return;
        }

        string line;
        int mode = 0;

        while (getline(file, line)) {
            if (line.find("INSERT INTO Items") != string::npos) {
                mode = 1;
                continue;
            } else if (line.find("INSERT INTO Interactions") != string::npos) {
                mode = 2;
                continue;
            } else if (line.find("INSERT INTO Users") != string::npos) {
                mode = 0; 
                continue;
            }

            if (mode == 1) { // Items
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
                            items[itemCount].setId(itemCount + 1); // Mock 1-based ID
                            items[itemCount].setTitle(t);
                            items[itemCount].setGenre(g);
                            items[itemCount].setType(Ty);
                            itemCount++; 
                        }
                    }
                }
            } else if (mode == 2) { // Interactions
                size_t pos = 0;
                while ((pos = line.find("(", pos)) != string::npos) {
                    size_t end = line.find(")", pos);
                    if (end == string::npos) break;
                    
                    string content = line.substr(pos + 1, end - pos - 1);
                    
                    stringstream ss(content);
                    string segment;
                    string val1, val2, val3;
                    
                    getline(ss, val1, ','); // User ID
                    getline(ss, val2, ','); // Item ID
                    getline(ss, val3, ','); // Rating
                    
                    if (!val2.empty() && !val3.empty()) {
                        int i_id = stoi(val2);
                        double rate = stod(val3);
                        
                        // Map SQL ID (1-based) to Array Index (0-based)
                        if (i_id > 0 && i_id <= itemCount) {
                            items[i_id - 1].addRating(rate);
                        }
                    }
                    pos = end + 1;
                }
            }
            
            if (line.find(";") != string::npos) {
                 mode = 0; 
            }
        }
        
        file.close();
        
        cout << "Data Loaded from Database." << endl;
        cout << "Total Items: " << itemCount << endl;
    }

    void showGenre(string Genre) {
        cout << endl << "Filtering for Genre: " << Genre << endl;
        bool found = false;
        for (int i = 0; i < itemCount; i++) {
            if (items[i].getGenre() == Genre) {
                cout << " - " << items[i].getTitle() << " (" << items[i].getType() << ")" << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "Moye Moye -- No items found for this genre" << Genre << "'." << endl;
        }
    }

    void TopPicks() {
        cout << endl << "Top 3 Picks:" << endl;
        
        Item tempItems[MAX_ITEMS]; 
        int count = itemCount;
        
        for(int i=0; i<count; i++) {
            tempItems[i] = items[i];
        }
        
        // Bubble Sort (Descending by Average Rating)
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < count - 1; j++) {
                if (tempItems[j].getAverageRating() < tempItems[j+1].getAverageRating()) {
                    // Manual Swap Objects
                    Item temp = tempItems[j];
                    tempItems[j] = tempItems[j+1];
                    tempItems[j+1] = temp;
                }
            }
        }
        
        // Print top 3
        for (int i = 0; i < 3 && i < count; i++) {
            cout << (i+1) << ". " << tempItems[i].getTitle() 
                 << " (Rating: " << tempItems[i].getAverageRating() << ")" << endl;
        }
    }
};


















int main() {
    cout << "-- Recommendation System --" << endl;

    RecommendationEngine Mango;

    Mango.loadData();

    string inputGenre;
    cout << endl << "Enter a Genre to filter (e.g., Action, Comedy, Drama): ";
    getline(cin, inputGenre);

    Mango.showGenre(inputGenre);

    Mango.TopPicks();

    return 0;
}
