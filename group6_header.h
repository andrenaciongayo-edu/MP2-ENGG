#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>              // today's date (birthday and movie date checks)

using namespace std;

// =====================================================================================
// Constants
// =====================================================================================
const string MANAGER_CODE = "123";
const int MAX_MANAGER_LOGIN_ATTEMPTS = 5;   // wrong codes allowed in total before the lock-out

const int NUM_PAYMENT_TYPES = 3;
const string PAYMENT_TYPES[NUM_PAYMENT_TYPES] = {"Credit card", "Gcash", "PayPal"};

const int NUM_MOVIE_GENRES = 8;
const string MOVIE_GENRES[NUM_MOVIE_GENRES] = {"Action", "Romance", "Comedy", "Drama", "Horror",
                                               "Science Fiction", "Animated", "Documentary"};
const int NUM_MOVIE_RATINGS = 5;
const string MOVIE_RATINGS[NUM_MOVIE_RATINGS] = {"G", "PG", "R-13", "R-16", "R-18"};

const int NUM_CINEMA_TYPES = 7;
const string CINEMA_TYPES[NUM_CINEMA_TYPES] = {"Regular", "IMAX", "IMAX with 3D", "Digital Theater",
                                               "ScreenX", "Director's Club", "4DX"};

const int NUM_EMAIL_DOMAINS = 5;
const string EMAIL_DOMAINS[NUM_EMAIL_DOMAINS] = {"gmail.com", "yahoo.com", "outlook.com",
                                                 "hotmail.com", "icloud.com"};

const int MAX_MOVIES = 50;
const int MAX_CINEMAS = 50;
const int MAX_ROWS = 26;                // Rows A to Z
const int MAX_SEATS_PER_ROW = 50;
const int MAX_SEATS_PER_BOOKING = 50;
const int MAX_RECORDS = 1000;           // Most entries in any one list file
const int MAX_RECORD_NUMBER = 999;      // IDs go from X-001 to X-999
const int MAX_FILE_LINES = 12;          // Most lines read from any one record file
const int MAX_TICKET_PRICE = 100000;
const int RECORD_ID_LENGTH = 5;         // e.g. B-001, C-001
const int MOBILE_LENGTH = 13;           // +639 followed by 9 digits
const int CUSTOMER_MIN_AGE = 18;        // only adults can register
const int LINE_WIDTH = 46;              // Width of formatted screen titles

// =====================================================================================
// HELPER FUNCTIONS (all helpers are here, at the top)
// =====================================================================================

// ---------- Text helpers ----------

int textLength(const string &text) {
    int length = 0;
    while (text[length] != '\0') {
        length++;
    }
    return length;
}

bool isDigitChar(char c) {
    return (c >= '0' && c <= '9');
}

