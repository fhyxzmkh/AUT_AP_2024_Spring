#ifndef BANK_H // 防止头文件重复包含
#define BANK_H

#include <compare>  // 用于 std::strong_ordering
#include <map>      // 用于 std::map
#include <optional> // 用于 std::optional
#include <string>   // 用于 std::string
#include <vector>   // 用于 std::vector

class Account; // Account 的前向声明
class Person; // Person 的前向声明

// 代表银行机构
class Bank {
public:
    // 带有银行名称和安全指纹的构造函数
    Bank(const std::string& bank_name, const std::string& bank_fingerprint)
        : bank_name{bank_name},
          hashed_bank_fingerprint{std::hash<std::string>{}(bank_fingerprint)},
          bank_customers{},
          bank_accounts{},
          account_2_customer{},
          customer_2_accounts{},
          customer_2_paid_loan{},
          customer_2_unpaid_loan{},
          bank_total_balance{},
          bank_total_loan{}
    {}


    ~Bank() {} // 析构函数

    // 银行操作
    Account* create_account(Person& owner, const std::string& owner_fingerprint, std::string password);
    bool delete_account(Account& account, const std::string& owner_fingerprint);
    bool delete_customer(Person& owner, const std::string& owner_fingerprint);
    bool deposit(Account& account, const std::string& owner_fingerprint, double amount);
    bool withdraw(Account& account, const std::string& owner_fingerprint, double amount);
    bool transfer(Account& source, Account& destination, const std::string& owner_fingerprint,
                  const std::string& CVV2, const std::string& password, const std::string& exp_date, double amount);
    bool take_loan(Account& account, const std::string& owner_fingerprint, double amount);
    bool pay_loan(Account& account, double amount);

    // Getters
    const std::string& get_bank_name() const;
    std::size_t get_hashed_bank_fingerprint() const;

    // 需要银行身份验证的 Getters
    const std::vector<Person*>& get_bank_customers(std::string& bank_fingerprint) const;
    const std::vector<Account*>& get_bank_accounts(std::string& bank_fingerprint) const;
    const std::map<Account*, Person*>& get_account_2_customer_map(std::string& bank_fingerprint) const;
    const std::map<Person*, std::vector<Account*>>& get_customer_2_accounts_map(std::string& bank_fingerprint) const;
    const std::map<Person*, double>& get_customer_2_paid_loan_map(std::string& bank_fingerprint) const;
    const std::map<Person*, double>& get_customer_2_unpaid_loan_map(std::string& bank_fingerprint) const;
    double get_bank_total_balance(std::string& bank_fingerprint) const;
    double get_bank_total_loan(std::string& bank_fingerprint) const;

    // 需要所有者和银行身份验证的账户 Setters
    bool set_owner(Account& account, const Person* new_owner, std::string& owner_fingerprint, std::string& bank_fingerprint);

    // 需要银行身份验证的账户 Setters
    bool set_account_status(Account& account, bool status, std::string& bank_fingerprint);
    bool set_exp_date(Account& account, std::string& exp_date, std::string& bank_fingerprint);

    // 输出银行信息，支持写入文件
    void get_info(std::optional<std::string> file_name = std::nullopt) const;

private:
    // 私有成员变量
    const std::string bank_name;
    const std::size_t hashed_bank_fingerprint;
    std::vector<Person*> bank_customers;
    std::vector<Account*> bank_accounts;
    std::map<Account*, Person*> account_2_customer;
    std::map<Person*, std::vector<Account*>> customer_2_accounts;
    std::map<Person*, double> customer_2_paid_loan;
    std::map<Person*, double> customer_2_unpaid_loan;
    double bank_total_balance; // 银行总利润
    double bank_total_loan; // 发放的总贷款
};

#endif // BANK_H