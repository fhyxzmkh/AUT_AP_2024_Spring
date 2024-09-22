#include "Person.h"
#include "Utils.h"

// Getters
std::string Person::get_name() const {
    return this->name;
}

std::size_t Person::get_age() const {
    return this->age;
}

std::string Person::get_gender() const {
    return this->gender;
}

std::size_t Person::get_hashed_fingerprint() const {
    return this->hashed_fingerprint;
}

std::size_t Person::get_socioeconomic_rank() const {
    return this->socioeconomic_rank;
}

bool Person::get_is_alive() const {
    return this->is_alive;
}

// Setters
bool Person::set_age(std::size_t age) {
    this->age = age;
    return true;
}

bool Person::set_socioeconomic_rank(std::size_t rank) {
    if (rank <= 0 || rank >= 11)
        throw std::invalid_argument(&"Invalid socioeconomic rank: " [ socioeconomic_rank]);

    this->socioeconomic_rank = rank;

    return true;
}

bool Person::set_is_alive(bool is_alive) {
    this->is_alive = is_alive;

    return true;
}

std::strong_ordering Person::operator<=>(const Person &other) const {
    return this->hashed_fingerprint <=> other.hashed_fingerprint;
}

void Person::get_info(std::optional<std::string> file_name) const {
    if (file_name.value().empty())
        return ;

    std::ofstream outFile(file_name.value(), std::ios::out);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing!" << std::endl;
        return;
    }

    outFile << this->name << '\n';
    outFile << this->age << '\n';
    outFile << this->gender << '\n';
    outFile << this->hashed_fingerprint << '\n';
    outFile << this->socioeconomic_rank << '\n';
    outFile << this->is_alive << '\n';

    outFile.close();
}
