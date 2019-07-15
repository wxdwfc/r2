#pragma once 

#include "rlib/common.hpp"

namespace r2 {

/**
 *  metadata in server:
 *       count, addr, client latest pointer, session size, ...
 *  think more to abstract these to some meaningful class
 */

    /**
     *  a class for connnect info 
     */
    class Session {
        public:
            int size = 0;
            
        private:
    }

    /**
     *  msg interface 
     */
    class Message {
        public:
            int size = 0;
    }

    /**
     *  deal with comminication protocol
     */
    class Controller {

    }

}