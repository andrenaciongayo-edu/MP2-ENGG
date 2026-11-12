#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

// ---------- Constants ----------
const string MANAGER_CODE = "123";
const int NUM_PAYMENT_TYPES = 3;
const string PAYMENT_TYPES[NUM_PAYMENT_TYPES] = {"Credit card", "Gcash", "PayPal"};

const int MAX_MOVIES = 50;
const int MAX_CINEMAS = 50;
const int MAX_ROWS = 26;            // Rows A to Z
const int MAX_SEATS_PER_ROW = 50;
const int MAX_RECORDS = 1000;       // Most entries in any one list file
const int MAX_SEATS_PER_BOOKING = 50;
const int MAX_FILE_LINES = 12;      // Most lines read from any one record file
const int LINE_WIDTH = 46;          // Width of formatted screen titles

const int RECORD_ID_LENGTH = 5;     // e.g. B-001, C-001
const int BOOKING_ID_LENGTH = 5;
const int MAX_BOOKING_NUMBER = 999;
const int MAX_MANAGER_LOGIN_ATTEMPTS = 5;   // wrong codes allowed in total before the lock-out

// =====================================================================================
// Helper Functions & Tokenizer (same as MP1, except where marked NEW)
// =====================================================================================

// Calculates the length of a string
int textLength(const string &text) {
    int length = 0;
    while (text[length] != '\0') {
        length++;
    }
    return length;
}

// Checks if a character is a numeric digit
bool isDigitChar(char c) {
    return (c >= '0' && c <= '9');
}

// Checks if a character is whitespace
bool isBlankChar(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

// Converts a lowercase character to uppercase
char toUpperChar(char c) {
    if (c >= 'a' && c <= 'z') {
        return (char)(c - 'a' + 'A');
    }
    return c;
}

// Converts an entire string to uppercase
string toUpper(const string &text) {
    string result = text;
    int len = textLength(result);
    for (int i = 0; i < len; i++) {
        result[i] = toUpperChar(result[i]);
    }
    return result;
}

// Extracts a substring from 'start' up to (but not including) 'end'
string cutText(const string &text, int start, int end) {
    string result = "";
    int len = end - start;
    if (len > 0) {
        for (int i = start; i < end; i++) {
            result += text[i];
        }
    }
    return result;
}

// Removes leading and trailing whitespace from a string
string trim(const string &text) {
    int start = 0;
    int end = textLength(text);
    while (start < end && isBlankChar(text[start])) {
        start++;
    }
    while (end > start && isBlankChar(text[end - 1])) {
        end--;
    }
    return cutText(text, start, end);
}

// Extracts the next non-whitespace word starting from 'index'
string extractNextToken(const string &text, int &index) {
    int len = textLength(text);
    while (index < len && isBlankChar(text[index])) {
        index++;
    }
    if (index >= len) return "";

    int start = index;
    while (index < len && !isBlankChar(text[index])) {
        index++;
    }
    return cutText(text, start, index);
}

// Parses a space-separated string of seats into an array
int parseSeatTokens(const string &text, string seats[MAX_SEATS_PER_BOOKING], bool &overflow) {
    int count = 0;
    int index = 0;
    overflow = false;
    string token = "";

    while ((token = extractNextToken(text, index)) != "") {
        if (count < MAX_SEATS_PER_BOOKING) {
            seats[count] = token;
        } else {
            overflow = true;
        }
        count++;
    }
    return count;
}

// Converts a string to an integer safely
int toInt(const string &text) {
    double value = 0;
    int len = textLength(text);
    for (int i = 0; i < len && isDigitChar(text[i]); i++) {
        value = value * 10 + (text[i] - '0');
        if (value > 2000000000.0) return 2000000000;
    }
    return (int)value;
}

// Parses a string into a decimal value
bool parseDecimal(const string &text, double &value) {
    int len = textLength(text);
    bool valid = true;
    bool afterDot = false;
    int digitCount = 0;
    double number = 0;
    double divisor = 1;

    for (int i = 0; i < len && valid; i++) {
        if (isDigitChar(text[i])) {
            number = number * 10 + (text[i] - '0');
            if (afterDot) divisor = divisor * 10;
            digitCount++;
        } else if (text[i] == '.' && !afterDot) {
            afterDot = true;
        } else {
            valid = false;
        }
    }
    if (digitCount == 0) valid = false;
    if (valid) value = number / divisor;
    return valid;
}

// Converts an integer to a string
string intToString(int number) {
    if (number == 0) return "0";
    string digits = "0123456789";
    string temp = "";
    while (number > 0) {
        temp += digits[number % 10];
        number /= 10;
    }
    string result = "";
    int len = textLength(temp);
    for (int i = len - 1; i >= 0; i--) {
        result += temp[i];
    }
    return result;
}

// Formats a double into a standard currency string (e.g. 10.00)
string formatMoney(double amount) {
    int cents = (int)(amount * 100.0 + 0.5);
    int whole = cents / 100;
    int fraction = cents % 100;
    string result = intToString(whole) + ".";
    if (fraction < 10) result += "0";
    result += intToString(fraction);
    return result;
}

// Creates an ID string given a prefix and number
string makeID(char prefix, int number) {
    string id = "";
    id += prefix;
    id += "-";
    id += (char)('0' + (number / 100));
    id += (char)('0' + ((number / 10) % 10));
    id += (char)('0' + (number % 10));
    return id;
}

// NEW (MP2): Validates an ID such as B-001 or C-012 (pass by value so the caller's string is untouched)
bool isValidRecordID(char prefix, string recordID) {
    recordID = toUpper(trim(recordID));
    if (textLength(recordID) != RECORD_ID_LENGTH) return false;
    if (recordID[0] != prefix || recordID[1] != '-') return false;
    for (int i = 2; i < RECORD_ID_LENGTH; i++) {
        if (!isDigitChar(recordID[i])) return false;
    }
    return true;
}

// NEW (MP2): Turns "Reyes, Rose Marie A." (CUSTOMER block format) into "Rose Marie A. Reyes"
string flipName(const string &fileName) {
    int len = textLength(fileName);
    int commaPos = -1;
    for (int i = 0; i < len && commaPos == -1; i++) {
        if (fileName[i] == ',') commaPos = i;
    }
    if (commaPos == -1) return trim(fileName);

    string lastName = trim(cutText(fileName, 0, commaPos));
    string givenNames = trim(cutText(fileName, commaPos + 1, len));
    return givenNames + " " + lastName;
}

// Extracts the data part of a saved line (after the first colon)
string extractAfterColon(const string &line) {
    int colonPos = -1;
    int len = textLength(line);
    for (int i = 0; i < len; i++) {
        if (line[i] == ':') {
            colonPos = i;
            break;
        }
    }
    if (colonPos == -1) return trim(line);
    return trim(cutText(line, colonPos + 1, len));
}

// NEW (MP2): Reads up to maxLines lines of a file into an array (each line trimmed).
// Returns the number of lines read, or -1 if the file cannot be opened.
int readFileLines(const string &fileName, string lines[], int maxLines) {
    ifstream in(fileName);
    if (!in.is_open()) return -1;

    int count = 0;
    string line;
    while (count < maxLines && getline(in, line)) {
        lines[count] = trim(line);
        count++;
    }
    in.close();
    return count;
}

// ---------- Screen Formatting Helpers ----------

// Draws a horizontal line of a specified length
void drawLine(char symbol, int length) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }
    cout << "\n";
}

