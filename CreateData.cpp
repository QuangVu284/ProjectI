#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <vector>

using namespace std;

vector<string> firstNames = {"Tran", "Nguyen", "Hoang", "Ly", "Phung", "Le", "Pham", "Phan", "Truong", "Bui", "Dang", "Ngo", "Vuong"};
vector<string> lastNames = {"Hong", "Minh", "Anh", "Duy", "Van", "Linh", "Khanh", "Quoc", "Lan", "Ngoc", "Huy", "Khang", "Khoa" ,"Dat", "Long", "Tuan", "Vu", "Thuy", "Thuy", "Duong"};
vector<string> middleNames = {"Van", "Huu", "Hoang", "Anh", "Quang"};
// Hàm tạo số ngẫu nhiên trong một khoảng
double randomBalance() {
    return (rand() % 123)*100000;
}

int randomPinCode(){
    return (rand() % 9000) + 1000;
}

// Hàm tạo số thẻ ngẫu nhiên
string randomCardNumber() {
    string cardNumber = "4912";
    for (int i = 0; i < 9; ++i) {
        cardNumber += to_string(rand() % 10);
    }
    return cardNumber;
}

string ranFulName(vector<string> &firstNames, vector<string> &middleNames, vector<string> &lastNames){
    string lastName = lastNames[rand() % lastNames.size()];
    string firstName = firstNames[rand() % firstNames.size()];
    string middleName = middleNames[rand() % middleNames.size()];
    return firstName + " " + middleName + " " + lastName;
}



int main() {

    srand(static_cast<unsigned>(time(0)));

    ofstream outputFile("Account_data.txt");

    if (!outputFile.is_open()) {
        cerr << "Khong the mo file." << endl;
        return 1;
    }

    for (int i = 0; i < 50000; ++i) {
        string cardNumber = randomCardNumber();
        int pinCode = randomPinCode();
        string fullName = ranFulName(firstNames, middleNames, lastNames);
        string role = (i % 100 != 0) ? "User" : "Admin";
        double balance = randomBalance();

         outputFile << cardNumber << " " << pinCode << " " << fullName << " " << role << " " << fixed << setprecision(3) << balance << endl;
    }



    outputFile.close();

    cout << "Da tao file Account_data.txt thanh cong." << endl;

    return 0;
}
