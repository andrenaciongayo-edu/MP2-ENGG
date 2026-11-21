#include "group6_header.h"

int main()
{
    BookingSystem booking;
    CinemaSystem cinema;
    CustomerSystem customer;
    MovieSystem movie;

    int wrongCodes = 0; // wrong manager codes entered so far
    int roleChoice = 0; // 1 = customer, 2 = manager, 3 = exit
    bool isManager = false;
    bool userChosen = false;

    // ---------- START: ask if the user is a manager or a customer ----------
    do
    {
        printTitle("MAIN MENU");
        cout << "Welcome to the Movie Booking System!\n";
        cout << "Are you a:\n";
        cout << "   [1] Customer\n";
        cout << "   [2] Manager\n";
        cout << "   [3] Exit\n";
        drawLine('-', LINE_WIDTH);
        cout << "\n Choice: ";
        roleChoice = getIntInRange(1, 3);

        if (roleChoice == 1)
        {
            isManager = false;
            userChosen = true;
        }
        else if (roleChoice == 2)
        {
            // ---------- Manager: input the 3-digit code ----------
            do
            {
                printTitle("MANAGER LOGIN");
                cout << " Manager code : ";
                string code;
                getline(cin, code);

                if (trim(code) == MANAGER_CODE)
                {
                    isManager = true;
                    userChosen = true;
                }
                else
                {
                    wrongCodes++;
                    cout << "\n Access denied.\n";
                    if (wrongCodes >= MAX_MANAGER_LOGIN_ATTEMPTS)
                    {
                        cout << " Too many wrong codes. You are locked out of the program.\n";
                        return 1; // lock the user out
                    }
                    cout << " Attempts left: " << (MAX_MANAGER_LOGIN_ATTEMPTS - wrongCodes) << "\n";
                }
            } while (!userChosen);
        }
        // ---------- Manager menu ----------
        if (isManager)
        {
            int managerChoice = 0;
            do
            {
                printTitle("MANAGER OPTIONS");
                cout << "   [1] Customer Block\n";
                cout << "   [2] Movie Block\n";
                cout << "   [3] Cinema Block\n";
                cout << "   [4] Booking Block\n";
                cout << "   [5] End\n";
                drawLine('-', LINE_WIDTH);
                cout << " Choice: ";
                managerChoice = getIntInRange(1, 5);

                switch (managerChoice)
                {
                case 1:
                    customer.runManager();
                    break;
                case 2:
                    movie.runManager();
                    break;
                case 3:
                    cinema.runManager();
                    break;
                case 4:
                    booking.runManager();
                    break;
                default:

                    break;
                }
            } while (managerChoice != 5);
        }
        // ---------- Customer menu ----------
        else
        {
            int customerChoice = 0;
            do
            {
                printTitle("CUSTOMER OPTIONS");
                cout << "   [1] Customer Block\n";
                cout << "   [2] Booking Block\n";
                cout << "   [3] End\n";
                drawLine('-', LINE_WIDTH);
                cout << " Choice: ";
                customerChoice = getIntInRange(1, 3);

                switch (customerChoice)
                {
                case 1:
                    customer.runCustomer();
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
    } while (roleChoice != 3);

    cout << "\n Thank you. Goodbye!\n";

    return 0;
}