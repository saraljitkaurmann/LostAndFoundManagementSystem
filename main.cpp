
#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <fstream>
#include <sstream>

using namespace std;

// Convert text into lowercase
string toLowerCase(string text)
{
    for (int i = 0; i < text.length(); i++)
    {
        text[i] = tolower(text[i]);
    }

    return text;
}

// Read a valid integer
int readNumber(string message)
{
    int number;

    while (true)
    {
        cout << message;

        if (cin >> number)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return number;
        }

        cout << "Invalid input! Please enter a number.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Read non-empty text
string readRequiredText(string message)
{
    string value;

    while (true)
    {
        cout << message;
        getline(cin, value);

        if (value.find_first_not_of(" \t") != string::npos &&
            value.find('|') == string::npos)
        {
            return value;
        }

        cout << "Invalid input! Field cannot be empty or contain |.\n";
    }
}

// Validate contact number
bool isValidContact(string contact)
{
    int digitCount = 0;

    for (int i = 0; i < contact.length(); i++)
    {
        char ch = contact[i];

        if (isdigit(ch))
        {
            digitCount++;
        }
        else if (ch != '+' && ch != '-' && ch != ' ' &&
                 ch != '(' && ch != ')')
        {
            return false;
        }
    }

    return digitCount >= 7;
}

// Read valid contact number
string readContact()
{
    string contact;

    while (true)
    {
        contact = readRequiredText("Enter contact number: ");

        if (isValidContact(contact))
        {
            return contact;
        }

        cout << "Invalid contact number! Try again.\n";
    }
}

// Forward declaration
class LostAndFoundSystem;

// Abstract Base Class
class Item
{
protected:
    int id;
    string name;
    string description;
    string location;
    string date;
    string reporterName;
    string urn;
    string contact;

    // Friend class
    friend class LostAndFoundSystem;

public:

    // Parameterized constructor
    Item(int i, string n, string d, string l, string dt,
         string rn, string u, string c)
    {
        id = i;
        name = n;
        description = d;
        location = l;
        date = dt;
        reporterName = rn;
        urn = u;
        contact = c;
    }

    // Pure virtual function
    virtual string getType() const = 0;

    // Display common details
    virtual void display() const
    {
        cout << "Report ID       : " << id << endl;
        cout << "Item Name       : " << name << endl;
        cout << "Description     : " << description << endl;
        cout << "Location        : " << location << endl;
        cout << "Date            : " << date << endl;
        cout << "Reported By     : " << reporterName << endl;
        cout << "URN             : " << urn << endl;
        cout << "Contact         : " << contact << endl;
    }

    // Search item by name
    bool matchesName(string keyword) const
    {
        return toLowerCase(name).find(toLowerCase(keyword))
               != string::npos;
    }

    // Virtual destructor
    virtual ~Item()
    {
    }
};

// Derived Class: Lost Item
class LostItem : public Item
{
public:

    LostItem(int i, string n, string d, string l, string dt,
             string rn, string u, string c)
        : Item(i, n, d, l, dt, rn, u, c)
    {
    }

    string getType() const override
    {
        return "Lost";
    }

    void display() const override
    {
        cout << "\n----------- LOST ITEM -----------\n";
        Item::display();
    }
};

// Derived Class: Found Item
class FoundItem : public Item
{
public:

    FoundItem(int i, string n, string d, string l, string dt,
              string rn, string u, string c)
        : Item(i, n, d, l, dt, rn, u, c)
    {
    }

    string getType() const override
    {
        return "Found";
    }

    void display() const override
    {
        cout << "\n----------- FOUND ITEM ----------\n";
        Item::display();
    }
};

// Main Management System Class
class LostAndFoundSystem
{
private:

    Item** items;
    int count;
    int capacity;
    int nextID;

    // Resize the dynamic array
    void resizeArray()
    {
        int newCapacity = capacity * 2;

        Item** newItems = new Item*[newCapacity];

        for (int i = 0; i < count; i++)
        {
            newItems[i] = items[i];
        }

        delete[] items;

        items = newItems;
        capacity = newCapacity;
    }

    // Save reports into a file
    void saveToFile()
    {
        ofstream file("lost_found.txt");

        if (!file)
        {
            cout << "Error opening file for saving!\n";
            return;
        }

        for (int i = 0; i < count; i++)
        {
            file << items[i]->id << "|"
                 << items[i]->getType() << "|"
                 << items[i]->name << "|"
                 << items[i]->description << "|"
                 << items[i]->location << "|"
                 << items[i]->date << "|"
                 << items[i]->reporterName << "|"
                 << items[i]->urn << "|"
                 << items[i]->contact << endl;
        }

        file.close();
    }

    // Load reports from a file
    void loadFromFile()
    {
        ifstream file("lost_found.txt");

        if (!file)
        {
            return;
        }

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string idText, type, name, description;
            string location, date, reporterName, urn, contact;

            if (!getline(ss, idText, '|') ||
                !getline(ss, type, '|') ||
                !getline(ss, name, '|') ||
                !getline(ss, description, '|') ||
                !getline(ss, location, '|') ||
                !getline(ss, date, '|') ||
                !getline(ss, reporterName, '|') ||
                !getline(ss, urn, '|') ||
                !getline(ss, contact))
            {
                continue;
            }

            int id;
            stringstream idStream(idText);

            if (!(idStream >> id))
            {
                continue;
            }

            Item* newItem = nullptr;

            if (type == "Lost")
            {
                newItem = new LostItem(id, name, description, location,
                                       date, reporterName, urn, contact);
            }
            else if (type == "Found")
            {
                newItem = new FoundItem(id, name, description, location,
                                        date, reporterName, urn, contact);
            }

            if (newItem != nullptr)
            {
                if (count == capacity)
                {
                    resizeArray();
                }

                items[count] = newItem;
                count++;

                if (id >= nextID)
                {
                    nextID = id + 1;
                }
            }
        }

        file.close();
    }

public:

    // Constructor
    LostAndFoundSystem()
    {
        count = 0;
        capacity = 2;
        nextID = 101;

        items = new Item*[capacity];

        // Load previously saved reports
        loadFromFile();
    }

    // Report a new item
    void reportItem()
    {
        cout << "\n========== REPORT AN ITEM ==========\n";
        cout << "1. Lost Item\n";
        cout << "2. Found Item\n";

        int choice;

        do
        {
            choice = readNumber("Enter your choice: ");

            if (choice != 1 && choice != 2)
            {
                cout << "Please enter 1 or 2.\n";
            }

        } while (choice != 1 && choice != 2);

        string name = readRequiredText("Enter item name: ");
        string description = readRequiredText("Enter description: ");
        string location = readRequiredText("Enter location: ");
        string date = readRequiredText("Enter date (DD-MM-YYYY): ");

        cout << "\n--- REPORTER DETAILS (PRIVATE) ---\n";
        

        string reporterName = readRequiredText("Enter student name: ");
        string urn = readRequiredText("Enter URN / Roll Number: ");
        string contact = readContact();

        Item* newItem;

        if (choice == 1)
        {
            newItem = new LostItem(nextID, name, description, location,
                                   date, reporterName, urn, contact);
        }
        else
        {
            newItem = new FoundItem(nextID, name, description, location,
                                    date, reporterName, urn, contact);
        }

        if (count == capacity)
        {
            resizeArray();
        }

        items[count] = newItem;
        count++;

        cout << "\nReport submitted successfully!\n";
        cout << "Your Report ID is: " << nextID << endl;

        nextID++;

        // Save reports after submission
        saveToFile();
    }

    // Display all reports
    void displayAll() const
    {
        if (count == 0)
        {
            cout << "\nNo reports available.\n";
            return;
        }

        cout << "\n========== LOST ITEMS ==========\n";

        bool lostFound = false;

        for (int i = 0; i < count; i++)
        {
            if (items[i]->getType() == "Lost")
            {
                items[i]->display();
                lostFound = true;
            }
        }

        if (!lostFound)
        {
            cout << "No lost item reports available.\n";
        }

        cout << "\n========== FOUND ITEMS ==========\n";

        bool foundFound = false;

        for (int i = 0; i < count; i++)
        {
            if (items[i]->getType() == "Found")
            {
                items[i]->display();
                foundFound = true;
            }
        }

        if (!foundFound)
        {
            cout << "No found item reports available.\n";
        }
    }

    // Search for an item
    void searchItem()
    {
        if (count == 0)
        {
            cout << "\nNo reports available to search.\n";
            return;
        }

        string keyword = readRequiredText("\nEnter item name to search: ");

        bool found = false;

        for (int i = 0; i < count; i++)
        {
            if (items[i]->matchesName(keyword))
            {
                items[i]->display();
                found = true;
            }
        }

        if (!found)
        {
            cout << "\nNo matching item found.\n";
        }
    }

    // Destructor
    ~LostAndFoundSystem()
    {
        for (int i = 0; i < count; i++)
        {
            delete items[i];
        }

        delete[] items;
    }
};

// Main Function
int main()
{
    LostAndFoundSystem system;

    int choice;

    do
    {
        cout << "\n";
        cout << "========================================\n";
        cout << "       LOST AND FOUND MANAGEMENT        \n";
        cout << "========================================\n";

        cout << "1. Report a Lost / Found Item\n";
        cout << "2. Display All Reports\n";
        cout << "3. Search for an Item\n";
        cout << "4. Exit\n";

        cout << "----------------------------------------\n";

        choice = readNumber("Enter your choice: ");

        switch (choice)
        {
            case 1:
                system.reportItem();
                break;

            case 2:
                system.displayAll();
                break;

            case 3:
                system.searchItem();
                break;

            case 4:
                cout << "\nThank you for using the system!\n";
                break;

            default:
                cout << "\nInvalid choice! Please try again.\n";
        }

    } while (choice != 4);

    return 0;
}
