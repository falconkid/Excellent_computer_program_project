#include <iostream>
#include <algorithm>
using namespace std;

#include "MJhand.h"

#define _ABS(_input_x) ((_input_x)>0?(_input_x):(-(_input_x)))

// Compare 
bool MJcompare(const MJtile& i, const MJtile& j) { 
	if((i.suit()) != (j.suit())){ // sort by suit
		return (i.suit()) < (j.suit());
	}
	if((i.rank()) != (j.rank())){ // sort by rank
		return (i.rank()) < (j.rank());
	}
	if((i.flower())>0 && (j.flower())>0){ // sort by flower
		return (i.flower()) < (j.flower());
	}
	return i.getTileId() < j.getTileId(); // sort by Id
}

MJhand::MJhand(){
    _total_len=_faceup_len=_stage=0;
    for (int i=0; i<30; i++) {
        _tiles[i]=MJtile();
    }
}

MJhand::MJhand(MJtile* yen, int n){
    _faceup_len=0;
    _total_len=n;
    _stage=0;
    for (int i=0; i<n; i++) {
        _tiles[i].setfromId(yen[i].getTileId());
    }
}

MJhand::~MJhand(){
// empty
}

int MJhand::faceup_len() const{
    return _faceup_len;
}

int MJhand::caneat(const MJtile& _draw){
    if(_draw.suit()==4){ // cannot eat wind & flower
    	return 0;
    }
    
    int check[5]={0,0,0,0,0}; // -2,-1,0,1,2 -> 0,1,2,3,4
    for(int i=_faceup_len;i<_total_len;++i){
    	if(_tiles[i].suit() == _draw.suit()){
    		int rank_dist=_tiles[i].rank()-_draw.rank();
    		if(_ABS(rank_dist)<=2){
    			++check[rank_dist+2];
    		}
    	}
    }
    int _eat_amount=0;
    if(check[0]>0 && check[1]>0)_eat_amount |= 1; // -2,-1, 0
    if(check[1]>0 && check[3]>0)_eat_amount |= 2; // -1, 0,+1
    if(check[3]>0 && check[4]>0)_eat_amount |= 4; //  0,+1,+2
    return _eat_amount;
}

bool MJhand::canpong(const MJtile& onemoreyen){
    int aretherepair=0;
    for(int i=_faceup_len;i<_total_len;++i){
    	if(onemoreyen == _tiles[i])aretherepair++;
    }

    if(aretherepair<=1)return 0;
    else return 1;
}

bool MJhand::canminggone(const MJtile& onemoreyen){
    int aretherepair=0;
    for(int i=_faceup_len;i<_total_len;++i){
    	if(onemoreyen == _tiles[i])aretherepair++;
    }
    
    if(aretherepair<=2)return 0;
    else return 1;
}

bool MJhand::canangone(const MJtile&){
	
}
bool MJhand::canbugone(const MJtile&){
	
}

void MJhand::arrange(){
	//sort(_tiles,_tiles+_faceup_len,MJcompare);
	sort(_tiles+_faceup_len,_tiles+_total_len+_stage,MJcompare);
	// _stage??
}

void MJhand::draw(MJtile* mjtiles, int& frontind, int& backind){
	// mjtiles are shuffled
	_stage=1;
	_tiles[_total_len].setfromId(mjtiles[frontind].getTileId());
	++frontind;

	while(frontind<=backind){
		if(_tiles[_total_len].flower()){
			applique(_total_len,mjtiles,frontind,backind);
		}
		else break;
	}
}

MJtile MJhand::play(int index){
	// counting from index=1 (1 -> _faceup_len)
	if(index>_total_len+_stage-_faceup_len || index<1){
		MJtile False_Return=MJtile(0); // id=0
		//_stage=0;
		return False_Return;
	}
	MJtile _Play=MJtile(_tiles[_faceup_len+index-1]);
	for(int i=_faceup_len+index;i<_total_len+_stage;++i){
		_tiles[i-1]=_tiles[i];
	}
	
	_stage=0;
	arrange();

	/*printf("_total_len = %d , _stage = %d , _faceup_len = %d\n",
    	_total_len,_stage,_faceup_len);*/

	return _Play;
}

void MJhand::faceup(int index){
	// counting from _faceup_len
	if(index>_total_len+_stage-_faceup_len || index<1){
		return;
	}
	//cout<<"bang!\n";
	MJtile _Faceup=MJtile(_tiles[_faceup_len+index-1]);
	for(int i=_faceup_len+index-1;i>=_faceup_len+1;--i){
		_tiles[i]=_tiles[i-1];
	}
	_tiles[_faceup_len]=_Faceup;
	_faceup_len++;
	arrange();
}

