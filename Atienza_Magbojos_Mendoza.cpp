#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <algorithm> // for std::find in addOrder();
#include <fstream>
#include <set>

using namespace std;

bool isValidDateFormat(const string &date){
    if (date.length() != 10){
        return false;
    }

    if (date[4] != '-' || date[7] != '-'){
        return false;
    }

    for (int i = 0; i < date.length(); i++){
        if (i == 4 || i == 7){
            continue;
        }

        if (!isdigit(date[i])){
            return false;
        }
    }

    // Get the current date
    time_t now = time(0);
    struct tm tstruct;
    char currentDate[11];
    tstruct = *localtime(&now);
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", &tstruct); // Current date as string

    // Compare entered date with current date
    if (date < currentDate){
        return false; // The entered date is in the past
    }
    return true; // The date format is valid and the date is recent
}

bool isValidDate(const string &date){
    regex dateRegex("^\\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$");
    return regex_match(date, dateRegex);
}

bool isValidContactInput(const string &contactInput){
    return regex_match(contactInput, regex("^\\d{11}$")); // Allows 11 digits only
}

bool isValidCustomerName(const string &customerNameInput){
    return regex_match(customerNameInput, regex("^[a-zA-Z ]+$")); // Allows letters and spaces only
}

class BaseReservation{
public:
    virtual void displayCustomerDetails() const = 0; // Pure virtual function for polymorphism
    virtual void inputCustomerDetails() = 0;         // Pure virtual function for polymorphism
    virtual ~BaseReservation() {}
};

class Reservation : public BaseReservation{ // Inherit from BaseReservation
private:
    map<string, vector<bool>> bookings; // Key: Date (YYYY-MM-DD), Value: Slots (true=reserved, false=available)
    static const int totalSlots = 5;    // Number of slots per day (10 AM to 8 PM at 2-hour intervals)

public:
    Reservation() {}

    bool checkIfValidDate(const string &date){
        regex dateRegex("^\\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$");
        return regex_match(date, dateRegex);
    }

    void checkAvailability(const string &date){
        if (bookings.find(date) == bookings.end()){
            bookings[date] = vector<bool>(totalSlots, false); // Initialize all slots as available
        }

        cout << "Availability for " << date << ":\n";
        for (int i = 0; i < totalSlots; i++){
            cout << "Slot " << i + 1 << " (" << 10 + 2 * i << ":00 - " << 12 + 2 * i << ":00): "
                 << (bookings[date][i] ? "Reserved" : "Available") << "\n"; // Show slot status
        }
    }

    bool reserveSlot(const string &date, int slot){
        if (bookings.find(date) == bookings.end()){
            bookings[date] = vector<bool>(totalSlots, false); // Initialize slots if not already done
        }

        if (slot < 0 || slot >= totalSlots){ // validation for time slot
            cout << "Invalid slot number." << endl;
            return false;
        }

        if (bookings[date][slot]){
            cout << "Slot already reserved." << endl;
            return false;
        }

        bookings[date][slot] = true; // Mark slot as reserved
        cout << endl << "Reservation successful for Slot " << slot + 1 << " on " << date << "." << endl;
        return true;
    }
    void inputCustomerDetails() override {}         // No input for Reservation, hence not needed here
    void displayCustomerDetails() const override {} // No details to display for Reservation
};

class Menu : public BaseReservation{ // Inherit from BaseReservation
protected:
    string category;
    vector<int> menuID;
    vector<string> menuName;
    vector<float> menuPrice;

    void loadMenuData(){
        if (category == "PAMAWING-GUTOM"){
            menuID = {0, 1, 2, 3, 4};
            menuName = {"Calamares", "Chicharon Bulaklak", "Dynamite", "Lumpiang Shanghai", "Lumpiang Sariwa"};
            menuPrice = {150, 150, 100, 180, 180};
        } else if (category == "PANGUNAHING PAGKAIN"){
            menuID = {5, 6, 7, 8, 9};
            menuName = {"Adobo", "Sinigang", "Dinuguan", "Kare Kare", "Pinakbet"};
            menuPrice = {450, 450, 400, 500, 400};
        } else if (category == "PANGHIMAGAS"){
            menuID = {10, 11, 12, 13, 14};
            menuName = {"Sorbetes", "Halo Halo", "Leche Flan", "Buko Pandan", "Mais Con Yelo"};
            menuPrice = {100, 150, 150, 100, 100};
        } else if (category == "PANULAK"){
            menuID = {15, 16, 17, 18, 19};
            menuName = {"Sago't Gulaman", "Mango Shake", "Buko Juice", "Calamansi Juice", "Pineapple Juice"};
            menuPrice = {100, 120, 80, 80, 80};
        }
    }

public:
    Menu() : category("") {}

