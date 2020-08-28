//
// Created by mani on 8/24/2020.
//

#include <symbios/io_clients/file_io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <symbios/common/error_code.h>

void FileIOClient::Read(Data &source, Data &destination) {
    char* fileName = source.id_.c_str();
    int fileFd = open(fileName, O_RDONLY);
    if(fileFd == -1){
        throw ErrorException(OPEN_FILE_FAILED);
    } else {
        if(lseek(fileFd, source.position_, SEEK_SET) == -1){
            close(fileFd);
            throw ErrorException(SEEK_FILE_FAILED);
        } else{
            ssize_t data_size = read(fileFd, destination.buffer_ + destination.position, source.data_size_);
            if (data_size == source.data_size_){
                // read data from file successful
                destination.data_size_ = data_size;
                close(fileFd);
            } else {
                // read data from file failed.
                close(fileFd);
                throw ErrorException(READ_DATA_FROM_FILE_FAILED);
            }
        }
    }
}

void FileIOClient::Write(Data &source, Data &destination) {
    char* destFileName = destination.id_.c_str();
    int fileFd = open (destFileName, O_RDWR | O_CREAT | O_APPEND, 0644);
    if(fileFd == -1){
        throw ErrorException(OPEN_FILE_FAILED);
    } else {
        if(lseek(fileFd, destination.position_, SEEK_SET) == -1){
            close(fileFd);
            throw ErrorException(SEEK_FILE_FAILED);
        } else {
            ssize_t size = write(fileFd, source.buffer_ + source.position_, source.data_size_);
            if (size < source.data_size_){
                // write data to file failed
                close(fileFd);
                throw ErrorException(WRITE_DATA_TO_FILE_FAILED);
            }
            close(fileFd);
        }
    }
}