void MJhand::applique(int index, MJtile* mjtiles, int& frontind, int& backind){
	// index starts from 0
	if(index<_faceup_len || index>=_total_len+_stage)return;
	if(_tiles[index].flower()==0)return;
	if(backind<frontind)return;

	faceup(index-_faceup_len+1);
	_tiles[_total_len+_stage].setfromId(mjtiles[backind].getTileId());
	++_total_len;
	--backind;
}

void MJhand::initial(MJtile* mjtiles, int& frontind, int& backind){
	arrange();
	// applique
	int count_flowers=0;
	do{
		count_flowers=0;
		for(int i=_faceup_len;i<_total_len+_stage;++i){
			if(_tiles[i].flower()>0){
				applique(i,mjtiles,frontind,backind);
				++count_flowers;
				//++_total_len;
				break;
			}
		}
	}while(count_flowers>0);
	
	arrange();
}

void MJhand::eat(const MJtile& t, int method){
	int _c_caneat=caneat(t);
	if(method==3)method=4; // 100
	if(_c_caneat==0)return; // cannot eat
	if(t.suit()==4)return; // wind or flower
	if(((_c_caneat|1) && (_c_caneat|2)) || 
	   ((_c_caneat|2) && (_c_caneat|4)) || 
	   ((_c_caneat|4) && (_c_caneat|1))){
		// at least two methods
		if((method & _c_caneat)==0)return;
	}
	else {
		method=_c_caneat;
	}
	
	if(method == 1){
		for(int i=_faceup_len;i<_total_len;++i){
			if((_tiles[i].suit() != t.suit()) || (_tiles[i].suit()==4))continue;
			if(_tiles[i].rank()==t.rank()-2){
				faceup(i-_faceup_len+1);
				break;
			}
		}
		for(int i=_faceup_len;i<_total_len;++i){
			if((_tiles[i].suit() != t.suit()) || (_tiles[i].suit()==4))continue;
			if(_tiles[i].rank()==t.rank()-1){
				faceup(i-_faceup_len+1);
				break;
			}
		}
		_tiles[_total_len].setfromId(t.getTileId());
		_stage=1;
		faceup(_total_len-1+_stage-_faceup_len+1);
	}
	else if(method == 2){
		for(int i=_faceup_len;i<_total_len;++i){
			if((_tiles[i].suit() != t.suit()) || (_tiles[i].suit()==4))continue;
			if(_tiles[i].rank()==t.rank()-1){
				faceup(i-_faceup_len+1);
				break;
			}
		}
		
		_tiles[_total_len].setfromId(t.getTileId());
		_stage=1;
		faceup(_total_len-1+_stage-_faceup_len+1);
		
		for(int i=_faceup_len;i<_total_len;++i){
			if((_tiles[i].suit() != t.suit()) || (_tiles[i].suit()==4))continue;
			if(_tiles[i].rank()==t.rank()+1){
				faceup(i-_faceup_len+1);
				break;
			}
		}
	}
	else if(method == 4){
		_tiles[_total_len].setfromId(t.getTileId());
		_stage=1;
		faceup(_total_len-1+_stage-_faceup_len+1);
		
		for(int i=_faceup_len;i<_total_len;++i){
			if((_tiles[i].suit() != t.suit()) || (_tiles[i].suit()==4))continue;
			if(_tiles[i].rank()==t.rank()+1){
				faceup(i-_faceup_len+1);
				break;
			}
		}
		for(int i=_faceup_len;i<_total_len;++i){
			if((_tiles[i].suit() != t.suit()) || (_tiles[i].suit()==4))continue;
			if(_tiles[i].rank()==t.rank()+2){
				faceup(i-_faceup_len+1);
				break;
			}
		}
	}
}

void MJhand::pong(const MJtile& _draw){
	if(canpong(_draw) == false)return;

	//cout<<"pong!\n";

    for(int cnt=0;cnt<2;++cnt){
    	for(int i=_faceup_len;i<_total_len;++i){
    		if(_tiles[i] == _draw){
    			faceup(i-_faceup_len+1);
    			//cout<<"pong!\n";
    			break;
    		}
    	}
    	if(cnt>0)continue;
    	_tiles[_total_len].setfromId(_draw.getTileId());
		//++_total_len;
		_stage=1;
		faceup(_total_len-1+_stage-_faceup_len+1);
    }
    
    /*printf("_total_len = %d , _stage = %d , _faceup_len = %d\n",
    	_total_len,_stage,_faceup_len);*/
}
/*void MJhand::gone(const MJtile& t){
}*/

