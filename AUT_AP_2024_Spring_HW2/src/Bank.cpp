#include "Bank.h"
#include "Person.h"
#include "Account.h"
#include "Utils.h"

Account *Bank::create_account(Person &owner, const std::string &owner_fingerprint, std::string password) {
    if (std::hash<std::string>{}(owner_fingerprint) != owner.get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    auto* new_account = new Account(&owner, this, password);

    bank_customers.emplace_back(&owner);
    bank_accounts.emplace_back(new_account);

    customer_2_accounts[&owner] = bank_accounts;
    account_2_customer[new_account] = &owner;

    return new_account;
}

bool Bank::delete_account(Account &account, const std::string &owner_fingerprint) {
    auto* owner = account.get_owner();
    if (std::hash<std::string>{}(owner_fingerprint) != owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    if (customer_2_unpaid_loan.contains(const_cast<Person *>(owner)))
        throw std::invalid_argument("Unpaid loan");

    // 银行账户表中删除
    if (auto it = std::find(bank_accounts.begin(), bank_accounts.end(), &account);
        it == bank_accounts.end())
        return false;
    else
        bank_accounts.erase(it);

    // 账户到客户映射表中删除
    account_2_customer.erase(&account);

    // 客户到账户映射表中删除
    auto& accounts = customer_2_accounts[const_cast<Person*>(owner)];
    if (auto it = std::find(accounts.begin(), accounts.end(), &account); it == accounts.end())
        return false;
    else
        accounts.erase(it);

    return true;
}

bool Bank::delete_customer(Person &owner, const std::string &owner_fingerprint) {
    if (std::hash<std::string>{}(owner_fingerprint) != owner.get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    if (customer_2_unpaid_loan[&owner] > 0)
        throw std::invalid_argument("Unpaid loan");

    // 删除与该客户相关的所有账户
    auto& accounts = customer_2_accounts[&owner];
    for (auto account : accounts) {
        // 从银行的账户列表中删除账户

        if (auto it = std::find(bank_accounts.begin(), bank_accounts.end(), account);
            it != bank_accounts.end())
            bank_accounts.erase(it);

        // 从账户到客户的映射中删除账户
        account_2_customer.erase(account);

        // 删除账户对象
        delete account;
    }

    if (auto it = std::find(bank_customers.begin(), bank_customers.end(), &owner);
        it != bank_customers.end()) {
        bank_customers.erase(it);
    }

    customer_2_accounts.erase(&owner);

    return true;
}

bool Bank::deposit(Account &account, const std::string &owner_fingerprint, double amount) {
    auto* owner = account.get_owner();
    if (std::hash<std::string>{}(owner_fingerprint) != owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    account.balance += amount;

    return true;
}

bool Bank::withdraw(Account &account, const std::string &owner_fingerprint, double amount) {
    auto* owner = account.get_owner();
    if (std::hash<std::string>{}(owner_fingerprint) != owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    if (account.balance < amount)
        throw std::invalid_argument("Invalid amount.");

    account.balance -= amount;

    return true;
}

bool Bank::transfer(Account &source, Account &destination, const std::string &owner_fingerprint, const std::string &CVV2, const std::string &password, const std::string &exp_date, double amount) {
    auto* owner = source.get_owner();
    if (std::hash<std::string>{}(owner_fingerprint) != owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    if (source.get_CVV2(const_cast<std::string&>(owner_fingerprint)) != CVV2)
        return false;

    if (source.password != password)
        return false;

    int year1{}, month1{}, year2{}, month2{};
    char dash{};

    std::istringstream in(exp_date);
    in >> year1 >> dash >> month1;
    std::istringstream ddl(source.get_exp_date(const_cast<std::string&>(owner_fingerprint)));
    ddl >> year2 >> dash >> month2;

    bool is_ok_exp_date = (year1 <= year2) && (month1 <= month2);
    if (!is_ok_exp_date)
        return false;

    if (source.balance < amount)
        return false;

    source.balance -= amount;
    destination.balance += amount;

    return true;
}

bool Bank::take_loan(Account &account, const std::string &owner_fingerprint, double amount) {
    auto* owner = account.get_owner();
    if (std::hash<std::string>{}(owner_fingerprint) != owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    // 计算客户可以请求的最大贷款金额
    double max_can_loan = account.get_balance() * (static_cast<double>(owner->get_socioeconomic_rank()) * 10.) * 0.01;

    // 确保请求的贷款不超过客户的贷款限额，考虑其当前未偿还的贷款
    auto it = customer_2_unpaid_loan.find(const_cast<Person *>(owner));
    if (amount + (it == customer_2_unpaid_loan.end() ? 0 : it->second) > max_can_loan)
        throw std::invalid_argument("Invalid amount.");

    // 计算利息并更新银行的总利润
    double interest = amount * (10.0 / owner->get_socioeconomic_rank()) * 0.01;
    bank_total_balance += interest;

    // 将贷款金额分配到指定账户
    account.balance += amount;

    // 更新相关贷款跟踪变量
    bank_total_loan += amount + interest;
    customer_2_unpaid_loan[const_cast<Person *>(owner)] += amount + interest;

    return true;
}

bool Bank::pay_loan(Account &account, double amount) {
    auto* owner = account.get_owner();

    account.balance -= amount;

    customer_2_unpaid_loan[const_cast<Person *>(owner)] -= amount;
    if (customer_2_unpaid_loan[const_cast<Person *>(owner)] == 0)
        customer_2_unpaid_loan.erase(const_cast<Person *>(owner));

    auto threshold = pow(10, owner->get_socioeconomic_rank());

    customer_2_paid_loan[const_cast<Person *>(owner)] += amount;
    if (customer_2_paid_loan[const_cast<Person *>(owner)] >= threshold)
        const_cast<Person *>(owner)->set_socioeconomic_rank(owner->get_socioeconomic_rank() + 1);

    return true;
}

const std::string &Bank::get_bank_name() const {
    return this->bank_name;
}

size_t Bank::get_hashed_bank_fingerprint() const {
    return this->hashed_bank_fingerprint;
}

const std::vector<Person *> &Bank::get_bank_customers(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->bank_customers;
}

const std::vector<Account *> &Bank::get_bank_accounts(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->bank_accounts;
}

const std::map<Account *, Person *> &Bank::get_account_2_customer_map(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->account_2_customer;
}

const std::map<Person *, std::vector<Account *>> &Bank::get_customer_2_accounts_map(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->customer_2_accounts;
}

const std::map<Person *, double> &Bank::get_customer_2_paid_loan_map(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->customer_2_paid_loan;
}

const std::map<Person *, double> &Bank::get_customer_2_unpaid_loan_map(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->customer_2_unpaid_loan;
}

double Bank::get_bank_total_balance(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->bank_total_balance;
}

double Bank::get_bank_total_loan(std::string &bank_fingerprint) const {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    return this->bank_total_loan;
}

bool Bank::set_owner(Account &account, const Person *new_owner, std::string &owner_fingerprint, std::string &bank_fingerprint) {
    auto* owner = account.get_owner();
    if (std::hash<std::string>{}(owner_fingerprint) != owner->get_hashed_fingerprint())
        throw std::invalid_argument("Invalid fingerprint.");

    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    account.owner = const_cast<Person *>(new_owner);

    account_2_customer[&account] = const_cast<Person *>(new_owner);

    auto& accounts = customer_2_accounts[const_cast<Person*>(owner)];
    if (auto it = std::find(accounts.begin(), accounts.end(), &account);
        it != accounts.end())
        accounts.erase(it);
    customer_2_accounts[const_cast<Person*>(new_owner)].emplace_back(&account);

    return true;
}

bool Bank::set_account_status(Account &account, bool status, std::string &bank_fingerprint) {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    account.account_status = status;

    return true;
}

bool Bank::set_exp_date(Account &account, std::string &exp_date, std::string &bank_fingerprint) {
    if (std::hash<std::string>{}(bank_fingerprint) != this->hashed_bank_fingerprint)
        throw std::invalid_argument("Invalid fingerprint.");

    account.exp_date = exp_date;

    return true;
}

void Bank::get_info(std::optional<std::string> file_name) const {
    if (file_name.value().empty())
        return ;

    std::ofstream outFile(file_name.value(), std::ios::out);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing!" << std::endl;
        return;
    }

    outFile << this->bank_name << '\n';
    outFile << this->bank_total_balance << '\n';
    outFile << this->bank_total_loan << '\n';

    outFile.close();
}

