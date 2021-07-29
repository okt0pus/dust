#include <iostream>

#include "../include/saturating_types.hpp"

typedef xint_sat_t<unsigned char> uint8_sat_t;

std::size_t test()
{
    
    // return integral_types<unsigned long, double, int>::value;
    // return one_signed_types<long, unsigned char>::value;
    // return max_sizeof<unsigned char, char, char, long long>::value;
    //return sizeof(next_up<char, short>::type);
    //xint_sat_t<unsigned char> t1{ 250U };
    //xint_sat_t<unsigned char> t2{ 6U };
    //return sizeof(t1 + t2);

    const uint8_sat_t x = static_cast<uint8_sat_t>(23.23);
    const uint8_sat_t y = static_cast<uint8_sat_t>(233.43);

    const uint8_sat_t res = x - y;
    //return x;
    // uint8_sat_t::clamp<uint8_sat_t::type>(x);
    // uint8_sat_t::clamp<uint8_t>(x);
    return res;
}

int main()
{
	std::cout << test() << std::endl;
	return 0;
}