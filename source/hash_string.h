/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <hash_map>

template <>
    class hash<string> 
    { 
        public: 
            size_t operator()(string const &str) const 
            { 
                hash<char const *> 
                    h; 

                return (h(str.c_str())); 
            } 
    }; 
