#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <ctime>
#include <cmath>
#include <sstream>
#include <windows.h>

using namespace std;

struct account {
    string cardNumber;
    string pinCode;
    string userName;
    string role;
    double balance;

    account() : cardNumber(""), pinCode(""), userName(""), role(""), balance(0.0) {}

    account(const string &number, const string &pin, const string &name, const string &userrole, double initialBalance)
        : cardNumber(number), pinCode(pin), userName(name), role(userrole), balance(initialBalance) {}
};

map<string, account> accountMap; // lưu trữ tài khoản

bool isTrue = true;

void saveToFile(const string &filename, const map<string, account> &accountMap) { // Hàm cập nhật và lưu lại file khi có thay đổi
    ofstream outputFile(filename, ios::trunc); // Mở tệp tin và xóa dữ liệu cũ

    if (outputFile.is_open()) {
        for (const auto &it : accountMap) {
            // Ghi thông tin từ map vào tệp tin
            outputFile << it.second.cardNumber << " "
                        << it.second.pinCode << " "
                        << it.second.userName << " "
                        << it.second.role << " "
                      << fixed << setprecision(3) << it.second.balance << endl;
        }

        outputFile.close();
    } else {
        cerr << "Không thể mở file để ghi !" << endl;
    }
}

void displayMenu() { // Hiển thị menu chính
    cout << "========== MENU ===========" << endl;
    cout << "1. ADMIN LOGIN" << endl;
    cout << "2. USER LOGIN" << endl;
    cout << "0. EXIT" << endl;
    cout << "===========================" << endl;
}

void displayUserMenu() {
    cout << "========== USER MENU ===========" << endl;
    cout << "1. Xem thông tin tài khoản" << endl;
    cout << "2. Rút tiền" << endl;
    cout << "3. Nạp tiền" << endl;
    cout << "4. Chuyển khoản" << endl;
    cout << "5. Lịch sử giao dịch" << endl;
    cout << "0. Đăng xuất" << endl;
    cout << "================================" << endl;
}

void displayAdminMenu() {
    cout << "========== ADMIN MENU ===========" << endl;
    cout << "1. Xem thông tin tài khoản" << endl;
    cout << "2. Rút tiền" << endl;
    cout << "3. Nạp tiền" << endl;
    cout << "4. Chuyển khoản" << endl;
    cout << "5. Lịch sử giao dịch ATM" << endl;
    cout << "6. Thêm tài khoản" << endl;
    cout << "7. Xóa tài khoản" << endl;
    cout << "0. Đăng xuất" << endl;
    cout << "=================================" << endl;
}

void displayAccountinfo(const account &account) {
    cout << "=== THÔNG TIN TÀI KHOẢN ===" << endl;
    cout << "Số tài khoản: " << account.cardNumber << endl;
    cout << "Họ tên: " << account.userName << endl;
    cout << account.role << endl;
    cout << "Số dư tài khoản: " << fixed << setprecision(3) << account.balance << " VNĐ " << endl;
    cout << "===========================" << endl;
}

void history(const string &filename) {
    ifstream inputfile(filename);
    if(inputfile.is_open()) {
        string transaction;
        while(getline(inputfile, transaction)) {
            cout << transaction << endl;
        }
        inputfile.close();
    } else {
        cerr << "Không thể đọc lịch sử giao dịch !" << endl;
    }
}


// Hàm lưu thông tin giao dịch vào file mỗi khi thực hiện chức năng
void writeTransactiontoFile(const string &filename, double amount, const string &sourceAccount, const string &destinationAccount, const string transactiontype) {
    ofstream outputfile(filename, ios::app);

    if (outputfile.is_open()) {
        // Lấy thông tin thời gian tại thời điểm giao dịch
        time_t nowtime;
        struct tm *timeinfo;
        time(&nowtime);
        timeinfo = localtime(&nowtime);

        // Định dạng thời gian
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Ghi thông tin giao dịch vào tệp tin
        if(destinationAccount != " ") {
            outputfile << buffer << " --- " << transactiontype <<  fixed << setprecision(3)
                       << amount << " VNĐ ,"
                       << "Tài khoản nguồn: " << sourceAccount
                       << ", Tài khoản thụ hưởng: " << destinationAccount << endl;
        } else {
            outputfile << buffer << " --- " << transactiontype <<  fixed << setprecision(3)
                       << amount << " VNĐ ,"
                       << "Tài khoản nguồn: " << sourceAccount << endl;
        }

        outputfile.close();
    } else {
        cerr << "Không thể mở file để ghi !" << endl;
    }
}

