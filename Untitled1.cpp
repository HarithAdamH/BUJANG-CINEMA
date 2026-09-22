/*******************************************************************
 * Universiti Teknologi MARA (UiTM)
 * Project Title: Cinema Ticket Booking System (Full Working Code)
 * Elements: Structs, Arrays, Functions, String Parsing, File I/O
 *******************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

// ==========================================
// STRUCT DEFINITIONS
// ==========================================
struct Movie {
    string title;
    double price;      
    string showTime;   
};

struct Branch {
    string name;
    Movie movies[10]; 
    int movieCount;   
};

struct Booking {
    string branchName;
    string movieTitle;
    double moviePrice; 
    string roomType;
    double roomPrice;
    string showTime;
    string seatNumber; 
    int totalSeats;    
    int adultQty;
    int childQty;
    string addOnName;
    double addOnPrice;
    double totalPrice;
    string paymentMethod;
};

// ==========================================
// FUNCTION DECLARATIONS (PROTOTYPES)
// ==========================================
void initializeData(Branch branches[]); 
void saveData(Branch branches[]);       
void displayMainPage();
void handleExit();
int askProceedChangeCancel();

// Admin Functions (CRUD)
void adminMenu(Branch branches[]);
void insertMovie(Branch &branch, Branch branches[]);
void updateMovie(Branch &branch, Branch branches[]);
void deleteMovie(Branch &branch, Branch branches[]);

// Customer Functions
void selectBranch(Branch branches[], Booking &book);
void selectMovie(Branch branches[], int branchIdx, Booking &book);
void selectRoom(Booking &book);
void selectSeat(Booking &book); 
void selectAddOnsAndPayment(Booking &book); 
void calculateTotal(Booking &book);
void displayAndSaveReceipt(Booking book);

// ==========================================
// MAIN FUNCTION
// ==========================================
int main() {
    Branch branches[5];
    Booking currentBooking;
    int mainChoice;
    
    initializeData(branches);
    displayMainPage();
    
    do {
        system("CLS");
        cout << "==================================================\n";
        cout << "                MAIN MENU SYSTEM                  \n";
        cout << "==================================================\n";
        cout << "1. Admin Mode (Insert / Update / Delete Movie Details)\n";
        cout << "2. Customer Mode (Book Ticket)\n";
        cout << "3. Exit Program\n";
        cout << "Enter your choice: ";
        cin >> mainChoice;
        
        if (mainChoice == 1) {
            adminMenu(branches);
        } else if (mainChoice == 2) {
            selectBranch(branches, currentBooking);
            selectAddOnsAndPayment(currentBooking);
            calculateTotal(currentBooking);
            displayAndSaveReceipt(currentBooking);
            cout << "\nPress Enter to return to Main Menu...";
            cin.ignore();
            cin.get();
        } else if (mainChoice == 3) {
            handleExit();
        }
    } while (mainChoice != 3);
    
    return 0;
}

// ===================================================
// FILE INPUT FUNCTION (Membaca Data Dari Fail Teks)
// ===================================================
void initializeData(Branch branches[]) {
    ifstream branchFile("branches.txt");
    
    if (!branchFile.is_open()) {
        cout << "CRITICAL ERROR: 'branches.txt' not found!\n";
        cout << "Sila pastikan fail 'branches.txt' berada dalam folder yang sama dengan program.\n";
        system("PAUSE");
        exit(1);
    }

    for(int i = 0; i < 5; i++) {
        if (!getline(branchFile, branches[i].name)) break;
        branches[i].movieCount = 0; 

        string movieFileName = "movies_" + to_string(i + 1) + ".txt";
        ifstream movieFile(movieFileName);

        if (movieFile.is_open()) {
            string line;
            while (branches[i].movieCount < 10 && getline(movieFile, line)) {
                if (line.empty()) continue;

                size_t firstPipe = line.find('|');
                size_t secondPipe = line.find('|', firstPipe + 1);

                if (firstPipe != string::npos && secondPipe != string::npos) {
                    branches[i].movies[branches[i].movieCount].title = line.substr(0, firstPipe);
                    
                    string priceStr = line.substr(firstPipe + 1, secondPipe - firstPipe - 1);
                    branches[i].movies[branches[i].movieCount].price = stod(priceStr);
                    
                    branches[i].movies[branches[i].movieCount].showTime = line.substr(secondPipe + 1);
                    
                    branches[i].movieCount++;
                }
            }
            movieFile.close();
        } else {
            cout << "Warning: File '" << movieFileName << "' not found. Mulakan dengan 0 filem.\n";
        }
    }
    branchFile.close();
}

// ===================================================
// FILE OUTPUT FUNCTION (Menulis & Simpan Perubahan Data)
// ===================================================
void saveData(Branch branches[]) {
    ofstream branchFile("branches.txt");
    if (!branchFile.is_open()) return;

    for (int i = 0; i < 5; i++) {
        branchFile << branches[i].name << "\n";

        string movieFileName = "movies_" + to_string(i + 1) + ".txt";
        ofstream movieFile(movieFileName);
        
        if (movieFile.is_open()) {
            for (int j = 0; j < branches[i].movieCount; j++) {
                movieFile << branches[i].movies[j].title << "|"
                          << fixed << setprecision(2) << branches[i].movies[j].price << "|"
                          << branches[i].movies[j].showTime << "\n";
            }
            movieFile.close();
        }
    }
    branchFile.close();
}

void displayMainPage() {
    cout << "==================================================\n";
    cout << "        WELCOME TO GSC TICKET BOOKING SYSTEM      \n";
    cout << "==================================================\n";
    cout << "Press Enter to start system...";
    cin.get();
}

void handleExit() {
    cout << "\nTHANK YOU, PLEASE COME AGAIN\n";
    exit(0);
}

int askProceedChangeCancel() {
    int choice;
    cout << "\nDo you want to proceed?\n";
    cout << "1. Proceed\n";
    cout << "2. Change Selection\n";
    cout << "3. Cancel Booking\n";
    cout << "Enter choice (1-3): ";
    cin >> choice;
    
    if (choice == 3) {
        handleExit();
    }
    return choice; 
}

// ==========================================
// CRUD - ADMIN FUNCTIONS
// ==========================================
void adminMenu(Branch branches[]) {
    int adminChoice, branchIdx;
    
    system("CLS");
    cout << "--- ADMIN MAIN MENU ---\n";
    for (int i = 0; i < 5; i++) {
        cout << i + 1 << ". " << branches[i].name << endl;
    }
    cout << "Select which branch to manage (1-5): ";
    cin >> branchIdx;
    branchIdx--;
    
    do {
        system("CLS");
        cout << "--- MANAGE MOVIES FOR: " << branches[branchIdx].name << " ---\n";
        cout << "1. [INSERT] Add New Movie\n";
        cout << "2. [UPDATE] Edit Movie Details (Name, Price, Showtime)\n";
        cout << "3. [DELETE] Remove Movie\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter choice (1-4): ";
        cin >> adminChoice;
        
        switch(adminChoice) {
            case 1: insertMovie(branches[branchIdx], branches); break;
            case 2: updateMovie(branches[branchIdx], branches); break;
            case 3: deleteMovie(branches[branchIdx], branches); break;
        }
    } while (adminChoice != 4);
}

void insertMovie(Branch &branch, Branch branches[]) {
    system("CLS");
    cout << "--- [INSERT MODE] ADD NEW MOVIE ---\n";
    if (branch.movieCount >= 10) {
        cout << "Error: Movie list is FULL! (Max 10 movies)\n";
    } else {
        cin.ignore();
        cout << "Enter New Movie Title: ";
        getline(cin, branch.movies[branch.movieCount].title);
        cout << "Enter Base Ticket Price (RM): ";
        cin >> branch.movies[branch.movieCount].price;
        cin.ignore();
        cout << "Enter Show Time (e.g., 8:30 PM): ";
        getline(cin, branch.movies[branch.movieCount].showTime);
        
        branch.movieCount++;
        saveData(branches); 
        cout << "\nSuccess: New movie added and written to database file!\n";
    }
    cout << "\nPress Enter to continue...";
    cin.get();
}

void updateMovie(Branch &branch, Branch branches[]) {
    system("CLS");
    cout << "--- [UPDATE MODE] EDIT MOVIE DETAILS ---\n";
    for (int i = 0; i < branch.movieCount; i++) {
        cout << i + 1 << ". " << left << setw(35) << branch.movies[i].title 
             << " | RM " << fixed << setprecision(2) << setw(6) << branch.movies[i].price 
             << " | " << branch.movies[i].showTime << endl;
    }
    
    int select;
    cout << "\nSelect movie number to update: ";
    cin >> select;
    
    if (select < 1 || select > branch.movieCount) {
        cout << "Invalid Selection!\n";
    } else {
        int idx = select - 1;
        int updateOption;
        
        cout << "\nWhat detail do you want to change?\n";
        cout << "1. Movie Name / Title\n";
        cout << "2. Ticket Price\n";
        cout << "3. Show Time\n";
        cout << "4. Change All Details\n";
        cout << "Enter choice (1-4): ";
        cin >> updateOption;
        cin.ignore();
        
        if (updateOption == 1 || updateOption == 4) {
            cout << "Enter New Title: ";
            getline(cin, branch.movies[idx].title);
        }
        if (updateOption == 2 || updateOption == 4) {
            cout << "Enter New Price (RM): ";
            cin >> branch.movies[idx].price;
            cin.ignore();
        }
        if (updateOption == 3 || updateOption == 4) {
            cout << "Enter New Show Time: ";
            getline(cin, branch.movies[idx].showTime);
        }
        
        saveData(branches); 
        cout << "\nSuccess: Movie details updated and saved to file!\n";
    }
    cout << "\nPress Enter to continue...";
    cin.get();
}

void deleteMovie(Branch &branch, Branch branches[]) {
    system("CLS");
    cout << "--- [DELETE MODE] REMOVE MOVIE ---\n";
    for (int i = 0; i < branch.movieCount; i++) {
        cout << i + 1 << ". " << branch.movies[i].title << endl;
    }
    
    int select;
    cout << "\nSelect movie number to delete: ";
    cin >> select;
    
    if (select < 1 || select > branch.movieCount) {
        cout << "Invalid Selection!\n";
    } else {
        int indexToDelete = select - 1;
        for (int i = indexToDelete; i < branch.movieCount - 1; i++) {
            branch.movies[i] = branch.movies[i + 1];
        }
        branch.movieCount--;
        saveData(branches); 
        cout << "Success: Movie removed and database updated!\n";
    }
    cout << "\nPress Enter to continue...";
    cin.get();
}

// ==========================================
// CUSTOMER FUNCTIONS
// ==========================================
void selectBranch(Branch branches[], Booking &book) {
    int branchChoice, action;
    do {
        system("CLS");
        cout << "--- SELECT BRANCH ---\n";
        for (int i = 0; i < 5; i++) {
            cout << i + 1 << ". " << branches[i].name << endl;
        }
        cout << "Select cawangan (1-5): ";
        cin >> branchChoice;
        
        while (branchChoice < 1 || branchChoice > 5) {
            cout << "Invalid choice! Please select 1-5: ";
            cin >> branchChoice;
        }
        book.branchName = branches[branchChoice - 1].name;
        cout << "\nYou have selected: " << book.branchName << endl;
        action = askProceedChangeCancel();
    } while (action == 2);
    
    selectMovie(branches, branchChoice - 1, book);
}

void selectMovie(Branch branches[], int branchIdx, Booking &book) {
    int movieChoice, action;
    do {
        system("CLS");
        cout << "--- SELECT MOVIE AVAILABLE AT " << branches[branchIdx].name << " ---\n";
        cout << left << setw(5) << "NO" << setw(35) << "MOVIE TITLE" << setw(15) << "BASE PRICE" << "SHOWTIME" << endl;
        cout << "------------------------------------------------------------------------\n";
        for (int i = 0; i < branches[branchIdx].movieCount; i++) {
            cout << left << setw(5) << i + 1 
                 << setw(35) << branches[branchIdx].movies[i].title 
                 << "RM " << fixed << setprecision(2) << setw(12) << branches[branchIdx].movies[i].price 
                 << branches[branchIdx].movies[i].showTime << endl;
        }
        cout << "\nSelect movie (1-" << branches[branchIdx].movieCount << "): ";
        cin >> movieChoice;
        
        while (movieChoice < 1 || movieChoice > branches[branchIdx].movieCount) {
            cout << "Invalid choice! Please re-select: ";
            cin >> movieChoice;
        }
        
        book.movieTitle = branches[branchIdx].movies[movieChoice - 1].title;
        book.moviePrice = branches[branchIdx].movies[movieChoice - 1].price;
        book.showTime = branches[branchIdx].movies[movieChoice - 1].showTime;
        
        cout << "\nYou selected: " << book.movieTitle << endl;
        action = askProceedChangeCancel();
    } while (action == 2);
    
    selectRoom(book);
}

void selectRoom(Booking &book) {
    int roomChoice, action;
    string roomTypes[] = {"Standard", "Deluxe", "Premium", "IMAX"};
    double roomSurcharges[] = {0.00, 5.00, 10.00, 15.00}; 
    
    do {
        system("CLS");
        cout << "--- SELECT ROOM TYPE ---\n";
        for (int i = 0; i < 4; i++) {
            cout << i + 1 << ". " << left << setw(10) << roomTypes[i] << " (Add-on +RM " << roomSurcharges[i] << ")" << endl;
        }
        cout << "Select type (1-4): ";
        cin >> roomChoice;
        
        while (roomChoice < 1 || roomChoice > 4) {
            cout << "Invalid choice! Please select 1-4: ";
            cin >> roomChoice;
        }
        
        book.roomType = roomTypes[roomChoice - 1];
        book.roomPrice = roomSurcharges[roomChoice - 1]; 
        
        cout << "\nYou selected: " << book.roomType << " Room\n";
        action = askProceedChangeCancel();
    } while (action == 2);
    
    selectSeat(book);
}

void selectSeat(Booking &book) {
    int action;
    do {
        system("CLS");
        cout << "--- SELECTION CONFIRMATION & SEAT BOOKING ---\n";
        cout << "Movie   : " << book.movieTitle << " (" << book.showTime << ")\n\n";
        
        cout << "--- AVAILABLE SEATS ---\n";
        cout << "[A1] [A2] [A3] [A4] [A5]\n";
        cout << "[B1] [B2] [B3] [B4] [B5]\n\n";
        
        cout << "How many seats do you want to book?: ";
        cin >> book.totalSeats;
        
        while(book.totalSeats <= 0) {
            cout << "Invalid! Please enter at least 1 seat: ";
            cin >> book.totalSeats;
        }
        
        book.seatNumber = ""; 
        string singleSeat;
        
        for (int i = 0; i < book.totalSeats; i++) {
            cout << "Enter seat number for ticket #" << i + 1 << " (e.g., A3): ";
            cin >> singleSeat;
            
            if (i == 0) {
                book.seatNumber = singleSeat;
            } else {
                book.seatNumber += ", " + singleSeat; 
            }
        }
        
        cout << "\nSeats Confirmed: " << book.seatNumber << " (Total: " << book.totalSeats << " seats)\n";
        action = askProceedChangeCancel();
        
    } while (action == 2); 
}

void selectAddOnsAndPayment(Booking &book) {
    int addOnChoice, paymentChoice;
    char foodOption;
    
    system("CLS");
    cout << "--- TICKET QUANTITY DISTRIBUTION ---\n";
    cout << "Total seats booked: " << book.totalSeats << " [" << book.seatNumber << "]\n\n";
    
    do {
        cout << "How many from " << book.totalSeats << " seats are for ADULT?: ";
        cin >> book.adultQty;
        
        if (book.adultQty > book.totalSeats || book.adultQty < 0) {
            cout << "Error: Adult quantity cannot exceed total seats (" << book.totalSeats << ")!\n";
        }
    } while (book.adultQty > book.totalSeats || book.adultQty < 0);
    
    book.childQty = book.totalSeats - book.adultQty;
    cout << "Number of CHILD tickets (Auto-calculated): " << book.childQty << endl;
    cout << "--------------------------------------------------\n";
    
    cout << "\nDo you want to add snacks? (Y/N): ";
    cin >> foodOption;
    
    if (foodOption == 'Y' || foodOption == 'y') {
        cout << "\n--- SELECT ADD ON DISH ---\n";
        cout << "1. Popcorn Combo - RM 15.00\n";
        cout << "2. Hotdog Combo  - RM 12.00\n";
        cout << "3. Nachos Combo  - RM 18.00\n";
        cout << "Enter choice (1-3): ";
        cin >> addOnChoice;
        
        if (addOnChoice == 1) { book.addOnName = "Popcorn Combo"; book.addOnPrice = 15.00; }
        else if (addOnChoice == 2) { book.addOnName = "Hotdog Combo"; book.addOnPrice = 12.00; }
        else { book.addOnName = "Nachos Combo"; book.addOnPrice = 18.00; }
    } else {
        book.addOnName = "None";
        book.addOnPrice = 0.00;
    }
    
    system("CLS");
    cout << "--- PAYMENT METHOD ---\n";
    cout << "1. Counter\n";
    cout << "2. Debit Card\n";
    cout << "3. Credit Card\n";
    cout << "4. Online Banking\n";
    cout << "Select payment method (1-4): ";
    cin >> paymentChoice;
    
    if (paymentChoice == 1) book.paymentMethod = "Counter";
    else if (paymentChoice == 2) book.paymentMethod = "Debit Card";
    else if (paymentChoice == 3) book.paymentMethod = "Credit Card";
    else book.paymentMethod = "Online Banking";
}

void calculateTotal(Booking &book) {
    double finalTicketPrice = book.moviePrice + book.roomPrice;
    
    double adultTotal = book.adultQty * finalTicketPrice;
    double childPrice = finalTicketPrice - 5.00; 
    if (childPrice < 5.00) childPrice = 5.00; 
    
    double childTotal = book.childQty * childPrice;
    book.totalPrice = adultTotal + childTotal + book.addOnPrice;
}

void displayAndSaveReceipt(Booking book) {
    system("CLS");
    
    double singleAdultPrice = book.moviePrice + book.roomPrice;
    double singleChildPrice = singleAdultPrice - 5.00;
    if (singleChildPrice < 5.00) singleChildPrice = 5.00;
    
    char cMovie[20], cRoom[20], cAdult[20], cChild[20], cAddOn[20], cTotal[20];
    sprintf(cMovie, "RM %.2f", book.moviePrice);
    sprintf(cRoom, "RM %.2f", book.roomPrice);
    sprintf(cAdult, "RM %.2f", singleAdultPrice);
    sprintf(cChild, "RM %.2f", singleChildPrice);
    sprintf(cAddOn, "RM %.2f", book.addOnPrice);
    sprintf(cTotal, "RM %.2f", book.totalPrice);

    string receiptOutput = "";
    receiptOutput += "\n==================================================\n";
    receiptOutput += "                 GSC OFFICIAL RECEIPT             \n";
    receiptOutput += "==================================================\n";
    receiptOutput += " Branch    : " + book.branchName + "\n";
    receiptOutput += " Movie     : " + book.movieTitle + "\n";
    receiptOutput += " Show Time : " + book.showTime + " (Seats: " + book.seatNumber + ")\n";
    receiptOutput += "--------------------------------------------------\n";
    receiptOutput += " PRICE BREAKDOWN / DETAILED CHARGES:\n";
    receiptOutput += "--------------------------------------------------\n";
    receiptOutput += " Movie Base Price       : " + string(cMovie) + "\n";
    receiptOutput += " Room Surcharge (" + book.roomType + ") : " + string(cRoom) + "\n\n";
    
    receiptOutput += " 1x Adult Ticket Price  : " + string(cAdult) + "\n";
    receiptOutput += "    Quantity Purchased  : " + to_string(book.adultQty) + " ticket(s)\n\n";
    
    receiptOutput += " 1x Child Ticket Price  : " + string(cChild) + " (Discounted)\n";
    receiptOutput += "    Quantity Purchased  : " + to_string(book.childQty) + " ticket(s)\n\n";
    
    receiptOutput += " Add-on Snack Selected  : " + book.addOnName + "\n";
    receiptOutput += " Add-on Total Price     : " + string(cAddOn) + "\n";
    receiptOutput += "--------------------------------------------------\n";
    receiptOutput += " TOTAL AMOUNT PAID      : " + string(cTotal) + "\n";
    receiptOutput += " Payment Method         : " + book.paymentMethod + "\n";
    receiptOutput += "==================================================\n";
    receiptOutput += "         **** THANK YOU, PLEASE COME AGAIN **** \n";
    receiptOutput += "==================================================\n";
    
    cout << receiptOutput;
    
    ofstream outFile("receipt.txt");
    if (outFile.is_open()) {
        outFile << receiptOutput;
        outFile.close();
        cout << "\n[System Notification: Receipt saved to 'receipt.txt']\n";
    }
}