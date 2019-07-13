#pragma once

//#include "../scheduler.hpp"

#include "net_naming.hpp"
#include "msg.hpp"

#include "rlib/common.hpp"

namespace r2
{
/*!
    Session manages a connection from myself to a specific remote point
*/
#define R2Future class
template <R2Future Future>
class Session
{
public:
    /*!
        Send a message, and spawn a future in the scheduler
        usage example:
            auto future = session.send(msg);        
            R2_SPAWN(future);
            R2_YIELD;
     */
    Future send(Msg *msg) = 0;
};

} // namespace r2