    Menu(const string &category) : category(category){
        loadMenuData(); // Load menu data for the given category
    }

    const vector<int> &getMenuIDs() const{
        return menuID;
    }

    void displayMenu() const{
        int padding = (43 - category.length()) / 2;
        cout << string(padding, ' ') << category << endl;
        cout << string(43, '-') << endl;
        cout << setw(10) << left << "ID"
             << setw(25) << left << "Name"
             << setw(10) << left << "Price" << endl;
        cout << string(43, '-') << endl;

        for (size_t i = 0; i < menuID.size(); i++){
            cout << setw(10) << left << menuID[i]
                 << setw(25) << left << menuName[i]
                 << setw(10) << left << fixed << setprecision(2)
                 << menuPrice[i] << endl;
        }
        cout << string(43, '-') << endl;
    }
    void inputCustomerDetails() override {}         // No input for Menu, hence not needed here
    void displayCustomerDetails() const override {} // No details to display for Menu
};

class Customer : public BaseReservation{ // Inherit from BaseReservation
private:
    string customerName;
    string contactNumber;
    string customerEmail;
    string customerID;

    static set<string> customerIDs;

public:
    Customer() {}

    Customer(string name, string contact, string email, string id){
        customerName = name;
        contactNumber = contact;
        customerEmail = email;
        customerID = id; 
    }

    string getCustomerName() const { return customerName; }
    string getCustomerID() const { return customerID; }

    void inputCustomerDetails() override {
        cin.ignore();
        bool isCustomerNameValid = false;

        do {
            system("cls");
            cout << "INPUT CUSTOMER DETAILS" << endl << endl;
            cout << "Enter your name: ";
            getline(cin, customerName);

            if (isValidCustomerName(customerName)) {
                isCustomerNameValid = true;
            } else {
                cout << "Invalid name. It should only contain letters and spaces." << endl;
                system("pause");
            }
        } while (!isCustomerNameValid);

        bool isCustomerIDValid = false;
        do {
            system("cls");
            cout << "INPUT CUSTOMER DETAILS" << endl << endl;
            cout << "Name: " << customerName << endl << endl;
            cout << "Enter your ID: ";
            getline(cin, customerID);

            if (customerIDs.find(customerID) != customerIDs.end()) {
                cout << "Customer ID already exists. Please enter a unique ID." << endl;
                system("pause");
            } else {
                isCustomerIDValid = true;
                customerIDs.insert(customerID);
            }
        } while (!isCustomerIDValid);

            bool isContactNumberValid = false;
            do {
                system("cls");
                cout << "INPUT CUSTOMER DETAILS" << endl << endl;
                cout << "Name: " << customerName << endl;
                cout << "ID: " << customerID << endl << endl;
                cout << "Enter your contact number: ";
                getline(cin, contactNumber);

                if (isValidContactInput(contactNumber)) {
                    isContactNumberValid = true;
                } else {
                    cout << "Invalid contact number. Please try again." << endl;
                    system("pause");
                }
            } while (!isContactNumberValid);

            system("cls");
            cout << "INPUT CUSTOMER DETAILS" << endl << endl;
            cout << "Name: " << customerName << endl;
            cout << "ID: " << customerID << endl;
            cout << "Contact Number: " << contactNumber << endl << endl;
            cout << "Enter your email: ";
            getline(cin, customerEmail);

            system("cls");
            cout << "INPUT CUSTOMER DETAILS" << endl << endl;
            cout << "Name: " << customerName << endl;
            cout << "ID: " << customerID << endl;
            cout << "Contact Number: " << contactNumber << endl;
            cout << "Email: " << customerEmail << endl << endl;
            system("pause");
    }

