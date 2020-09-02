//
// Created by neeraj on 9/1/20.
//

#include "iris.h"
#include <common/debug.h>
#include <symbios/common/enumerations.h>
#include <symbios/client/client.h>
#include <common/debug.h>

#define  MAX_OBJ_SIZE 4


std::ostream &operator<<(std::ostream &os, DataDescriptor &m) {
    return os << "{position_:" << m.position_ << ","
              << "size:" << m.size << "}";
}

int main(int argc, char * argv[]){
    auto tracer=common::debug::AutoTrace(std::string("iris_emu::main"));
    MPI_Init(&argc,&argv);
    MPI_Barrier(MPI_COMM_WORLD);
    if(argc > 1) SYMBIOS_CONF->CONFIGURATION_FILE=argv[1];
    BASKET_CONF->BACKED_FILE_DIR=SYMBIOS_CONF->SERVER_DIR;

    std::string data = "Hello Symbios";  // argv[2]
    auto type_ = IOClientType ::FILE_IO; //argv[3]
    auto file_ = "iris_emu_file3"; //argv[4]
    auto max_obj_size = MAX_OBJ_SIZE; //argv[5]
    DataMapper mapper_(type_, max_obj_size);

    DataDescriptor src = {file_,0,  data.size(), 0 };
    auto objs = mapper_.generateDataObjects(src);
//    COMMON_DBGVAR(objs);

    doOp operation(type_);
//    std::cout<<"Writing Data"<<std::endl;
    for (auto &i : objs){
        auto data_obj = Data();
        data_obj.id_= i.id_;

        data_obj.position_=i.position_;
        data_obj.buffer_= data.substr(i.position_+i.chunk_index*max_obj_size, i.size);
        data_obj.storage_index_ = type_;

        COMMON_DBGVAR(data_obj);
        operation.Write(data_obj, data_obj);
//        .buffer_=std::string().data();
    }

    DataDescriptor read_src = {file_, 0,  5, 0 };
    objs = mapper_.generateDataObjects(read_src);

//    std::cout<<"Reading Data"<<std::endl;
    for (auto &i : objs){
        auto data_obj = Data();
        data_obj.id_= i.id_;

        data_obj.position_=i.position_;
        data_obj.buffer_.resize(i.size);
        data_obj.storage_index_ = type_;

        COMMON_DBGVAR2(data_obj, i);
        operation.Read(data_obj, data_obj);
        COMMON_DBGVAR2(data_obj, i);
        std::cout<<data_obj.buffer_<<std::endl;
    }

    MPI_Finalize();
    return 0;
}