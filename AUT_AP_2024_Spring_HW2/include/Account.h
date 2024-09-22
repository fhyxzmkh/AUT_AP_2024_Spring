#ifndef ACCOUNT_H // 防止头文件重复包含
#define ACCOUNT_H

#include <compare>  // 用于 std::strong_ordering
#include <optional> // 用于 std::optional
#include <string>   // 用于 std::string

#include "Utils.h"

class Bank; // Bank 的前向声明
class Person; // Person 的前向声明

// 代表具有所有者、银行、余额、状态和凭据的银行账户
class Account {
    friend class Bank; // 使 Bank 成为 Account 的朋友以获得完全访问权限

public:
    // 带有所有者、银行和密码的构造函数
    Account(const Person* const owner, const Bank* const bank, std::string& password)
        : owner{const_cast<Person*>(owner)},
          bank {bank},
          account_number{generateRandomString(16)},
          balance{},
          account_status{true},
          CVV2{generateRandomString(4)},
          password{password},
          exp_date{generateRandomExpirationDate()}
    {}


    // Getters
    const Person* get_owner() const;
    double get_balance() const;
    std::string get_account_number() const;
    bool get_status() const;

    // 需要所有者指纹进行认证的 Getters
    std::string get_CVV2(std::string& owner_fingerprint) const;
    std::string get_password(std::string& owner_fingerprint) const;
    std::string get_exp_date(std::string& owner_fingerprint) const;

    // 需要所有者指纹进行认证的 Setters
    bool set_password(std::string& password, std::string& owner_fingerprint);

    // Spaceship 运算符用于 Account 比较
    std::strong_ordering operator<=>(const Account& other) const;

    // 输出账户信息，支持写入文件
    void get_info(std::optional<std::string> file_name = std::nullopt) const;

private:
    // 成员变量
    Person* owner;
    const Bank* bank;
    const std::string account_number;
    double balance;
    bool account_status;

    // 凭据变量
    const std::string CVV2;
    std::string password;
    std::string exp_date;
};

#endif // ACCOUNT_H