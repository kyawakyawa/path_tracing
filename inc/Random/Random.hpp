#pragma once

#include<climits>

namespace Random {

    inline static unsigned long long  xor128(void) {
    // x, y, z, w が乱数のシード
        static unsigned long long x=123456789,y=362436069,z=521288629,w=88675123;
        unsigned long long t;
        t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
    }

    inline static R rando(){
    	unsigned long long n = xor128();

    	R M = (R)ULLONG_MAX;
    	M += 1.0;
    	return (R)n / M;
    }
}