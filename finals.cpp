#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <set>
using namespace std;

const string ALLOWED_ROOM_TYPES[] = {
    "Classroom", "Faculty", "Laboratory Room", "Computer Laboratory", "Electrical Room"
};
const string ALLOWED_ISSUES[] = {
    "Cleaning Maintenance",
    "Repair Maintenance",
    "Equipment Maintenance"
};

struct Room {
    string number, type, issue;
    int floor;
};

struct PendingRequest {
    string number, issue;
};

// --- Merge all data and remove duplicates based on room number and floor ---
vector<Room> merged_rooms = {
    // Existing data (original)
    {"101", "Classroom", "Cleaning Maintenance", 1},
    {"102", "Classroom", "No issue", 1},
    {"CPE Faculty", "Faculty", "Cleaning Maintenance", 1},
    {"302", "Classroom", "No issue", 3},
    {"201", "Stock Room", "Equipment Maintenance", 2},
    {"302A", "Classroom", "Repair Maintenance", 3},
    {"CLR1", "Computer Laboratory", "Equipment Maintenance, Cleaning Maintenance", 2},
    {"ECE1", "Electrical Room", "Equipment Issue", 2},
    {"102A", "Laboratory Room", "Cleaning Maintenance", 1},
    {"419", "Classroom", "No issue", 4},
    {"CLR4", "Computer Laboratory", "Equipment Maintenance", 2},
    {"CLR2", "Computer Laboratory", "Equipment Maintenance", 2},
    {"EE Faculty", "Faculty", "Cleaning Maintenance", 1},
    {"317", "Classroom", "No issue", 3},
    {"421", "Classroom", "Cleaning Maintenance, Equipment Maintenance", 4},
    {"117", "Laboratory Room", "Repair Maintenance", 1},
    {"CE Faculty", "Faculty", "Repair Maintenance", 1},
    {"402", "Classroom", "Repair Maintenance", 4},

    // New data (to be merged, see below for deduplication)
    {"201", "Classroom", "No Issue", 2},
    {"205", "Classroom", "No Issue", 2},
    {"219", "Classroom", "Cleaning Maintenance", 2},
    {"Net Lab", "Computer Laboratory", "Cleaning Maintenance", 2},
    {"EPE4", "Electrical Laboratory", "No Issue", 2},
    {"206", "Classroom", "Cleaning Maintenance", 2},
    {"EPE3", "Electrical Room", "Cleaning Maintenance", 2},
    {"EPE2", "Electrical Room", "Cleaning Maintenance", 2},
    {"CLR8", "Computer Laboratory", "Cleaning Maintenance", 2},
    {"EPE1", "Computer Laboratory", "No Issue", 2},
    {"CLR7", "Computer Laboratory", "Cleaning Maintenance", 2},
    {"CLR6", "Computer Laboratory", "No Issue", 2},
    {"CLR5", "Computer Laboratory", "No Issue", 2},
    // CLR4 exists above, but with different issue, so keep both for review
    // CLR2 exists above, but with different issue, so keep both for review
    {"ECE4", "Electrical Room", "Cleaning Maintenance", 2},
    {"ECE3", "Electrical Room", "Cleaning Maintenance", 2},
    {"ECE2", "Electrical Room", "Cleaning Maintenance", 2},
    {"CLR3", "Computer Laboratory", "Cleaning Maintenance", 2},
    {"CLR2", "Computer Laboratory", "No Issue", 2},
    {"ECE1", "Electrical Room", "Cleaning Maintenance", 2},
    {"Open Laboratory Room", "Computer Laboratory", "Equipment Maintenance", 2},
    {"215", "Classroom", "Cleaning Maintenance", 2},
    {"217", "Classroom", "No Issue", 2}
};

// Deduplicate: keep the *first* occurrence of a (number, floor) pair.
vector<Room> rooms;
void deduplicate_rooms() {
    set<pair<string, int>> seen;
    for (const auto& r : merged_rooms) {
        auto key = make_pair(r.number, r.floor);
        if (seen.count(key) == 0) {
            rooms.push_back(r);
            seen.insert(key);
        }
    }
}

vector<PendingRequest> pending;

