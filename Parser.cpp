#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <memory>
#include "Parser.h"

namespace {
    struct runtime : std::runtime_error {
        explicit runtime(const std::string &msg) : std::runtime_error(msg) {}
    };
}

int main(int argc, char **argv) {
    Parser parser(argv[1]);
    Parser::ClassFile classFile = parser.parseClassFile();

    auto i = classFile.constant_pool;
    std::cout << classFile.constant_pool.size() << std::endl << std::endl;
    for (char &ch : parser.bytes)
        std::cout << std::hex << (int) ((unsigned char) ch) << std::endl;
    return 0;
}

Parser::Parser(char *const fileName)
        : bytes(getVector(fileName)) {}

Parser::u4 Parser::getMagicNum(std::vector<char> &bytes, unsigned long &offset) {
    u4 temp = 0;
    unsigned long stopPoint = offset + 4;
    while (offset < stopPoint) {
        temp = 0x100 * temp + (unsigned int) ((unsigned char) bytes[offset]);
        ++offset;
    }
    return temp;
}

Parser::u2 Parser::getU2(std::vector<char> &bytes, unsigned long &offset) {
    u2 temp = 0;
    unsigned long stopPoint = offset + 2;
    while (offset < stopPoint) {
        temp = 0x100 * temp + (unsigned int) ((unsigned char) bytes[offset]);
        ++offset;
    }
//        std::cout << "u2: " << temp << std::endl;
    return temp;
}

std::vector<char> Parser::getVector(char *const fileName) {
    std::ifstream ifs(fileName, std::ios_base::binary | std::ios_base::ate);
    std::basic_ifstream<char>::pos_type pos = ifs.tellg();

    std::cout << fileName << pos << std::endl;
    std::vector<char> result(static_cast<unsigned long>(pos));
    ifs.seekg(0, std::ios_base::beg);
    ifs.read(result.data(), pos);
//        for (char & ch : result)
//            std::cout << std::hex << (int) ((unsigned char) ch) << std::endl;
    return result;
}

Parser::ClassFile Parser::parseClassFile() {
    std::cout << "Type " << typeid(float).name() << std::endl;
    ClassFile classFile{};
    unsigned long i = 0, end;

    // Magic number
    classFile.magic = getMagicNum(bytes, i);
    classFile.minor_version = getU2(bytes, i);
    classFile.major_version = getU2(bytes, i);
    classFile.constant_pool_count = getU2(bytes, i);

    std::vector<u1> s;
    end = classFile.constant_pool_count;
    for (int index = 1; index < end; ++index) {
        cp_info temp;
        temp.tag = getU1(bytes, i);

        switch (Constants(temp.tag)) {
            case CONSTANT_Class:
                temp.c_info = std::shared_ptr<CONSTANT_abstract>(new CONSTANT_Class_info(bytes, i));
                break;
            case CONSTANT_Fieldref:
                temp.c_info = std::shared_ptr<CONSTANT_abstract>(new CONSTANT_Fieldref_info(bytes, i));
                break;
            case CONSTANT_Methodref:
                temp.c_info = std::shared_ptr<CONSTANT_abstract>(new CONSTANT_Methodref_info(bytes, i));
                break;
            case CONSTANT_InterfaceMethodref:
                temp.c_info = std::shared_ptr<CONSTANT_abstract>(new CONSTANT_InterfaceMethodref_info(bytes, i));
                break;
            case CONSTANT_MethodType:
                temp.c_info = std::shared_ptr<CONSTANT_abstract>(new CONSTANT_MethodType_info(bytes, i));
                break;
            case CONSTANT_Utf8:
                temp.c_info = std::shared_ptr<CONSTANT_abstract>(new CONSTANT_Utf8_info(bytes, i));
                break;
            case CONSTANT_NameAndType:
                temp.c_info = std::shared_ptr<CONSTANT_abstract>(new CONSTANT_NameAndType_info(bytes, i));
                break;


            default:
                throw runtime(std::string("Implement next constants ") +
                              std::to_string((unsigned int) temp.tag));
        }

        std::cout << "Constant " << index << ": " <<
                  temp.tag << " size " << (unsigned long) temp.c_info->size << std::endl;
        classFile.constant_pool.emplace_back(temp);
    }

    classFile.access_flags = getU2(bytes, i);
    classFile.this_class = getU2(bytes, i);
    classFile.super_class = getU2(bytes, i);
    classFile.interfaces_count = getU2(bytes, i);
    end = classFile.interfaces_count;
    for (int index = 1; index < end; ++index) {
        classFile.interfaces.emplace_back(getU2(bytes, i));
    }
    return classFile;
}

std::vector<Parser::u1> Parser::getBytesVector(std::vector<char> &bytes, unsigned long &offset, unsigned long size) {
    std::vector<u1> vector;
    for (int i = 0; i < size; ++i)
        vector.push_back(getU1(bytes, offset));
    return vector;
}

Parser::u1 Parser::getU1(std::vector<char> &bytes, unsigned long &offset) {
    return static_cast<u1>((unsigned char) bytes[offset++]);
}

Parser::CONSTANT_abstract::CONSTANT_abstract(int s) : size(s) {}

Parser::CONSTANT_Fieldref_info::CONSTANT_Fieldref_info(std::vector<char> &bytes, unsigned long &offset)
        : CONSTANT_abstract(5),
          tag(CONSTANT_Fieldref),
          class_index(getU2(bytes, offset)),
          name_and_type_index(getU2(bytes, offset)) {}

Parser::CONSTANT_Methodref_info::CONSTANT_Methodref_info(std::vector<char> &bytes, unsigned long &offset)
        : CONSTANT_abstract(5),
          tag(CONSTANT_Methodref),
          class_index(getU2(bytes, offset)),
          name_and_type_index(getU2(bytes, offset)) {}

Parser::CONSTANT_InterfaceMethodref_info::CONSTANT_InterfaceMethodref_info(std::vector<char> &bytes,
                                                                           unsigned long &offset) :
        CONSTANT_abstract(5),
        tag(CONSTANT_InterfaceMethodref),
        class_index(getU2(bytes, offset)),
        name_and_type_index(getU2(bytes, offset)) {}

Parser::CONSTANT_MethodType_info::CONSTANT_MethodType_info(std::vector<char> &bytes, unsigned long &offset)
        : CONSTANT_abstract(3),
          tag(CONSTANT_MethodType),
          descriptor_index(getU2(bytes, offset)) {}

Parser::CONSTANT_Class_info::CONSTANT_Class_info(std::vector<char> &bytes, unsigned long &offset)
        : CONSTANT_abstract(3),
          tag(CONSTANT_Class),
          name_index(getU2(bytes, offset)) {}

Parser::CONSTANT_Utf8_info::CONSTANT_Utf8_info(std::vector<char> &b, unsigned long &offset)
        : tag(CONSTANT_Utf8),
          length(getU2(b, offset)),
          CONSTANT_abstract(0) {
    size = length + 2;
    bytes = getBytesVector(b, offset, (unsigned long) length);
}

Parser::CONSTANT_NameAndType_info::CONSTANT_NameAndType_info(std::vector<char> &bytes, unsigned long &offset)
        : CONSTANT_abstract(5),
          tag(CONSTANT_NameAndType),
          name_index(getU2(bytes, offset)),
          descriptor_index(getU2(bytes, offset)) {}
