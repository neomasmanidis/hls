#include <iostream>
#include <stdlib.h>
#include <bitset>
#include "ac_int.h"
#include "ac_channel.h"

void ask1(ac_channel<ac_int<32,false> > &input,ac_channel<ac_int<16,false> > &output){
    ac_int<32,false> data;
    ac_int<16,false> count_aces = 0;

    if (input.available(1))
        data = input.read();

    for (int i=0; i<32; i++){
        if (data[i] == 1)
            count_aces ++;
    }

    output.write(count_aces);
}


void ask2(ac_channel<ac_int<32,false> > &input,ac_channel<ac_int<32,false> > &output){

    ac_int<32,false> number;
    ac_int<8,false> first;
    ac_int<8,false> second;
    ac_int<32,false> checksum = 0;

    if (input.available(1))
        number = input.read();

    while(number>0){
        //1st digit as is
        first = number%10;

        //update number
        number = number/10;

        //2nd digit
        second = (number%10)<<1;

        if (second>10)
            second = second/10 + second%10;    
        
        //update number
        number = number/10;

        //update checksum
        checksum += first + second;
    }

    output.write(checksum);
}

int main(){

    int rand_num = rand() % 1000;
    rand_num = 69;
    std::bitset<32> x(rand_num);
    std::cout << "Count aces in: "<< rand_num << "(" << x <<")"<< std::endl;


    ac_channel<ac_int<32,false> > input_test1;
    ac_channel<ac_int<16,false> > output_test1;

    input_test1.write(rand_num);

    while(input_test1.available(1))
        ask1(input_test1,output_test1);

     std::cout << output_test1 << std::endl;

    ac_channel<ac_int<32,false> > input_test2;
    ac_channel<ac_int<32,false> > output_test2;

    std::cout << "Decimal checksum for: "<< rand_num << std::endl;

    input_test2.write(rand_num);

    while(input_test2.available(1))
        ask2(input_test2,output_test2);

     std::cout << output_test2 << std::endl;
}