// Design helpers
const int DESIGN_WIDTH = 110;
const string BORDER_CHAR = "*";

void coutBorder() {
    for (int i = 0; i < DESIGN_WIDTH; ++i) cout << BORDER_CHAR;
    cout << endl;
}

void coutCentered(const string& text, char pad = ' ') {
    int inner_width = DESIGN_WIDTH - 2;
    int padding = (inner_width - text.size()) / 2;
    if (padding < 0) padding = 0;
    cout << BORDER_CHAR;
    for (int i = 0; i < padding; ++i) cout << pad;
    cout << text;
    for (int i = 0; i < inner_width - padding - text.size(); ++i) cout << pad;
    cout << BORDER_CHAR << endl;
}

void coutLeftAlignedOption(const string& text) {
    int inner_width = DESIGN_WIDTH - 2;
    int padding = inner_width - 1 - text.size();
    if (padding < 0) padding = 0;
    cout << BORDER_CHAR << " " << text;
    for (int i = 0; i < padding; ++i) cout << ' ';
    cout << BORDER_CHAR << endl;
}

void coutEmptyLine() {
    cout << BORDER_CHAR;
    for (int i = 0; i < DESIGN_WIDTH-2; ++i) cout << ' ';
    cout << BORDER_CHAR << endl;
}

void showRoom(const Room& r) {
    coutBorder();
    coutEmptyLine();
    coutCentered("Room: " + r.number);
    coutCentered("Type: " + r.type);
    coutCentered("Issue: " + r.issue);
    coutEmptyLine();
    coutBorder();
}

bool isFullyNumeric(const string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit(c)) return false;
    return true;
}

void showRoomsByFloor() {
    int floor;
    cout << endl; coutBorder();
    coutEmptyLine();
    coutCentered("Enter floor (1-4):");
    coutEmptyLine();
    coutBorder();
    cout << "                                                 CHOICE: ";
    cin >> floor;
    vector<Room> filtered;
    for (const auto& r : rooms)
        if (r.floor == floor)
            filtered.push_back(r);

    if (filtered.empty()) {
        coutBorder();
        coutEmptyLine();
        coutCentered("No rooms on this floor.");
        coutEmptyLine();
        coutBorder();
        return;
    }

    sort(filtered.begin(), filtered.end(), [](const Room& a, const Room& b) {
        bool a_num = isFullyNumeric(a.number);
        bool b_num = isFullyNumeric(b.number);
        if (a_num && !b_num) return true;
        if (!a_num && b_num) return false;
        if (a_num && b_num) return stoi(a.number) < stoi(b.number);
        auto isCLRorECE = [](const string& num) {
            return num.find("CLR") == 0 || num.find("ECE") == 0;
        };
        bool a_clr_ece = isCLRorECE(a.number);
        bool b_clr_ece = isCLRorECE(b.number);
        if (a_clr_ece && !b_clr_ece) return true;
        if (!a_clr_ece && b_clr_ece) return false;
        return a.number < b.number;
    });

    for (const auto& r : filtered)
        showRoom(r);
}

void searchRoomsByNumber() {
    cin.ignore();
    coutBorder();
    coutEmptyLine();
    coutCentered("Enter room number to search:");
    coutEmptyLine();
    coutBorder();
    cout << "                                                 CHOICE: ";
    string key; getline(cin, key);
    bool found = false;
    for (const auto& r : rooms) {
        if (r.number.find(key) != string::npos) {
            showRoom(r); found = true;
        }
    }
    if (!found) {
        coutBorder();
        coutEmptyLine();
        coutCentered("No rooms found with that number.");
        coutEmptyLine();
        coutBorder();
    }
}

