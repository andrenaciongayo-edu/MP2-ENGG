#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>              // today's date, used to check that movie dates are in the future

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

// ---------- MOVIE block constants ----------
const int NUM_MOVIE_GENRES = 8;
const string MOVIE_GENRES[NUM_MOVIE_GENRES] = {"Action", "Romance", "Comedy", "Drama", "Horror",
                                               "Science Fiction", "Animated", "Documentary"};
const int NUM_MOVIE_RATINGS = 5;
const string MOVIE_RATINGS[NUM_MOVIE_RATINGS] = {"G", "PG", "R-13", "R-16", "R-18"};
const int NUM_EMAIL_DOMAINS = 5;
const string EMAIL_DOMAINS[NUM_EMAIL_DOMAINS] = {"gmail.com", "yahoo.com", "outlook.com",
                                                 "hotmail.com", "icloud.com"};
const int MOVIE_MOBILE_LENGTH = 13;         // +639 followed by 9 digits

// ---------- CUSTOMER block constants ----------
const int CUSTOMER_MIN_AGE = 18;            // only adults can register

// ---------- CINEMA block constants ----------
// To be moved here the constants from the Cinema block below.

// =====================================================================================
// Helper Functions & Tokenizer
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

// Validates an ID such as B-001 or C-012 (pass by value so the caller's string is untouched)
bool isValidRecordID(char prefix, string recordID) {
    recordID = toUpper(trim(recordID));
    if (textLength(recordID) != RECORD_ID_LENGTH) return false;
    if (recordID[0] != prefix || recordID[1] != '-') return false;
    for (int i = 2; i < RECORD_ID_LENGTH; i++) {
        if (!isDigitChar(recordID[i])) return false;
    }
    return true;
}

// Turns "Reyes, Rose Marie A." (CUSTOMER block format) into "Rose Marie A. Reyes"
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