time_t convertStringToTime(const string &timeStr) { // Hàm chuyển đổi chuỗi thời gian thành kiểu "time_t"
    struct tm timeinfo = {};
    stringstream ss(timeStr);
    ss >> get_time(&timeinfo, "%Y-%m-%d");

    return mktime(&timeinfo);
}

// Hàm lọc tìm kiếm lịch sử giao dịch theo khoảng thời gian
void filterTransactionsByTime(const string &filename, const string &startTimeStr, const string &endTimeStr) {
    ifstream inputFile(filename);

    if (inputFile.is_open()) {
        string transaction;
        time_t startTime = convertStringToTime(startTimeStr);
        time_t endTime = convertStringToTime(endTimeStr);

        while (getline(inputFile, transaction)) {
            // Phân tích thông tin thời gian từ dòng giao dịch
            size_t timestampPos = transaction.find(" --- ");
            if (timestampPos != string::npos) {
                string timeStr = transaction.substr(0, timestampPos);
                time_t timestamp = convertStringToTime(timeStr);

                // Kiểm tra xem giao dịch có trong khoảng thời gian cần lọc hay không
                if (timestamp >= startTime && timestamp <= endTime) {
                    cout << transaction << endl;
                }
            }
        }
        inputFile.close();
    } else {
        cerr << "Không thể đọc lịch sử giao dịch !" << endl;
    }
}

// Hàm hiển thị lịch sử giao dịch của người dùng
void displayUserTransactionHistory(const string &filename, const string &cardNumber) {
    ifstream inputFile(filename);

    if (inputFile.is_open()) {
        string transaction;

        while (getline(inputFile, transaction)) {
            // Phân tích thông tin thời gian từ dòng giao dịch
            size_t timestampPos = transaction.find(" --- ");

            if (timestampPos != string::npos) {
                string timeStr = transaction.substr(0, timestampPos);
                time_t timestamp = convertStringToTime(timeStr);

                // Kiểm tra xem dòng có chứa thông tin về tài khoản cần lấy lịch sử không
                if (transaction.find(cardNumber) != string::npos) {
                    cout << transaction << endl;
                }
            }
        }

        inputFile.close();
    } else {
        cerr << "Không thể đọc lịch sử giao dịch !" << endl;
    }
}

// Hàm tính thời gian thực hiện
double measureExecutionTime(clock_t start, clock_t end) {
        return static_cast<double>(end - start) / CLOCKS_PER_SEC;
    }

void withdraw(account &account, double amount) {
//    clock_t start = clock(); // Bắt đầu đo thời gian

    if (amount > 0 && amount <= account.balance && fmod(amount, 50000.0) == 0 ) {
        account.balance -= amount;
        cout << "Rút tiền thành công, số dư mới: " << fixed << setprecision(3) << account.balance << " VNĐ " << endl;

        writeTransactiontoFile("TransactionHistory.txt", amount, account.cardNumber, " " ,"Rút tiền : ");
        saveToFile("Account_data.txt", accountMap);
    } else {
        cout << "Số tiền rút không hợp lệ !" << endl;
    }

//    clock_t end = clock(); // Kết thúc đo thời gian
//    cout << "Thời gian rút tiền: " << measureExecutionTime(start, end) << " giây" << endl;


}