void searchRoomsByType() {
    coutBorder();
    coutEmptyLine();
    coutCentered("Available Room Types:");
    coutEmptyLine();
    for (int i = 0; i < 5; ++i) {
        int center = (DESIGN_WIDTH - 2) / 26 - 4; // Centering based on 24 characters per type
        string option = to_string(i+1) + ". " + ALLOWED_ROOM_TYPES[i];
        string padded(center, ' ');
        coutCentered(padded + option); // Prints out the options
    }
    coutEmptyLine();
    coutBorder();

    int choice;
    coutCentered("Select Room Type (1-5):");
    coutEmptyLine();
    coutBorder();
    cout << "                                                 CHOICE: ";
    cin >> choice;
    if (choice < 1 || choice > 5) {
        coutBorder();
        coutEmptyLine();
        coutCentered("Invalid choice.");
        coutEmptyLine();
        coutBorder();
        return;
    }
    string type = ALLOWED_ROOM_TYPES[choice-1];
    bool found = false;
    for (const auto& r : rooms)
        if (r.type == type) { showRoom(r); found = true; }
    if (!found) {
        coutBorder();
        coutEmptyLine();
        coutCentered("No rooms of this type.");
        coutEmptyLine();
        coutBorder();
    }
}

void searchRoomsByIssue() {
    coutBorder();
    coutEmptyLine();
    coutCentered("Available Issues:");
    coutEmptyLine();
    for (int i = 0; i < 3; ++i) {
        int center = (DESIGN_WIDTH - 2) / 24 - 4; // Centering based on 24 characters per issue
        string option = to_string(i+1) + ". " + ALLOWED_ISSUES[i]; 
        string padded(center, ' '); 
        coutCentered(padded + option); // prints out the options
    }
    coutEmptyLine();
    coutBorder();

    int choice;
    coutCentered("Select Issue Type (1-3):");
    coutEmptyLine();
    coutBorder();
    cout << "                                                 CHOICE: ";
    cin >> choice;
    if (choice < 1 || choice > 3) {
        coutBorder();
        coutEmptyLine();
        coutCentered("Invalid choice.");
        coutEmptyLine();
        coutBorder();
        return;
    }
    string issue = ALLOWED_ISSUES[choice-1];
    bool found = false;
    for (const auto& r : rooms)
        if (r.issue.find(issue) != string::npos) { showRoom(r); found = true; }
    if (!found) {
        coutBorder();
        coutEmptyLine();
        coutCentered("No rooms with this issue.");
        coutEmptyLine();
        coutBorder();
    }
}

void searchRoomsMenu() {
    while (true) {
        coutBorder();
        coutEmptyLine();
        coutCentered("Search Rooms by:");
        coutEmptyLine();
        coutLeftAlignedOption("                                             [1] Room Number");
        coutLeftAlignedOption("                                             [2] Room Type");
        coutLeftAlignedOption("                                             [3] Issues");
        coutLeftAlignedOption("                                             [0] Back");
        coutEmptyLine();
        coutBorder();
        cout << "                                                 CHOICE: ";
        int ch; cin >> ch;
        if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); continue; }
        if (ch == 1) searchRoomsByNumber();
        else if (ch == 2) searchRoomsByType();
        else if (ch == 3) searchRoomsByIssue();
        else if (ch == 0) break;
        else {
            coutBorder();
            coutEmptyLine();
            coutCentered("Invalid option.");
            coutEmptyLine();
            coutBorder();
        }
    }
    coutBorder();
}

bool isDuplicateReq(const string& num, const string& issue) {
    for (const auto& req : pending)
        if (req.number == num && req.issue == issue)
            return true;
    return false;
}

void requestIssue() {
    cin.ignore();
    coutBorder();
    coutEmptyLine();
    coutCentered("Enter room number:");
    coutEmptyLine();
    coutBorder();
    cout << "                                                 CHOICE: ";
    string num; getline(cin, num);
    auto it = find_if(rooms.begin(), rooms.end(),
        [&](const Room& r){ return r.number == num; });
    if (it == rooms.end()) {
        coutBorder();
        coutEmptyLine();
        coutCentered("Room not found.");
        coutEmptyLine();
        coutBorder();
        return;
    }

    coutBorder();
    coutEmptyLine();
    coutCentered("Select issue type:");
    coutEmptyLine();
    for (int i = 0; i < 3; ++i)
        coutCentered(to_string(i+1) + ". " + ALLOWED_ISSUES[i]);
    coutEmptyLine();
    coutBorder();
    cout << "                                                 CHOICE: ";
    int choice; cin >> choice;
    if (choice < 1 || choice > 3) {
        coutBorder();
        coutEmptyLine();
        coutCentered("Invalid choice.");
        coutEmptyLine();
        coutBorder();
        return;
    }
    string issue = ALLOWED_ISSUES[choice-1];

    if (isDuplicateReq(num, issue)) {
        coutBorder();
        coutEmptyLine();
        coutCentered("Already a pending request for this issue in this room.");
        coutEmptyLine();
        coutBorder();
        return;
    }
    it->issue = issue;
    pending.push_back({num, issue});
    coutBorder();
    coutEmptyLine();
    coutCentered("Request added.");
    coutEmptyLine();
    coutBorder();
}