// Centers a title on the screen inside horizontal lines
void printTitle(const string &title) {
    cout << "\n";
    drawLine('=', LINE_WIDTH);
    int space = (LINE_WIDTH - textLength(title)) / 2;
    for (int i = 0; i < space; i++) {
        cout << ' ';
    }
    cout << title << "\n";
    drawLine('=', LINE_WIDTH);
}

// ---------- Input Validation Helpers ----------

// Prompts the user continuously until a non-empty string is provided
string getNonEmptyLine(const string &prompt) {
    string text;
    do {
        cout << prompt;
        getline(cin, text);
        text = trim(text);
        if (text == "") {
            cout << "  ! This field cannot be blank.\n";
        }
    } while (text == "");
    return text;
}

// Prompts the user continuously until an integer within the range is provided
int getIntInRange(int low, int high) {
    string text;
    bool valid;
    int value = 0;
    do {
        getline(cin, text);
        text = trim(text);
        valid = (text != "");
        int len = textLength(text);
        for (int i = 0; i < len && valid; i++) {
            if (!isDigitChar(text[i])) valid = false;
        }
        if (valid) {
            value = toInt(text);
            if (value < low || value > high) valid = false;
        }
        if (!valid) {
            cout << "  ! Enter a number from " << low << " to " << high << ": ";
        }
    } while (!valid);
    return value;
}

// ---------- Seat Name Helpers ----------

// Parses a single seat string into row index and seat number (e.g. J20 -> 9, 20)
bool parseSeat(const string &seat, int &rowIndex, int &seatNo) {
    int len = textLength(seat);
    if (len < 2) return false;
    char letter = toUpperChar(seat[0]);
    if (letter < 'A' || letter > 'Z') return false;

    double number = 0;
    for (int i = 1; i < len; i++) {
        if (!isDigitChar(seat[i])) return false;
        number = number * 10 + (seat[i] - '0');
        if (number > 2000000000.0) return false;
    }
    rowIndex = letter - 'A';
    seatNo = (int)number;
    return seatNo >= 1;
}

// Reads and validates a space-delimited list of requested seats from the user
int readSeatList(int minSeats, string seats[MAX_SEATS_PER_BOOKING]) {
    bool valid;
    int count = 0;
    do {
        string line;
        getline(cin, line);
        line = toUpper(trim(line));

        bool overflow = false;
        count = parseSeatTokens(line, seats, overflow);

        valid = (count >= minSeats) && !overflow;
        for (int i = 0; i < count && valid; i++) {
            int rowIndex, seatNo;
            if (!parseSeat(seats[i], rowIndex, seatNo)) valid = false;
        }
        for (int i = 0; i < count && valid; i++) {
            for (int j = 0; j < i; j++) {
                if (seats[j] == seats[i]) valid = false;
            }
        }
        if (overflow) {
            cout << "  ! Too many seats entered (Max: " << MAX_SEATS_PER_BOOKING << ").\n  Try again: ";
        } else if (!valid) {
            cout << "  ! Invalid seat list. Example: J20 K20\n  Try again: ";
        }
    } while (!valid);
    return count;
}

