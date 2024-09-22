#ifndef PERSON_H
#define PERSON_H

#include <compare>  // 用于 std::strong_ordering
#include <optional> // 用于 std::optional
#include <string>   // 用于 std::string

#include <functional>
#include <stdexcept>
#include <fstream>
#include <iostream>

// 代表具有个人属性的个人
class Person {
public:
    // 带有个人属性的构造函数
    Person(std::string &name, std::size_t age, std::string &gender,
           std::string &fingerprint, std::size_t socioeconomic_rank, bool is_alive)
        : name{name},
          age{age},
          gender{gender},
          hashed_fingerprint{std::hash<std::string>{}(fingerprint)},
          socioeconomic_rank{socioeconomic_rank},
          is_alive{is_alive}
    {
        if (gender != "Female" && gender != "Male")
            throw std::invalid_argument("Invalid gender: " + gender);

        if (socioeconomic_rank <= 0 || socioeconomic_rank >= 11)
            throw std::invalid_argument(&"Invalid socioeconomic rank: " [ socioeconomic_rank]);
    }


    // Getters
    std::string get_name() const;
    std::size_t get_age() const;
    std::string get_gender() const;
    std::size_t get_hashed_fingerprint() const;
    std::size_t get_socioeconomic_rank() const;
    bool get_is_alive() const;

    // Setters
    bool set_age(std::size_t age);
    bool set_socioeconomic_rank(std::size_t rank);
    bool set_is_alive(bool is_alive);

    // Spaceship 运算符用于 Person 比较
    std::strong_ordering operator<=>(const Person &other) const;

    // 输出个人信息，支持写入文件（如果传递了文件名）
    void get_info(std::optional<std::string> file_name = std::nullopt) const;

private:
    const std::string name;
    std::size_t age;
    const std::string gender;
    const std::size_t hashed_fingerprint;
    std::size_t socioeconomic_rank;
    bool is_alive;
};

#endif // PERSON_H