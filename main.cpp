#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <memory>


class Parser {
    typedef unsigned int u1;
    typedef unsigned int u2;
    typedef unsigned int u4;
public:
    std::vector<char> bytes;

    explicit Parser(char *const fileName)
            : bytes(getVector(fileName)) {
    }

    enum Constants {
        CONSTANT_Class = 7,
        CONSTANT_Fieldref = 9,
        CONSTANT_Methodref = 10,
        CONSTANT_InterfaceMethodref = 11,
        CONSTANT_String = 8,
        CONSTANT_Integer = 3,
        CONSTANT_Float = 4,
        CONSTANT_Long = 5,
        CONSTANT_Double = 6,
        CONSTANT_NameAndType = 12,
        CONSTANT_Utf8 = 1,
        CONSTANT_MethodHandle = 15,
        CONSTANT_MethodType = 16,
        CONSTANT_InvokeDynamic = 18,
    };


    struct CONSTANT_abstract {
        int size;

        explicit CONSTANT_abstract(int s) : size(s) {};
    };

    struct cp_info {
        u1 tag{};
        std::vector<u1> info;
        std::shared_ptr<CONSTANT_abstract> c_info;
    };

    struct CONSTANT_Fieldref_info : CONSTANT_abstract {
        u1 tag{};
        u2 class_index{};
        u2 name_and_type_index{};

        CONSTANT_Fieldref_info(std::vector<char> &bytes, unsigned long &offset)
                : CONSTANT_abstract(5),
                  tag(CONSTANT_Fieldref),
                  class_index(getU2(bytes, offset)),
                  name_and_type_index(getU2(bytes, offset)) {};
    };

    struct CONSTANT_Methodref_info : CONSTANT_abstract {
        u1 tag{};
        u2 class_index{};
        u2 name_and_type_index{};

        CONSTANT_Methodref_info(std::vector<char> &bytes, unsigned long &offset)
                : CONSTANT_abstract(5),
                  tag(CONSTANT_Methodref),
                  class_index(getU2(bytes, offset)),
                  name_and_type_index(getU2(bytes, offset)) {};
    };

    struct CONSTANT_InterfaceMethodref_info : CONSTANT_abstract {
        u1 tag{};
        u2 class_index{};
        u2 name_and_type_index{};

        CONSTANT_InterfaceMethodref_info(std::vector<char> &bytes, unsigned long &offset) :
                CONSTANT_abstract(5),
                tag(CONSTANT_InterfaceMethodref),
                class_index(getU2(bytes, offset)),
                name_and_type_index(getU2(bytes, offset)) {};
    };

    struct CONSTANT_MethodType_info : CONSTANT_abstract {
        u1 tag;
        u2 descriptor_index;

        CONSTANT_MethodType_info(std::vector<char> &bytes, unsigned long &offset)
                : CONSTANT_abstract(3),
                  tag(CONSTANT_MethodType),
                  descriptor_index(getU2(bytes, offset)) {}
    };

    struct CONSTANT_Class_info : CONSTANT_abstract {
        u1 tag;
        u2 name_index;

        CONSTANT_Class_info(std::vector<char> &bytes, unsigned long &offset)
                : CONSTANT_abstract(3),
                  tag(CONSTANT_Class),
                  name_index(getU2(bytes, offset)) {};
    };

    struct CONSTANT_Utf8_info : CONSTANT_abstract {
        u1 tag;
        u2 length;
        std::vector<u1> bytes;

        CONSTANT_Utf8_info(std::vector<char> &b, unsigned long &offset)
                : tag(CONSTANT_Utf8),
                  length(getU2(b, offset)),
                  CONSTANT_abstract(0) {
            size = length + 2;
            bytes = getBytesVector(b, offset, (unsigned long) length);
        };
    };

    struct CONSTANT_NameAndType_info : CONSTANT_abstract {
        u1 tag;
        u2 name_index;
        u2 descriptor_index;

        CONSTANT_NameAndType_info(std::vector<char> &bytes, unsigned long &offset)
                : CONSTANT_abstract(5),
                  tag(CONSTANT_NameAndType),
                  name_index(getU2(bytes, offset)),
                  descriptor_index(getU2(bytes, offset)) {};
    };

    struct ClassFile {
        u4 magic;
        u2 minor_version;
        u2 major_version;
        u2 constant_pool_count;
        u2 methods_count;
        u2 fields_count;
        u2 access_flags;
        u2 this_class;
        u2 super_class;
        u2 interfaces_count;
        u2 attributes_count;

        std::vector<cp_info> constant_pool;
        std::vector<u2> interfaces;
        std::vector<u2> fields;
        std::vector<u2> methods;
        std::vector<u2> attributes;
    };

    static u4 getMagicNum(std::vector<char> &bytes, unsigned long &offset) {
        u4 temp = 0;
        unsigned long stopPoint = offset + 4;
        while (offset < stopPoint) {
            temp = 0x100 * temp + (unsigned int) ((unsigned char) bytes[offset]);
            ++offset;
        }
        return temp;
    }

    static u2 getU2(std::vector<char> &bytes, unsigned long &offset) {
        u2 temp = 0;
        unsigned long stopPoint = offset + 2;
        while (offset < stopPoint) {
            temp = 0x100 * temp + (unsigned int) ((unsigned char) bytes[offset]);
            ++offset;
        }
//        std::cout << "u2: " << temp << std::endl;
        return temp;
    }

    static u1 getU1(std::vector<char> &bytes, unsigned long &offset) {
        return static_cast<u1>((unsigned char) bytes[offset++]);
    }

    static std::vector<u1> getBytesVector(std::vector<char> &bytes, unsigned long &offset, unsigned long size) {
        std::vector<u1> vector;
        for (int i = 0; i < size; ++i)
            vector.push_back(getU1(bytes, offset));
        return vector;
    }

    ClassFile parseClassFile() {
        std::cout << "Type " << typeid(float).name() << std::endl;
        ClassFile classFile{};
        unsigned long length = bytes.size(), i = 0, end;

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
                    throw std::runtime_error(std::string("Implement next constants ") +
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

    std::vector<char> getVector(char *const fileName) {
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
};

int main(int argc, char **argv) {
    Parser parser(argv[1]);
    Parser::ClassFile classFile = parser.parseClassFile();

    auto i = classFile.constant_pool;
    std::cout << classFile.constant_pool.size() << std::endl << std::endl;
//    for (char & ch : parser.bytes)
//        std::cout << std::hex << (int) ((unsigned char) ch) << std::endl;
    return 0;
}