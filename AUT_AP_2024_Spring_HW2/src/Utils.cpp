#include "Utils.h"

std::string generateRandomString(std::size_t length) {
    // 定义一个包含所有可能字符的字符串
    const std::string characters = "0123456789";

    // 创建一个随机数生成器
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<std::size_t> distribution(0, characters.size() - 1);

    // 生成随机字符串
    std::string randomString(length, ' ');
    for (std::size_t i = 0; i < length; ++i) {
        randomString[i] = characters[distribution(generator)];
    }

    return randomString;
}

std::string generateRandomExpirationDate() {
    // 创建一个随机数生成器
    std::random_device rd;
    std::mt19937_64 generator(rd());

    // 生成随机的年份 (00-99)
    std::uniform_int_distribution<int> yearDistribution(0, 99);
    int year = yearDistribution(generator);

    // 生成随机的月份 (01-12)
    std::uniform_int_distribution<int> monthDistribution(1, 12);
    int month = monthDistribution(generator);

    // 格式化为 YY-MM 的字符串
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << year << "-"
        << std::setw(2) << std::setfill('0') << month;

    return oss.str();
}