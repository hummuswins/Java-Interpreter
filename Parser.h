//
// Created by james on 4/10/2018.
//

#ifndef JAVAINTERPRETER_PARSER_H
#define JAVAINTERPRETER_PARSER_H

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

    explicit Parser(char *fileName);

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

        explicit CONSTANT_abstract(int s);;
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

        CONSTANT_Fieldref_info(std::vector<char> &bytes, unsigned long &offset);;
    };

    struct CONSTANT_Methodref_info : CONSTANT_abstract {
        u1 tag{};
        u2 class_index{};
        u2 name_and_type_index{};

        CONSTANT_Methodref_info(std::vector<char> &bytes, unsigned long &offset);;
    };

    struct CONSTANT_InterfaceMethodref_info : CONSTANT_abstract {
        u1 tag{};
        u2 class_index{};
        u2 name_and_type_index{};

        CONSTANT_InterfaceMethodref_info(std::vector<char> &bytes, unsigned long &offset);;
    };

    struct CONSTANT_MethodType_info : CONSTANT_abstract {
        u1 tag;
        u2 descriptor_index;

        CONSTANT_MethodType_info(std::vector<char> &bytes, unsigned long &offset);
    };

    struct CONSTANT_Class_info : CONSTANT_abstract {
        u1 tag;
        u2 name_index;

        CONSTANT_Class_info(std::vector<char> &bytes, unsigned long &offset);;
    };

    struct CONSTANT_Utf8_info : CONSTANT_abstract {
        u1 tag;
        u2 length;
        std::vector<u1> bytes;

        CONSTANT_Utf8_info(std::vector<char> &b, unsigned long &offset);;
    };

    struct CONSTANT_NameAndType_info : CONSTANT_abstract {
        u1 tag;
        u2 name_index;
        u2 descriptor_index;

        CONSTANT_NameAndType_info(std::vector<char> &bytes, unsigned long &offset);;
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

    static u4 getMagicNum(std::vector<char> &bytes, unsigned long &offset);

    static u2 getU2(std::vector<char> &bytes, unsigned long &offset);

    static u1 getU1(std::vector<char> &bytes, unsigned long &offset);

    static std::vector<u1> getBytesVector(std::vector<char> &bytes, unsigned long &offset, unsigned long size);

    ClassFile parseClassFile();

    std::vector<char> getVector(char *fileName);
};

#endif //JAVAINTERPRETER_PARSER_H
