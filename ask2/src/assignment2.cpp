#include <iostream>
#include "ac_int.h"
#include "ac_channel.h"
#include "ac_fixed.h"


template <int W>
struct CSD {
    ac_int<W,false> x_p;
    ac_int<W,false> x_m;
};


template <int W>
void csd_encode (ac_int<W,true> &num, CSD<W> &num_csd){

    ac_int<W,false> num_abs;
    if (num>=0)
        num_abs = num;
    else
        num_abs = -num;

    ac_int<W,false> t_p;
    ac_int<W,false> t_m;
    ac_int<1,false> carry = 0;

    for (int i=0; i<W-1; i++){
        if (carry==0){
            if (num_abs[i]==0){
                t_p[i]=0;
                t_m[i]=0;
                carry =0;
            }
            else if (num_abs[i+1]==0){
                t_p[i]=1;
                t_m[i]=0;
                carry =0;
            }
            else{
                t_p[i]=0;
                t_m[i]=1;
                carry =1;             
            }
        }
        else{
            if (num_abs[i]==1){
                t_p[i]=0;
                t_m[i]=0;
                carry =1;
            }
            else if (num_abs[i+1]==1){
                t_p[i]=0;
                t_m[i]=1;
                carry =1;
            }
            else{
                t_p[i]=1;
                t_m[i]=0;
                carry =0;             
            }
        }
    }

    if (carry == 1){
        t_p[W-1]=1;
    }
    else{
        t_p[W-1]=0;
    }
    
    t_m[W-1]=0;
    
    if (num[W-1]==0){
        num_csd.x_p = t_p;
        num_csd.x_m = t_m;
    }
    else{
        num_csd.x_p = t_m;
        num_csd.x_m = t_p;
    }

}

template <int Wout, int W>
ac_int<Wout,true> csd_mult (ac_int<W,true> &in, const CSD<W> &constant_csd){

    ac_int<Wout,true> result = 0;
    ac_int<Wout,true> padded_in = in;

    for (int i = 0; i<W; i++){
        if (constant_csd.x_p[i]==1){
            result += padded_in<<i;
        }
        else if (constant_csd.x_m[i]==1){
            result -= padded_in<<i;
        }
    }

    return result;
}

void runlength_encode (ac_channel<ac_int<4,false> > &in, ac_channel<ac_int<4,false> > &out){

    ac_int<4,false> current;
    ac_int<4,false> count=0;
    ac_int<4,false> previous = 0;

    if (in.available(10)){
        while(in.available(1)){
            current = in.read();
            if (current==previous){
                count ++;
            }
            else{
                if (count>0){
                    out.write(previous);
                    out.write(count);
                }
                count = 1;
                previous = current;
            }
        }
        out.write(previous);
        out.write(count);
    }

}

int main(){

//Ask1 
ac_int<8,false> a = 255;
ac_int<8,false> b = 255;
ac_int<9,true> c  = 256;
ac_int<9,true> d  = 256;
ac_int<18,true> e = a*b + c*d;
ac_int<17,true> e_overflow = a*b + c*d;


std::cout<< "e : Correct result(18 bits): " << e << " Overflow(17 bits): "<< e_overflow << std::endl;

ac_fixed<5,2,false> f = 3.625;
ac_fixed<5,4,true>  g = 7;
ac_fixed<9,6,true> h = f*g;
ac_fixed<8,5,true> h_overflow = f*g;
ac_fixed<8,6,true> h_underflow = f*g;

std::cout<< "h : Correct result(9 bits, 6 int): " << h << ", Overflow(8 bits, 5 int): "<< h_overflow << ", Underflow(8 bits, 6 int): "<< h_underflow << std::endl<<std::endl;

CSD<10> num_csd_1;
ac_int<10,true> num_simple_1 = -505;

csd_encode(num_simple_1, num_csd_1);

std::cout<< "Number "<< num_simple_1<< " is "<< num_csd_1.x_p << " - " << num_csd_1.x_m << " in CSD"<< std::endl;

const CSD<10> num_const_csd_1 = num_csd_1;
ac_int<10,true> num_simple_2 = -500;
ac_int<19,true> num_mul = csd_mult<19>(num_simple_2, num_const_csd_1);
std::cout<< num_simple_2<< " x "<< num_simple_1 << " = " << num_mul << std::endl;

ac_channel<ac_int<4,false> > input_ask3;
ac_channel<ac_int<4,false> > output_ask3;

//input_ask3.write(2,2,2,2,3,12,12,1,1,5);
input_ask3.write(2);
input_ask3.write(2);
input_ask3.write(2);
input_ask3.write(2);
input_ask3.write(3);
input_ask3.write(12);
input_ask3.write(12);
input_ask3.write(1);
input_ask3.write(1);
input_ask3.write(5);

runlength_encode(input_ask3,output_ask3);
std::cout<< "Runlenght encode "<< output_ask3 << std::endl;



}