// =====================================================================================
// CLASS: Record  (PARENT class)
// =====================================================================================
class Record {
protected:
    string id;                              // e.g. M-001, I-001, B-001

public:
    Record() {                              // constructor
        id = "";
    }

    Record(string recordID) {               // constructor with a value
        id = recordID;
    }

    string getID() {
        return id;
    }

    string getFileName() {
        return id + ".txt";
    }

    void display() {                        // parent version: only the ID
        cout << id << "\n";
    }

    bool loadFromFile() {                   // parent version: nothing to load
        return false;
    }
};

// =====================================================================================
// CLASS: Customer  (CHILD of Record) - data comes from the CUSTOMER block's C-XXX.txt
// A customer is registered once (and gets a C-XXX ID); a booking only needs that ID.
// =====================================================================================
class Customer : public Record {
private:
    string name;                // shown as "Rose Marie A. Reyes"
    string mobile;
    string email;

public:
    Customer() {
        name = "";
        mobile = "";
        email = "";
    }

    Customer(string customerID) {
        id = customerID;                    // 'id' is inherited from Record
        name = "";
        mobile = "";
        email = "";
    }

    Customer(string customerID, string custName, string custMobile, string custEmail) {
        id = customerID;
        name = custName;
        mobile = custMobile;
        email = custEmail;
    }

    string getName() { return name; }
    string getMobile() { return mobile; }
    string getEmail() { return email; }

    // Same name as Record::display(), different implementation
    void display() {
        cout << name << "\n";
        cout << "     Mobile : " << mobile << "\n";
        cout << "     Email  : " << email << "\n";
    }

    /* C-XXX.txt (one value per line):
     *  0 Last, Given Name MI      1 address       2 mobile number
     *  3 birthday                 4 email         5 Customer ID # C-XXX */
    bool loadFromFile() {
        string lines[MAX_FILE_LINES];
        int count = readFileLines(getFileName(), lines, MAX_FILE_LINES);
        if (count < 5) return false;

        name = flipName(lines[0]);
        mobile = lines[2];
        email = lines[4];
        return (name != "");
    }
};

// =====================================================================================
// CLASS: Movie  (CHILD of Record) - data comes from the MOVIE block's M-XXX.txt
// =====================================================================================
class Movie : public Record {
private:
    string name;
    string genre;
    string startDate;
    string endDate;
    string rating;

public:
    Movie() {
        name = "";
        genre = "";
        startDate = "";
        endDate = "";
        rating = "";
    }

    Movie(string movieID, string movieName) {
        id = movieID;                       // 'id' is inherited from Record
        name = movieName;
        genre = "";
        startDate = "";
        endDate = "";
        rating = "";
    }

    string getName() { return name; }
    string getGenre() { return genre; }
    string getRating() { return rating; }

    // Same name as Record::display(), different implementation
    void display() {
        cout << name << "\n";
        cout << "     Genre  : " << genre << "   Rating: " << rating << "\n";
        if (startDate != "" && endDate != "") {
            cout << "     Runs   : " << startDate << " to " << endDate << "\n";
        }
    }

    // M-XXX.txt: name / genre / Start date: ... / End date: ... / Rating: ... / ...
    bool loadFromFile() {
        string lines[MAX_FILE_LINES];
        int count = readFileLines(getFileName(), lines, MAX_FILE_LINES);
        if (count < 1) return false;

        if (lines[0] != "") name = lines[0];
        if (count > 1) genre = lines[1];
        if (count > 2) startDate = extractAfterColon(lines[2]);
        if (count > 3) endDate = extractAfterColon(lines[3]);
        if (count > 4) rating = extractAfterColon(lines[4]);
        return true;
    }
};

// =====================================================================================
// CLASS: Cinema  (CHILD of Record) - data comes from the CINEMA block's I-XXX.txt
// =====================================================================================
class Cinema : public Record {
private:
    string name;
    int rows;
    int seatsPerRow;
    double price;
    string type;

public:
    Cinema() {
        name = "";
        rows = 0;
        seatsPerRow = 0;
        price = 0;
        type = "";
    }

    Cinema(string cinemaID, string cinemaName) {
        id = cinemaID;                      // 'id' is inherited from Record
        name = cinemaName;
        rows = 0;
        seatsPerRow = 0;
        price = 0;
        type = "";
    }

    string getName() { return name; }
    int getRows() { return rows; }
    int getSeatsPerRow() { return seatsPerRow; }
    int getCapacity() { return rows * seatsPerRow; }
    double getPrice() { return price; }

