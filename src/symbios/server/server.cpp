#include <symbios/server/server.h>

symbios::Server::Server(){
    SYMBIOS_CONF->ConfigureSymbiosServer();
    auto basket=BASKET_CONF;
    rpc=basket::Singleton<RPCFactory>::GetInstance()->GetRPC(BASKET_CONF->RPC_PORT);
    std::function<int(Data)> functionPosixRequest(std::bind(&Server::PosixRequest,this,std::placeholders::_1));
    rpc->bind("Posix_Request", functionPosixRequest);
}

void symbios::Server::RunInternal(std::future<void> futureObj) {
    while(futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout){
        usleep(10000);
    }
}

int symbios::Server::PosixRequest(Data req){
    printf("Call from client\n");
    return rand();
}

void symbios::Server::Run(std::future<void> futureObj) {
    RunInternal(std::move(futureObj));
}