    void displayCustomerDetails() const override{ // Display customer details
        system("cls");
        cout << "Customer Details:" << endl;
        cout << "Name: " << customerName << endl;
        cout << "Contact Number: " << contactNumber << endl;
        cout << "Email: " << customerEmail << endl;
        cout << "Customer ID: " << customerID << endl;
    }

    void saveToFile() const{ // Save customer details to a file
        ofstream outFile("customerss.txt", ios::app); // Open in append mode
        if (outFile.is_open()){
            outFile << "Customer Details:" << endl;
            outFile << "Name: " << customerName << endl;
            outFile << "Contact Number: " << contactNumber << endl;
            outFile << "Email: " << customerEmail << endl;
            outFile << "Customer ID: " << customerID << endl;
            outFile << "-------------------------" << endl;
            outFile.close(); // Close file after writing
            cout << endl << "Customer details saved to file successfully." << endl;
            system("pause");
        } else{
            cout << endl << "Error: Unable to open file for writing." << endl;
            system("pause");
        }
    }
};

set<string> Customer::customerIDs;

class TableArea : public BaseReservation{ // Inherit from BaseReservation
private:
    int tableId, numberOfSeats;
    bool isAvailable;

public:
    TableArea(int tableId, int numberOfSeats, bool isAvailable) : tableId(tableId), numberOfSeats(numberOfSeats), isAvailable(isAvailable) {}

    void viewAvailableAreas(){
        cout << "Available Tables:" << endl;
        cout << "Table 1: Good for 2 people" << endl;
        cout << "Table 2: Good for 2 people" << endl;
        cout << "Table 3: Good for 4 people" << endl;
        cout << "Table 4: Good for 4 people" << endl;
        cout << "Table 5: Good for 6 people" << endl;
        cout << "Table 6: Good for 6 people" << endl;
        cout << "Table 7: Good for 8 people" << endl;
        cout << "Table 8: Good for 8 people" << endl;
        cout << "Table 9: Good for 10 people" << endl;
        cout << "Table 10: Good for 10 people" << endl << endl;
    }

    void reserveTable(){
        cout << "RESERVE TABLE AREA" << endl;

        bool isReserved = false;
        while (!isReserved){
            system("cls");
            viewAvailableAreas(); // display tables

            cout << "Enter table number (1-10): ";
            int reservedTable;
            cin >> reservedTable;

            if (reservedTable < 1 || reservedTable > 10){ // validation for table
                cout << "Invalid table number. Try again." << endl;
                reservedTable = -1;
                system("pause");
            } else{
                isReserved = true;
                cout << "You have successfully reserved Table " << reservedTable << endl;
            }
        }
    }
    void inputCustomerDetails() override {}         // No input for TableArea, hence not needed here
    void displayCustomerDetails() const override {} // No details to display for TableArea
};

class ReservationSystem{
private:
    static ReservationSystem *instance; // Static instance of the class
    Customer customer;                  // To store customer details
    Reservation reservation;            // To handle reservation slots
    Menu menu;
    TableArea tableArea;               // To handle table information
    vector<pair<int, int>> menuOrders; // Stores menu item ID and quantity
    vector<int> orders;                // Store ordered item IDs
    vector<Customer> customers;

    string reservationDate;
    int reservationSlot = -1; // Initially, no slot selected
    int reservedTable = -1;   // Initially, no table selected
    bool reservationWithMenu = false;

    ReservationSystem() : tableArea(0, 0, true) {} // Private constructor to prevent instantiation

public:
    // Delete copy constructor and assignment operator to prevent copies
    ReservationSystem(const ReservationSystem &) = delete;
    ReservationSystem &operator=(const ReservationSystem &) = delete;

    // Static method to get the instance of ReservationSystem
    static ReservationSystem *getInstance(){
        if (instance == nullptr){
            instance = new ReservationSystem();
        }
        return instance;
    }