    // A cinema can only be booked if its layout fits the seat names A1..Z50 and it has a price
    bool isUsable() {
        return (rows >= 1 && rows <= MAX_ROWS &&
                seatsPerRow >= 1 && seatsPerRow <= MAX_SEATS_PER_ROW &&
                price > 0);
    }

    // Same name as Record::display(), different implementation
    void display() {
        cout << name << "\n";
        cout << "     Type   : " << type << "   Capacity: " << getCapacity() << "\n";
        cout << "     Price  : PHP " << formatMoney(price) << " per seat\n";
    }

    /* I-XXX.txt (one value per line):
     *  0 name              1 address            2 Number of rows: N
     *  3 Number of seats per row: N             4 Capacity: N
     *  5 contact number    6 Price: N           7 Cinema type: X      8 code */
    bool loadFromFile() {
        string lines[MAX_FILE_LINES];
        int count = readFileLines(getFileName(), lines, MAX_FILE_LINES);
        if (count < 8) return false;

        if (lines[0] != "") name = lines[0];
        rows = toInt(extractAfterColon(lines[2]));
        seatsPerRow = toInt(extractAfterColon(lines[3]));
        if (!parseDecimal(extractAfterColon(lines[6]), price)) price = 0;
        type = extractAfterColon(lines[7]);
        return true;
    }
};

// =====================================================================================
// CLASS: Booking  (CHILD of Record) - one B-XXX.txt file
// =====================================================================================
class Booking : public Record {
private:
    Customer customer;          // a booking HAS a customer (an object inside an object)
    string payment;
    string movieName;
    string cinemaName;
    string seatText;            // "J21 J22 J23"
    string totalText;           // empty until CHECK AVAILABILITY runs

public:
    Booking() {
        payment = "";
        movieName = "";
        cinemaName = "";
        seatText = "";
        totalText = "";
    }

    Booking(string bookingID) {
        id = bookingID;                     // 'id' is inherited from Record
        payment = "";
        movieName = "";
        cinemaName = "";
        seatText = "";
        totalText = "";
    }

    Booking(string bookingID, Customer cust, string pay,
            string movie, string cinema, string seats) {
        id = bookingID;
        customer = cust;
        payment = pay;
        movieName = movie;
        cinemaName = cinema;
        seatText = seats;
        totalText = "";
    }

    string getPayment() { return payment; }
    string getMovieName() { return movieName; }
    string getCinemaName() { return cinemaName; }
    string getSeatText() { return seatText; }
    string getTotalText() { return totalText; }
    void setTotalText(string total) { totalText = total; }

    // Same name as Record::display(), different implementation
    void display() {
        cout << " Customer name: " << customer.getName() << "\n";
        cout << " " << customer.getMobile() << "\n";
        cout << " " << customer.getEmail() << "\n";
        cout << " Type of payment: " << payment << "\n";
        cout << " Movie name: " << movieName << "\n";
        cout << " Cinema: " << cinemaName << "\n";
        cout << " Seat number/s: " << seatText << "\n";
        cout << " " << id << "\n";
        if (totalText != "") {
            cout << " Total payment: " << totalText << "\n";
        }
    }

    // Creates (or overwrites) B-XXX.txt
    bool saveToFile() {
        ofstream out(getFileName());
        if (!out.is_open()) return false;
        out << "Customer name: " << customer.getName() << "\n";
        out << customer.getMobile() << "\n";
        out << customer.getEmail() << "\n";
        out << "Type of payment: " << payment << "\n";
        out << "Movie name: " << movieName << "\n";
        out << "Cinema: " << cinemaName << "\n";
        out << "Seat number/s: " << seatText << "\n";
        out << id << "\n";
        if (totalText != "") {
            out << "Total payment: " << totalText << "\n";
        }
        out.close();
        return true;
    }

    // Reads B-XXX.txt back into the object
    bool loadFromFile() {
        string lines[MAX_FILE_LINES];
        int count = readFileLines(getFileName(), lines, MAX_FILE_LINES);
        if (count < 8) return false;

        // The booking file has no customer ID line (format fixed by the handout)
        customer = Customer("", extractAfterColon(lines[0]), lines[1], lines[2]);
        payment = extractAfterColon(lines[3]);
        movieName = extractAfterColon(lines[4]);
        cinemaName = extractAfterColon(lines[5]);
        seatText = extractAfterColon(lines[6]);
        totalText = (count > 8) ? extractAfterColon(lines[8]) : "";
        return true;
    }
};

// =====================================================================================
// CLASS: SeatMap - the seat grid of one cinema (replaces the global seatMap[][] array)
// =====================================================================================
class SeatMap {
private:
    int rows;
    int seatsPerRow;
    int cells[MAX_ROWS][MAX_SEATS_PER_ROW];     // 0 = free, 1 = reserved

public:
    SeatMap() {
        setLayout(0, 0);
    }

    // Sets the size of the grid and frees every seat
    void setLayout(int numRows, int numSeatsPerRow) {
        rows = numRows;
        seatsPerRow = numSeatsPerRow;
        for (int r = 0; r < MAX_ROWS; r++) {
            for (int c = 0; c < MAX_SEATS_PER_ROW; c++) {
                cells[r][c] = 0;
            }
        }
    }

