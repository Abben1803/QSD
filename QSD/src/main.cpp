#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

struct Condition
{
    uint32_t command;
    uint32_t length;
    std::vector<uint8_t> data;

    void setOriginalData(const std::vector<uint8_t> &original)
    {
        data = original;
    }
};

struct Action
{
    uint32_t command;
    uint32_t length;
    std::vector<uint8_t> data;

    void setOriginalData(const std::vector<uint8_t> &original)
    {
        data = original;
    }
};

struct Record
{
    bool check_next;
    uint32_t condition_count;
    uint32_t action_count;
    std::string name;
    std::vector<Condition> conditions;
    std::vector<Action> actions;
};

struct Block
{
    std::string name;
    std::vector<Record> records;
};

struct QSDFile
{
    uint32_t file_version;
    uint32_t block_count;
    std::string qsd_name;
    std::vector<Block> blocks;
};

std::string convert(const std::string& eucKrStr);
std::string readShortString(std::ifstream& file);
void loadQSDFile(const std::string& filename, QSDFile& qsd_file);
void saveQSD(const std::string& filename, QSDFile& qsd_file);


int main()
{
    QSDFile qsd_file;
    std::string filename = "PVP13-01.QSD";

    loadQSDFile(filename, qsd_file);

    std::cout << "\nQSD:" << std::endl;
    std::cout << "File Version: " << qsd_file.file_version << std::endl;
    std::cout << "Block Count: " << qsd_file.block_count << std::endl;
    std::cout << "QSD Name: " << qsd_file.qsd_name << std::endl;

    for (size_t b = 0; b < qsd_file.blocks.size(); ++b)
    {
        const auto &block = qsd_file.blocks[b];
        std::cout << "\nBlock " << b << ": " << block.name << std::endl;

        for (size_t r = 0; r < block.records.size(); ++r)
        {
            const auto &record = block.records[r];
            std::cout << "  Record " << r << ": " << record.name << std::endl;
            std::cout << "    Conditions: " << record.conditions.size() << std::endl;
            std::cout << "    Actions: " << record.actions.size() << std::endl;
        }
    }

    saveQSD("PVP13-01.QSD", qsd_file);

    return 0;
}

// AI generated convertion function.
std::string convert(const std::string &eucKrStr)
{
    int wideLen = MultiByteToWideChar(949, 0, eucKrStr.c_str(), -1, nullptr, 0);
    if (wideLen == 0)
        return "";
    std::wstring wideStr(wideLen, L'\0');
    MultiByteToWideChar(949, 0, eucKrStr.c_str(), -1, &wideStr[0], wideLen);
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Len == 0)
        return "";
    std::string utf8Str(utf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Len, nullptr, nullptr);
    return utf8Str;
}

std::string readShortString(std::ifstream &file)
{
    uint16_t length; 
    file.read(reinterpret_cast<char *>(&length), 2);
    std::string str(length, '\0');
    file.read(str.data(), length);

    str.erase(std::find(str.begin(), str.end(), '\0'), str.end());

    return str;
}

