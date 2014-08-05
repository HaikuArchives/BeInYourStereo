/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#ifndef _SLOOPER_H
#define _SLOOPER_H

#include <Looper.h> 
class BMessenger; 

/** 
Class SLooper (for Steve's Looper) provides an easy means of connecting two 
BeOS applications via the Looper/Messenger construct. 

It extends BLooper by adding a port-based discovery protocol based on a unique 
name. 

If you have a message loop in application A that you want to discover from 
remote applications B, C, D... replace it with a SLooper.  Construct the 
SLooper with a name that is unique on the local BeOS machine - a huge 
random number would be a fine choice. 

Then, in applications, B, C, and D, use the FindOtherEnd method with that 
unique name to obtain a BMessenger that can deliver messages into 
application A. 

Refer to the BMessenger and BLooper BeBook entries for more detail. 
*/ 
class SLooper : public BLooper { 
public: 

        /** 
                Buids a SLooper with a unique identifying name. 
                The remaining arguments are passed to the BLooper constructor. 
        */ 
        SLooper(char *unique_name, 
                int32 priority = B_NORMAL_PRIORITY, 
                int32 portCapacity = B_LOOPER_PORT_DEFAULT_CAPACITY); 

        /** 
                Locates a SLooper in a different application and returns 
                a BMessenger that can deliver messages to it. 
        */ 
        static BMessenger *FindOtherEnd(char *unique_name); 

protected: 
        /** 
                The destructor is not called directly. Use Quit(). 
        */ 
        virtual ~SLooper(); 
        
private: 
        BMessenger *myMessenger; 

        port_id finder; 
        thread_id finderThread; 
        
        static int32 RunFinderThread(void *data); 
        int32 FinderThread(); 
};

#endif