    int getCapacity() { return rows * seatsPerRow; }

    // True if the seat name (e.g. J20) exists in this cinema's layout
    bool seatExists(const string &seat) {
        int rowIndex, seatNo;
        if (!parseSeat(seat, rowIndex, seatNo)) return false;
        return (rowIndex < rows) && (seatNo <= seatsPerRow);
    }

    bool isReserved(const string &seat) {
        int rowIndex, seatNo;
        if (!seatExists(seat)) return false;
        parseSeat(seat, rowIndex, seatNo);
        return cells[rowIndex][seatNo - 1] == 1;
    }

    // Sets one seat to value (1 = reserve, 0 = free)
    void setSeat(const string &seat, int value) {
        int rowIndex, seatNo;
        if (!seatExists(seat)) return;
        parseSeat(seat, rowIndex, seatNo);
        cells[rowIndex][seatNo - 1] = value;
    }

    // Reserves every seat of a text such as "A1 A2 C10"
    void reserveFromText(const string &text) {
        int index = 0;
        string token = "";
        while ((token = extractNextToken(text, index)) != "") {
            setSeat(token, 1);
        }
    }

    int countRemaining() {
        int available = 0;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < seatsPerRow; c++) {
                if (cells[r][c] == 0) available++;
            }
        }
        return available;
    }

    // Displays the grid of free [ ] and reserved [X] seats
    void display() {
        int mapWidth = 3 + seatsPerRow * 4;
        int screenLineWidth = mapWidth > LINE_WIDTH ? mapWidth : LINE_WIDTH;

        drawLine('~', screenLineWidth);
        cout << "                 S C R E E N            \n";
        drawLine('~', screenLineWidth);
        cout << "\n";

        cout << "   ";
        for (int c = 0; c < seatsPerRow; c++) {
            int num = c + 1;
            if (num < 10) cout << "  " << num << " ";
            else cout << " " << num << " ";
        }
        cout << "\n";

        for (int r = 0; r < rows; r++) {
            char letter = (char)('A' + r);
            cout << letter << "  ";
            for (int c = 0; c < seatsPerRow; c++) {
                if (cells[r][c] == 1) cout << "[X] ";
                else cout << "[ ] ";
            }
            cout << "\n";
        }
        cout << "\n   [ ] = available      [X] = reserved  \n";
    }
};

// =====================================================================================
// CLASS: RecordList  (PARENT class) - a list file such as "Booking List.txt"
// =====================================================================================
class RecordList {
protected:
    string listFileName;                    // e.g. "Booking List.txt"
    string counterFileName;                 // e.g. "Booking Counter.txt"
    string ids[MAX_RECORDS];
    string texts[MAX_RECORDS];              // the rest of each line (name, total, ...)
    int count;

public:
    RecordList() {
        listFileName = "";
        counterFileName = "";
        count = 0;
    }

    RecordList(string listFile, string counterFile) {
        listFileName = listFile;
        counterFileName = counterFile;
        count = 0;
    }

    int getCount() { return count; }
    string getID(int i) { return (i >= 0 && i < count) ? ids[i] : ""; }
    string getText(int i) { return (i >= 0 && i < count) ? texts[i] : ""; }

    // Reads the list file (a missing file means the list is empty)
    int load() {
        count = 0;
        ifstream in(listFileName);
        if (!in.is_open()) return 0;

        string line;
        while (count < MAX_RECORDS && getline(in, line)) {
            line = trim(line);
            int index = 0;
            string idToken = extractNextToken(line, index);
            if (idToken != "") {
                ids[count] = idToken;
                texts[count] = trim(cutText(line, index, textLength(line)));
                count++;
            }
        }
        in.close();
        return count;
    }

    // Position of an ID in the list, or -1
    int findIndex(const string &recordID) {
        string target = toUpper(recordID);
        for (int i = 0; i < count; i++) {
            if (toUpper(ids[i]) == target) return i;
        }
        return -1;
    }

    // Adds a line to the end of the file and to memory
    bool add(const string &recordID, const string &text) {
        if (count >= MAX_RECORDS) return false;
        ofstream out(listFileName, ios::app);
        if (!out.is_open()) return false;
        out << recordID << " " << text << "\n";
        out.close();
        ids[count] = recordID;
        texts[count] = text;
        count++;
        return true;
    }

    // Removes one line and rewrites the file without it
    bool removeAt(int index) {
        if (index < 0 || index >= count) return false;
        ofstream out(listFileName);
        if (!out.is_open()) return false;
        for (int i = 0; i < count; i++) {
            if (i != index) {
                out << ids[i] << " " << texts[i] << "\n";
            }
        }
        out.close();
        for (int i = index; i < count - 1; i++) {
            ids[i] = ids[i + 1];
            texts[i] = texts[i + 1];
        }
        count--;
        return true;
    }