void loadQSDFile(const std::string &filename, QSDFile &qsd_file)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    file.read(reinterpret_cast<char *>(&qsd_file.file_version), 4);
    file.read(reinterpret_cast<char *>(&qsd_file.block_count), 4);
    qsd_file.qsd_name = readShortString(file);

    std::cout << "File version: " << qsd_file.file_version << std::endl;
    std::cout << "Block count: " << qsd_file.block_count << std::endl;
    std::cout << "QSD name: " << convert(qsd_file.qsd_name) << std::endl;

    for (uint32_t i = 0; i < qsd_file.block_count; ++i)
    {
        Block block;

        uint32_t record_count;
        file.read(reinterpret_cast<char *>(&record_count), 4);
        block.name = readShortString(file);

        std::cout << "Block " << i << ": " << convert(block.name) << ", Records: " << record_count << std::endl;

        for (uint32_t r = 0; r < record_count; ++r)
        {
            Record record;

            uint8_t check_next;
            file.read(reinterpret_cast<char *>(&check_next), 1);
            record.check_next = (check_next != 0);

            file.read(reinterpret_cast<char *>(&record.condition_count), 4);
            file.read(reinterpret_cast<char *>(&record.action_count), 4);
            record.name = convert(readShortString(file));

            std::cout << "  Record " << r << ": " << record.name
                      << ", Conditions: " << record.condition_count
                      << ", Actions: " << record.action_count << std::endl;

            for (uint32_t c = 0; c < record.condition_count; ++c)
            {
                try
                {
                    Condition condition;

                    file.read(reinterpret_cast<char *>(&condition.length), 4);
                    file.read(reinterpret_cast<char *>(&condition.command), 4);

                    std::vector<uint8_t> data(condition.length - 8);
                    file.read(reinterpret_cast<char *>(data.data()), condition.length - 8);

                    condition.setOriginalData(data);
                    record.conditions.push_back(condition);

                    std::cout << "    Condition " << c << ": Command: " << condition.command
                              << ", Length: " << condition.length << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing condition: " << e.what() << std::endl;

                    Condition condition;
                    record.conditions.push_back(condition);
                }
            }

            for (uint32_t a = 0; a < record.action_count; ++a)
            {
                try
                {
                    Action action;

                    file.read(reinterpret_cast<char *>(&action.length), 4);

                    uint32_t raw_command;
                    file.read(reinterpret_cast<char *>(&raw_command), 4);
                    action.command = raw_command & 0x0000FFFF;

                    std::vector<uint8_t> data(action.length - 8);
                    file.read(reinterpret_cast<char *>(data.data()), action.length - 8);

                    action.setOriginalData(data);
                    record.actions.push_back(action);

                    std::cout << "    Action " << a << ": Command: " << action.command
                              << ", Length: " << action.length << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing action: " << e.what() << std::endl;

                    Action action;
                    record.actions.push_back(action);
                }
            }

            block.records.push_back(record);
        }

        qsd_file.blocks.push_back(block);
    }

    file.close();
}

void saveQSD(const std::string& filename, QSDFile &qsd_file)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char *>(&qsd_file.file_version), 4);
    file.write(reinterpret_cast<const char *>(&qsd_file.block_count), 4);
    uint16_t name_length = static_cast<uint16_t>(qsd_file.qsd_name.length() + 1);
    file.write(reinterpret_cast<const char *>(&name_length), 2);
    file.write(qsd_file.qsd_name.c_str(), name_length);

    for (const auto &block : qsd_file.blocks)
    {
        uint32_t record_count = static_cast<uint32_t>(block.records.size());
        file.write(reinterpret_cast<const char *>(&record_count), 4);
        uint16_t block_name_length = static_cast<uint16_t>(block.name.length() + 1);
        file.write(reinterpret_cast<const char *>(&block_name_length), 2);
        file.write(block.name.c_str(), block_name_length);

        for (const auto &record : block.records)
        {
            uint8_t check_next = record.check_next ? 1 : 0;
            file.write(reinterpret_cast<const char *>(&check_next), 1);
            file.write(reinterpret_cast<const char *>(&record.condition_count), 4);
            file.write(reinterpret_cast<const char *>(&record.action_count), 4);
            uint16_t record_name_length = static_cast<uint16_t>(record.name.length() + 1);
            file.write(reinterpret_cast<const char *>(&record_name_length), 2);
            file.write(record.name.c_str(), record_name_length);

            for (const auto &condition : record.conditions)
            {
                file.write(reinterpret_cast<const char *>(&condition.length), 4);
                file.write(reinterpret_cast<const char *>(&condition.command), 4);
                file.write(reinterpret_cast<const char *>(condition.data.data()), condition.data.size());
            }

            for (const auto &action : record.actions)
            {
                file.write(reinterpret_cast<const char *>(&action.length), 4);
                file.write(reinterpret_cast<const char *>(&action.command), 4);
                file.write(reinterpret_cast<const char *>(action.data.data()), action.data.size());
            }
        }
    }
    file.close();
}

