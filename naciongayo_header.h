/*
 * =====================================================================================
 * File:        naciongayo_header.h
 * Description: Block 4 (BOOKING) - Machine Problem 2, object-oriented version.
 *              Contains constants, text/validation helpers, and the classes:
 *
 *                Record  (abstract parent)
 *                  |-- Movie     (read from M-XXX.txt of the MOVIE block)
 *                  |-- Cinema    (read from I-XXX.txt of the CINEMA block)
 *                  `-- Booking   (B-XXX.txt of this block)
 *                Customer        (name, mobile number, email address)
 *                SeatMap         (seat grid of one cinema, dynamic array)
 *                BookingList     (Booking List.txt and Booking Counter.txt)
 *                BookingSystem   (menus and workflows; ties everything together)
 *
 * Author:      Naciongayo
 * Course:      Engg 30.02 - Machine Problem 2
 * =====================================================================================
 */

#ifndef BOOKING_HEADER_H
#define BOOKING_HEADER_H

#include <iostream>
#include <fstream>
#include <string>
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
const int MAX_BOOKINGS = 1000;
const int MAX_SEATS_PER_BOOKING = 50;
const int MAX_FILE_LINES = 12;      // Most lines read from any one record file
const int LINE_WIDTH = 46;          // Width of formatted screen titles

const int MOBILE_NUMBER_LENGTH = 13;
const int BOOKING_ID_LENGTH = 5;
const int MAX_BOOKING_NUMBER = 999;
const int MAX_MANAGER_LOGIN_ATTEMPTS = 3;

// =====================================================================================
// Helper Functions & Tokenizer (unchanged from MP1, except where noted)
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