void deposit(account &account, double amount) {
//    clock_t start = clock(); // Bắt đầu đo thời gian

    if (amount > 0) {
        account.balance += amount;
        cout << "Nạp tiền thành công, số dư mới: " << fixed << setprecision(3) << account.balance << " VNĐ " << endl;

        writeTransactiontoFile("TransactionHistory.txt", amount, account.cardNumber, " ", "Nạp tiền : ");
        saveToFile("Account_data.txt", accountMap);
    } else {
        cout << "Số tiền rút không hợp lệ!" << endl;
    }
//    clock_t end = clock(); // Kết thúc đo thời gian
//    cout << "Thời gian nạp tiền: " << measureExecutionTime(start, end) << " giây" << endl;
}

void transfer(account &source, account &destination, double amount) {
//    clock_t start = clock(); // Bắt đầu đo thời gian

    if (amount > 0 && amount <= source.balance) {
        source.balance -= amount;
        destination.balance += amount;
        cout << "Chuyển khoản thành công! \nSố dư mới của nguồn: " << fixed << setprecision(3) << source.balance << " VNĐ " << endl;
        cout << "Số dư mới của tài khoản " << destination.cardNumber + " : " <<  fixed << setprecision(3) << destination.balance << " VNĐ " << endl;

        writeTransactiontoFile("TransactionHistory.txt", amount, source.cardNumber, destination.cardNumber, "Chuyển tiền : ");
        saveToFile("Account_data.txt", accountMap);
    } else {
        cout << "Số tiền chuyển không hợp lệ !" << endl;
    }
//    clock_t end = clock(); // Kết thúc đo thời gian
//    cout << "Thời gian chuyển tiền: " << measureExecutionTime(start, end) << " giây" << endl;
}

void addAccount() {
    string cardNumber;
    string pinCode;
    string lastName, middleName, firstName;
    string role;
    double balance;

    cout << "Nhập số tài khoản: ";
    cin >> cardNumber;
    cout << "Nhập mã PIN: ";
    cin >> pinCode;
    cout << "Nhập họ tên: ";
    cin >> firstName >> middleName >> lastName;
    cout << "Nhập vai trò (User hoặc Admin): ";
    cin >> role;
    cout << "Nhập số dư ban đầu: ";
    cin >> balance;

    accountMap.emplace(cardNumber, account(cardNumber, pinCode, firstName + " " + middleName + " " + lastName, role, balance));
    cout << "Tài khoản đã được thêm thành công !" << endl;

    saveToFile("Account_data.txt", accountMap);
}

void deleteAccount() {
    string cardNumber;
    cout << "Nhập số tài khoản cần xóa: ";
    cin >> cardNumber;

    auto it = accountMap.find(cardNumber);
    if (it != accountMap.end()) {
        cout << "Tài khoản " << accountMap[cardNumber].userName <<" đã được xóa thành công !" << endl;
        accountMap.erase(it);
        saveToFile("Account_data.txt", accountMap);
    } else {
        cout << "Tài khoản không tồn tại !" << endl;
    }

}

string findAccount(const map<string, account> &accountMap, const string &searchkey) {
    auto it = accountMap.find(searchkey);

    if (it != accountMap.end()) {
        return it->second.cardNumber;
    } else {
        return "";
    }
}

