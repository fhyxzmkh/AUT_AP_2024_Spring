#include "Account.h"
#include "Bank.h"
#include "Person.h"
#include "Utils.h"

// Getters
const Person *Account::get_owner() const {
    return this->owner;
}

double Account::get_balance() const {
    return this->balance;
}

std::string Account::get_account_number() const {
    return this->account_number;
}

bool Account::get_status() const {
    return this->account_status;
}

// 需要所有者指纹进行认证的 Getters
std::string Account::get_CVV2(std::string &owner_fingerprint) const {
    if (std::hash<std::string>{}(owner_fingerprint) != this->owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    return this->CVV2;
}

std::string Account::get_password(std::string &owner_fingerprint) const {
    if (std::hash<std::string>{}(owner_fingerprint) != this->owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    return this->password;
}

std::string Account::get_exp_date(std::string &owner_fingerprint) const {
    if (std::hash<std::string>{}(owner_fingerprint) != this->owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    return this->exp_date;
}

bool Account::set_password(std::string &password, std::string &owner_fingerprint) {
    if (std::hash<std::string>{}(owner_fingerprint) != this->owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    this->password = password;

    return false;
}

std::strong_ordering Account::operator<=>(const Account &other) const {
    return this->account_number <=> other.account_number;
}

void Account::get_info(std::optional<std::string> file_name) const {
    if (file_name.value().empty())
        return ;

    std::ofstream outFile(file_name.value(), std::ios::out);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing!" << std::endl;
        return;
    }

    outFile << this->owner << '\n';
    outFile << this->bank << '\n';
    outFile << this->account_number << '\n';
    outFile << this->balance << '\n';
    outFile << this->account_status << '\n';

    outFile.close();
}