/*
ostream& operator << (ostream& os, const MJhand& h){
	
	//part 1 " __ "
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<" __ ";
	}
	os<<endl;
	
	//part 2 "| N|"
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<"|";
		if(h._tiles[i].suit()==4){ 
			if(h._tiles[i].flower())
				os<<" "<<h._tiles[i].flower();
			else 
				os<<"  ";
		} 
		else{
			switch(h._tiles[i].rank()){
				case 1:
					os<<" 1";
					break; 
				case 2:
					os<<" 2";
					break; 
				case 3:
					os<<" 3";
					break; 
				case 4:
					os<<" 4";
					break; 
				case 5:
					os<<" 5";
					break; 
				case 6:
					os<<" 6";
					break; 
				case 7:
					os<<" 7";
					break; 
				case 8:
					os<<" 8";
					break; 
				case 9:
					os<<" 9";
					break; 
				default:
					assert(false); 
			}
		} 
		os<<"|";
	}
	os<<endl;
	
	//part 3 "|XX|"
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<"|";
		if(h._tiles[i].suit()==4)
			switch(h._tiles[i].rank()){
				case 1:
					os<<"Ea";
					break; 
				case 2:
					os<<"So";
					break; 
				case 3:
					os<<"We";
					break; 
				case 4:
					os<<"No";
					break; 
				case 5:
					os<<"Mi";
					break; 
				case 6:
					os<<"Fa";
					break; 
				case 7:
					os<<"  ";
					break; 
				case 8:
					switch(h._tiles[i].flower()){
						case 1:
							os<<"Me";
							break; 
						case 2:
							os<<"La";
							break; 
						case 3:
							os<<"Zu";
							break; 
						case 4:
							os<<"Ju";
							break; 
						default:
							assert(false); 
					} 
					break; 
				case 9:
					switch(h._tiles[i].flower()){
						case 1:
							os<<"Sp";
							break; 
						case 2:
							os<<"Su";
							break; 
						case 3:
							os<<"Au";
							break; 
						case 4:
							os<<"Wi";
							break; 
						default:
							assert(false); 
					} 
					break; 
				default:
					assert(false); 
			}
		else{
			switch(h._tiles[i].suit()){
				case 1:
					os<<" O";
					break; 
				case 2:
					os<<" I";
					break; 
				case 3:
					os<<" W";
					break; 
				default:
					assert(false); 
			}
		} 
		os<<"|";
	}
	os<<endl;
	
	//part 4 " -- "
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<" -- ";
	}
	os<<endl; 
	return os;
} 
//*/
///* 
ostream& operator << (ostream& os, const MJhand& h){
	
	//part 1 " __ "
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<" __ ";
	}
	os<<endl;
	
	//part 2 "| N|"
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<"|";
		if(h._tiles[i].suit()==4){ 
			if(h._tiles[i].flower())
				os<<" "<<h._tiles[i].flower();
			else 
				os<<"  ";
		} 
		else{
			switch(h._tiles[i].rank()){
				case 1:
					os<<"一";
					break; 
				case 2:
					os<<"二";
					break; 
				case 3:
					os<<"三";
					break; 
				case 4:
					os<<"四";
					break; 
				case 5:
					os<<"五";
					break; 
				case 6:
					os<<"六";
					break; 
				case 7:
					os<<"七";
					break; 
				case 8:
					os<<"八";
					break; 
				case 9:
					os<<"九";
					break; 
				default:
					assert(false); 
			}
		} 
		os<<"|";
	}
	os<<endl;
	
	//part 3 "|XX|"
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<"|";
		if(h._tiles[i].suit()==4)
			switch(h._tiles[i].rank()){
				case 1:
					os<<"東";
					break; 
				case 2:
					os<<"南";
					break; 
				case 3:
					os<<"西";
					break; 
				case 4:
					os<<"北";
					break; 
				case 5:
					os<<"中";
					break; 
				case 6:
					os<<"發";
					break; 
				case 7:
					os<<"  ";
					break; 
				case 8:
					switch(h._tiles[i].flower()){
						case 1:
							os<<"梅";
							break; 
						case 2:
							os<<"蘭";
							break; 
						case 3:
							os<<"竹";
							break; 
						case 4:
							os<<"菊";
							break; 
						default:
							assert(false); 
					} 
					break; 
				case 9:
					switch(h._tiles[i].flower()){
						case 1:
							os<<"春";
							break; 
						case 2:
							os<<"夏";
							break; 
						case 3:
							os<<"秋";
							break; 
						case 4:
							os<<"冬";
							break; 
						default:
							assert(false); 
					} 
					break; 
				default:
					assert(false); 
			}
		else{
			switch(h._tiles[i].suit()){
				case 1:
					os<<"筒";
					break; 
				case 2:
					os<<"條";
					break; 
				case 3:
					os<<"萬";
					break; 
				default:
					assert(false); 
			}
		} 
		os<<"|";
	}
	os<<endl;
	
	//part 4 " -- "
	for(int i=0;i<h._total_len+h._stage;i++){
		if(i==h.faceup_len())
			os<<"    ";//4 spaces
		if(h._stage && i==h._total_len+h._stage-1)
			os<<"    ";//4 spaces
		os<<" ￣ ";
	}
	os<<endl; 
	return os;
} 
//*/
