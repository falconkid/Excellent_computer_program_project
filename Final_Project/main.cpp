#include <iostream>
#include <assert.h>
#include <cstdio>
#include "MJtile.h"
#include "MJhand.h"
#include "Shuffler.h"
using namespace std;

inline void Test_MJhand(){
	Shuffler S;
	S.init();
	MJtile all_MJ[144];
	S.fill(all_MJ);
	all_MJ[0].setfromId(141);
	
	MJhand Hand_Show=MJhand(all_MJ,30);
    cout<<"Shuffled, front 30:\n";
    cout<<"     ";
	for(int i=0;i<30;++i){
		printf("%2d  ",i);
	}
    cout<<endl<<Hand_Show;

    int my_play;
    int _front=11, _back=29;
    char input_s[10];

    cout<<"Hand:\n";
    MJhand Hand=MJhand(all_MJ,10);
    cout<<Hand;
    Hand.initial(all_MJ,_front,_back);
    cout<<Hand;

    while(cin>>input_s){
        if(input_s[0]=='e'){ // check eat?
            cin>>my_play;
            if(my_play<0)break;
            MJtile tmp_tile=MJtile(my_play);
            cout<<Hand.caneat(tmp_tile);
            continue;
        }
        if(input_s[0]=='p'){ // pong
            cin>>my_play;
            if(my_play<0)break;

            MJtile tmp_tile=MJtile(my_play);
            Hand.pong(tmp_tile);
            cout<<"Pong "<<my_play<<" : front = "<<_front<<", back = "<<_back<<endl<<Hand;
        }
        Hand.draw(all_MJ,_front,_back);
        cout<<"Draw: front = "<<_front<<", back = "<<_back<<endl<<Hand;

        cin>>my_play;
        if(my_play<0)break;
        
        cout<<Hand.play(my_play);
        cout<<"Play "<<my_play<<" :\n"<<Hand;

        Hand.initial(all_MJ,_front,_back);
        cout<<"Initial: front = "<<_front<<", back = "<<_back<<endl<<Hand;
    }
}

int main(){
	Test_MJhand();
	
    return 0;
}