// Position of 'input' in a list of allowed words (ignoring upper/lower case), or -1
int findChoice(const string choices[], int numChoices, const string &input) {
    string target = toUpper(trim(input));
    for (int i = 0; i < numChoices; i++) {
        if (toUpper(choices[i]) == target) return i;
    }
    return -1;
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
// CLASS: Movie  (CHILD of Record) - one M-XXX.txt file, written by the MOVIE block
// and read by the BOOKING block
// =====================================================================================
class Movie : public Record {
private:
    string name;
    string genre;
    string startDate;
    string endDate;
    string rating;
    string contactPerson;
    string mobile;
    string email;

public:
    Movie() {
        name = "";
        genre = "";
        startDate = "";
        endDate = "";
        rating = "";
        contactPerson = "";
        mobile = "";
        email = "";
    }

    Movie(string movieID, string movieName) {
        id = movieID;                       // 'id' is inherited from Record
        name = movieName;
        genre = "";
        startDate = "";
        endDate = "";
        rating = "";
        contactPerson = "";
        mobile = "";
        email = "";
    }

    string getName() { return name; }
    string getGenre() { return genre; }
    string getRating() { return rating; }

    // Sets every detail asked by the MOVIE block (the values were already validated)
    void setDetails(string movieName, string movieGenre, string start, string end,
                    string movieRating, string contact, string movieMobile, string movieEmail) {
        name = movieName;
        genre = movieGenre;
        startDate = start;
        endDate = end;
        rating = movieRating;
        contactPerson = contact;
        mobile = movieMobile;
        email = movieEmail;
    }

    // Same name as Record::display()
    void display() {
        cout << name << "\n";
        cout << "     Genre  : " << genre << "   Rating: " << rating << "\n";
        if (startDate != "" && endDate != "") {
            cout << "     Runs   : " << startDate << " to " << endDate << "\n";
        }
    }

    void displayDetails() {
        cout << name << "\n";
        cout << genre << "\n";
        cout << "Start date: " << startDate << "\n";
        cout << "End date: " << endDate << "\n";
        cout << "Rating: " << rating << "\n";
        cout << contactPerson << "\n";
        cout << "Mobile Number: " << mobile << "\n";
        cout << "Email Address: " << email << "\n";
        cout << "Movie ID # " << id << "\n";
    }

    // Creates (or overwrites) M-XXX.txt
    bool saveToFile() {
        ofstream out(getFileName());
        if (!out.is_open()) return false;
        out << name << "\n";
        out << genre << "\n";
        out << "Start date: " << startDate << "\n";
        out << "End date: " << endDate << "\n";
        out << "Rating: " << rating << "\n";
        out << contactPerson << "\n";
        out << "Mobile Number: " << mobile << "\n";
        out << "Email Address: " << email << "\n";
        out << "Movie ID # " << id << "\n";
        out.close();
        return true;
    }

    /* M-XXX.txt (one value per line):
     *  0 name       1 genre       2 Start date: ...     3 End date: ...     4 Rating: ...
     *  5 contact person           6 Mobile Number: ...  7 Email Address: ...
     *  8 Movie ID # M-XXX */
    bool loadFromFile() {
        string lines[MAX_FILE_LINES];
        int count = readFileLines(getFileName(), lines, MAX_FILE_LINES);
        if (count < 1) return false;

        if (lines[0] != "") name = lines[0];
        if (count > 1) genre = lines[1];
        if (count > 2) startDate = extractAfterColon(lines[2]);
        if (count > 3) endDate = extractAfterColon(lines[3]);
        if (count > 4) rating = extractAfterColon(lines[4]);
        if (count > 5) contactPerson = lines[5];
        if (count > 6) mobile = extractAfterColon(lines[6]);
        if (count > 7) email = extractAfterColon(lines[7]);
        return true;
    }
};

// =====================================================================================
// CLASS: Cinema  (CHILD of Record) - one I-XXX.txt file, written by the CINEMA block
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

// =====================================================================================
// CLASS: CustomerSystem - menus and workflows of the CUSTOMER block
// Writes C-XXX.txt and Customer List.txt (read by the BOOKING block through the
// Customer and CustomerList classes above, so the file formats must stay the same).
// =====================================================================================
class CustomerSystem {
private:
    CustomerList customerList;              // Customer List.txt in memory

    // ---------- Validations of the incoming inputs ----------

    bool isLetterChar(char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    // Letters, spaces, '-', '\'' and '.' only (digits too if allowDigits). No commas, because the
    // files use ", " to separate the parts of a name and address.
    bool isValidText(const string &text, bool allowDigits) {
        int len = textLength(text);
        bool hasLetterOrDigit = false;
        for (int i = 0; i < len; i++) {
            char c = text[i];
            if (isLetterChar(c) || (allowDigits && isDigitChar(c))) {
                hasLetterOrDigit = true;
            } else if (c != ' ' && c != '-' && c != '\'' && c != '.') {
                return false;
            }
        }
        return hasLetterOrDigit;
    }

    // MM/DD/YYYY with a real month, a real day, and a four-digit year
    bool isValidDate(string date) {
        if (textLength(date) != 10) return false;
        if (date[2] != '/' || date[5] != '/') return false;
        for (int i = 0; i < 10; i++) {
            if (i != 2 && i != 5 && !isDigitChar(date[i])) return false;
        }

        int month = toInt(cutText(date, 0, 2));
        int day = toInt(cutText(date, 3, 5));
        int year = toInt(cutText(date, 6, 10));
        if (month < 1 || month > 12 || year < 1) return false;

        int daysInMonth = 31;
        if (month == 4 || month == 6 || month == 9 || month == 11) {
            daysInMonth = 30;
        } else if (month == 2) {
            bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            daysInMonth = leapYear ? 29 : 28;
        }
        return (day >= 1 && day <= daysInMonth);
    }

    // Turns MM/DD/YYYY into the number YYYYMMDD so that two dates can be compared
    int convertDate(string date) {
        int month = toInt(cutText(date, 0, 2));
        int day = toInt(cutText(date, 3, 5));
        int year = toInt(cutText(date, 6, 10));
        return year * 10000 + month * 100 + day;
    }

    // Today as YYYYMMDD
    int todayNumber() {
        time_t currentTime = time(0);
        tm *today = localtime(&currentTime);        // POINTER: points to the current date and time
        if (today == 0) return 0;
        return (today->tm_year + 1900) * 10000 + (today->tm_mon + 1) * 100 + today->tm_mday;
    }

    // True if the person born on 'birthday' is at least 18 years old today
    bool isAdult(string birthday) {
        return (todayNumber() - convertDate(birthday)) >= CUSTOMER_MIN_AGE * 10000;
    }

    // +639 followed by 9 digits
    bool isValidMobile(string mobile) {
        if (textLength(mobile) != MOVIE_MOBILE_LENGTH) return false;
        if (mobile[0] != '+' || mobile[1] != '6' || mobile[2] != '3' || mobile[3] != '9') return false;
        for (int i = 4; i < MOVIE_MOBILE_LENGTH; i++) {
            if (!isDigitChar(mobile[i])) return false;
        }
        return true;
    }

    // One '@' with something before it, and a domain that is one of EMAIL_DOMAINS
    bool isValidEmail(string email) {
        int len = textLength(email);
        int atPos = -1;
        for (int i = 0; i < len; i++) {
            if (isBlankChar(email[i])) return false;
            if (email[i] == '@') {
                if (atPos != -1) return false;      // only one '@' allowed
                atPos = i;
            }
        }
        if (atPos < 1) return false;                // missing '@' or nothing before it

        string domain = cutText(email, atPos + 1, len);
        return (findChoice(EMAIL_DOMAINS, NUM_EMAIL_DOMAINS, domain) != -1);
    }

    // ---------- Input helpers (all re-ask until the value is valid) ----------

    // A required text value (name, house number, barangay, ...)
    string askText(const string &prompt, bool allowDigits) {
        string text;
        bool valid = false;
        do {
            text = getNonEmptyLine(prompt);
            valid = isValidText(text, allowDigits);
            if (!valid) cout << "  ! Invalid entry. Use letters" << (allowDigits ? ", numbers" : "")
                             << ", spaces, - ' and . only (no commas).\n";
        } while (!valid);
        return text;
    }

    // An optional text value: pressing Enter skips it (returns "")
    string askOptionalText(const string &prompt) {
        string text;
        bool valid = false;
        do {
            cout << prompt;
            getline(cin, text);
            text = trim(text);
            valid = (text == "") || isValidText(text, true);
            if (!valid) cout << "  ! Invalid entry. Use letters, numbers, spaces, - ' and . only (no commas).\n";
        } while (!valid);
        return text;
    }

    // Middle initial is optional: returns "" or a letter followed by a period (e.g. "A.")
    string askMiddleInitial() {
        string text;
        bool valid = false;
        do {
            cout << " Middle initial (Enter to skip) : ";
            getline(cin, text);
            text = trim(text);
            if (textLength(text) == 2 && text[1] == '.') text = cutText(text, 0, 1);
            valid = (text == "") || (textLength(text) == 1 && isLetterChar(text[0]));
            if (!valid) cout << "  ! Enter one letter only (e.g. A).\n";
        } while (!valid);
        if (text == "") return "";
        return toUpper(text) + ".";
    }

    // ---------- Customer IDs and duplicates ----------

    // Highest customer number found in Customer List.txt (the list must be loaded first)
    int highestNumberInList() {
        int highest = 0;
        for (int i = 0; i < customerList.getCount(); i++) {
            int number = toInt(cutText(customerList.getID(i), 2, RECORD_ID_LENGTH));
            if (number > highest) highest = number;
        }
        return highest;
    }

    // Position in the list of a customer with the same name AND birthday, or the same email.
    // Returns -1 if this customer is not registered yet.
    int findDuplicate(const string &fullName, const string &birthday, const string &email) {
        customerList.load();
        for (int i = 0; i < customerList.getCount(); i++) {
            string lines[MAX_FILE_LINES];
            int count = readFileLines(customerList.getID(i) + ".txt", lines, MAX_FILE_LINES);
            if (count < 5) continue;                // no readable file: nothing to compare

            bool samePerson = (toUpper(lines[0]) == toUpper(fullName) && lines[3] == birthday);
            bool sameEmail = (toUpper(lines[4]) == toUpper(email));
            if (samePerson || sameEmail) return i;
        }
        return -1;
    }

    // Asks for a customer ID until one in Customer List.txt is given. Returns false if 0 (cancel) was typed.
    // 'customerID' and 'listIndex' are passed by reference and filled with the customer that was found.
    bool askExistingCustomer(string prompt, string &customerID, int &listIndex) {
        bool found = false;
        bool cancelled = false;
        do {
            string idInput = getNonEmptyLine(prompt);
            if (idInput == "0") {
                cancelled = true;
            } else if (!isValidRecordID('C', idInput)) {
                cout << "  ! Invalid Customer ID. Use C-XXX.\n";
            } else {
                idInput = toUpper(trim(idInput));
                customerList.load();
                listIndex = customerList.findIndex(idInput);
                if (listIndex == -1) {
                    cout << "  ! Customer " << idInput << " does not exist.\n";
                } else {
                    customerID = idInput;
                    found = true;
                }
            }
        } while (!found && !cancelled);
        return found;
    }

    // ---------- Tasks ----------

    // ADD CUSTOMER: asks for every detail, validates it, then assigns the customer ID
    void addCustomer() {
        printTitle("ADD CUSTOMER");

        // ----- name -----
        cout << " NAME\n";
        string lastName = askText("   Last name : ", false);
        string givenName = askText("   Given name : ", false);
        string middleInitial = askMiddleInitial();

        string fullName = lastName + ", " + givenName;
        if (middleInitial != "") fullName += " " + middleInitial;

        // ----- address -----
        cout << "\n ADDRESS (Enter to skip the optional parts)\n";
        string houseNo = askText("   House # : ", true);
        string street = askOptionalText("   Street name (optional) : ");
        string subdivision = askOptionalText("   Subdivision/Building name (optional) : ");
        string barangay = askText("   Barangay name : ", true);
        if (toUpper(cutText(barangay, 0, 4)) == "BGY.") {      // the file adds "Bgy." itself
            barangay = trim(cutText(barangay, 4, textLength(barangay)));
        }
        string city = askText("   City/Municipality : ", false);

        string address = houseNo;
        if (street != "") address += " " + street;
        if (subdivision != "") address += ", " + subdivision;
        address += ", Bgy. " + barangay + ", " + city;

        // ----- mobile number -----
        cout << "\n";
        string mobile;
        bool validMobile = false;
        do {
            mobile = getNonEmptyLine(" Mobile number (+639XXXXXXXXX) : ");
            validMobile = isValidMobile(mobile);
            if (!validMobile) cout << "  ! Invalid mobile number. Use +639 followed by 9 digits.\n";
        } while (!validMobile);

        // ----- birthday (adults only) -----
        string birthday;
        bool validBirthday = false;
        do {
            birthday = getNonEmptyLine(" Birthday (MM/DD/YYYY) : ");
            if (!isValidDate(birthday)) {
                cout << "  ! Invalid date. Use MM/DD/YYYY.\n";
            } else if (convertDate(birthday) > todayNumber()) {
                cout << "  ! The birthday cannot be in the future.\n";
            } else if (!isAdult(birthday)) {
                cout << "  ! Only adults (" << CUSTOMER_MIN_AGE << " years old and above) can register.\n";
                cout << "  Registration cancelled.\n";
                return;
            } else {
                validBirthday = true;
            }
        } while (!validBirthday);

        // ----- email -----
        string email;
        bool validMail = false;
        do {
            email = getNonEmptyLine(" Email address : ");
            validMail = isValidEmail(email);
            if (!validMail) {
                cout << "  ! Invalid email. Accepted: ";
                for (int i = 0; i < NUM_EMAIL_DOMAINS; i++) {
                    if (i > 0) cout << ", ";
                    cout << "@" << EMAIL_DOMAINS[i];
                }
                cout << "\n";
            }
        } while (!validMail);

        // The same customer cannot be registered twice
        int duplicateIndex = findDuplicate(fullName, birthday, email);
        if (duplicateIndex != -1) {
            cout << "\n  ! This customer is already registered as " << customerList.getID(duplicateIndex) << ".\n";
            cout << "  Customer was not added.\n";
            return;
        }

        // The customer ID is assigned only after all the information was submitted.
        // Numbers of deleted customers are never used again: the counter keeps the highest number ever used.
        customerList.load();
        int lastNumber = customerList.readCounter();
        if (highestNumberInList() > lastNumber) lastNumber = highestNumberInList();
        int newNumber = lastNumber + 1;
        if (newNumber > MAX_BOOKING_NUMBER) {
            cout << "\n Sorry, the customer limit was reached.\n";
            return;
        }
        string customerID = makeID('C', newNumber);

        // C-XXX.txt (same layout as the handout; Customer::loadFromFile() reads it back)
        ofstream out(customerID + ".txt");
        if (!out.is_open()) {
            cout << "  ! Unable to create the customer file.\n";
            return;
        }
        out << fullName << "\n";
        out << address << "\n";
        out << mobile << "\n";
        out << birthday << "\n";
        out << email << "\n";
        out << "Customer ID # " << customerID << "\n";
        out.close();

        customerList.add(customerID, fullName);
        customerList.writeCounter(newNumber);

        cout << "\n Customer successfully added!\n";
        cout << " Customer ID: " << customerID << "\n";
    }

    // SHOW ALL CUSTOMERS: the list of customer IDs and names
    void showAllCustomers() {
        printTitle("ALL CUSTOMERS");
        int count = customerList.load();
        if (count == 0) {
            cout << " No Customers Found.\n";
        } else {
            customerList.display();
        }
        drawLine('-', LINE_WIDTH);
    }

    // SHOW CUSTOMER: the contents of one customer file
    void showCustomer() {
        printTitle("SHOW CUSTOMER");
        string customerID;
        int listIndex = -1;
        if (!askExistingCustomer(" Customer ID (C-XXX, or 0 to cancel) : ", customerID, listIndex)) {
            return;
        }

        string lines[MAX_FILE_LINES];
        int count = readFileLines(customerID + ".txt", lines, MAX_FILE_LINES);
        if (count < 1) {
            cout << "Unable to open file\n";
            return;
        }
        cout << "\n";
        drawLine('-', LINE_WIDTH);
        for (int i = 0; i < count; i++) {
            cout << lines[i] << "\n";
        }
        drawLine('-', LINE_WIDTH);
    }

    // DELETE CUSTOMER: the customer must exist first; then the file is deleted and the list is updated
    void deleteCustomer() {
        printTitle("DELETE CUSTOMER");
        string customerID;
        int listIndex = -1;
        if (!askExistingCustomer(" Customer ID to delete (C-XXX, or 0 to cancel) : ", customerID, listIndex)) {
            return;
        }
        cout << "  [OK] Customer " << customerID << " exists. Proceeding with deletion...\n";

        // Keep the counter at or above this number so the customer number cannot be used again
        int deletedNumber = toInt(cutText(customerID, 2, RECORD_ID_LENGTH));
        if (deletedNumber > customerList.readCounter()) {
            customerList.writeCounter(deletedNumber);
        }

        string fileName = customerID + ".txt";
        remove(fileName.c_str());           // the file may already be missing: the list is still fixed

        if (!customerList.removeAt(listIndex)) {
            cout << "  ! Unable to update Customer List.txt.\n";
            return;
        }
        cout << "\n Customer " << customerID << " deleted successfully.\n";
    }

    // Manager menu loop (runs until the manager chooses Back)
    void managerFlow() {
        int choice = 0;
        do {
            printTitle("CUSTOMER MENU");
            cout << "   [1] Add customer\n";
            cout << "   [2] Show all customers\n";
            cout << "   [3] Show customer\n";
            cout << "   [4] Delete customer\n";
            cout << "   [5] Back\n";
            drawLine('-', LINE_WIDTH);
            cout << " Choice: ";
            choice = getIntInRange(1, 5);

            switch (choice) {
                case 1: addCustomer(); break;
                case 2: showAllCustomers(); break;
                case 3: showCustomer(); break;
                case 4: deleteCustomer(); break;
                default: cout << "\n Exiting Customer Block.\n"; break;
            }
        } while (choice != 5);
    }

public:
    CustomerSystem() {
    }

    void runCustomer() {
        addCustomer();
    }

    // Customer block for the MANAGER: add, show all, show, delete
    void runManager() {
        managerFlow();
    }
};


class MovieSystem {
private:
    MovieList movieList;                  

    // ---------- Validations of the incoming inputs ----------

    // MM/DD/YYYY with a real month, a real day, and a four-digit year
    bool isValidDate(string date) {
        if (textLength(date) != 10) return false;
        if (date[2] != '/' || date[5] != '/') return false;
        for (int i = 0; i < 10; i++) {
            if (i != 2 && i != 5 && !isDigitChar(date[i])) return false;
        }

        int month = toInt(cutText(date, 0, 2));
        int day = toInt(cutText(date, 3, 5));
        int year = toInt(cutText(date, 6, 10));
        if (month < 1 || month > 12 || year < 1) return false;

        int daysInMonth = 31;
        if (month == 4 || month == 6 || month == 9 || month == 11) {
            daysInMonth = 30;
        } else if (month == 2) {
            bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            daysInMonth = leapYear ? 29 : 28;
        }
        return (day >= 1 && day <= daysInMonth);
    }

    // Turns MM/DD/YYYY into the number YYYYMMDD so that two dates can be compared
    int convertDate(string date) {
        int month = toInt(cutText(date, 0, 2));
        int day = toInt(cutText(date, 3, 5));
        int year = toInt(cutText(date, 6, 10));
        return year * 10000 + month * 100 + day;
    }

    // True if the date is later than today
    bool isFutureDate(string date) {
        time_t currentTime = time(0);
        tm *today = localtime(&currentTime);        // POINTER: points to the current date and time
        if (today == 0) return false;

        int currentDate = (today->tm_year + 1900) * 10000 + (today->tm_mon + 1) * 100 + today->tm_mday;
        return (convertDate(date) > currentDate);
    }

    // +639 followed by 9 digits
    bool isValidMobile(string mobile) {
        if (textLength(mobile) != MOVIE_MOBILE_LENGTH) return false;
        if (mobile[0] != '+' || mobile[1] != '6' || mobile[2] != '3' || mobile[3] != '9') return false;
        for (int i = 4; i < MOVIE_MOBILE_LENGTH; i++) {
            if (!isDigitChar(mobile[i])) return false;
        }
        return true;
    }

    // One '@' with something before it, and a domain that is one of EMAIL_DOMAINS
    bool isValidEmail(string email) {
        int len = textLength(email);
        int atPos = -1;
        for (int i = 0; i < len; i++) {
            if (isBlankChar(email[i])) return false;
            if (email[i] == '@') {
                if (atPos != -1) return false;      // only one '@' allowed
                atPos = i;
            }
        }
        if (atPos < 1) return false;                // missing '@' or nothing before it

        string domain = cutText(email, atPos + 1, len);
        return (findChoice(EMAIL_DOMAINS, NUM_EMAIL_DOMAINS, domain) != -1);
    }

    // ---------- Movie IDs ----------

    // Highest movie number found in Movie List.txt (the list must be loaded first)
    int highestNumberInList() {
        int highest = 0;
        for (int i = 0; i < movieList.getCount(); i++) {
            int number = toInt(cutText(movieList.getID(i), 2, RECORD_ID_LENGTH));
            if (number > highest) highest = number;
        }
        return highest;
    }

    // True if a movie with this name is already in Movie List.txt (ignores upper/lower case)
    bool movieAlreadyExists(string movieName) {
        movieList.load();
        for (int i = 0; i < movieList.getCount(); i++) {
            if (toUpper(movieList.getText(i)) == toUpper(movieName)) return true;
        }
        return false;
    }

    // Asks for a movie ID until an existing movie is given. Returns false if the user typed 0 to cancel.
    // 'movie' is passed by reference and is filled with the movie that was found.
    bool askExistingMovie(string prompt, Movie &movie, int &listIndex) {
        bool found = false;
        bool cancelled = false;
        do {
            string idInput = getNonEmptyLine(prompt);
            if (idInput == "0") {
                cancelled = true;
            } else if (!isValidRecordID('M', idInput)) {
                cout << "  ! Invalid Movie ID. Use M-XXX.\n";
            } else {
                idInput = toUpper(trim(idInput));
                movieList.load();
                listIndex = movieList.findIndex(idInput);
                if (listIndex == -1) {
                    cout << "  ! Movie " << idInput << " does not exist.\n";
                } else {
                    movie = Movie(idInput, movieList.getText(listIndex));
                    if (movie.loadFromFile()) {
                        found = true;
                    } else {
                        cout << "  ! Movie file of " << idInput << " does not exist.\n";
                    }
                }
            }
        } while (!found && !cancelled);
        return found;
    }

    // ADD MOVIE: asks for every detail, validates it, then assigns the movie ID
    void addMovie() {
        printTitle("ADD MOVIE");

        string name = getNonEmptyLine(" Movie name : ");
        while (movieAlreadyExists(name)) {
            cout << "  ! This movie is already registered.\n";
            name = getNonEmptyLine(" Please enter a different movie name : ");
        }

        cout << " Genres: ";
        for (int i = 0; i < NUM_MOVIE_GENRES; i++) {
            if (i > 0) cout << ", ";
            cout << MOVIE_GENRES[i];
        }
        cout << "\n";
        int genreIndex = -1;
        do {
            genreIndex = findChoice(MOVIE_GENRES, NUM_MOVIE_GENRES, getNonEmptyLine(" Genre : "));
            if (genreIndex == -1) cout << "  ! Invalid genre. Choose one from the list.\n";
        } while (genreIndex == -1);

        string startDate;
        bool validStart = false;
        do {
            startDate = getNonEmptyLine(" Start date (MM/DD/YYYY) : ");
            if (!isValidDate(startDate)) {
                cout << "  ! Invalid date. Use MM/DD/YYYY.\n";
            } else if (!isFutureDate(startDate)) {
                cout << "  ! The start date must be in the future.\n";
            } else {
                validStart = true;
            }
        } while (!validStart);

        string endDate;
        bool validEnd = false;
        do {
            endDate = getNonEmptyLine(" End date (MM/DD/YYYY) : ");
            if (!isValidDate(endDate)) {
                cout << "  ! Invalid date. Use MM/DD/YYYY.\n";
            } else if (convertDate(endDate) <= convertDate(startDate)) {
                cout << "  ! The end date must be after the start date.\n";
            } else {
                validEnd = true;
            }
        } while (!validEnd);

        cout << " Ratings: G, PG, R-13, R-16, R-18\n";
        int ratingIndex = -1;
        do {
            ratingIndex = findChoice(MOVIE_RATINGS, NUM_MOVIE_RATINGS, getNonEmptyLine(" Rating : "));
            if (ratingIndex == -1) cout << "  ! Invalid rating. Choose one from the list.\n";
        } while (ratingIndex == -1);

        string contactPerson = getNonEmptyLine(" Contact person : ");

        string mobile;
        bool validMobile = false;
        do {
            mobile = getNonEmptyLine(" Mobile number (+639XXXXXXXXX) : ");
            validMobile = isValidMobile(mobile);
            if (!validMobile) cout << "  ! Invalid mobile number. Use +639 followed by 9 digits.\n";
        } while (!validMobile);

        string email;
        bool validMail = false;
        do {
            email = getNonEmptyLine(" Email address : ");
            validMail = isValidEmail(email);
            if (!validMail) {
                cout << "  ! Invalid email. Accepted: ";
                for (int i = 0; i < NUM_EMAIL_DOMAINS; i++) {
                    if (i > 0) cout << ", ";
                    cout << "@" << EMAIL_DOMAINS[i];
                }
                cout << "\n";
            }
        } while (!validMail);

        // The movie ID is assigned only after all the information was submitted.
        // Numbers of deleted movies are never used again: the counter keeps the highest number ever used.
        movieList.load();
        int lastNumber = movieList.readCounter();
        if (highestNumberInList() > lastNumber) lastNumber = highestNumberInList();
        int newNumber = lastNumber + 1;
        if (newNumber > MAX_BOOKING_NUMBER) {
            cout << "\n Sorry, the movie limit was reached.\n";
            return;
        }
        string movieID = makeID('M', newNumber);

        Movie movie(movieID, name);
        movie.setDetails(name, MOVIE_GENRES[genreIndex], startDate, endDate,
                         MOVIE_RATINGS[ratingIndex], contactPerson, mobile, email);
        if (!movie.saveToFile()) {
            cout << "  ! Unable to create the movie file.\n";
            return;
        }
        movieList.add(movieID, name);
        movieList.writeCounter(newNumber);

        cout << "\n Movie successfully added!\n";
        cout << " Movie ID: " << movieID << "\n";
    }

    // SHOW ALL MOVIES: the list of movie IDs and names
    void showAllMovies() {
        printTitle("ALL MOVIES");
        int count = movieList.load();
        if (count == 0) {
            cout << " No Movies Found.\n";
        } else {
            movieList.display();
        }
        drawLine('-', LINE_WIDTH);
    }

    // SHOW MOVIE: the contents of one movie file
    void showMovie() {
        printTitle("SHOW MOVIE");
        Movie movie;
        int listIndex = -1;
        if (askExistingMovie(" Movie ID (M-XXX, or 0 to cancel) : ", movie, listIndex)) {
            cout << "\n";
            drawLine('-', LINE_WIDTH);
            movie.displayDetails();
            drawLine('-', LINE_WIDTH);
        }
    }

    // DELETE MOVIE: the movie must exist first; then its file is deleted and the list is updated
    void deleteMovie() {
        printTitle("DELETE MOVIE");
        Movie movie;
        int listIndex = -1;
        if (!askExistingMovie(" Movie ID to delete (M-XXX, or 0 to cancel) : ", movie, listIndex)) {
            return;
        }
        cout << "  [OK] Movie " << movie.getID() << " exists. Proceeding with deletion...\n";

        // Keep the counter at or above this number so the movie number cannot be used again
        int deletedNumber = toInt(cutText(movie.getID(), 2, RECORD_ID_LENGTH));
        if (deletedNumber > movieList.readCounter()) {
            movieList.writeCounter(deletedNumber);
        }

        string fileName = movie.getFileName();
        if (remove(fileName.c_str()) != 0) {
            cout << "  ! Unable to delete the movie file.\n";
            return;
        }
        if (!movieList.removeAt(listIndex)) {
            cout << "  ! Unable to update Movie List.txt.\n";
            return;
        }
        cout << "\n Movie " << movie.getID() << " deleted successfully.\n";
    }

public:
    MovieSystem() {
    }

    // Movie menu loop (runs until the manager chooses Back)
    void runManager() {
        int choice = 0;
        do {
            printTitle("MOVIE MENU");
            cout << "   [1] Add movie\n";
            cout << "   [2] Show all movies\n";
            cout << "   [3] Show movie\n";
            cout << "   [4] Delete movie\n";
            cout << "   [5] Back\n";
            drawLine('-', LINE_WIDTH);
            cout << " Choice: ";
            choice = getIntInRange(1, 5);

            switch (choice) {
                case 1: addMovie(); break;
                case 2: showAllMovies(); break;
                case 3: showMovie(); break;
                case 4: deleteMovie(); break;
                default: cout << "\n Exiting Movie Block.\n"; break;
            }
        } while (choice != 5);
    }
};
// =====================================================================================
// CINEMA block 
// =====================================================================================
//Cinema constants
const int MAX_CINEMA_NUMBER = 999;       // codes go from I-001 to I-999
const int MAX_TICKET_PRICE  = 100000;
const int NUM_CINEMA_TYPES  = 7;
const string CINEMA_TYPES[NUM_CINEMA_TYPES] = {      // single-dimension array
    "Regular", "IMAX", "IMAX with 3D", "Digital Theater",
    "ScreenX", "Director's Club", "4DX"
};
const string CINEMA_LIST_FILE    = "Cinema List.txt";
const string CINEMA_COUNTER_FILE = "Cinema Counter.txt";   // last number ever used
const int CINEMA_FILE_LINES = 9;

// labels used inside I-XXX.txt (same text is used for saving and loading)
const string LABEL_ROWS     = "Number of rows: ";
const string LABEL_SEATS    = "Number of seats per row: ";
const string LABEL_CAPACITY = "Capacity: ";
const string LABEL_PRICE    = "Price: ";
const string LABEL_TYPE     = "Cinema type: ";
const string LABEL_CODE     = "Cinema Code # ";

// CLASS 1: CinemaHelper
class CinemaHelper {
public:
    static char lowerChar(char c) {
        if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
        return c;
    }

    static char upperChar(char c) {
        if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
        return c;
    }

    static string toLowerText(const string &text) {
        string result = text;
        for (int i = 0; i < textLength(result); i++)
            result[i] = lowerChar(result[i]);
        return result;
    }

    static string toUpperText(const string &text) {
        string result = text;
        for (int i = 0; i < textLength(result); i++)
            result[i] = upperChar(result[i]);
        return result;
    }

    static bool isAllDigits(const string &text) {
        int length = textLength(text);
        if (length == 0) return false;
        for (int i = 0; i < length; i++)
            if (text[i] < '0' || text[i] > '9') return false;
        return true;
    }

    static int toNumber(const string &text) {
        int number = 0;
        for (int i = 0; i < textLength(text); i++)
            if (text[i] >= '0' && text[i] <= '9')
                number = number * 10 + (text[i] - '0');
        return number;
    }

    // part of a text: 'count' characters starting at 'start'
    static string slice(const string &text, int start, int count) {
        string result = "";
        int length = textLength(text);
        for (int i = start; i < start + count && i < length; i++)
            result += text[i];
        return result;
    }

    // everything after the first 'skip' characters
    static string afterPrefix(const string &line, int skip) {
        return slice(line, skip, textLength(line) - skip);
    }

    // 7 -> "I-007"
    static string makeId(int number) {
        string id = "I-";
        id += char('0' + number / 100);
        id += char('0' + (number / 10) % 10);
        id += char('0' + number % 10);
        return id;
    }

    static bool isValidId(const string &id) {
        if (textLength(id) != RECORD_ID_LENGTH) return false;
        if (id[0] != 'I' || id[1] != '-') return false;
        return isAllDigits(slice(id, 2, 3));
    }

    // "I-007" -> 7
    static int idNumber(const string &id) {
        return toNumber(slice(id, 2, 3));
    }

    // +63 followed by 9 digits (landline) or 10 digits (mobile)
    static bool isValidContact(const string &contact) {
        int length = textLength(contact);
        if (length != 12 && length != 13) return false;
        if (slice(contact, 0, 3) != "+63") return false;
        return isAllDigits(slice(contact, 3, length - 3));
    }
};

// CLASS 2: CinemaRecord (PARENT / base class)
// Holds what every record has: an id and a name.

class CinemaRecord {
protected:
    string id;
    string name;

public:
    // constructors
    CinemaRecord() {
        id = "";
        name = "";
    }

    CinemaRecord(const string &recordId, const string &recordName) {
        id = recordId;
        name = recordName;
    }

    virtual ~CinemaRecord() {}

    // getters
    string getId() const { return id; }
    string getName() const { return name; }
    string getFileName() const { return id + ".txt"; }
    string getListLine() const { return id + " " + name; }   // "I-001 SM North EDSA Cinema 1"

    bool deleteFile() const {
        return remove(getFileName().c_str()) == 0;
    }

    // pure virtual: the child class must provide these
    virtual bool saveToFile() const = 0;
    virtual bool loadFromFile() = 0;
    virtual void display() const = 0;
};

// CLASS 3: CinemaData (CHILD class of CinemaRecord)
// INHERITANCE: CinemaData gets id and name from CinemaRecord.

class CinemaData : public CinemaRecord {
private:
    string address;
    int rows;
    int seatsPerRow;
    int capacity;
    string contact;
    int price;
    string type;

public:
    // constructors
    CinemaData() : CinemaRecord() {
        address = "";
        rows = 0;
        seatsPerRow = 0;
        capacity = 0;
        contact = "";
        price = 0;
        type = "";
    }

    CinemaData(const string &cinemaId, const string &cinemaName, const string &cinemaAddress,
           int numRows, int numSeats, const string &cinemaContact,
           int ticketPrice, const string &cinemaType)
        : CinemaRecord(cinemaId, cinemaName) {
        address = cinemaAddress;
        rows = numRows;
        seatsPerRow = numSeats;
        capacity = numRows * numSeats;        // capacity is computed, not typed in
        contact = cinemaContact;
        price = ticketPrice;
        type = cinemaType;
    }

    // getters (the Booking block can use these)
    string getAddress() const { return address; }
    int getRows() const { return rows; }
    int getSeatsPerRow() const { return seatsPerRow; }
    int getCapacity() const { return capacity; }
    string getContact() const { return contact; }
    int getPrice() const { return price; }
    string getType() const { return type; }

    // writes I-XXX.txt in the order the MP2 specification shows
    bool saveToFile() const override {
        ofstream out(getFileName().c_str());
        if (!out.is_open()) return false;
        out << name << "\n";
        out << address << "\n";
        out << LABEL_ROWS << rows << "\n";
        out << LABEL_SEATS << seatsPerRow << "\n";
        out << LABEL_CAPACITY << capacity << "\n";
        out << contact << "\n";
        out << LABEL_PRICE << price << "\n";
        out << LABEL_TYPE << type << "\n";
        out << LABEL_CODE << id << "\n";
        out.close();
        return true;
    }

    // reads I-XXX.txt back into this object
    bool loadFromFile() override {
        ifstream in(getFileName().c_str());
        if (!in.is_open()) return false;

        string lines[CINEMA_FILE_LINES];
        int linesRead = 0;
        while (linesRead < CINEMA_FILE_LINES && getline(in, lines[linesRead]))
            linesRead++;
        in.close();
        if (linesRead < CINEMA_FILE_LINES) return false;

        name = trim(lines[0]);
        address = trim(lines[1]);
        rows = CinemaHelper::toNumber(CinemaHelper::afterPrefix(lines[2], textLength(LABEL_ROWS)));
        seatsPerRow = CinemaHelper::toNumber(CinemaHelper::afterPrefix(lines[3], textLength(LABEL_SEATS)));
        capacity = CinemaHelper::toNumber(CinemaHelper::afterPrefix(lines[4], textLength(LABEL_CAPACITY)));
        contact = trim(lines[5]);
        price = CinemaHelper::toNumber(CinemaHelper::afterPrefix(lines[6], textLength(LABEL_PRICE)));
        type = trim(CinemaHelper::afterPrefix(lines[7], textLength(LABEL_TYPE)));
        return true;
    }

    // shows the cinema the same way it is stored in the file
    void display() const override {
        cout << name << "\n";
        cout << address << "\n";
        cout << LABEL_ROWS << rows << "\n";
        cout << LABEL_SEATS << seatsPerRow << "\n";
        cout << LABEL_CAPACITY << capacity << "\n";
        cout << contact << "\n";
        cout << LABEL_PRICE << price << "\n";
        cout << LABEL_TYPE << type << "\n";
        cout << LABEL_CODE << id << "\n";
    }
};

// CLASS 4: CinemaSystem (runs the whole CINEMA block)

class CinemaSystem {
private:
    // POINTERS: every slot points to a Cinema object created with new
    CinemaData *cinemas[MAX_CINEMAS];
    int count;

    // ---------- numbering (a deleted number is never used again) ----------
    int readCounter() const {
        int last = 0;
        ifstream in(CINEMA_COUNTER_FILE.c_str());
        if (!(in >> last)) last = 0;
        in.close();

        // safety: never go lower than the biggest code in the list
        for (int i = 0; i < count; i++) {
            int number = CinemaHelper::idNumber(cinemas[i]->getId());
            if (number > last) last = number;
        }
        return last;
    }

    void writeCounter(int number) const {
        ofstream out(CINEMA_COUNTER_FILE.c_str());
        out << number << "\n";
        out.close();
    }

    // ---------- list file ----------
    void loadAll() {
        ifstream in(CINEMA_LIST_FILE.c_str());
        string line;
        while (count < MAX_CINEMAS && getline(in, line)) {
            line = trim(line);
            string id = CinemaHelper::slice(line, 0, RECORD_ID_LENGTH);
            if (textLength(line) > RECORD_ID_LENGTH + 1 && CinemaHelper::isValidId(id)) {
                string name = CinemaHelper::afterPrefix(line, RECORD_ID_LENGTH + 1);
                CinemaData *loaded = new CinemaData(id, name, "", 0, 0, "", 0, "");   // POINTER
                if (loaded->loadFromFile()) {
                    cinemas[count] = loaded;
                    count++;
                } else {
                    delete loaded;               // file missing, so skip it
                }
            }
        }
        in.close();
    }

    void saveList() const {
        ofstream out(CINEMA_LIST_FILE.c_str());
        for (int i = 0; i < count; i++)
            out << cinemas[i]->getListLine() << "\n";
        out.close();
    }

    void appendToList(const CinemaData *cinema) const {
        ofstream out(CINEMA_LIST_FILE.c_str(), ios::app);
        out << cinema->getListLine() << "\n";
        out.close();
    }

    //  searching
    int findIndexById(const string &id) const {
        int found = -1;
        for (int i = 0; i < count && found == -1; i++)
            if (cinemas[i]->getId() == id) found = i;
        return found;
    }

    // same name, ignoring upper and lower case
    bool nameExists(const string &name) const {
        bool exists = false;
        string wanted = CinemaHelper::toLowerText(name);
        for (int i = 0; i < count && !exists; i++)
            if (CinemaHelper::toLowerText(cinemas[i]->getName()) == wanted) exists = true;
        return exists;
    }

    // input with validation
    string askRequired(const string &prompt) const {
        string text = "";
        cout << prompt;
        getline(cin, text);
        text = trim(text);
        while (textLength(text) == 0) {
            cout << " This field is required. Please try again.\n";
            cout << prompt;
            getline(cin, text);
            text = trim(text);
        }
        return text;
    }

    string askOptional(const string &prompt) const {
        string text = "";
        cout << prompt;
        getline(cin, text);
        return trim(text);
    }

    string askContact() const {
        string contact = "";
        cout << " Contact number (+63 then 9 or 10 digits): ";
        getline(cin, contact);
        contact = trim(contact);
        while (!CinemaHelper::isValidContact(contact)) {
            cout << " Invalid format. Examples: +63285884000 or +639101234567\n";
            cout << " Contact number (+63 then 9 or 10 digits): ";
            getline(cin, contact);
            contact = trim(contact);
        }
        return contact;
    }

    bool askYesNo(const string &prompt) const {
        string answer = "";
        cout << prompt << " (y/n): ";
        getline(cin, answer);
        answer = CinemaHelper::toLowerText(trim(answer));
        while (answer != "y" && answer != "n") {
            cout << " Please type y or n.\n";
            cout << prompt << " (y/n): ";
            getline(cin, answer);
            answer = CinemaHelper::toLowerText(trim(answer));
        }
        return answer == "y";
    }

    // returns false if the manager typed 0 to cancel
    bool askCinemaCode(string &code) const {
        cout << " Cinema code (e.g. I-001) or 0 to cancel: ";
        getline(cin, code);
        code = CinemaHelper::toUpperText(trim(code));
        while (code != "0" && !CinemaHelper::isValidId(code)) {
            cout << " Invalid code. The format is I-XXX (e.g. I-001).\n";
            cout << " Cinema code (e.g. I-001) or 0 to cancel: ";
            getline(cin, code);
            code = CinemaHelper::toUpperText(trim(code));
        }
        return code != "0";
    }

    // sorting helpers
    // POINTER TO POINTER: swaps two Cinema pointers (the objects stay where they are)
    static void swapCinemas(CinemaData **first, CinemaData **second) {
        CinemaData *temp = *first;
        *first = *second;
        *second = temp;
    }

    // mode 2 = name, 3 = capacity, 4 = ticket price
    static bool shouldSwap(const CinemaData *a, const CinemaData *b, int mode) {
        if (mode == 2)
            return CinemaHelper::toLowerText(a->getName()) > CinemaHelper::toLowerText(b->getName());
        if (mode == 3) return a->getCapacity() > b->getCapacity();
        if (mode == 4) return a->getPrice() > b->getPrice();
        return false;
    }

    //  the three tasks

    // TASK 1: ADD CINEMA
    void addCinema() {
        cout << "\n";
        printTitle("ADD CINEMA");

        if (count >= MAX_CINEMAS) {
            cout << " The cinema list is full. A cinema must be deleted first.\n";
            return;
        }

        string name = askRequired(" Cinema name: ");
        if (nameExists(name)) {
            cout << "\n This cinema is already registered. Cinema was not added.\n";
            return;
        }

        // address: street, subdivision and barangay can be skipped
        cout << " Address (press Enter to skip an optional part)\n";
        string addressParts[4];
        addressParts[0] = askOptional("   Street name (optional): ");
        addressParts[1] = askOptional("   Subdivision/Building name (optional): ");
        addressParts[2] = askOptional("   Barangay name (optional): ");
        addressParts[3] = askRequired("   City/Municipality: ");

        string address = "";
        for (int i = 0; i < 4; i++) {
            if (textLength(addressParts[i]) > 0) {
                if (textLength(address) > 0) address += ", ";
                address += addressParts[i];
            }
        }

        cout << " Number of rows (1-" << MAX_ROWS << "): ";
        int rows = getIntInRange(1, MAX_ROWS);
        cout << " Number of seats per row (1-" << MAX_SEATS_PER_ROW << "): ";
        int seatsPerRow = getIntInRange(1, MAX_SEATS_PER_ROW);
        cout << " Capacity (rows x seats per row): " << rows * seatsPerRow << "\n";

        string contact = askContact();

        cout << " Ticket price (1-" << MAX_TICKET_PRICE << "): ";
        int price = getIntInRange(1, MAX_TICKET_PRICE);

        cout << " Cinema type:\n";
        for (int i = 0; i < NUM_CINEMA_TYPES; i++)
            cout << "   [" << i + 1 << "] " << CINEMA_TYPES[i] << "\n";
        cout << " Choose a cinema type: ";
        int typeChoice = getIntInRange(1, NUM_CINEMA_TYPES);

        // create the code
        int number = readCounter() + 1;
        if (number > MAX_CINEMA_NUMBER) {
            cout << "\n The cinema codes are used up (I-999 reached).\n";
            return;
        }
        string id = CinemaHelper::makeId(number);

        // POINTER: a new Cinema object is created on the heap
        CinemaData *newCinema = new CinemaData(id, name, address, rows, seatsPerRow,
                                       contact, price, CINEMA_TYPES[typeChoice - 1]);

        if (!newCinema->saveToFile()) {
            cout << "\n Error: the cinema file could not be created.\n";
            delete newCinema;
            return;
        }

        cinemas[count] = newCinema;
        count++;
        writeCounter(number);
        appendToList(newCinema);

        cout << "\n Cinema added successfully!\n";
        cout << " Cinema Code # " << id << "\n";
    }

    // TASK 2A: SHOW ALL CINEMAS
    void showAllCinemas() {
        cout << "\n";
        printTitle("SHOW ALL CINEMAS");

        if (count == 0) {
            cout << " There are no cinemas yet.\n";
            return;
        }

        cout << " Display the cinemas:\n";
        cout << "   [1] Cinema list format (by code)\n";
        cout << "   [2] Alphabetical (by name)\n";
        cout << "   [3] Increasing capacity\n";
        cout << "   [4] Increasing ticket price\n";
        cout << " Choice: ";
        int mode = getIntInRange(1, 4);

        // copy the pointers so the saved list keeps its order
        CinemaData *sorted[MAX_CINEMAS];
        for (int i = 0; i < count; i++)
            sorted[i] = cinemas[i];

        // bubble sort: only the pointers are swapped
        for (int i = 0; i < count - 1; i++)
            for (int j = 0; j < count - 1 - i; j++)
                if (shouldSwap(sorted[j], sorted[j + 1], mode))
                    swapCinemas(&sorted[j], &sorted[j + 1]);

        cout << "\n";
        for (int i = 0; i < count; i++) {
            cout << sorted[i]->getListLine();
            if (mode == 3) cout << "  (Capacity: " << sorted[i]->getCapacity() << ")";
            if (mode == 4) cout << "  (Price: " << sorted[i]->getPrice() << ")";
            cout << "\n";
        }
    }

    // TASK 2B: SHOW CINEMA
    void showCinema() {
        cout << "\n";
        printTitle("SHOW CINEMA");

        if (count == 0) {
            cout << " There are no cinemas yet.\n";
            return;
        }

        string code = "";
        if (!askCinemaCode(code)) return;

        int index = findIndexById(code);
        if (index == -1) {
            cout << "\n Cinema " << code << " does not exist.\n";
            return;
        }

        // POLYMORPHISM: a base-class pointer calls CinemaData's own display()
        CinemaRecord *record = cinemas[index];
        cout << "\n";
        record->display();
    }

    // TASK 3: DELETE CINEMA
    void deleteCinema() {
        cout << "\n";
        printTitle("DELETE CINEMA");

        if (count == 0) {
            cout << " There are no cinemas yet.\n";
            return;
        }

        string code = "";
        if (!askCinemaCode(code)) return;

        int index = findIndexById(code);
        if (index == -1) {
            cout << "\n Cinema " << code << " does not exist. Nothing was deleted.\n";
            return;
        }

        cout << "\n Cinema found: " << cinemas[index]->getListLine() << "\n";
        if (!askYesNo(" Are you sure you want to delete this cinema?")) {
            cout << " Deletion cancelled.\n";
            return;
        }

        if (!cinemas[index]->deleteFile())
            cout << " Warning: the cinema file could not be deleted.\n";

        delete cinemas[index];                       // POINTER: free the object
        for (int i = index; i < count - 1; i++)      // close the gap
            cinemas[i] = cinemas[i + 1];
        count--;
        cinemas[count] = nullptr;

        saveList();                                  // the counter file is not changed
        cout << " Cinema " << code << " was deleted.\n";
    }

public:
    // constructor: starts empty, then loads the saved cinemas
    CinemaSystem() {
        count = 0;
        for (int i = 0; i < MAX_CINEMAS; i++)
            cinemas[i] = nullptr;
        loadAll();
    }

    // destructor: frees every Cinema object
    ~CinemaSystem() {
        for (int i = 0; i < count; i++)
            delete cinemas[i];
    }

    // the menu of the CINEMA block (manager only)
    void runManager() {
        int choice = 0;
        do {
            printTitle("CINEMA BLOCK");
            cout << "   [1] Add Cinema\n";
            cout << "   [2] Show All Cinemas\n";
            cout << "   [3] Show Cinema\n";
            cout << "   [4] Delete Cinema\n";
            cout << "   [5] Back to Manager Menu\n";
            drawLine('-', LINE_WIDTH);
            cout << " Choice: ";
            choice = getIntInRange(1, 5);

            switch (choice) {
                case 1: addCinema();      break;
                case 2: showAllCinemas(); break;
                case 3: showCinema();     break;
                case 4: deleteCinema();   break;
                default: break;
            }
        } while (choice != 5);
    }

    //  for the BOOKING block
    int getCinemaCount() const { return count; }

    const CinemaData *getCinema(int index) const {
        if (index >= 0 && index < count) return cinemas[index];
        return nullptr;
    }

    const CinemaData *findCinemaById(const string &id) const {
        int index = findIndexById(id);
        if (index == -1) return nullptr;
        return cinemas[index];
    }

    const CinemaData *findCinemaByName(const string &name) const {
        string wanted = CinemaHelper::toLowerText(name);
        for (int i = 0; i < count; i++)
            if (CinemaHelper::toLowerText(cinemas[i]->getName()) == wanted) return cinemas[i];
        return nullptr;
    }
};

#endif