void adminLogin() {
    int choice;
    int count = 0;
    string cardNumberinput;
    cout << "Nhập số tài khoản Admin: ";
    cin >> cardNumberinput;

    string foundaccount = findAccount(accountMap, cardNumberinput);

    if (!foundaccount.empty() && accountMap[foundaccount].role == "Admin") {
        system("cls");
        cout << "*****************************************************"  <<  endl;
        cout << "           WELCOME " << accountMap[foundaccount].userName << " to ATM              " << endl;
        cout << "*****************************************************" << endl;
        cout << "Nhập mã PIN để đăng nhập : ";
        do{

            string PIN;
            cin >> PIN;
        if (PIN == accountMap[foundaccount].pinCode) {
            cout << "Đăng nhập Admin thành công !" << endl;
            do {
                displayAdminMenu();
                cout << "Nhập lựa chọn (0-7): ";
                cin >> choice;
                switch (choice) {
                    case 0: {
                        system("cls");
                        cout << "Đăng xuất Admin thành công !" << endl;
                        isTrue = false;
                        break;
                    }
                    case 1: {
                            system("cls");
                            displayAccountinfo(accountMap[foundaccount]);
                            break;
                    }
                    case 2: {
                            system("cls");
                            double withdrawamount;
                            cout << "Nhập số tiền muốn rút (BỘI SỐ 50000): ";
                            cin >> withdrawamount;
                            withdraw(accountMap[foundaccount], withdrawamount);
                            break;
                    }
                    case 3: {
                            system("cls");
                            double depositamount;
                            cout << "Nhập số tiền muốn nạp: ";
                            cin >> depositamount;
                            deposit(accountMap[foundaccount], depositamount);
                            break;
                    }
                    case 4: {
                            system("cls");
                            string destinationcardNumber;
                            cout << "Nhập số tài khoản thụ hưởng: ";
                            cin >> destinationcardNumber;

                            string foundaccount2 = findAccount(accountMap, destinationcardNumber);

                            if (!foundaccount2.empty()) {
                                cout << "Người thụ hưởng: " << accountMap[destinationcardNumber].userName << endl;
                                double transferamount;
                                cout << "Nhập số tiền muốn chuyển: ";
                                cin >> transferamount;
                                transfer(accountMap[foundaccount], accountMap[foundaccount2], transferamount);
                            } else {
                                cout << "Tài khoản thụ hưởng không tồn tại !" << endl;
                            }
                            break;
                    }
                    case 5: {
                            system("cls");
                            int historyChoice;
                cout << "1. Hiện thị tất cả lịch sử giao dịch" << endl;
                cout << "2. Chọn khoảng thời gian giao dịch" << endl;
                cout << "Nhập lựa chọn (1-2): ";
                cin >> historyChoice;
                switch (historyChoice) {
                    case 1: {
                        cout << "------------------------------------ HISTORY ------------------------------------" << endl;
                        history("TransactionHistory.txt");
                        cout << "---------------------------------------------------------------------------------" << endl;
                        break;
                    }
                    case 2: {
                        string startTime, endTime;
                        cout << "Nhập thời gian bắt đầu (YYYY-MM-DD): ";
                        cin.ignore();
                        getline(cin, startTime);

                        cout << "Nhập thời gian kết thúc (YYYY-MM-DD): ";
                        getline(cin, endTime);
                        cout << "------------------------------------ HISTORY ------------------------------------" << endl;
                        filterTransactionsByTime("TransactionHistory.txt", startTime, endTime);
                        cout << "---------------------------------------------------------------------------------" << endl;

                        break;
                    }
                    default: {
                        cout << "Lựa chọn không hợp lệ !" << endl;
                        break;
                    }
                }
                break;
                    }
                    case 6: {
                        addAccount();
                        break;
                    }
                    case 7: {
                        deleteAccount();
                        break;
                    }
                    default: {
                        cout << "Lựa chọn không hợp lệ !" << endl;
                        break;
                    }
                }
            } while (choice != 0);
        } else {
            count ++;
            if(count == 5) {
                cout << "Đăng nhập Admin thất bại !" << endl;
                isTrue = false;
            } else cout << "Mã PIN không đúng, vui lòng nhập lại : ";
        }

        }while(isTrue);
    } else {
        cout << "Tài khoản Admin không tồn tại !" << endl;
    }
}
void userLogin() {
    int count = 0;
    int choice;
    string cardNumberinput;
    cout << "Nhập số tài khoản: ";
    cin >> cardNumberinput;

    string foundaccount = findAccount(accountMap, cardNumberinput);

    if (!foundaccount.empty() && accountMap[foundaccount].role == "User") {
        system("cls");
        cout << "*****************************************************"  <<  endl;
        cout << "           WELCOME " << accountMap[foundaccount].userName << " to ATM              " << endl;
        cout << "*****************************************************" << endl;
        cout << "Nhập mã PIN để đăng nhập : ";
        do{
            string PIN;
            cin >> PIN;

        if (PIN == accountMap[foundaccount].pinCode) {
            cout << "Đăng nhập User thành công !" << endl;
            do {
                displayUserMenu();
                cout << "Nhập lựa chọn (0-5): ";
                cin >> choice;
                switch (choice) {
                    case 0: {
                        cout << "Đăng xuất User thành công !" << endl;
                        isTrue = false;
                        break;
                    }
                    case 1: {
                        system("cls");
                        displayAccountinfo(accountMap[foundaccount]);
                        break;
                    }
                    case 2: {
                        system("cls");
                        double withdrawamount;
                        cout << "Nhập số tiền muốn rút (BỘI SỐ 50000): ";
                        cin >> withdrawamount;
                        withdraw(accountMap[foundaccount], withdrawamount);
                        break;
                    }
                    case 3: {
                        system("cls");
                        double depositamount;
                        cout << "Nhập số tiền muốn nạp: ";
                        cin >> depositamount;
                        deposit(accountMap[foundaccount], depositamount);
                        break;
                    }
                    case 4: {
                        system("cls");
                        string destinationcardNumber;
                        cout << "Nhập số tài khoản thụ hưởng: ";
                        cin >> destinationcardNumber;

                        string foundaccount2 = findAccount(accountMap, destinationcardNumber);

                        if (!foundaccount2.empty()) {
                            cout << "Người thụ hưởng: " << accountMap[destinationcardNumber].userName << endl;
                            double transferamount;
                            cout << "Nhập số tiền muốn chuyển: ";
                            cin >> transferamount;
                            transfer(accountMap[foundaccount], accountMap[foundaccount2], transferamount);
                        } else {
                            cout << "Tài khoản thụ hưởng không tồn tại !" << endl;
                        }
                        break;
                    }
                    case 5:{

                        cout << "------------------------------------ HISTORY ------------------------------------" << endl;
                        displayUserTransactionHistory("TransactionHistory.txt", accountMap[foundaccount].cardNumber) ;
                        cout << "---------------------------------------------------------------------------------" << endl;
                        break;
                    }

                    default: {
                        cout << "Lựa chọn không hợp lệ" << endl;
                        break;
                    }
                }
            } while (choice != 0);
        } else {
            count++;
            if(count == 5) {
                cout << "Đăng nhập User thất bại !" << endl;
                isTrue = false;
            } else cout << "Mã PIN không đúng, vui lòng nhập lại : ";
        }

        }while(isTrue);
    } else {
        cout << "Tài khoản không tồn tại hoặc không phải là User !" << endl;
    }
}

