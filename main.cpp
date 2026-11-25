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
    const string mainMenuItems[3] = {"Customer", "Manager", "Exit"};
    const string managerMenuItems[5] = {"Customer Block", "Movie Block", "Cinema Block", "Booking Block", "Back"};
    const string customerMenuItems[3] = {"Customer Block", "Booking Block", "Back"};
    // ---------- START: ask if the user is a manager or a customer ----------
    do
    {
        roleChoice = askMenu("WELCOME TO THE CINEMA BOOKING SYSTEM", mainMenuItems, 3);
        isManager = false;
        userChosen = false;

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
                readInputLine(code);

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
        else if(roleChoice == 3)
        {
            cout << "\n Thank you. Goodbye!\n";
            return 0;
        }
        // ---------- Manager menu ----------
        if (isManager)
        {
            int managerChoice = 0;
            do
            {
                managerChoice = askMenu("MANAGER OPTIONS", managerMenuItems, 5);

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
                customerChoice = askMenu("CUSTOMER OPTIONS", customerMenuItems, 3);

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
    
    return 0;
}