    // The counter file keeps the last number used
    int readCounter() {
        int lastNumber = 0;
        ifstream in(counterFileName);
        if (in.is_open()) {
            in >> lastNumber;
            in.close();
        }
        return lastNumber;
    }

    void writeCounter(int number) {
        ofstream out(counterFileName);
        if (out.is_open()) {
            out << number;
            out.close();
        }
    }

    // Parent version: prints every line as "ID text"
    void display() {
        for (int i = 0; i < count; i++) {
            cout << " " << ids[i] << "  " << texts[i] << "\n";
        }
    }
};

// ---------- Children of RecordList: each one only sets its own file names ----------

class CustomerList : public RecordList {
public:
    CustomerList() {
        listFileName = "Customer List.txt";
        counterFileName = "Customer Counter.txt";
    }
};

class MovieList : public RecordList {
public:
    MovieList() {
        listFileName = "Movie List.txt";
        counterFileName = "Movie Counter.txt";
    }
};

class CinemaList : public RecordList {
public:
    CinemaList() {
        listFileName = "Cinema List.txt";
        counterFileName = "Cinema Counter.txt";
    }
};

// Booking List.txt: every line is "B-XXX total", so this child adds the total payments
class BookingList : public RecordList {
public:
    BookingList() {
        listFileName = "Booking List.txt";
        counterFileName = "Booking Counter.txt";
    }

    // Adds up every total in the list
    double sumTotals() {
        double grandTotal = 0;
        for (int i = 0; i < count; i++) {
            double value = 0;
            if (parseDecimal(texts[i], value)) {
                grandTotal += value;
            }
        }
        return grandTotal;
    }

    // Same name as RecordList::display(), different implementation
    void display() {
        cout << " Booking ID    Total\n";
        drawLine('-', LINE_WIDTH);
        for (int i = 0; i < count; i++) {
            cout << " " << ids[i] << "         PHP " << texts[i] << "\n";
        }
    }
};

// =====================================================================================
// CLASS: BookingSystem - menus and workflows of the BOOKING block
// =====================================================================================
class BookingSystem {
private:
    Movie movies[MAX_MOVIES];               // array of Movie objects
    Cinema cinemas[MAX_CINEMAS];            // array of Cinema objects
    int movieCount;
    int cinemaCount;

    // POINTER: these point to the movie and cinema the customer is booking
    Movie *selectedMovie;
    Cinema *selectedCinema;

    SeatMap seatMap;                        // seat grid of the selected movie + cinema
    BookingList ledger;                     // Booking List.txt in memory
    CustomerList customerList;              // Customer List.txt (written by the CUSTOMER block)
    MovieList movieList;                    // Movie List.txt (written by the MOVIE block)
    CinemaList cinemaList;                  // Cinema List.txt (written by the CINEMA block)

    // ---------- Catalog (reads the MOVIE and CINEMA blocks' files) ----------

    // Reads Movie List.txt, then every M-XXX.txt in it
    void loadMovies() {
        movieList.load();
        for (int i = 0; i < movieList.getCount() && movieCount < MAX_MOVIES; i++) {
            Movie movie(toUpper(movieList.getID(i)), movieList.getText(i));
            movie.loadFromFile();           // fills genre, rating, dates if the file exists
            if (movie.getName() != "") {
                movies[movieCount] = movie;
                movieCount++;
            }
        }
    }

    // Reads Cinema List.txt, then every I-XXX.txt in it (cinemas without a usable file are skipped)
    void loadCinemas() {
        cinemaList.load();
        for (int i = 0; i < cinemaList.getCount() && cinemaCount < MAX_CINEMAS; i++) {
            Cinema cinema(toUpper(cinemaList.getID(i)), cinemaList.getText(i));
            if (cinema.loadFromFile() && cinema.isUsable()) {
                cinemas[cinemaCount] = cinema;
                cinemaCount++;
            }
        }
    }

    void loadCatalog() {
        movieCount = 0;
        cinemaCount = 0;
        loadMovies();
        loadCinemas();
    }

    // Builds the seat grid of the selected movie in the selected cinema from the confirmed
    // bookings. Seats of a deleted booking are free again because it is gone from the list.
    void buildSeatMap() {
        seatMap.setLayout(selectedCinema->getRows(), selectedCinema->getSeatsPerRow());
        ledger.load();
        for (int i = 0; i < ledger.getCount(); i++) {
            Booking saved(ledger.getID(i));
            if (saved.loadFromFile() &&
                toUpper(saved.getMovieName()) == toUpper(selectedMovie->getName()) &&
                toUpper(saved.getCinemaName()) == toUpper(selectedCinema->getName())) {
                seatMap.reserveFromText(saved.getSeatText());
            }
        }
    }

    // ---------- Customer Workflow ----------

