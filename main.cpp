#include "group6_header.h" 

int main() {
    BookingSystem booking;                  // the BOOKING block
    int wrongCodes = 0;                 // wrong manager codes entered so far
    bool isManager = false;
    bool userChosen = false;

    // ---------- START: ask if the user is a manager or a customer ----------
    do {
        printTitle("MAIN MENU");
        cout << "Welcome to the Movie Booking System!\n";
        cout << "Are you a:\n";
        cout << "   [1] Customer\n";
        cout << "   [2] Manager\n";
        cout << "\n Choice: ";
        int roleChoice = getIntInRange(1, 2);

        if (roleChoice == 1) {
            isManager = false;
            userChosen = true;
        } else {
            // ---------- Manager: input the 3-digit code ----------
            printTitle("MANAGER LOGIN");
            cout << " Manager code : ";
            string code;
            getline(cin, code);

            if (trim(code) == MANAGER_CODE) {
                isManager = true;
                userChosen = true;
            } else {
                wrongCodes++;
                cout << "\n Access denied.\n";
                if (wrongCodes >= MAX_MANAGER_LOGIN_ATTEMPTS) {
                    cout << " Too many wrong codes. You are locked out of the program.\n";
                    return 1;                       // lock the user out
                }
                cout << " Attempts left: " << (MAX_MANAGER_LOGIN_ATTEMPTS - wrongCodes) << "\n";
            }
        }
    } while (!userChosen);

    // ---------- Manager menu ----------
    if (isManager) {
        int managerChoice = 0;
        do {
            printTitle("MANAGER OPTIONS");
            cout << "   [1] Customer Block\n";
            cout << "   [2] Movie Block\n";
            cout << "   [3] Cinema Block\n";
            cout << "   [4] Booking Block\n";
            cout << "   [5] End\n";
            drawLine('-', LINE_WIDTH);
            cout << " Choice: ";
            managerChoice = getIntInRange(1, 5);

            switch (managerChoice) {
                case 1:
                    // INTEGRATION POINT: call the manager side of the CUSTOMER block here
                    cout << "\n The CUSTOMER block is not connected yet.\n";
                    break;
                case 2:
                    // INTEGRATION POINT: call the MOVIE block here
                    cout << "\n The MOVIE block is not connected yet.\n";
                    break;
                case 3:
                    // INTEGRATION POINT: call the CINEMA block here
                    cout << "\n The CINEMA block is not connected yet.\n";
                    break;
                case 4:
                    booking.runManager();
                    break;
                default:
                    cout << "\n Thank you. Goodbye!\n";
                    break;
            }
        } while (managerChoice != 5);
    }
    // ---------- Customer menu ----------
    else {
        int customerChoice = 0;
        do {
            printTitle("CUSTOMER OPTIONS");
            cout << "   [1] Customer Block\n";
            cout << "   [2] Booking Block\n";
            cout << "   [3] End\n";
            drawLine('-', LINE_WIDTH);
            cout << " Choice: ";
            customerChoice = getIntInRange(1, 3);

            switch (customerChoice) {
                case 1:
                    // INTEGRATION POINT: call the customer side of the CUSTOMER block here (ADD CUSTOMER)
                    cout << "\n The CUSTOMER block is not connected yet.\n";
                    break;
                case 2:
                    booking.runCustomer();
                    break;
                default:
                    cout << "\n Thank you. Goodbye!\n";
                    break;
            }
        } while (customerChoice != 3);
    }

    return 0;
}