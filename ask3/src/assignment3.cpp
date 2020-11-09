#include <iostream>
#include "ac_int.h"
#include "ac_channel.h"

typedef ac_int<4,false> dtype;


class RunLengthEncoder{
    private:
        dtype counter;
        dtype current;
        dtype previous;
    public:
        RunLengthEncoder(){
            counter = 0;
            previous = 0;
        }

        void run (ac_channel<dtype> &in, ac_channel<dtype> &out){
            while(in.available(1)){
                current = in.read();
                if (current==previous){
                    counter ++;
                }
                else{
                    if (counter>0){
                        out.write(previous);
                        out.write(counter);
                    }
                    counter = 1;
                    previous = current;
                }
            }
            out.write(previous);
            out.write(counter);
        }
};

typedef ac_int<4,false> card;

class BlackJack{
    private:
        ac_int<5,false> sum;
        ac_int<2,false> aces;
        ac_int<4,false> counter;
        card current;
    public:
        BlackJack(){
            sum = 0;
            aces = 0;
            counter = 0;
        }

        void run(ac_channel<card> &in_card, bool &end_round, bool &win){
            while(in_card.available(1)){
                current = in_card.read();
                if (end_round==0){
                    counter++;
                    if (current==1){
                        sum += 11;
                        aces ++;
                    }
                    else{
                        sum += current;
                    }

                    if(sum==21){
                        end_round = 1;
                        win = 1;
                    }
                    else if (sum > 21){
                        end_round = 1;
                        if (counter == 2 && aces == 2){
                            win = 1;
                        }
                    }
                    else if (counter == 5){
                        end_round = 1;
                        win = 1;
                    }
                    //std::cout<<"Counter: "<<counter<<std::endl;
                    //std::cout<<"Sum: "<<sum<<std::endl;
                }
            }

        }

        void clear(){
            sum = 0;
            aces = 0;
            counter = 0;
        }

};


int main(){
    ac_channel<ac_int<4,0> > input_encode;
    ac_channel<ac_int<4,0> > output_encode;
    RunLengthEncoder test_encoder;

    input_encode.write(2);
    input_encode.write(2);
    input_encode.write(2);
    input_encode.write(2);
    input_encode.write(3);
    input_encode.write(12);
    input_encode.write(12);
    input_encode.write(1);
    input_encode.write(1);
    input_encode.write(5);

    test_encoder.run(input_encode,output_encode);
    std::cout<< "Runlenght encode "<< output_encode << std::endl;

    ac_channel<card> input_cards;
    bool check_end = 0;
    bool check_win = 0;
    BlackJack player1;

    

    input_cards.write(5);
    input_cards.write(8);
    input_cards.write(2);
    input_cards.write(6);
    player1.run(input_cards, check_end, check_win);
    std::cout<< "Simple win: end_round="<< check_end << ", win=" << check_win << std::endl;

    check_win = 0;
    check_end = 0;
    player1.clear();
    input_cards.write(5);
    input_cards.write(9);
    input_cards.write(2);
    input_cards.write(6);
    player1.run(input_cards, check_end, check_win);
    std::cout<< "Simple loss: end_round="<< check_end << ", win=" << check_win << std::endl;

    check_win = 0;
    check_end = 0;
    player1.clear();
    input_cards.write(1);
    input_cards.write(1);
    input_cards.write(2);
    input_cards.write(6);
    player1.run(input_cards, check_end, check_win);
    std::cout<< "Double ace win: end_round="<< check_end << ", win=" << check_win << std::endl;

    check_win = 0;
    check_end = 0;
    player1.clear();
    input_cards.write(7);
    input_cards.write(5);
    input_cards.write(1);
    input_cards.write(6);
    player1.run(input_cards, check_end, check_win);
    std::cout<< "Simple ace loss: end_round="<< check_end << ", win=" << check_win << std::endl;
}