    // GET BOOKING: fills 'booking' (passed by reference). Returns false if no booking was made.
    bool getBooking(Booking &booking) {
        printTitle("CHOOSE A MOVIE");
        for (int i = 0; i < movieCount; i++) {
            cout << " [" << (i + 1) << "] ";
            movies[i].display();
        }
        drawLine('-', LINE_WIDTH);
        cout << " Choose a movie (1-" << movieCount << ") : ";
        selectedMovie = &movies[getIntInRange(1, movieCount) - 1];          // POINTER

        printTitle("CHOOSE A CINEMA");
        for (int i = 0; i < cinemaCount; i++) {
            cout << " [" << (i + 1) << "] ";
            cinemas[i].display();
        }
        drawLine('-', LINE_WIDTH);
        cout << " Choose a cinema (1-" << cinemaCount << ") : ";
        selectedCinema = &cinemas[getIntInRange(1, cinemaCount) - 1];       // POINTER

        buildSeatMap();

        printTitle("SELECT SEATS");
        cout << " Movie  : " << selectedMovie->getName() << "\n";
        cout << " Cinema : " << selectedCinema->getName() << "\n";
        cout << " Price  : PHP " << formatMoney(selectedCinema->getPrice()) << " per seat\n";
        drawLine('-', LINE_WIDTH);
        cout << "\n";
        seatMap.display();

        // the customer sees the remaining seats BEFORE the booking is made
        int remaining = seatMap.countRemaining();
        cout << "\n Remaining seats: " << remaining << " / " << seatMap.getCapacity() << "\n";
        if (remaining == 0) {
            cout << "\n Sorry, this movie is fully booked in this cinema.\n";
            return false;
        }

        string requestedSeats[MAX_SEATS_PER_BOOKING];
        cout << "\n Seat number/s, separated by spaces (e.g. J21 J22)\n";
        cout << " Seats      : ";
        int requestedCount = readSeatList(1, requestedSeats);

        string seatText = "";
        for (int i = 0; i < requestedCount; i++) {
            if (i > 0) seatText += " ";
            seatText += requestedSeats[i];
        }

        // The customer was registered in the CUSTOMER block: only the customer ID is needed
        printTitle("CUSTOMER");
        Customer customer;
        bool customerFound = false;
        bool cancelled = false;
        do {
            string idInput = getNonEmptyLine(" Customer ID (C-XXX, or 0 to cancel) : ");
            if (idInput == "0") {
                cancelled = true;
            } else if (!isValidRecordID('C', idInput)) {
                cout << "  ! Invalid format. Use C-XXX.\n";
            } else {
                idInput = toUpper(trim(idInput));
                customerList.load();
                if (customerList.findIndex(idInput) == -1) {
                    cout << "  ! Customer " << idInput << " is not registered.\n";
                } else {
                    customer = Customer(idInput);
                    if (customer.loadFromFile()) {
                        customerFound = true;
                    } else {
                        cout << "  ! Unable to read the file of " << idInput << ".\n";
                    }
                }
            }
        } while (!customerFound && !cancelled);

        if (cancelled) {
            cout << "\n Booking cancelled.\n";
            return false;
        }
        cout << "\n Welcome, " << customer.getName() << "!\n";

        cout << "\n Payment type:\n";
        for (int i = 0; i < NUM_PAYMENT_TYPES; i++) {
            cout << "   [" << (i + 1) << "] " << PAYMENT_TYPES[i] << "\n";
        }
        cout << " Choose (1-3) : ";
        string payment = PAYMENT_TYPES[getIntInRange(1, NUM_PAYMENT_TYPES) - 1];

        // The ID is assigned only after all information was given
        int newNumber = ledger.readCounter() + 1;
        if (newNumber > MAX_BOOKING_NUMBER) {
            cout << "\n Sorry, booking limit reached.\n";
            return false;
        }
        string bookingID = makeID('B', newNumber);

        booking = Booking(bookingID, customer, payment,
                          selectedMovie->getName(),
                          selectedCinema->getName(), seatText);
        if (!booking.saveToFile()) {
            cout << "Unable to open file\n";
        }
        cout << "\n Your Booking ID is " << bookingID << "\n";
        return true;
    }

    // CHECK AVAILABILITY: verifies the seats, computes the total, saves and displays the order
    // POINTER: 'booking' is a pointer to the Booking made in getBooking()
    void checkAvailability(Booking *booking) {
        string requestedSeats[MAX_SEATS_PER_BOOKING];
        bool overflow = false;
        int seatCount = parseSeatTokens(booking->getSeatText(), requestedSeats, overflow);

        bool available = true;
        for (int i = 0; i < seatCount; i++) {
            if (!seatMap.seatExists(requestedSeats[i]) || seatMap.isReserved(requestedSeats[i])) {
                available = false;
            }
        }

        if (available) {
            double total = selectedCinema->getPrice() * seatCount;
            booking->setTotalText(formatMoney(total));
            booking->saveToFile();                          // file now ends with the total

            ledger.writeCounter(toInt(cutText(booking->getID(), 2, BOOKING_ID_LENGTH)));
            ledger.load();
            ledger.add(booking->getID(), booking->getTotalText());   // only now: information is complete

            for (int i = 0; i < seatCount; i++) {
                seatMap.setSeat(requestedSeats[i], 1);
            }
        } else {
            booking->setTotalText("NOT AVAILABLE");
            booking->saveToFile();      // counter and list unchanged: the next booking reuses this ID
        }

        cout << "\nYour order:\n";
        cout << "Movie name: " << booking->getMovieName() << "\n";
        cout << "Cinema: " << booking->getCinemaName() << "\n";
        cout << "Seat number/s: " << booking->getSeatText() << "\n";
        cout << "Type of payment: " << booking->getPayment() << "\n";
        cout << "Total payment: " << booking->getTotalText() << "\n";

        if (available) {
            cout << "Remaining seats: " << seatMap.countRemaining()
                 << " / " << seatMap.getCapacity() << "\n";
        } else {
            cout << "  ! Some seats are taken or invalid. Please try again.\n";
        }
    }