    bool searchReservationByID(const string &id){
        // First, check in memory
        for (const auto &customer : customers){
            if (customer.getCustomerID() == id){
                customer.displayCustomerDetails(); // display customer details
                return true;
            }
        }

        // Then, search in the file
        ifstream file("customerss.txt"); // Correct file name

        if (file.is_open()){
            string line;
            bool found = false;

            while (getline(file, line)){
                // Parse the line to extract the CustomerID
                size_t pos = line.find("CustomerID: ");
                if (pos != string::npos){
                    size_t endPos = line.find(",", pos); // Find the end of the ID field
                    string extractedID = line.substr(pos + 12, endPos - (pos + 12));

                    // Compare extracted ID with the searched ID
                    if (extractedID == id){
                        found = true;
                        cout << "Found in file: " << endl << line << endl;
                        break; // Exit loop once the customer is found
                    }
                }
            }
            file.close();
            if (found){
                return true;
            }
        } else{
            cout << "Error: Unable to open the file for searching." << endl;
        }
        cout << "Reservation ID " << id << " not found." << endl << endl;
        return false;
    }

    // Static method to get the single instance
    void displayAllCustomers(){
        if (customers.empty()){
            cout << "No customers have made a reservation yet." << endl;
        } else{
            for (size_t i = 0; i < customers.size(); ++i){
                cout << "Customer " << (i + 1) << " details:" << endl;
                customers[i].displayCustomerDetails(); // Call Customer's displayCustomerDetails
            }
        }
    }

    void displaySummary(){
        cout << "\n--- Reservation Summary ---\n";
        customer.displayCustomerDetails();

        if (reservationSlot != -1){
            cout << "Date: " << reservationDate << endl;
            cout << "Time Slot: " << reservationSlot << " (" << 10 + 2 * (reservationSlot - 1)
                 << ":00 - " << 12 + 2 * (reservationSlot - 1) << ":00)" << endl;
        }

        if (reservedTable != -1){
            cout << "Reserved Table: " << reservedTable << endl;
        }

        if (reservationWithMenu && !menuOrders.empty()){
            cout << endl << "Menu Order: " << endl;
            cout << setw(10) << left << "ID"
                 << setw(25) << left << "Name"
                 << setw(10) << left << "Quantity" << endl;

            for (const auto &item : menuOrders){
                cout << setw(10) << left << item.first
                     << setw(25) << left << "Item " + to_string(item.first)
                     << setw(10) << left << item.second << endl;
            }
        }
        cout << string(30, '-') << endl;
    }

    void makeReservation(){
        orders.clear(); // Clear orders for the new reservation

        Customer newCustomer;               // Create a new Customer object
        newCustomer.inputCustomerDetails(); // Input details for the new customer
        newCustomer.saveToFile();
        customers.push_back(newCustomer); // Store the new customer in the customers vector

        system("cls");

        // Handle reservation choice (Reservation only or Reservation + Food Order)
        int reservationChoice;
        bool validChoice = false;

        while (!validChoice){
            system("cls");
            cout << "RESERVATION MENU" << endl;
            cout << "1. Reservation Only" << endl;
            cout << "2. Reservation + Food Order" << endl;
            cout << endl << "Enter choice: ";
            cin >> reservationChoice;

            if (reservationChoice == 1 || reservationChoice == 2){
                validChoice = true;
            } else{
                cout << "Invalid input. Please enter 1 or 2 only." << endl;
                system("pause");
            }
        }

        // Reserve date
        string date;
        bool isValidDate = false;

        while (!isValidDate){
            system("cls");
            cout << "CHOOSE DATE" << endl << endl;
            cout << "Enter reservation date (YYYY-MM-DD): ";
            cin >> date;

            // Validate the date format and check if it's available
            if (isValidDateFormat(date)){
                if (reservation.checkIfValidDate(date)){                       // Check if date is available
                    isValidDate = true; // Exit loop if valid

                    system("cls");
                    cout << "CHOOSE TABLE" << endl << endl;
                    tableArea.reserveTable(); // call function to reserve table

                    // Reserve slot
                    int slot;
                    bool validSlot = false;

                    while (!validSlot){
                        system("cls");
                        cout << "CHOOSE TIME" << endl << endl;
                        reservation.checkAvailability(date); // Check if date is available or not

                        cout << endl << "Enter slot number (1-5): ";
                        cin >> slot;

                        if (slot < 1 || slot > 5){ // validation for time slot
                            cout << "Invalid input. Please enter a number between 1 to 5 only." << endl << endl;
                            system("pause");
                        } else if (!reservation.reserveSlot(date, slot - 1)){
                            cout << "Unable to reserve slot. Please try again." << endl << endl;
                            system("pause");
                            return;
                        } else{
                            validSlot = true;
                            system("pause");
                        }
                    }
                } else{
                    cout << "The entered date is not available for reservation. Try another date." << endl << endl;
                    system("pause");
                }
            } else{
                cout << "Invalid date format or the date is in the past. Please follow the format (YYYY-MM-DD)." << endl << endl;
                system("pause");
            }
        }

        if (reservationChoice == 2){ // display all menu at once
            system("cls");
            cout << "RESTAURANT MENU" << endl << endl;

            Menu appetizer("PAMAWING-GUTOM");
            appetizer.displayMenu();
            cout << endl;

            Menu mainCourse("PANGUNAHING PAGKAIN");
            mainCourse.displayMenu();
            cout << endl;

            Menu dessert("PANGHIMAGAS");
            dessert.displayMenu();
            cout << endl;

            Menu beverage("PANULAK");
            beverage.displayMenu();
            cout << endl;

            ReservationSystem::getInstance()->menuOrder(); // allow customer to order from menu
        }
    }

