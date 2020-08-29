//
// Created by Jie on 8/25/20.
//

#include <symbios/io_clients/redis_io.h>
#include <cstring>
#include <basket/common/singleton.h>
#include <symbios/common/configuration_manager.h>
#include <symbios/common/error_codes.h>


void RedisIOClient::Read(Data &source, Data &destination) {
    try {

        auto resp = m_redisCluster->get(source.id_.c_str());
        if(resp){
            std::string value = *resp;
            std::string::size_type value_size = value.length();
            if(source.position_ + source.data_size_ > value_size ){
                throw ErrorException(READ_REDIS_POSITION_OR_SIZE_FAILED);
            }
            else {
                // read data from Redis successful
                memcpy(destination.buffer_, value.c_str() + source.position_, source.data_size_);
                destination.data_size_ = source.data_size_;
            }
        } else {
            throw ErrorException(READ_REDIS_DATA_FAILED);
        }
    } catch (const Error &err){
        throw ErrorException(REDIS_SERVER_SIDE_FAILED);
    }
}

void RedisIOClient::Write(Data &source, Data &destination) {
    try {
        auto resp = m_redisCluster->get(destination.id_.c_str());
        if(resp) {
            // The key has been existed in redis cluster.
            std::string old_value = *resp;
            std::string::size_type old_value_size = old_value.length();
            if (source.data_size_ >= old_value_size){
                std::string new_value = std::string((char*)(source.buffer_ + source.position_), source.data_size_);
                bool result = m_redisCluster->set(destination.id_.c_str(), new_value);
                if (!result) {
                    throw ErrorException(WRITE_REDIS_DATA_FAILED);
                }
                destination.data_size_ = source.data_size_;
            }
            else
            {
                // update the old_value
                memcpy((void*)old_value.c_str(), source.buffer_ + source.position_, source.data_size_);
                // put the updated data back
                bool result = m_redisCluster->set(destination.id_.c_str(), old_value);
                if (!result) {
                    throw ErrorException(WRITE_REDIS_DATA_FAILED);
                }
                destination.data_size_ = source.data_size_;
            }
        }
        else {
            // The key isn't exist in redis cluster
            std::string value = std::string((char*)(source.buffer_ + source.position_), source.data_size_);
            bool result = m_redisCluster->set(destination.id_.c_str(), value);
            if (!result) {
                throw ErrorException(WRITE_REDIS_DATA_FAILED);
            }
            destination.data_size_ = source.data_size_;
        }

    } catch (const Error &err){
        throw ErrorException(REDIS_SERVER_SIDE_FAILED);
    }
}