    void customerFlow() {
        loadCatalog();
        if (movieCount == 0 || cinemaCount == 0) {
            cout << "\n No showing is open for booking yet.\n";
            return;
        }

        Booking booking;
        if (getBooking(booking)) {
            checkAvailability(&booking);                    // POINTER: address of the booking
        }
        selectedMovie = 0;                                  // POINTER: no selection anymore
        selectedCinema = 0;
    }

    // ---------- Manager Workflow ----------

    void displayBooking() {
        printTitle("DISPLAY BOOKING");
        string bookingID = getNonEmptyLine(" Booking ID (B-XXX): ");
        if (!isValidRecordID('B', bookingID)) {
            cout << "  ! Invalid format. Use B-XXX.\n";
            return;
        }
        bookingID = toUpper(trim(bookingID));

        ledger.load();
        if (ledger.findIndex(bookingID) == -1) {
            cout << "  ! Booking not found.\n";
            return;
        }

        Booking booking(bookingID);
        cout << "\n";
        drawLine('-', LINE_WIDTH);
        if (booking.loadFromFile()) {
            booking.display();
        } else {
            cout << "Unable to open file\n";
        }
        drawLine('-', LINE_WIDTH);
    }

    void displayAllBookings() {
        printTitle("ALL BOOKINGS");
        int count = ledger.load();
        if (count == 0) {
            cout << " No bookings yet.\n";
            drawLine('-', LINE_WIDTH);
            return;
        }

        ledger.display();                   // BookingList's own version of display()
        drawLine('-', LINE_WIDTH);
    }

    void displayTotalPayments() {
        printTitle("TOTAL PAYMENTS");
        int count = ledger.load();
        cout << " Total Bookings : " << count << "\n";
        cout << " Total Earnings : PHP " << formatMoney(ledger.sumTotals()) << "\n";
        drawLine('-', LINE_WIDTH);
    }

    // DELETE BOOKING: checks that the booking exists, deletes its file and updates the list
    void deleteBooking() {
        printTitle("DELETE BOOKING");
        string bookingID = getNonEmptyLine(" Booking ID (B-XXX): ");
        if (!isValidRecordID('B', bookingID)) {
            cout << "  ! Invalid format. Use B-XXX.\n";
            return;
        }
        bookingID = toUpper(trim(bookingID));

        ledger.load();
        int targetIndex = ledger.findIndex(bookingID);
        if (targetIndex == -1) {
            cout << "  ! Booking " << bookingID << " does NOT exist in database.\n";
            return;
        }
        cout << "  [OK] Booking " << bookingID << " found. Proceeding with deletion...\n";

        // The seats become free again automatically: seat maps are built from the list
        string fileName = bookingID + ".txt";
        remove(fileName.c_str());

        if (!ledger.removeAt(targetIndex)) {
            cout << "Unable to open file\n";
            return;
        }
        cout << "\n Booking " << bookingID << " deleted successfully.\n";
    }

    // Manager menu loop (runs until the manager chooses Back)
    void managerFlow() {
        int managerChoice = 0;
        do {
            printTitle("MANAGER MENU");
            cout << "   [1] Display booking\n";
            cout << "   [2] Display all bookings\n";
            cout << "   [3] Delete booking\n";
            cout << "   [4] Total payments received\n";
            cout << "   [5] Back\n";
            drawLine('-', LINE_WIDTH);
            cout << " Choice: ";
            managerChoice = getIntInRange(1, 5);

            switch (managerChoice) {
                case 1: displayBooking(); break;
                case 2: displayAllBookings(); break;
                case 3: deleteBooking(); break;
                case 4: displayTotalPayments(); break;
                case 5: cout << "\n Returning to booking menu.\n"; break;
                default: cout << "  ! Invalid choice.\n"; break;
            }
        } while (managerChoice != 5);
    }

public:
    BookingSystem() {
        movieCount = 0;
        cinemaCount = 0;
        selectedMovie = 0;                  // POINTER: 0 means "points to nothing"
        selectedCinema = 0;
    }

    // Entry points used by the main menu in main.cpp
    // Booking block for a CUSTOMER: GET BOOKING, then CHECK AVAILABILITY
    void runCustomer() {
        customerFlow();
    }

    // Booking block for the MANAGER: display, delete, total payments
    void runManager() {
        managerFlow();
    }
};

class CustomerSystem{

};

class MovieSystem{

};
class CinemaSystem{

};
#endif