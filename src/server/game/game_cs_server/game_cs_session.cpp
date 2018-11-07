#include "game_cs_session.h"


namespace zq {



GameCSSession::GameCSSession(tcp::socket&& socket) : Socket(std::move(socket))
{
   
}

void GameCSSession::start()
{

}

bool GameCSSession::update()
{
    if (!BaseSocket::update())
        return false;

    return true;
}

void GameCSSession::readHandler()
{

    asyncRead();
}


}