bool isLetterChar(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isBlankChar(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

char toUpperChar(char c) {
    if (c >= 'a' && c <= 'z') {
        return (char)(c - 'a' + 'A');
    }
    return c;
}

string toUpper(const string &text) {
    string result = text;
    for (int i = 0; i < textLength(result); i++) {
        result[i] = toUpperChar(result[i]);
    }
    return result;
}

// Substring from 'start' up to (but not including) 'end'
string cutText(const string &text, int start, int end) {
    string result = "";
    for (int i = start; i < end; i++) {
        result += text[i];
    }
    return result;
}

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

// Next non-blank word starting from 'index' ("" when there is none)
string extractNextToken(const string &text, int &index) {
    int len = textLength(text);
    while (index < len && isBlankChar(text[index])) {
        index++;
    }
    int start = index;
    while (index < len && !isBlankChar(text[index])) {
        index++;
    }
    return cutText(text, start, index);
}

// Splits "J20 K20" into an array of seats. 'overflow' is true if there were too many.
int parseSeatTokens(const string &text, string seats[MAX_SEATS_PER_BOOKING], bool &overflow) {
    int count = 0;
    int index = 0;
    string token = "";
    overflow = false;

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

// ---------- Number helpers ----------

int toInt(const string &text) {
    double value = 0;
    for (int i = 0; i < textLength(text) && isDigitChar(text[i]); i++) {
        value = value * 10 + (text[i] - '0');
        if (value > 2000000000.0) return 2000000000;
    }
    return (int)value;
}

bool parseDecimal(const string &text, double &value) {
    bool valid = true;
    bool afterDot = false;
    int digitCount = 0;
    double number = 0;
    double divisor = 1;

    for (int i = 0; i < textLength(text) && valid; i++) {
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

string intToString(int number) {
    if (number == 0) return "0";
    string result = "";
    while (number > 0) {
        result = (char)('0' + number % 10) + result;
        number /= 10;
    }
    return result;
}

// 10 -> "10.00"
string formatMoney(double amount) {
    int cents = (int)(amount * 100.0 + 0.5);
    string result = intToString(cents / 100) + ".";
    if (cents % 100 < 10) result += "0";
    return result + intToString(cents % 100);
}

// ---------- ID and file-line helpers ----------

// ('B', 7) -> "B-007"
string makeID(char prefix, int number) {
    string id = "";
    id += prefix;
    id += "-";
    id += (char)('0' + (number / 100));
    id += (char)('0' + ((number / 10) % 10));
    id += (char)('0' + (number % 10));
    return id;
}

// Validates an ID such as B-001 (passed by value so the caller's string is untouched)
bool isValidRecordID(char prefix, string recordID) {
    recordID = toUpper(trim(recordID));
    if (textLength(recordID) != RECORD_ID_LENGTH) return false;
    if (recordID[0] != prefix || recordID[1] != '-') return false;
    for (int i = 2; i < RECORD_ID_LENGTH; i++) {
        if (!isDigitChar(recordID[i])) return false;
    }
    return true;
}

// "Reyes, Rose Marie A." -> "Rose Marie A. Reyes" (text without a comma is returned as it is)
string flipName(const string &fileName) {
    int len = textLength(fileName);
    int commaPos = -1;
    for (int i = 0; i < len && commaPos == -1; i++) {
        if (fileName[i] == ',') commaPos = i;
    }
    if (commaPos == -1) return trim(fileName);
    return trim(cutText(fileName, commaPos + 1, len)) + " " + trim(cutText(fileName, 0, commaPos));
}

// The data part of a saved line (after the first colon)
string extractAfterColon(const string &line) {
    int len = textLength(line);
    for (int i = 0; i < len; i++) {
        if (line[i] == ':') return trim(cutText(line, i + 1, len));
    }
    return trim(line);
}

// Position of 'input' in a list of allowed words (ignoring upper/lower case), or -1
int findChoice(const string choices[], int numChoices, const string &input) {
    string target = toUpper(trim(input));
    for (int i = 0; i < numChoices; i++) {
        if (toUpper(choices[i]) == target) return i;
    }
    return -1;
}

// Reads up to maxLines lines of a file into an array (each line trimmed).
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

// ---------- Validations (shared by the CUSTOMER, MOVIE and CINEMA blocks) ----------

// Letters, spaces, '-', '\'' and '.' only (digits too if allowDigits). No commas, because the
// files use ", " to separate the parts of a name and address.
bool isValidText(const string &text, bool allowDigits) {
    bool hasLetterOrDigit = false;
    for (int i = 0; i < textLength(text); i++) {
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
bool isValidDate(const string &date) {
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

// MM/DD/YYYY -> YYYYMMDD, so that two dates can be compared
int convertDate(const string &date) {
    return toInt(cutText(date, 6, 10)) * 10000 + toInt(cutText(date, 0, 2)) * 100 +
           toInt(cutText(date, 3, 5));
}

// "12/01/2027" -> "December 1, 2027" (format used inside the movie file)
string longDate(const string &date) {
    const string months[12] = {"January", "February", "March", "April", "May", "June", "July",
                               "August", "September", "October", "November", "December"};
    int month = toInt(cutText(date, 0, 2));
    int day = toInt(cutText(date, 3, 5));
    if (month < 1 || month > 12) return date;
    return months[month - 1] + " " + intToString(day) + ", " + cutText(date, 6, 10);
}

// Today as YYYYMMDD
int todayNumber() {
    time_t currentTime = time(0);
    tm *today = localtime(&currentTime);        // POINTER: points to the current date and time
    if (today == 0) return 0;
    return (today->tm_year + 1900) * 10000 + (today->tm_mon + 1) * 100 + today->tm_mday;
}

// +639 followed by 9 digits
bool isValidMobile(const string &mobile) {
    if (textLength(mobile) != MOBILE_LENGTH) return false;
    if (cutText(mobile, 0, 4) != "+639") return false;
    for (int i = 4; i < MOBILE_LENGTH; i++) {
        if (!isDigitChar(mobile[i])) return false;
    }
    return true;
}

// +63 followed by 9 digits (landline) or 10 digits (mobile)
bool isValidContact(const string &contact) {
    int len = textLength(contact);
    if ((len != 12 && len != 13) || cutText(contact, 0, 3) != "+63") return false;
    for (int i = 3; i < len; i++) {
        if (!isDigitChar(contact[i])) return false;
    }
    return true;
}

// One '@' with something before it, and a domain that is one of EMAIL_DOMAINS
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
    if (atPos < 1) return false;                // missing '@' or nothing before it
    return (findChoice(EMAIL_DOMAINS, NUM_EMAIL_DOMAINS, cutText(email, atPos + 1, len)) != -1);
}

// ---------- Screen helpers ----------

void drawLine(char symbol, int length) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }
    cout << "\n";
}

// Centers a title inside horizontal lines
void printTitle(const string &title) {
    cout << "\n";
    drawLine('=', LINE_WIDTH);
    for (int i = 0; i < (LINE_WIDTH - textLength(title)) / 2; i++) {
        cout << ' ';
    }
    cout << title << "\n";
    drawLine('=', LINE_WIDTH);
}

// ---------- Input helpers (all of them re-ask until the value is valid) ----------

// Reads one line from the keyboard. If the input is closed (Ctrl+D / Ctrl+Z or end of a piped
// file) the program ends cleanly instead of re-asking forever.
void readInputLine(string &text) {
    if (!getline(cin, text)) {
        cout << "\n Input closed. Goodbye!\n";
        exit(0);
    }
}

string getNonEmptyLine(const string &prompt) {
    string text;
    do {
        cout << prompt;
        readInputLine(text);
        text = trim(text);
        if (text == "") {
            cout << "  ! This field cannot be blank.\n";
        }
    } while (text == "");
    return text;
}

// An integer from low to high (the prompt is printed by the caller)
int getIntInRange(int low, int high) {
    string text;
    bool valid;
    int value = 0;
    do {
        readInputLine(text);
        text = trim(text);
        valid = (text != "");
        for (int i = 0; i < textLength(text) && valid; i++) {
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

// A required text value (name, house number, barangay, ...)
string askText(const string &prompt, bool allowDigits) {
    string text = getNonEmptyLine(prompt);
    while (!isValidText(text, allowDigits)) {
        cout << "  ! Invalid entry. Use letters" << (allowDigits ? ", numbers" : "")
             << ", spaces, - ' and . only (no commas).\n";
        text = getNonEmptyLine(prompt);
    }
    return text;
}

// An optional text value: pressing Enter skips it (returns "")
string askOptionalText(const string &prompt) {
    string text;
    bool valid = false;
    do {
        cout << prompt;
        readInputLine(text);
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
        readInputLine(text);
        text = trim(text);
        if (textLength(text) == 2 && text[1] == '.') text = cutText(text, 0, 1);
        valid = (text == "") || (textLength(text) == 1 && isLetterChar(text[0]));
        if (!valid) cout << "  ! Enter one letter only (e.g. A).\n";
    } while (!valid);
    return (text == "") ? "" : toUpper(text) + ".";
}

string askDate(const string &prompt) {
    string date = getNonEmptyLine(prompt);
    while (!isValidDate(date)) {
        cout << "  ! Invalid date. Use MM/DD/YYYY.\n";
        date = getNonEmptyLine(prompt);
    }
    return date;
}

string askMobile() {
    string mobile = getNonEmptyLine(" Mobile number (+639XXXXXXXXX) : ");
    while (!isValidMobile(mobile)) {
        cout << "  ! Invalid mobile number. Use +639 followed by 9 digits.\n";
        mobile = getNonEmptyLine(" Mobile number (+639XXXXXXXXX) : ");
    }
    return mobile;
}

string askContact() {
    string contact = getNonEmptyLine(" Contact number (+63 then 9 or 10 digits) : ");
    while (!isValidContact(contact)) {
        cout << "  ! Invalid format. Examples: +63285884000 or +639101234567\n";
        contact = getNonEmptyLine(" Contact number (+63 then 9 or 10 digits) : ");
    }
    return contact;
}

string askEmail() {
    string email = getNonEmptyLine(" Email address : ");
    while (!isValidEmail(email)) {
        cout << "  ! Invalid email. Accepted: ";
        for (int i = 0; i < NUM_EMAIL_DOMAINS; i++) {
            cout << (i > 0 ? ", @" : "@") << EMAIL_DOMAINS[i];
        }
        cout << "\n";
        email = getNonEmptyLine(" Email address : ");
    }
    return email;
}

bool askYesNo(const string &prompt) {
    string answer = toUpper(getNonEmptyLine(prompt + " (y/n) : "));
    while (answer != "Y" && answer != "N") {
        cout << "  ! Please type y or n.\n";
        answer = toUpper(getNonEmptyLine(prompt + " (y/n) : "));
    }
    return answer == "Y";
}

// Shows the allowed words and asks for one of them (typed). Returns its position.
int askWord(const string &label, const string choices[], int numChoices) {
    cout << " " << label << "s: ";
    for (int i = 0; i < numChoices; i++) {
        cout << (i > 0 ? ", " : "") << choices[i];
    }
    cout << "\n";
    int index = findChoice(choices, numChoices, getNonEmptyLine(" " + label + " : "));
    while (index == -1) {
        cout << "  ! Invalid " << label << ". Choose one from the list.\n";
        index = findChoice(choices, numChoices, getNonEmptyLine(" " + label + " : "));
    }
    return index;
}

// Shows a numbered list and returns the position (0, 1, ...) of the chosen item
int askFromList(const string &title, const string items[], int numItems) {
    cout << "\n " << title << ":\n";
    for (int i = 0; i < numItems; i++) {
        cout << "   [" << (i + 1) << "] " << items[i] << "\n";
    }
    cout << " Choose (1-" << numItems << ") : ";
    return getIntInRange(1, numItems) - 1;
}

// Prints a menu and returns the choice (1 to numItems)
int askMenu(const string &title, const string items[], int numItems) {
    printTitle(title);
    for (int i = 0; i < numItems; i++) {
        cout << "   [" << (i + 1) << "] " << items[i] << "\n";
    }
    drawLine('-', LINE_WIDTH);
    cout << " Choice: ";
    return getIntInRange(1, numItems);
}

// ---------- Seat helpers ----------

// "J20" -> row index 9 and seat number 20
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

// Reads and validates a space-separated list of seats (no duplicates)
int readSeatList(int minSeats, string seats[MAX_SEATS_PER_BOOKING]) {
    bool valid;
    int count = 0;
    do {
        string line;
        readInputLine(line);
        line = toUpper(trim(line));

        bool overflow = false;
        count = parseSeatTokens(line, seats, overflow);

        valid = (count >= minSeats) && !overflow;
        for (int i = 0; i < count && valid; i++) {
            int rowIndex, seatNo;
            if (!parseSeat(seats[i], rowIndex, seatNo)) valid = false;
            for (int j = 0; j < i && valid; j++) {
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
// CLASS: Record  (PARENT class of Customer, Movie, Cinema and Booking)
// =====================================================================================
class Record {
protected:
    string id;                              // e.g. M-001, I-001, B-001

public:
    Record() {
        id = "";
    }

    Record(string recordID) {
        id = recordID;
    }

    // VIRTUAL destructor: deleting a child through a Record* frees the child properly
    virtual ~Record() {}

    string getID() { return id; }
    string getFileName() { return id + ".txt"; }

    bool deleteFile() {
        return remove(getFileName().c_str()) == 0;
    }

    // VIRTUAL: every child overrides these, so a Record* runs the child's own version
    virtual void display() {                // parent version: only the ID
        cout << id << "\n";
    }

    virtual void displayDetails() {         // parent version: only the ID
        cout << id << "\n";
    }

    virtual bool loadFromFile() {           // parent version: nothing to load
        return false;
    }
};

// =====================================================================================
// CLASS: Customer  (CHILD of Record) - one C-XXX.txt file
// A customer is registered once (and gets a C-XXX ID); a booking only needs that ID.
// =====================================================================================
class Customer : public Record {
private:
    string name;                // as saved in the file: "Reyes, Rose Marie A."
    string address;
    string mobile;
    string birthday;
    string email;

    // Writes the file layout to any output (the screen or a file)
    void writeTo(ostream &out) {
        out << name << "\n" << address << "\n" << mobile << "\n" << birthday << "\n" << email << "\n";
        out << "Customer ID # " << id << "\n";
    }

public:
    Customer() {}                           // strings start empty

    Customer(string customerID) {
        id = customerID;                    // 'id' is inherited from Record
    }

    Customer(string customerID, string custName, string custMobile, string custEmail) {
        id = customerID;
        name = custName;
        mobile = custMobile;
        email = custEmail;
    }

    string getName() { return flipName(name); }         // shown as "Rose Marie A. Reyes"
    string getMobile() { return mobile; }
    string getBirthday() { return birthday; }
    string getEmail() { return email; }

    void setDetails(string custName, string custAddress, string custMobile,
                    string custBirthday, string custEmail) {
        name = custName;
        address = custAddress;
        mobile = custMobile;
        birthday = custBirthday;
        email = custEmail;
    }

    // Same name as Record::display(), different implementation
    void display() override {
        cout << getName() << "\n";
        cout << "     Mobile : " << mobile << "\n";
        cout << "     Email  : " << email << "\n";
    }

    void displayDetails() override {
        writeTo(cout);
    }

    bool saveToFile() {
        ofstream out(getFileName());
        if (!out.is_open()) return false;
        writeTo(out);
        out.close();
        return true;
    }

    /* C-XXX.txt (one value per line):
     *  0 Last, Given Name MI      1 address       2 mobile number
     *  3 birthday                 4 email         5 Customer ID # C-XXX */
    bool loadFromFile() override {
        string lines[MAX_FILE_LINES];
        if (readFileLines(getFileName(), lines, MAX_FILE_LINES) < 5) return false;

        name = lines[0];
        address = lines[1];
        mobile = lines[2];
        birthday = lines[3];
        email = lines[4];
        return (name != "");
    }
};

// =====================================================================================
// CLASS: Movie  (CHILD of Record) - one M-XXX.txt file
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

    void writeTo(ostream &out) {
        out << name << "\n" << genre << "\n";
        out << "Start date: " << longDate(startDate) << "\n";
        out << "End date: " << longDate(endDate) << "\n";
        out << "Rating: " << rating << "\n";
        out << contactPerson << "\n";
        out << mobile << "\n";
        out << email << "\n";
        out << "Movie ID # " << id << "\n";
    }

public:
    Movie() {}

    Movie(string movieID, string movieName) {
        id = movieID;
        name = movieName;
    }

    string getName() { return name; }

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
    void display() override {
        cout << name << "\n";
        cout << "     Genre  : " << genre << "   Rating: " << rating << "\n";
        if (startDate != "" && endDate != "") {
            cout << "     Runs   : " << startDate << " to " << endDate << "\n";
        }
    }

    void displayDetails() override {
        writeTo(cout);
    }

    bool saveToFile() {
        ofstream out(getFileName());
        if (!out.is_open()) return false;
        writeTo(out);
        out.close();
        return true;
    }

    /* M-XXX.txt (one value per line):
     *  0 name       1 genre       2 Start date: ...     3 End date: ...     4 Rating: ...
     *  5 contact person           6 Mobile Number: ...  7 Email Address: ...
     *  8 Movie ID # M-XXX */
    bool loadFromFile() override {
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
// CLASS: Cinema  (CHILD of Record) - one I-XXX.txt file
// =====================================================================================
class Cinema : public Record {
private:
    string name;
    string address;
    int rows;
    int seatsPerRow;
    string contact;
    int price;                  // ticket price per seat
    string type;

    void writeTo(ostream &out) {
        out << name << "\n" << address << "\n";
        out << "Number of rows: " << rows << "\n";
        out << "Number of seats per row: " << seatsPerRow << "\n";
        out << "Capacity: " << getCapacity() << "\n";
        out << contact << "\n";
        out << "Price: " << price << "\n";
        out << "Cinema type: " << type << "\n";
        out << "Cinema Code # " << id << "\n";
    }

public:
    Cinema() {
        rows = 0;
        seatsPerRow = 0;
        price = 0;
    }

    Cinema(string cinemaID, string cinemaName) {
        id = cinemaID;
        name = cinemaName;
        rows = 0;
        seatsPerRow = 0;
        price = 0;
    }

    string getName() { return name; }
    int getRows() { return rows; }
    int getSeatsPerRow() { return seatsPerRow; }
    int getCapacity() { return rows * seatsPerRow; }        
    int getPrice() { return price; }

    void setDetails(string cinemaName, string cinemaAddress, int numRows, int numSeats,
                    string cinemaContact, int ticketPrice, string cinemaType) {
        name = cinemaName;
        address = cinemaAddress;
        rows = numRows;
        seatsPerRow = numSeats;
        contact = cinemaContact;
        price = ticketPrice;
        type = cinemaType;
    }

    // A cinema can only be booked if its layout fits the seat names A1..Z50 and it has a price
    bool isUsable() {
        return (rows >= 1 && rows <= MAX_ROWS && seatsPerRow >= 1 &&
                seatsPerRow <= MAX_SEATS_PER_ROW && price > 0);
    }

    // Same name as Record::display(), different implementation
    void display() override {
        cout << name << "\n";
        cout << "     Type   : " << type << "   Capacity: " << getCapacity() << "\n";
        cout << "     Price  : PHP " << formatMoney(price) << " per seat\n";
    }

    void displayDetails() override {
        writeTo(cout);
    }

    bool saveToFile() {
        ofstream out(getFileName());
        if (!out.is_open()) return false;
        writeTo(out);
        out.close();
        return true;
    }

    /* I-XXX.txt (one value per line):
     *  0 name              1 address            2 Number of rows: N
     *  3 Number of seats per row: N             4 Capacity: N
     *  5 contact number    6 Price: N           7 Cinema type: X      8 Cinema Code # I-XXX */
    bool loadFromFile() override {
        string lines[MAX_FILE_LINES];
        if (readFileLines(getFileName(), lines, MAX_FILE_LINES) < 8) return false;

        name = lines[0];
        address = lines[1];
        rows = toInt(extractAfterColon(lines[2]));
        seatsPerRow = toInt(extractAfterColon(lines[3]));
        contact = lines[5];
        price = toInt(extractAfterColon(lines[6]));
        type = extractAfterColon(lines[7]);
        return (name != "");
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
    Booking() {}

    Booking(string bookingID) {
        id = bookingID;
    }

    Booking(string bookingID, Customer cust, string pay, string movie, string cinema, string seats) {
        id = bookingID;
        customer = cust;
        payment = pay;
        movieName = movie;
        cinemaName = cinema;
        seatText = seats;
    }

    string getPayment() { return payment; }
    string getMovieName() { return movieName; }
    string getCinemaName() { return cinemaName; }
    string getSeatText() { return seatText; }
    string getTotalText() { return totalText; }
    void setTotalText(string total) { totalText = total; }

    // Same name as Record::display(), different implementation
    void displayDetails() override {
        display();
    }

    // Same name as Record::display(), different implementation
    void display() override {
        cout << " Customer name: " << customer.getName() << "\n";
        cout << " " << customer.getMobile() << "\n";
        cout << " " << customer.getEmail() << "\n";
        cout << " Type of payment: " << payment << "\n";
        cout << " Movie name: " << movieName << "\n";
        cout << " Cinema: " << cinemaName << "\n";
        cout << " Seat number/s: " << seatText << "\n";
        cout << " " << id << "\n";
        if (totalText != "") cout << " Total payment: " << totalText << "\n";
    }

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
        if (totalText != "") out << "Total payment: " << totalText << "\n";
        out.close();
        return true;
    }

    bool loadFromFile() override {
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
// CLASS: SeatMap - the seat grid of one cinema
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
        cout << "\n   ";
        for (int c = 0; c < seatsPerRow; c++) {
            cout << ((c + 1 < 10) ? "  " : " ") << (c + 1) << " ";
        }
        cout << "\n";

        for (int r = 0; r < rows; r++) {
            cout << (char)('A' + r) << "  ";
            for (int c = 0; c < seatsPerRow; c++) {
                cout << (cells[r][c] == 1 ? "[X] " : "[ ] ");
            }
            cout << "\n";
        }
        cout << "\n   [ ] = available      [X] = reserved  \n";
    }
};

// =====================================================================================
// CLASS: RecordList  (PARENT class)
// =====================================================================================
class RecordList {
protected:
    string listFileName;                    // e.g. "Booking List.txt"
    string counterFileName;                 // e.g. "Booking Counter.txt"
    string *ids;                            // POINTER: array of IDs (made with new)
    string *texts;                          // POINTER: the rest of each line (name, total, ...)
    int count;

public:
    RecordList(const RecordList &) = delete;             // owns raw memory: copying is not allowed
    RecordList &operator=(const RecordList &) = delete;

    RecordList() {
        ids = new string[MAX_RECORDS];
        texts = new string[MAX_RECORDS];
        count = 0;
    }

    // DESTRUCTOR: gives the memory back
    virtual ~RecordList() {
        delete[] ids;
        delete[] texts;
    }

    int getCount() { return count; }
    string getID(int i) { return (i >= 0 && i < count) ? ids[i] : ""; }
    string getText(int i) { return (i >= 0 && i < count) ? texts[i] : ""; }

    // Reads the list file (a missing file means the list is empty)
    int load() {
        count = 0;
        ifstream in(listFileName);
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
        for (int i = 0; i < count; i++) {
            if (toUpper(ids[i]) == toUpper(recordID)) return i;
        }
        return -1;
    }

    // True if a line with this text (name) already exists, ignoring upper/lower case
    bool hasText(const string &text) {
        load();
        for (int i = 0; i < count; i++) {
            if (toUpper(texts[i]) == toUpper(text)) return true;
        }
        return false;
    }

    // The counter file keeps the highest number ever used
    int readCounter() {
        int lastNumber = 0;
        ifstream in(counterFileName);
        if (in.is_open()) in >> lastNumber;
        return lastNumber;
    }

    void writeCounter(int number) {
        ofstream out(counterFileName);
        out << number;
    }

    // Raises the counter to the number of this ID, so that the number is never used again
    void markUsed(const string &recordID) {
        int number = toInt(cutText(recordID, 2, RECORD_ID_LENGTH));
        if (number > readCounter()) writeCounter(number);
    }

    // A new ID such as M-004 (one more than the highest number ever used), or "" if 999 was reached
    string newID(char prefix) {
        load();
        int last = readCounter();
        for (int i = 0; i < count; i++) {
            int number = toInt(cutText(ids[i], 2, RECORD_ID_LENGTH));
            if (number > last) last = number;
        }
        return (last + 1 > MAX_RECORD_NUMBER) ? "" : makeID(prefix, last + 1);
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
        markUsed(recordID);
        return true;
    }

    // Removes one line and rewrites the file without it
    bool removeAt(int index) {
        if (index < 0 || index >= count) return false;
        markUsed(ids[index]);               // a deleted number is never reused
        ofstream out(listFileName);
        if (!out.is_open()) return false;
        for (int i = 0; i < count; i++) {
            if (i != index) out << ids[i] << " " << texts[i] << "\n";
        }
        out.close();
        for (int i = index; i < count - 1; i++) {
            ids[i] = ids[i + 1];
            texts[i] = texts[i + 1];
        }
        count--;
        return true;
    }

    // Asks for an ID until one that is in the list is given. Returns false if 0 (cancel) was typed.
    // 'recordID' and 'index' are passed by reference and filled with the record that was found.
    bool askExisting(char prefix, const string &prompt, string &recordID, int &index) {
        bool found = false;
        bool cancelled = false;
        do {
            string input = getNonEmptyLine(prompt);
            if (input == "0") {
                cancelled = true;
            } else if (!isValidRecordID(prefix, input)) {
                cout << "  ! Invalid ID. Use " << prefix << "-XXX.\n";
            } else {
                recordID = toUpper(trim(input));
                load();
                index = findIndex(recordID);
                if (index == -1) {
                    cout << "  ! " << recordID << " does not exist.\n";
                } else {
                    found = true;
                }
            }
        } while (!found && !cancelled);
        return found;
    }

    // Parent version: prints every line as "ID text"
    virtual void display() {
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

    double sumTotals() {
        double grandTotal = 0;
        for (int i = 0; i < count; i++) {
            double value = 0;
            if (parseDecimal(texts[i], value)) grandTotal += value;
        }
        return grandTotal;
    }

    // Same name as RecordList::display(), different implementation
    void display() override {
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
    Movie *movies;                          // POINTER: array of Movie objects (made with new)
    Cinema *cinemas;                        // POINTER: array of Cinema objects (made with new)
    int movieCount;
    int cinemaCount;

    // POINTER: these point to the movie and cinema the customer is booking
    Movie *selectedMovie;
    Cinema *selectedCinema;

    SeatMap seatMap;                        // seat grid of the selected movie + cinema
    BookingList ledger;                     // Booking List.txt
    CustomerList customerList;              // Customer List.txt
    MovieList movieList;                    // Movie List.txt
    CinemaList cinemaList;                  // Cinema List.txt

    // Reads Movie List.txt and Cinema List.txt, then every M-XXX.txt and I-XXX.txt in them
    void loadCatalog() {
        movieCount = 0;
        cinemaCount = 0;

        movieList.load();
        for (int i = 0; i < movieList.getCount() && movieCount < MAX_MOVIES; i++) {
            Movie movie(toUpper(movieList.getID(i)), movieList.getText(i));
            if (movie.loadFromFile()) {     // movies whose file is missing are skipped
                movies[movieCount] = movie;
                movieCount++;
            }
        }

        cinemaList.load();
        for (int i = 0; i < cinemaList.getCount() && cinemaCount < MAX_CINEMAS; i++) {
            Cinema cinema(toUpper(cinemaList.getID(i)), cinemaList.getText(i));
            if (cinema.loadFromFile() && cinema.isUsable()) {   // unusable cinemas are skipped
                cinemas[cinemaCount] = cinema;
                cinemaCount++;
            }
        }
    }

    // Builds the seat grid of the selected movie in the selected cinema from the confirmed booking
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
        string seatText = requestedSeats[0];
        for (int i = 1; i < requestedCount; i++) {
            seatText += " " + requestedSeats[i];
        }

        // The customer was registered in the CUSTOMER block: only the customer ID is needed
        printTitle("CUSTOMER");
        string customerID;
        int customerIndex = -1;
        if (!customerList.askExisting('C', " Customer ID (C-XXX, or 0 to cancel) : ",
                                      customerID, customerIndex)) {
            cout << "\n Booking cancelled.\n";
            return false;
        }
        Customer customer(customerID);
        if (!customer.loadFromFile()) {
            cout << "  ! Unable to read the file of " << customerID << ".\n";
            return false;
        }
        cout << "\n Welcome, " << customer.getName() << "!\n";

        string payment = PAYMENT_TYPES[askFromList("Payment type", PAYMENT_TYPES, NUM_PAYMENT_TYPES)];

        // The ID is assigned only after all information was given
        string bookingID = ledger.newID('B');
        if (bookingID == "") {
            cout << "\n Sorry, booking limit reached.\n";
            return false;
        }

        booking = Booking(bookingID, customer, payment, selectedMovie->getName(),
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
            booking->setTotalText(formatMoney((double)selectedCinema->getPrice() * seatCount));
            booking->saveToFile();                          // file now ends with the total
            ledger.load();
            ledger.add(booking->getID(), booking->getTotalText());
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
        string bookingID;
        int index = -1;
        if (!ledger.askExisting('B', " Booking ID (B-XXX, or 0 to cancel) : ", bookingID, index)) return;

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
        if (ledger.load() == 0) {
            cout << " No bookings yet.\n";
        } else {
            ledger.display();                   // BookingList's own version of display()
        }
        drawLine('-', LINE_WIDTH);
    }

    void displayTotalPayments() {
        printTitle("TOTAL PAYMENTS");
        cout << " Total Bookings : " << ledger.load() << "\n";
        cout << " Total Earnings : PHP " << formatMoney(ledger.sumTotals()) << "\n";
        drawLine('-', LINE_WIDTH);
    }

    // DELETE BOOKING: checks that the booking exists, deletes its file and updates the list.
    // The seats become free again automatically: seat maps are built from the list.
    void deleteBooking() {
        printTitle("DELETE BOOKING");
        string bookingID;
        int index = -1;
        if (!ledger.askExisting('B', " Booking ID (B-XXX, or 0 to cancel) : ", bookingID, index)) return;
        cout << "  [OK] Booking " << bookingID << " found. Proceeding with deletion...\n";

        Booking booking(bookingID);
        booking.deleteFile();
        if (!ledger.removeAt(index)) {
            cout << "Unable to open file\n";
            return;
        }
        cout << "\n Booking " << bookingID << " deleted successfully.\n";
    }

    // Manager menu loop (runs until the manager chooses Back)
    void managerFlow() {
        const string items[5] = {"Display booking", "Display all bookings", "Delete booking",
                                 "Total payments received", "Back"};
        int choice = 0;
        do {
            choice = askMenu("MANAGER MENU", items, 5);
            switch (choice) {
                case 1: displayBooking(); break;
                case 2: displayAllBookings(); break;
                case 3: deleteBooking(); break;
                case 4: displayTotalPayments(); break;
                default: cout << "\n Returning to booking menu.\n"; break;
            }
        } while (choice != 5);
    }

public:
    BookingSystem() {
        movies = new Movie[MAX_MOVIES];         // POINTER: memory is taken here ...
        cinemas = new Cinema[MAX_CINEMAS];
        movieCount = 0;
        cinemaCount = 0;
        selectedMovie = 0;                      // POINTER: 0 means "points to nothing"
        selectedCinema = 0;
    }

    // DESTRUCTOR
    ~BookingSystem() {
        delete[] movies;
        delete[] cinemas;
    }

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
// =====================================================================================
class CustomerSystem {
private:
    CustomerList customerList;              // Customer List.txt

    // Position in the list of a customer with the same name AND birthday, or the same email
    // (-1 if this customer is not registered yet)
    int findDuplicate(const string &fullName, const string &birthday, const string &email) {
        customerList.load();
        for (int i = 0; i < customerList.getCount(); i++) {
            Customer other(customerList.getID(i));
            if (!other.loadFromFile()) continue;        // no readable file: nothing to compare

            bool samePerson = (toUpper(other.getName()) == toUpper(flipName(fullName)) &&
                               other.getBirthday() == birthday);
            if (samePerson || toUpper(other.getEmail()) == toUpper(email)) return i;
        }
        return -1;
    }

    // ADD CUSTOMER: asks for every detail, validates it, then assigns the customer ID
    void addCustomer() {
        printTitle("ADD CUSTOMER");

        cout << " NAME\n";
        string lastName = askText("   Last name : ", false);
        string givenName = askText("   Given name : ", false);
        string middleInitial = askMiddleInitial();
        string fullName = lastName + ", " + givenName;
        if (middleInitial != "") fullName += " " + middleInitial;

        cout << "\n ADDRESS (Enter to skip the optional parts)\n";
        string houseNo = askText("   House # : ", true);
        string street = askOptionalText("   Street name (optional) : ");
        string subdivision = askOptionalText("   Subdivision/Building name (optional) : ");
        string barangay = askText("   Barangay name : ", true);
        if (toUpper(cutText(barangay, 0, 4)) == "BRGY.") {      // the file adds "Brgy." itself
            barangay = trim(cutText(barangay, 4, textLength(barangay)));
        }
        string city = askText("   City/Municipality : ", false);

        string address = houseNo;
        if (street != "") address += " " + street;
        if (subdivision != "") address += ", " + subdivision;
        address += ", Bgy. " + barangay + ", " + city;

        cout << "\n";
        string mobile = askMobile();

        string birthday = askDate(" Birthday (MM/DD/YYYY) : ");
        while (convertDate(birthday) > todayNumber()) {
            cout << "  ! The birthday cannot be in the future.\n";
            birthday = askDate(" Birthday (MM/DD/YYYY) : ");
        }
        if (todayNumber() - convertDate(birthday) < CUSTOMER_MIN_AGE * 10000) {
            cout << "  ! Only adults (" << CUSTOMER_MIN_AGE << " years old and above) can register.\n";
            cout << "  Registration cancelled.\n";
            return;
        }

        string email = askEmail();

        // The same customer cannot be registered twice
        int duplicateIndex = findDuplicate(fullName, birthday, email);
        if (duplicateIndex != -1) {
            cout << "\n  ! This customer is already registered as " << customerList.getID(duplicateIndex) << ".\n";
            cout << "  Customer was not added.\n";
            return;
        }

        // The ID is assigned only after all the information was submitted
        string customerID = customerList.newID('C');
        if (customerID == "") {
            cout << "\n Sorry, the customer limit was reached.\n";
            return;
        }

        Customer customer(customerID);
        customer.setDetails(fullName, address, mobile, birthday, email);
        if (!customer.saveToFile()) {
            cout << "  ! Unable to create the customer file.\n";
            return;
        }
        customerList.add(customerID, fullName);

        cout << "\n Customer successfully added!\n";
        cout << " Customer ID: " << customerID << "\n";
    }

    void showAllCustomers() {
        printTitle("ALL CUSTOMERS");
        if (customerList.load() == 0) {
            cout << " No Customers Found.\n";
        } else {
            customerList.display();
        }
        drawLine('-', LINE_WIDTH);
    }

    void showCustomer() {
        printTitle("SHOW CUSTOMER");
        string customerID;
        int index = -1;
        if (!customerList.askExisting('C', " Customer ID (C-XXX, or 0 to cancel) : ", customerID, index)) return;

        Customer customer(customerID);
        if (!customer.loadFromFile()) {
            cout << "Unable to open file\n";
            return;
        }
        cout << "\n";
        drawLine('-', LINE_WIDTH);
        Record *record = &customer;         // POINTER: Customer seen as its parent Record
        record->displayDetails();           // runs Customer::displayDetails()
        drawLine('-', LINE_WIDTH);
    }

    // DELETE CUSTOMER: the customer must exist first; then the file is deleted and the list is updated
    void deleteCustomer() {
        printTitle("DELETE CUSTOMER");
        string customerID;
        int index = -1;
        if (!customerList.askExisting('C', " Customer ID to delete (C-XXX, or 0 to cancel) : ",
                                      customerID, index)) return;
        cout << "  [OK] Customer " << customerID << " exists. Proceeding with deletion...\n";

        Customer customer(customerID);
        customer.deleteFile();              // the file may already be missing: the list is still fixed
        if (!customerList.removeAt(index)) {
            cout << "  ! Unable to update Customer List.txt.\n";
            return;
        }
        cout << "\n Customer " << customerID << " deleted successfully.\n";
    }

public:
    // Customer block for a CUSTOMER: register
    void runCustomer() {
        addCustomer();
    }

    // Customer block for the MANAGER: add, show all, show, delete
    void runManager() {
        const string items[5] = {"Add customer", "Show all customers", "Show customer",
                                 "Delete customer", "Back"};
        int choice = 0;
        do {
            choice = askMenu("CUSTOMER MENU", items, 5);
            switch (choice) {
                case 1: addCustomer(); break;
                case 2: showAllCustomers(); break;
                case 3: showCustomer(); break;
                case 4: deleteCustomer(); break;
                default: cout << "\n Exiting Customer Block.\n"; break;
            }
        } while (choice != 5);
    }
};

// =====================================================================================
// CLASS: MovieSystem - menus and workflows of the MOVIE block
// =====================================================================================
class MovieSystem {
private:
    MovieList movieList;                    // Movie List.txt

    // ADD MOVIE: asks for every detail, validates it, then assigns the movie ID
    void addMovie() {
        printTitle("ADD MOVIE");

        string name = getNonEmptyLine(" Movie name : ");
        while (movieList.hasText(name)) {
            cout << "  ! This movie is already registered.\n";
            name = getNonEmptyLine(" Please enter a different movie name : ");
        }

        int genreIndex = askWord("Genre", MOVIE_GENRES, NUM_MOVIE_GENRES);

        string startDate = askDate(" Start date (MM/DD/YYYY) : ");
        while (convertDate(startDate) <= todayNumber()) {
            cout << "  ! The start date must be in the future.\n";
            startDate = askDate(" Start date (MM/DD/YYYY) : ");
        }
        string endDate = askDate(" End date (MM/DD/YYYY) : ");

        while (convertDate(endDate) <= todayNumber() ||
        convertDate(endDate) <= convertDate(startDate)) {
            cout << "  ! End date must be after today and after the start date.\n";
            endDate = askDate(" End date (MM/DD/YYYY) : ");
        }

        int ratingIndex = askWord("Rating", MOVIE_RATINGS, NUM_MOVIE_RATINGS);
        string contactPerson = getNonEmptyLine(" Contact person : ");
        string mobile = askMobile();
        string email = askEmail();

        // The ID is assigned only after all the information was submitted
        string movieID = movieList.newID('M');
        if (movieID == "") {
            cout << "\n Sorry, the movie limit was reached.\n";
            return;
        }

        Movie movie(movieID, name);
        movie.setDetails(name, MOVIE_GENRES[genreIndex], startDate, endDate,
                         MOVIE_RATINGS[ratingIndex], contactPerson, mobile, email);
        if (!movie.saveToFile()) {
            cout << "  ! Unable to create the movie file.\n";
            return;
        }
        movieList.add(movieID, name);

        cout << "\n Movie successfully added!\n";
        cout << " Movie ID: " << movieID << "\n";
    }

    void showAllMovies() {
        printTitle("ALL MOVIES");
        if (movieList.load() == 0) {
            cout << " No Movies Found.\n";
        } else {
            movieList.display();
        }
        drawLine('-', LINE_WIDTH);
    }

    void showMovie() {
        printTitle("SHOW MOVIE");
        string movieID;
        int index = -1;
        if (!movieList.askExisting('M', " Movie ID (M-XXX, or 0 to cancel) : ", movieID, index)) return;

        Movie movie(movieID, movieList.getText(index));
        if (!movie.loadFromFile()) {
            cout << "  ! Movie file of " << movieID << " does not exist.\n";
            return;
        }
        cout << "\n";
        drawLine('-', LINE_WIDTH);
        Record *record = &movie;            // POINTER: Movie seen as its parent Record
        record->displayDetails();           // runs Movie::displayDetails()
        drawLine('-', LINE_WIDTH);
    }

    // DELETE MOVIE: the movie must exist first; then its file is deleted and the list is updated
    void deleteMovie() {
        printTitle("DELETE MOVIE");
        string movieID;
        int index = -1;
        if (!movieList.askExisting('M', " Movie ID to delete (M-XXX, or 0 to cancel) : ", movieID, index)) return;
        cout << "  [OK] Movie " << movieID << " exists. Proceeding with deletion...\n";

        Movie movie(movieID, movieList.getText(index));
        if (!movie.deleteFile()) {
            cout << "  ! Unable to delete the movie file.\n";
            return;
        }
        if (!movieList.removeAt(index)) {
            cout << "  ! Unable to update Movie List.txt.\n";
            return;
        }
        cout << "\n Movie " << movieID << " deleted successfully.\n";
    }

public:
    void runManager() {
        const string items[5] = {"Add movie", "Show all movies", "Show movie", "Delete movie", "Back"};
        int choice = 0;
        do {
            choice = askMenu("MOVIE MENU", items, 5);
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
// CLASS: CinemaSystem
// =====================================================================================
class CinemaSystem {
private:
    CinemaList cinemaList;                  // Cinema List.txt

    // mode 2 = name, 3 = capacity, 4 = ticket price
    bool shouldSwap(Cinema *a, Cinema *b, int mode) {
        if (mode == 2) return toUpper(a->getName()) > toUpper(b->getName());
        if (mode == 3) return a->getCapacity() > b->getCapacity();
        return a->getPrice() > b->getPrice();
    }

    // ADD CINEMA: asks for every detail, validates it, then assigns the cinema ID
    void addCinema() {
        printTitle("ADD CINEMA");
        if (cinemaList.load() >= MAX_CINEMAS) {
            cout << " The cinema list is full. A cinema must be deleted first.\n";
            return;
        }

        string name = getNonEmptyLine(" Cinema name : ");
        while (cinemaList.hasText(name)) {
            cout << "  ! This cinema is already registered.\n";
            name = getNonEmptyLine(" Please enter a different cinema name : ");
        }

        // address: street, subdivision and barangay can be skipped
        cout << " ADDRESS (Enter to skip the optional parts)\n";
        string parts[4];
        parts[0] = askOptionalText("   Street name (optional) : ");
        parts[1] = askOptionalText("   Subdivision/Building name (optional) : ");
        parts[2] = askOptionalText("   Barangay name (optional) : ");
        parts[3] = askText("   City/Municipality : ", false);
        string address = "";
        for (int i = 0; i < 4; i++) {
            if (parts[i] != "") address += (address == "" ? "" : ", ") + parts[i];
        }

        cout << " Number of rows (1-" << MAX_ROWS << ") : ";
        int rows = getIntInRange(1, MAX_ROWS);
        cout << " Number of seats per row (1-" << MAX_SEATS_PER_ROW << ") : ";
        int seatsPerRow = getIntInRange(1, MAX_SEATS_PER_ROW);
        cout << " Capacity (rows x seats per row) : " << rows * seatsPerRow << "\n";

        string contact = askContact();
        cout << " Ticket price (1-" << MAX_TICKET_PRICE << ") : ";
        int price = getIntInRange(1, MAX_TICKET_PRICE);
        int typeIndex = askFromList("Cinema type", CINEMA_TYPES, NUM_CINEMA_TYPES);

        // The ID is assigned only after all the information was submitted
        string cinemaID = cinemaList.newID('I');
        if (cinemaID == "") {
            cout << "\n The cinema codes are used up (I-999 reached).\n";
            return;
        }

        Cinema cinema(cinemaID, name);
        cinema.setDetails(name, address, rows, seatsPerRow, contact, price, CINEMA_TYPES[typeIndex]);
        if (!cinema.saveToFile()) {
            cout << "  ! Unable to create the cinema file.\n";
            return;
        }
        cinemaList.add(cinemaID, name);

        cout << "\n Cinema added successfully!\n";
        cout << " Cinema Code # " << cinemaID << "\n";
    }

    // SHOW ALL CINEMAS: by code, by name, by increasing capacity or by increasing price
    void showAllCinemas() {
        printTitle("SHOW ALL CINEMAS");
        int count = cinemaList.load();
        if (count == 0) {
            cout << " No Cinemas Found.\n";
            return;
        }

        const string items[4] = {"Cinema list format (by code)", "Alphabetical (by name)",
                                 "Increasing capacity", "Increasing ticket price"};
        int mode = askMenu("DISPLAY AS", items, 4);
        cout << "\n";
        if (mode == 1) {
            cinemaList.display();
            return;
        }

        // POINTER: a dynamic array of the cinemas
        Cinema *all = new Cinema[count];
        Cinema *sorted[MAX_CINEMAS];        // POINTERS: only these are rearranged by the sort
        int loaded = 0;
        for (int i = 0; i < count && loaded < MAX_CINEMAS; i++) {
            all[loaded] = Cinema(cinemaList.getID(i), cinemaList.getText(i));
            if (all[loaded].loadFromFile()) {
                sorted[loaded] = &all[loaded];
                loaded++;
            }
        }

        // bubble sort: swaps the pointers, the Cinema objects stay where they are
        for (int i = 0; i < loaded - 1; i++) {
            for (int j = 0; j < loaded - 1 - i; j++) {
                if (shouldSwap(sorted[j], sorted[j + 1], mode)) {
                    Cinema *temp = sorted[j];
                    sorted[j] = sorted[j + 1];
                    sorted[j + 1] = temp;
                }
            }
        }

        for (int i = 0; i < loaded; i++) {
            cout << " " << sorted[i]->getID() << "  " << sorted[i]->getName();
            if (mode == 3) cout << "  (Capacity: " << sorted[i]->getCapacity() << ")";
            if (mode == 4) cout << "  (Price: " << sorted[i]->getPrice() << ")";
            cout << "\n";
        }
        delete[] all;                    
    }

    void showCinema() {
        printTitle("SHOW CINEMA");
        string cinemaID;
        int index = -1;
        if (cinemaList.load() == 0) {
            cout << " No Cinemas Found.\n";
            return;
        }
        if (!cinemaList.askExisting('I', " Cinema code (I-XXX, or 0 to cancel) : ", cinemaID, index)) return;

        Cinema cinema(cinemaID, cinemaList.getText(index));
        if (!cinema.loadFromFile()) {
            cout << "  ! Cinema file of " << cinemaID << " does not exist.\n";
            return;
        }
        cout << "\n";
        drawLine('-', LINE_WIDTH);
        Record *record = &cinema;           // POINTER: Cinema seen as its parent Record
        record->displayDetails();           // runs Cinema::displayDetails()
        drawLine('-', LINE_WIDTH);
    }

    // DELETE CINEMA: the cinema must exist first; then its file is deleted and the list is updated
    void deleteCinema() {
        printTitle("DELETE CINEMA");
        string cinemaID;
        int index = -1;
        if (cinemaList.load() == 0) {
            cout << " No Cinemas Found.\n";
            return;
        }
        if (!cinemaList.askExisting('I', " Cinema code to delete (I-XXX, or 0 to cancel) : ",
                                    cinemaID, index)) return;

        cout << "\n Cinema found: " << cinemaID << " " << cinemaList.getText(index) << "\n";
        if (!askYesNo(" Are you sure you want to delete this cinema?")) {
            cout << " Deletion cancelled.\n";
            return;
        }

        Cinema cinema(cinemaID, cinemaList.getText(index));
        if (!cinema.deleteFile()) {
            cout << "  ! Warning: the cinema file could not be deleted.\n";
        }
        if (!cinemaList.removeAt(index)) {
            cout << "  ! Unable to update Cinema List.txt.\n";
            return;
        }
        cout << "\n Cinema " << cinemaID << " deleted successfully.\n";
    }

public:
    void runManager() {
        const string items[5] = {"Add cinema", "Show all cinemas", "Show cinema", "Delete cinema", "Back"};
        int choice = 0;
        do {
            choice = askMenu("CINEMA MENU", items, 5);
            switch (choice) {
                case 1: addCinema(); break;
                case 2: showAllCinemas(); break;
                case 3: showCinema(); break;
                case 4: deleteCinema(); break;
                default: cout << "\n Exiting Cinema Block.\n"; break;
            }
        } while (choice != 5);
    }
};

#endif
