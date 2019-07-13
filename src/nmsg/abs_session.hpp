#pragma once

//#include "../scheduler.hpp"

#include "net_naming.hpp"
#include "msg.hpp"

#include "rlib/common.hpp"

namespace r2
{
/*!
    Session manages a connection from myself to a specific remote point.
*/
#define R2Future class
template <R2Future Future>
class Session
{
public:
    constexpr double no_timeout = std::numeric_limits<double>::max();
    /*!
        Send a message, and spawn a future in the scheduler

        \param msg: the sending msg
        \param timeout: time out recorded in **ms**

        usage example:
            auto future = session.send(msg,120);        
            R2_SPAWN(future);
            R2_YIELD;
     */
    virtual Future
    send(Msg *msg,
         const double timeout = no_timeout) = 0;

    /*!
        A blocking version of send.

        \param msg: the sending msg
        \param timeout: time out recorded in **ms**

        usage example:
            auto ret = session.send_blocking(msg,120); // 120 ms
            ASSERT(ret == SUCC);
     */
    virtual rdmaio::IOStatus
    send_blocking(Msg *msg,
                  const double timeout = no_timeout) = 0;
};

} // namespace r2