void showPending() {
    if (pending.empty()) {
        coutBorder();
        coutEmptyLine();
        coutCentered("No pending requests.");
        coutEmptyLine();
        coutBorder();
        return;
    }
    vector<pair<int,string>> order;
    for (const auto& req : pending) {
        auto rit = find_if(rooms.begin(), rooms.end(),
            [&](const Room& r){ return r.number == req.number; });
        int fl = rit != rooms.end() ? rit->floor : 99;
        order.push_back({fl, req.number});
    }
    sort(order.begin(), order.end(), [](const pair<int,string>& a, const pair<int,string>& b){
        if (a.first != b.first) return a.first < b.first;
        bool a_num = isFullyNumeric(a.second);
        bool b_num = isFullyNumeric(b.second);
        if (a_num && !b_num) return true;
        if (!a_num && b_num) return false;
        if (a_num && b_num) return stoi(a.second) < stoi(b.second);
        auto isCLRorECE = [](const string& num) {
            return num.find("CLR") == 0 || num.find("ECE") == 0;
        };
        bool a_clr_ece = isCLRorECE(a.second);
        bool b_clr_ece = isCLRorECE(b.second);
        if (a_clr_ece && !b_clr_ece) return true;
        if (!a_clr_ece && b_clr_ece) return false;
        return a.second < b.second;
    });
    coutBorder();
    coutEmptyLine();
    coutCentered("Pending Requests:");
    coutEmptyLine();
    coutBorder();
    for (const auto& o : order) {
        auto rit = find_if(rooms.begin(), rooms.end(),
            [&](const Room& r){ return r.number == o.second; });
        if (rit != rooms.end()) showRoom(*rit);
    }
}

int main() {
    deduplicate_rooms();

    coutBorder();
    coutEmptyLine();
    cout << "*                              ░█▀▀░▀█▀░█░█░█▀▀░█▀█░█▀▄░█▀▀░▄▀▄░█░█░█▀▀░█▀▀░▀█▀                              *" << endl;
    cout << "*                              ░█▀▀░░█░░▄▀▄░█▀▀░█░█░█▀▄░█▀▀░█ █░█░█░█▀▀░▀▀█░░█░                              *" << endl;
    cout << "*                              ░▀░░░▀▀▀░▀░▀░▀▀▀░▀░▀░▀░▀░▀▀▀░░▀▀▀░▀░░▀▀▀░▀▀▀░░▀░                              *" << endl;
    coutCentered("Welcome to fixENrequest!");

    coutEmptyLine();
    coutBorder();
    while (true) {
        coutEmptyLine();
        coutCentered("MAIN MENU");
        coutEmptyLine();
        coutLeftAlignedOption("                                             [1] Show Rooms");
        coutLeftAlignedOption("                                             [2] Search Rooms");
        coutLeftAlignedOption("                                             [3] Request Issue");
        coutLeftAlignedOption("                                             [4] Pending Requests");
        coutLeftAlignedOption("                                             [0] Exit");
        coutEmptyLine(); 
        coutBorder();
        cout << "                                                 CHOICE: ";
        int ch; cin >> ch;
        if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); continue; }
        if (ch == 1) showRoomsByFloor();
        else if (ch == 2) searchRoomsMenu();
        else if (ch == 3) requestIssue();
        else if (ch == 4) showPending();
        else if (ch == 0) break;
        else {
            coutBorder();
            coutEmptyLine();
            coutCentered("Invalid option.");
            coutEmptyLine();
            coutBorder();
        }
    }
    coutBorder();
    coutEmptyLine();
    coutCentered("Thank you for using fixENrequest!");
    coutEmptyLine();
    coutBorder();
    return 0;
}