    void menuOrder(){ // function to allow customer to order from menu
        char continueOrdering = 'Y';
        reservationWithMenu = false;

        while (continueOrdering == 'Y'){
            cout << "What would you like to order?" << endl;
            cout << "Enter Menu ID: ";
            int orderItemID;
            cin >> orderItemID;

            if (cin.fail()){
                cin.clear();                                         // Clear the error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
                cout << "Invalid input. Please enter a valid Menu ID." << endl;
                continue;
            }

            const vector<int> &menuIDs = menu.getMenuIDs();

            if (orderItemID >= 0 && orderItemID < 20 && std::find(menuIDs.begin(), menuIDs.end(), orderItemID) == menuIDs.end()){
                orders.push_back(orderItemID); // Add the item to the orders list
                reservationWithMenu = true;
                cout << "Order added successfully!" << endl;
            } else{
                cout << "Invalid Menu ID. Try again." << endl;
            }

            cout << endl << "Would you like to order another item? (Y/N): ";
            cin >> continueOrdering;
            continueOrdering = toupper(continueOrdering);
            cout << endl;

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cout << "Your Order Summary: " << endl;
        for (int id : orders){
            cout << "- Item ID: " << id << endl;
        }
        cout << endl;
        system("pause");
    }

    void viewMenu() {
    bool continueViewing = true;

    while (continueViewing) {
        string category;
        system("cls");

        cout << "VIEW MENU" << endl;
        cout << endl << "Select a menu category:" << endl;
        cout << "1. PAMAWING-GUTOM" << endl;
        cout << "2. PANGUNAHING PAGKAIN" << endl;
        cout << "3. PANGHIMAGAS" << endl;
        cout << "4. PANULAK" << endl;
        cout << endl << "Enter your choice: ";

        int menuChoice;
        cin >> menuChoice;

        switch (menuChoice) {
        case 1:
            category = "PAMAWING-GUTOM";
            break;
        case 2:
            category = "PANGUNAHING PAGKAIN";
            break;
        case 3:
            category = "PANGHIMAGAS";
            break;
        case 4:
            category = "PANULAK";
            break;
        default:
            cout << "Invalid choice. Try again." << endl << endl;
            system("pause");
            continue; // Restart the loop for valid input
        }

        Menu categoryMenu(category);
        system("cls");
        cout << "MENU: " << category << endl;
        cout << "--------------------------------" << endl;
        categoryMenu.displayMenu();
        cout << endl;

        bool validInput = false;

        while (!validInput) {
            cout << "Would you like to view other menu categories? (Y/N): ";
            char viewMenuChoice;
            cin >> viewMenuChoice;
            viewMenuChoice = toupper(viewMenuChoice);

            if (viewMenuChoice == 'Y') {
                validInput = true; // Exit validation loop
            } else if (viewMenuChoice == 'N') {
                validInput = true;       // Exit validation loop
                continueViewing = false; // Exit outer loop
            } else {
                cout << "Invalid input. Please enter 'Y' or 'N' only." << endl << endl;
                system("pause");
                system("cls");
                cout << "MENU: " << category << endl;
                cout << "--------------------------------" << endl;
                categoryMenu.displayMenu();
                cout << endl;
            }
        }
    }
}
    bool isPaid = false;

    void updateReservation(){
        system("cls");
        cout << "UPDATE RESERVATION" << endl;
        cout << "1. Change date and time" << endl;
        cout << "2. Change table" << endl;
        cout << "3. Add to order" << endl;
        cout << "4. Proceed to payment" << endl;
        cout << "5. Cancel reservation" << endl; // Correct numbering
        cout << "\nEnter choice: ";
        int updateChoice;
        cin >> updateChoice;

        switch (updateChoice){
        case 1:
            system("cls");
            cout << "CHANGE DATE AND TIME" << endl << endl;
            cout << "Enter new reservation date (YYYY-MM-DD): ";
            cin >> reservationDate;

            cout << "Enter new slot number (1-5): ";
            cin >> reservationSlot;


            if (!reservation.reserveSlot(reservationDate, reservationSlot - 1)){
                cout << "Unable to reserve the new slot.\n";
            }
            break;

        case 2:
            system("cls");
            cout << "CHANGE TABLE" << endl << endl;
            tableArea.reserveTable();
            break;

        case 3:
            system("cls");
            cout << "CHANGE ORDER" << endl << endl;
            if (reservationWithMenu){
                cout << "RESTAURANT MENU" << endl << endl;
                Menu appetizer("PAMAWING-GUTOM");
                appetizer.displayMenu();
                cout << endl;

                Menu mainCourse("PANGUNAHING PAGKAIN");
                mainCourse.displayMenu();
                cout << endl;

                Menu dessert("PANGHIMAGAS");
                dessert.displayMenu();
                cout << endl;

                Menu beverage("PANULAK");
                beverage.displayMenu();
                cout << endl;

                menuOrder();
            }else{
                cout << "Menu was not selected initially." << endl;
            }
            break;

        case 4:
            system("cls");
            cout << "PROCEED TO PAYMENT" << endl << endl;

            // Check if the payment has already been made
            if (isPaid){
                cout << "Payment is already completed. Returning to the main menu.\n";
                break;
            }

            int paymentChoice;
            cout << "Select payment method:\n";
            cout << "1. Credit Card\n";
            cout << "2. Online Payment\n";
            cout << endl << "Enter choice: ";
            cin >> paymentChoice;

            if (paymentChoice == 1 || paymentChoice == 2){
                float amount = reservationWithMenu ? 1500.0 : 500.0;
                cout << endl << "Amount due: P" << amount << endl;

                if (paymentChoice == 1){
                    string cardNumber;
                    cout << "Enter credit card number: ";
                    cin.ignore();
                    getline(cin, cardNumber);

                    if (cardNumber.length() >= 13 && cardNumber.length() <= 19){
                        cout << "Payment successful using Credit Card!" << endl;
                        isPaid = true; // Mark payment as completed
                    } else{
                        cout << "Invalid credit card number. Payment failed." << endl;
                    }
                } else if (paymentChoice == 2){
                    string onlinePaymentID;
                    cout << "Enter Online Payment Transaction ID: ";
                    cin.ignore();
                    getline(cin, onlinePaymentID);

                    if (!onlinePaymentID.empty()){
                        cout << "Payment successful using Online Payment!" << endl;
                        isPaid = true; // Mark payment as completed
                    } else{
                        cout << "Invalid Transaction ID. Payment failed." << endl;
                    }
                }
            } else{
                cout << "Invalid payment method selected." << endl;
            }
            break;

        case 5:
            system("cls");
            cout << "CANCEL RESERVATION" << endl << endl;
            reservationDate.clear();
            reservationSlot = -1;
            reservedTable = -1;
            menuOrders.clear();
            reservationWithMenu = false;
            isPaid = false; // Reset payment status
            cout << "Your reservation has been cancelled." << endl;
            break;

        default:
            cout << "Invalid choice." << endl;
            break;
        }
    }

    void viewReservationSummary(){
        customer.displayCustomerDetails();
        displaySummary();
        updateReservation();
    }
};

ReservationSystem *ReservationSystem::instance = nullptr; // Initialize static member

int main(){
    Reservation reservation; // Non-singleton
    Customer customer;       // Non-singleton
    Menu menu;
    TableArea tableArea(0, 0, true); // Non-singleton

    ReservationSystem *reservationSystem = ReservationSystem::getInstance(); // Access the singleton instance of ReservationSystem

    bool systemRunning = true; // Flag to keep the system running

    while (systemRunning){
        system("cls"); // Main Menu
        cout << "Welcome to Sinaing Society Reservation System!" << endl << endl;
        cout << "Please choose an option:" << endl;
        cout << "1. Check Available Dates" << endl;
        cout << "2. View Available Table Areas" << endl;
        cout << "3. View Menu" << endl;
        cout << "4. Make a Reservation" << endl;
        cout << "5. View Reservation" << endl;
        cout << "6. Exit" << endl;
        cout << endl << "Enter your choice: ";

        int menuChoice;
        cin >> menuChoice;
        system("cls");

        switch (menuChoice){
        case 1:{ // Check Available Dates
            string date;
            bool isValidDate = false;

            while (!isValidDate){
                system("cls");
                cout << "CHECK AVAILABLE DATES" << endl << endl;
                cout << "Enter reservation date (YYYY-MM-DD): ";
                cin >> date;

                // Call isValidDateFormat to validate the date format and check if it's recent
                if (isValidDateFormat(date)){
                    if (reservation.checkIfValidDate(date)){ // Additional logic to check availability
                        isValidDate = true; // Exit loop if valid
                        cout << date << " is available for reservation!" << endl << endl;
                        system("pause");
                    } else{
                        cout << "The entered date is not available for reservation. Try another date." << endl << endl;
                        system("pause");
                    }
                } else{
                    cout << "Invalid date format or the date is in the past. Please follow the format (YYYY-MM-DD)." << endl << endl;
                    system("pause");
                }
            }
            break; // exit case 1
        }

        case 2: // View Available Table Areas
            cout << "VIEW AVAILABLE TABLE AREAS" << endl << endl;
            tableArea.viewAvailableAreas(); // display all tables
            system("pause");
            break; // exit case 2

        case 3:{ // View Menu
            reservationSystem->viewMenu(); // display menu per category
            break;                         // exit case 3
        }

        case 4:{ // Make a Reservation
            cout << "MAKE A RESERVATION" << endl << endl;
            reservationSystem->makeReservation(); // Use the singleton instance
            break;                                // exit case 4
        }

        case 5: { // View Reservation
            cout << "VIEW RESERVATION" << endl << endl;

            string reservationID;
            bool validInput = false;

            while (!validInput) {
                cout << "Enter Reservation ID: ";
                cin.ignore();                // Clear input buffer
                getline(cin, reservationID); // Read entire line

                if (reservationID.empty()) {
                    cout << "Reservation ID cannot be empty. Please enter a valid Reservation ID." << endl <<  endl;
                } else {
                    validInput = true;
                }
            }

            if (reservationSystem->searchReservationByID(reservationID)) {
              
                cout << endl << "Would you like to update your reservation?" << endl;
                cout << "1. Yes" << endl;
                cout << "2. No" << endl;

                cout << endl << "Enter your choice: ";
                int updateChoice;
                cin >> updateChoice;

                if (updateChoice == 1) {
                    reservationSystem->updateReservation(); // Display update reservation menu/choices
                } else if (updateChoice == 2) {
                    cout << "Returning to main menu..." << endl;
                } else {
                    cout << "Invalid input. Returning to main menu..." << endl;
                }
            } else {
                cout << "Returning to main menu..." << endl;
            }

            system("pause");
            break; // exit case 5
        }

        case 6: // Exit
            cout << endl << "Thank you for using Sinaing Society Reservation System. Goodbye!" << endl;
            exit(0);

        default:
            cout << "Invalid choice. Please select a valid option." << endl;
            break;
        }
    }
    return 0;
}