// Checks if a character is a letter
bool isLetterChar(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
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

// Validates a booking ID (pass by value so it doesn't modify the caller's string)
bool isValidBookingID(string bookingID) {
    bookingID = toUpper(trim(bookingID));
    if (textLength(bookingID) != BOOKING_ID_LENGTH) return false;
    if (bookingID[0] != 'B' || bookingID[1] != '-') return false;
    for (int i = 2; i < BOOKING_ID_LENGTH; i++) {
        if (!isDigitChar(bookingID[i])) return false;
    }
    return true;
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
    ifstream in(fileName.c_str());
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

// NEW (MP2): Checks that an email address looks valid (local@domain.tld)
bool isValidEmail(const string &email) {
    int len = textLength(email);
    int atPos = -1;

    for (int i = 0; i < len; i++) {
        if (isBlankChar(email[i])) return false;
        if (email[i] == '@') {
            if (atPos != -1) return false;      // only one '@' allowed
            atPos = i;
        }
    }
    if (atPos < 1 || atPos >= len - 3) return false;    // needs a local part and a domain like "a.b"

    // Local part: letters, digits and . _ % + -  (cannot start or end with a dot)
    for (int i = 0; i < atPos; i++) {
        char c = email[i];
        bool ok = isLetterChar(c) || isDigitChar(c) ||
                  c == '.' || c == '_' || c == '%' || c == '+' || c == '-';
        if (!ok) return false;
    }
    if (email[0] == '.' || email[atPos - 1] == '.') return false;

    // Domain: letters, digits, '-' and dots; at least one dot, none at the edges or doubled
    int dotCount = 0;
    for (int i = atPos + 1; i < len; i++) {
        char c = email[i];
        bool ok = isLetterChar(c) || isDigitChar(c) || c == '-' || c == '.';
        if (!ok) return false;
        if (c == '.') {
            if (i == atPos + 1 || i == len - 1) return false;
            if (email[i - 1] == '.') return false;
            dotCount++;
        }
    }
    return (dotCount > 0);
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

// Prompts the user continuously until a valid mobile number is provided
string getMobile() {
    string mobile;
    bool valid;
    do {
        getline(cin, mobile);
        mobile = trim(mobile);
        valid = (textLength(mobile) == MOBILE_NUMBER_LENGTH) &&
                (mobile[0] == '+' && mobile[1] == '6' && mobile[2] == '3');
        for (int i = 3; i < MOBILE_NUMBER_LENGTH && valid; i++) {
            if (!isDigitChar(mobile[i])) valid = false;
        }
        if (!valid) {
            cout << "  ! Format: +63 followed by 10 digits: ";
        }
    } while (!valid);
    return mobile;
}

// NEW (MP2): Prompts the user continuously until a valid email address is provided
string getEmail() {
    string email;
    bool valid;
    do {
        getline(cin, email);
        email = trim(email);
        valid = isValidEmail(email);
        if (!valid) {
            cout << "  ! Enter a valid email (e.g. name@mail.com): ";
        }
    } while (!valid);
    return email;
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

        if (overflow) {
            cout << "  ! Too many seats entered (Max: " << MAX_SEATS_PER_BOOKING << ").\n  Try again: ";
            valid = false;
            continue;
        }

        valid = (count >= minSeats);
        for (int i = 0; i < count && valid; i++) {
            int rowIndex, seatNo;
            if (!parseSeat(seats[i], rowIndex, seatNo)) valid = false;
        }
        for (int i = 0; i < count && valid; i++) {
            for (int j = 0; j < i; j++) {
                if (seats[j] == seats[i]) valid = false;
            }
        }
        if (!valid) {
            cout << "  ! Invalid seat list. Example: J20 K20\n  Try again: ";
        }
    } while (!valid);
    return count;
}

// =====================================================================================
// CLASS: Record  (abstract PARENT class)
// -------------------------------------------------------------------------------------
// INHERITANCE   : Movie, Cinema and Booking inherit the ID and the file name from here.
// ENCAPSULATION : 'id' is protected; the outside world uses getID().
// POLYMORPHISM  : listLine(), display() and loadFromFile() are pure virtual. Every child
//                 overrides them, and a Record pointer calls the child's version.
// =====================================================================================
class Record {
protected:
    string id;                              // e.g. M-001, I-001, B-001

public:
    Record(const string &recordID = "") : id(recordID) {}      // constructor
    virtual ~Record() {}                                       // virtual destructor

    string getID() const { return id; }
    string getFileName() const { return id + ".txt"; }

    virtual string listLine() const = 0;        // one-line text, e.g. "M-001 Forgotten Island"
    virtual void display() const = 0;           // full details on screen
    virtual bool loadFromFile() = 0;            // fills the object from its own .txt file
};

// =====================================================================================
// CLASS: Customer  (the details asked from the customer)
// =====================================================================================
class Customer {
private:
    string name;
    string mobile;
    string email;

public:
    Customer() : name(""), mobile(""), email("") {}            // default constructor
    Customer(const string &custName, const string &custMobile, const string &custEmail)
        : name(custName), mobile(custMobile), email(custEmail) {}

    string getName() const { return name; }
    string getMobile() const { return mobile; }
    string getEmail() const { return email; }

    // Asks the customer for every detail, with input validation
    void promptDetails() {
        name = getNonEmptyLine(" Customer name : ");
        cout << " Mobile number (+63XXXXXXXXXX) : ";
        mobile = ::getMobile();     // '::' = the global input function, not this class's getter
        cout << " Email address : ";
        email = ::getEmail();
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
    Movie(const string &movieID = "", const string &movieName = "")
        : Record(movieID), name(movieName), genre(""), startDate(""), endDate(""), rating("") {}

    string getName() const { return name; }
    string getGenre() const { return genre; }
    string getRating() const { return rating; }

    virtual string listLine() const { return id + " " + name; }

    virtual void display() const {
        cout << name << "\n";
        cout << "     Genre  : " << genre << "   Rating: " << rating << "\n";
        if (startDate != "" && endDate != "") {
            cout << "     Runs   : " << startDate << " to " << endDate << "\n";
        }
    }

    // M-XXX.txt: name / genre / Start date: ... / End date: ... / Rating: ... / ...
    virtual bool loadFromFile() {
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
    Cinema(const string &cinemaID = "", const string &cinemaName = "")
        : Record(cinemaID), name(cinemaName), rows(0), seatsPerRow(0), price(0), type("") {}

    string getName() const { return name; }
    int getRows() const { return rows; }
    int getSeatsPerRow() const { return seatsPerRow; }
    int getCapacity() const { return rows * seatsPerRow; }
    double getPrice() const { return price; }

    // A cinema can only be booked if its layout fits the seat names A1..Z50 and it has a price
    bool isUsable() const {
        return (rows >= 1 && rows <= MAX_ROWS &&
                seatsPerRow >= 1 && seatsPerRow <= MAX_SEATS_PER_ROW &&
                price > 0);
    }

    virtual string listLine() const { return id + " " + name; }

    virtual void display() const {
        cout << name << "\n";
        cout << "     Type   : " << type << "   Capacity: " << getCapacity() << "\n";
        cout << "     Price  : PHP " << formatMoney(price) << " per seat\n";
    }

    /* I-XXX.txt (one value per line):
     *  0 name              1 address            2 Number of rows: N
     *  3 Number of seats per row: N             4 Capacity: N
     *  5 contact number    6 Price: N           7 Cinema type: X      8 code */
    virtual bool loadFromFile() {
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
    Customer customer;          // composition: a booking HAS a customer
    string payment;
    string movieName;
    string cinemaName;
    string seatText;            // "J21 J22 J23"
    string totalText;           // "" until CHECK AVAILABILITY runs

    // Writes the booking in the file format; 'prefix' is put before every line
    void writeTo(ostream &out, const string &prefix) const {
        out << prefix << "Customer name: " << customer.getName() << "\n";
        out << prefix << customer.getMobile() << "\n";
        out << prefix << customer.getEmail() << "\n";
        out << prefix << "Type of payment: " << payment << "\n";
        out << prefix << "Movie name: " << movieName << "\n";
        out << prefix << "Cinema: " << cinemaName << "\n";
        out << prefix << "Seat number/s: " << seatText << "\n";
        out << prefix << id << "\n";
        if (totalText != "") {
            out << prefix << "Total payment: " << totalText << "\n";
        }
    }

public:
    Booking(const string &bookingID = "") : Record(bookingID) {}

    Booking(const string &bookingID, const Customer &cust, const string &pay,
            const string &movie, const string &cinema, const string &seats)
        : Record(bookingID), customer(cust), payment(pay), movieName(movie),
          cinemaName(cinema), seatText(seats), totalText("") {}

    string getPayment() const { return payment; }
    string getMovieName() const { return movieName; }
    string getCinemaName() const { return cinemaName; }
    string getSeatText() const { return seatText; }
    string getTotalText() const { return totalText; }
    void setTotalText(const string &total) { totalText = total; }

    virtual string listLine() const { return id + " " + totalText; }

    virtual void display() const { writeTo(cout, " "); }

    // Creates (or overwrites) B-XXX.txt
    bool saveToFile() const {
        ofstream out(getFileName().c_str());
        if (!out.is_open()) return false;
        writeTo(out, "");
        out.close();
        return true;
    }

    // Reads B-XXX.txt back into the object
    virtual bool loadFromFile() {
        string lines[MAX_FILE_LINES];
        int count = readFileLines(getFileName(), lines, MAX_FILE_LINES);
        if (count < 8) return false;

        customer = Customer(extractAfterColon(lines[0]), lines[1], lines[2]);
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
    int *cells;                             // POINTER: dynamic array, 0 = free, 1 = reserved

    SeatMap(const SeatMap &other);              // copying is disabled (the object owns 'cells')
    SeatMap &operator=(const SeatMap &other);

public:
    SeatMap(int numRows, int numSeatsPerRow) : rows(numRows), seatsPerRow(numSeatsPerRow) {
        cells = new int[rows * seatsPerRow];    // POINTER: memory is reserved here
        for (int i = 0; i < rows * seatsPerRow; i++) {
            cells[i] = 0;
        }
    }

    ~SeatMap() {
        delete [] cells;                        // POINTER: memory is given back here
    }

    int getCapacity() const { return rows * seatsPerRow; }

    // True if the seat name (e.g. J20) exists in this cinema's layout
    bool seatExists(const string &seat) const {
        int rowIndex, seatNo;
        if (!parseSeat(seat, rowIndex, seatNo)) return false;
        return (rowIndex < rows) && (seatNo <= seatsPerRow);
    }

    bool isReserved(const string &seat) const {
        int rowIndex, seatNo;
        if (!seatExists(seat)) return false;
        parseSeat(seat, rowIndex, seatNo);
        return cells[rowIndex * seatsPerRow + (seatNo - 1)] == 1;
    }

    // Sets one seat to value (1 = reserve, 0 = free)
    void setSeat(const string &seat, int value) {
        int rowIndex, seatNo;
        if (!seatExists(seat)) return;
        parseSeat(seat, rowIndex, seatNo);
        cells[rowIndex * seatsPerRow + (seatNo - 1)] = value;
    }

    // Reserves every seat of a text such as "A1 A2 C10"
    void reserveFromText(const string &text) {
        int index = 0;
        string token = "";
        while ((token = extractNextToken(text, index)) != "") {
            setSeat(token, 1);
        }
    }

    int countRemaining() const {
        int available = 0;
        for (int i = 0; i < rows * seatsPerRow; i++) {
            if (cells[i] == 0) available++;
        }
        return available;
    }

    // Displays the grid of free [ ] and reserved [X] seats
    void display() const {
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
                if (cells[r * seatsPerRow + c] == 1) cout << "[X] ";
                else cout << "[ ] ";
            }
            cout << "\n";
        }
        cout << "\n   [ ] = available      [X] = reserved  \n";
    }
};

// =====================================================================================
// CLASS: BookingList - Booking List.txt (confirmed bookings) and Booking Counter.txt
// =====================================================================================
class BookingList {
private:
    string ids[MAX_BOOKINGS];
    string totals[MAX_BOOKINGS];
    int count;

public:
    BookingList() : count(0) {}

    int getCount() const { return count; }
    string getID(int i) const { return (i >= 0 && i < count) ? ids[i] : ""; }
    string getTotal(int i) const { return (i >= 0 && i < count) ? totals[i] : ""; }

    // Reads Booking List.txt (a missing file means no bookings yet)
    int load() {
        count = 0;
        ifstream in("Booking List.txt");
        if (!in.is_open()) return 0;

        string line;
        while (count < MAX_BOOKINGS && getline(in, line)) {
            line = trim(line);
            int index = 0;
            string idToken = extractNextToken(line, index);
            string totalToken = extractNextToken(line, index);
            if (idToken != "" && totalToken != "") {
                ids[count] = idToken;
                totals[count] = totalToken;
                count++;
            }
        }
        in.close();
        return count;
    }

    // Position of a booking ID in the list, or -1
    int findIndex(const string &bookingID) const {
        string target = toUpper(bookingID);
        for (int i = 0; i < count; i++) {
            if (toUpper(ids[i]) == target) return i;
        }
        return -1;
    }

    // Adds a confirmed booking to the end of the file and to memory
    bool add(const string &bookingID, const string &total) {
        if (count >= MAX_BOOKINGS) return false;
        ofstream out("Booking List.txt", ios::app);
        if (!out.is_open()) return false;
        out << bookingID << " " << total << "\n";
        out.close();
        ids[count] = bookingID;
        totals[count] = total;
        count++;
        return true;
    }

    // Removes one booking and rewrites the file without it
    bool removeAt(int index) {
        if (index < 0 || index >= count) return false;
        ofstream out("Booking List.txt");
        if (!out.is_open()) return false;
        for (int i = 0; i < count; i++) {
            if (i != index) {
                out << ids[i] << " " << totals[i] << "\n";
            }
        }
        out.close();
        for (int i = index; i < count - 1; i++) {
            ids[i] = ids[i + 1];
            totals[i] = totals[i + 1];
        }
        count--;
        return true;
    }

    // Adds up every total in the list
    double sumTotals() const {
        double grandTotal = 0;
        for (int i = 0; i < count; i++) {
            double value = 0;
            if (parseDecimal(totals[i], value)) {
                grandTotal += value;
            }
        }
        return grandTotal;
    }

    // Booking Counter.txt keeps the last number used, so numbers are never reused
    static int readCounter() {
        int lastNumber = 0;
        ifstream in("Booking Counter.txt");
        if (in.is_open()) {
            in >> lastNumber;
            in.close();
        }
        return lastNumber;
    }

    static void writeCounter(int number) {
        ofstream out("Booking Counter.txt");
        if (out.is_open()) {
            out << number;
            out.close();
        }
    }
};

// POLYMORPHISM: one function works for a Movie, a Cinema or a Booking.
// The Record pointer calls the display() of the real (child) object.
void showRecord(const Record *record) {
    record->display();
}

// =====================================================================================
// CLASS: BookingSystem - menus and workflows of the BOOKING block
// =====================================================================================
class BookingSystem {
private:
    // POINTER: the catalog is an array of pointers to objects created with 'new'
    Movie *movies[MAX_MOVIES];
    Cinema *cinemas[MAX_CINEMAS];
    int movieCount;
    int cinemaCount;

    // POINTER: what the customer is currently booking (point into the arrays above)
    Movie *selectedMovie;
    Cinema *selectedCinema;
    SeatMap *seatMap;                       // POINTER: seat grid of the selected movie + cinema

    BookingList ledger;                     // Booking List.txt in memory

    BookingSystem(const BookingSystem &other);              // copying is disabled
    BookingSystem &operator=(const BookingSystem &other);

    // ---------- Catalog (reads the MOVIE and CINEMA blocks' files) ----------

    void clearCatalog() {
        for (int i = 0; i < movieCount; i++) {
            delete movies[i];
        }
        for (int i = 0; i < cinemaCount; i++) {
            delete cinemas[i];
        }
        movieCount = 0;
        cinemaCount = 0;
    }

    // Reads Movie List.txt, then every M-XXX.txt in it
    void loadMovies() {
        ifstream in("Movie List.txt");
        if (!in.is_open()) return;

        string line;
        while (movieCount < MAX_MOVIES && getline(in, line)) {
            line = trim(line);
            int index = 0;
            string idToken = extractNextToken(line, index);
            string listName = trim(cutText(line, index, textLength(line)));
            if (idToken == "") continue;

            Movie *movie = new Movie(toUpper(idToken), listName);   // POINTER: new object
            movie->loadFromFile();          // fills genre, rating, dates if the file exists
            if (movie->getName() == "") {
                delete movie;
            } else {
                movies[movieCount] = movie;
                movieCount++;
            }
        }
        in.close();
    }

    // Reads Cinema List.txt, then every I-XXX.txt in it (cinemas without a usable file are skipped)
    void loadCinemas() {
        ifstream in("Cinema List.txt");
        if (!in.is_open()) return;

        string line;
        while (cinemaCount < MAX_CINEMAS && getline(in, line)) {
            line = trim(line);
            int index = 0;
            string idToken = extractNextToken(line, index);
            string listName = trim(cutText(line, index, textLength(line)));
            if (idToken == "") continue;

            Cinema *cinema = new Cinema(toUpper(idToken), listName);    // POINTER: new object
            if (cinema->loadFromFile() && cinema->isUsable()) {
                cinemas[cinemaCount] = cinema;
                cinemaCount++;
            } else {
                delete cinema;
            }
        }
        in.close();
    }

    void loadCatalog() {
        clearCatalog();
        loadMovies();
        loadCinemas();
    }

    // Builds the seat grid of a movie in a cinema from the confirmed bookings.
    // Seats of a deleted booking are free again because its file is gone from the list.
    SeatMap *buildSeatMap(const Movie *movie, const Cinema *cinema) {
        SeatMap *map = new SeatMap(cinema->getRows(), cinema->getSeatsPerRow());   // POINTER
        ledger.load();
        for (int i = 0; i < ledger.getCount(); i++) {
            Booking saved(ledger.getID(i));
            if (saved.loadFromFile() &&
                toUpper(saved.getMovieName()) == toUpper(movie->getName()) &&
                toUpper(saved.getCinemaName()) == toUpper(cinema->getName())) {
                map->reserveFromText(saved.getSeatText());
            }
        }
        return map;
    }

    void releaseSelection() {
        delete seatMap;                     // POINTER: delete on a null pointer is safe
        seatMap = 0;
        selectedMovie = 0;
        selectedCinema = 0;
    }

    // ---------- Customer Workflow ----------

    // GET BOOKING: returns a new Booking (caller deletes it), or 0 if no booking was made
    Booking *getBooking() {
        printTitle("CHOOSE A MOVIE");
        for (int i = 0; i < movieCount; i++) {
            cout << " [" << (i + 1) << "] ";
            showRecord(movies[i]);          // POINTER: Record pointer, polymorphic call
        }
        drawLine('-', LINE_WIDTH);
        cout << " Choose a movie (1-" << movieCount << ") : ";
        selectedMovie = movies[getIntInRange(1, movieCount) - 1];

        printTitle("CHOOSE A CINEMA");
        for (int i = 0; i < cinemaCount; i++) {
            cout << " [" << (i + 1) << "] ";
            showRecord(cinemas[i]);         // POINTER: Record pointer, polymorphic call
        }
        drawLine('-', LINE_WIDTH);
        cout << " Choose a cinema (1-" << cinemaCount << ") : ";
        selectedCinema = cinemas[getIntInRange(1, cinemaCount) - 1];

        delete seatMap;
        seatMap = buildSeatMap(selectedMovie, selectedCinema);

        printTitle("SELECT SEATS");
        cout << " Movie  : " << selectedMovie->getName() << "\n";
        cout << " Cinema : " << selectedCinema->getName() << "\n";
        cout << " Price  : PHP " << formatMoney(selectedCinema->getPrice()) << " per seat\n";
        drawLine('-', LINE_WIDTH);
        cout << "\n";
        seatMap->display();

        // MP2: the customer sees the remaining seats BEFORE the booking is made
        int remaining = seatMap->countRemaining();
        cout << "\n Remaining seats: " << remaining << " / " << seatMap->getCapacity() << "\n";
        if (remaining == 0) {
            cout << "\n Sorry, this movie is fully booked in this cinema.\n";
            return 0;
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

        printTitle("YOUR DETAILS");
        Customer customer;
        customer.promptDetails();

        cout << "\n Payment type:\n";
        for (int i = 0; i < NUM_PAYMENT_TYPES; i++) {
            cout << "   [" << (i + 1) << "] " << PAYMENT_TYPES[i] << "\n";
        }
        cout << " Choose (1-3) : ";
        string payment = PAYMENT_TYPES[getIntInRange(1, NUM_PAYMENT_TYPES) - 1];

        // The ID is assigned only after all information was given
        int newNumber = BookingList::readCounter() + 1;
        if (newNumber > MAX_BOOKING_NUMBER) {
            cout << "\n Sorry, booking limit reached.\n";
            return 0;
        }
        string bookingID = makeID('B', newNumber);

        Booking *booking = new Booking(bookingID, customer, payment,       // POINTER: new object
                                       selectedMovie->getName(),
                                       selectedCinema->getName(), seatText);
        if (!booking->saveToFile()) {
            cout << "Unable to open file\n";
        }
        cout << "\n Your Booking ID is " << bookingID << "\n";
        return booking;
    }

    // CHECK AVAILABILITY: verifies the seats, computes the total, saves and displays the order
    void checkAvailability(Booking *booking) {
        string requestedSeats[MAX_SEATS_PER_BOOKING];
        bool overflow = false;
        int seatCount = parseSeatTokens(booking->getSeatText(), requestedSeats, overflow);

        bool available = true;
        for (int i = 0; i < seatCount; i++) {
            if (!seatMap->seatExists(requestedSeats[i]) || seatMap->isReserved(requestedSeats[i])) {
                available = false;
            }
        }

        if (available) {
            double total = selectedCinema->getPrice() * seatCount;
            booking->setTotalText(formatMoney(total));
            booking->saveToFile();                          // file now ends with the total

            BookingList::writeCounter(toInt(cutText(booking->getID(), 2, BOOKING_ID_LENGTH)));
            ledger.load();
            ledger.add(booking->getID(), booking->getTotalText());   // only now: information is complete

            for (int i = 0; i < seatCount; i++) {
                seatMap->setSeat(requestedSeats[i], 1);
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
            cout << "Remaining seats: " << seatMap->countRemaining()
                 << " / " << seatMap->getCapacity() << "\n";
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

        Booking *booking = getBooking();        // POINTER: 0 means no booking was made
        if (booking != 0) {
            checkAvailability(booking);
            delete booking;                     // POINTER: free the booking object
        }
        releaseSelection();
    }

    // ---------- Manager Workflow ----------

    bool verifyManager() {
        bool verified = false;
        int attempt = 0;
        printTitle("MANAGER LOGIN");
        while (attempt < MAX_MANAGER_LOGIN_ATTEMPTS && !verified) {
            string code;
            cout << " Manager code : ";
            getline(cin, code);
            if (trim(code) == MANAGER_CODE) {
                verified = true;
            } else {
                cout << "  ! Wrong code. Attempts left: "
                     << (MAX_MANAGER_LOGIN_ATTEMPTS - 1 - attempt) << "\n";
            }
            attempt++;
        }
        return verified;
    }

    void displayBooking() {
        printTitle("DISPLAY BOOKING");
        string bookingID = getNonEmptyLine(" Booking ID (B-XXX): ");
        if (!isValidBookingID(bookingID)) {
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
            const Record *record = &booking;    // POINTER: base-class pointer to a Booking
            record->display();                  // calls Booking::display()
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

        cout << " Booking ID    Total\n";
        drawLine('-', LINE_WIDTH);
        for (int i = 0; i < count; i++) {
            cout << " " << ledger.getID(i) << "         PHP " << ledger.getTotal(i) << "\n";
        }
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
        if (!isValidBookingID(bookingID)) {
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
    BookingSystem() : movieCount(0), cinemaCount(0),
                      selectedMovie(0), selectedCinema(0), seatMap(0) {}

    ~BookingSystem() {
        clearCatalog();
        releaseSelection();
    }

    // Main menu loop. Returns 0 on a normal exit, 1 if manager access was denied.
    int run() {
        int mainChoice = 0;
        do {
            printTitle("BOOKING MENU");
            cout << "Welcome to the Movie Booking System!\n";
            cout << "Please select an option:\n";
            cout << "   [1] Customer\n";
            cout << "   [2] Manager\n";
            cout << "   [3] Exit\n";
            cout << "\n Choice: ";
            mainChoice = getIntInRange(1, 3);

            if (mainChoice == 1) {
                customerFlow();
            } else if (mainChoice == 2) {
                if (!verifyManager()) {
                    cout << "\n Access denied. Exiting Program...\n";
                    return 1;
                }
                managerFlow();
            }
        } while (mainChoice != 3);
        return 0;
    }
};

#endif