int main() {

    SetConsoleOutputCP(CP_UTF8);
    system("color E");
    ifstream inputfile("Account_data.txt");

    if (!inputfile.is_open()) {
        cerr << "Không thể mở file !" << endl;
        return 1;
    }


    string cardNumber;
    string pinCode;
    while (inputfile >> cardNumber >> pinCode) {
        // Đọc họ và tên từ dòng
        string lastName, middleName, firstName;
        inputfile >> firstName >> middleName >> lastName;

        // Đọc phần còn lại của dòng (role và balance)
        string role;
        double balance;
        inputfile >> role >> balance;

        // Thêm vào map
        accountMap.emplace(cardNumber, account(cardNumber, pinCode, firstName + " " + middleName + " " + lastName, role, balance));
    }


    inputfile.close();

    int loginChoice;

    while (true) {
        displayMenu();
        cout << "Nhập lựa chọn (0-2): ";
        cin >> loginChoice;

        switch (loginChoice) {
            case 1: {
                adminLogin();
                break;
            }
            case 2: {
                userLogin();
                break;
            }
            case 0: {
                cout << "Đã thoát chương trình !" << endl;
                return 0;
            }
            default: {
                cout << "Lựa chọn không hợp lệ" << endl;
                break;
            }
        }
    }

    return 0;
}
