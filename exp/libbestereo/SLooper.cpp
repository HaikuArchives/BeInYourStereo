/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include "SLooper.h" 
#include <Messenger.h> 
#include <stdio.h> 

#define FORMULATE_UNIQUE(x) \
        int _x_length = strlen(x); \
		char unique[_x_length+2]; \
		strcpy(unique, x); \
		unique[_x_length] = _x_length;               \
		unique[_x_length+1] = 0;

/* Design: 
        - bind to a port, listening for a message to come in.  Put that port on a unique name so it can be found. 
        - the message ID is ignored 
        - the message payload is a port_id 
        
        - send back to that port_id a flattened BMessenger 
*/ 
SLooper::SLooper(char *unique_name, int32 prio, int32 cap) 
 : BLooper(unique_name, prio, cap) { 
        // make a port with a unique name
        FORMULATE_UNIQUE(unique_name);
        
        finder = create_port(10, unique); 
        
        // spawn a thread to answer requests to find ourselves 
        finderThread = spawn_thread(RunFinderThread, "message loop finder thread", B_DISPLAY_PRIORITY, this); 
        resume_thread(finderThread); 
} 

SLooper::~SLooper() { 
        status_t ignored; 

        // closing the thread causes the thread to exit 
        close_port(finder); 
        wait_for_thread(finderThread, &ignored); 
} 


////////// ==== 
// Facilities for finding this BLooper 
int32 SLooper::RunFinderThread(void *data) { return ((SLooper*)data)->FinderThread(); } 
int32 SLooper::FinderThread() { 
        status_t ignored; 

        BMessage msg; 

        // add a BMessenger into msg 
        BMessenger myMessenger(this,this, &ignored); 
        msg.AddMessenger("messageTarget", myMessenger); 
        
        //Flatten it 
        ssize_t messengerBufferLength = msg.FlattenedSize(); 
        char *messengerBuffer = new char[messengerBufferLength]; 
        msg.Flatten(messengerBuffer, messengerBufferLength); 

        // wait for requests to come in 
        port_id toRespondTo; 

        while (read_port(finder, &ignored, &toRespondTo, sizeof(toRespondTo))>=0) { 
                // send requests back to the port ID we were given 
                write_port(toRespondTo, 0, messengerBuffer, messengerBufferLength); 
        } 

        delete messengerBuffer; 
        return 0; 
} 

//////////// == 
// Function to find a remote BLooper 
BMessenger *SLooper::FindOtherEnd(char *unique_name) { 
        BMessenger *rc = new BMessenger(); 
        int32 ignored; 

        FORMULATE_UNIQUE(unique_name);
        
        // find port by name 
        port_id port = find_port(unique); 
        if (port < 0) { 
                printf("No other-end found for %s\n", unique_name); 
                return NULL; 
        } 

        // create a port for the reply
        
        port_id myPort = create_port(1, "temporary port for answer"); 
       
        // send the request to the other end with the port of our end 
        write_port(port, 0xdeadbeef, &myPort, sizeof(myPort)); 
        
        // wait for an answer 
        ssize_t messageSize = port_buffer_size(myPort); 
        char *message = new char[messageSize]; 
        read_port(myPort, &ignored, message, messageSize); 

        // get the BMessenger out of it 
        BMessage msg; 
        msg.Unflatten(message); 
        msg.FindMessenger("messageTarget", rc); 
        
        return rc; 
}
