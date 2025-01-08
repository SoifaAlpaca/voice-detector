#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <cstdint>
using namespace std;

template <typename T>
class CircBuffer{

    private:
        vector<T>  vec;
        unsigned int pos;
        unsigned int ins;
        unsigned int VecSize;
    public:

    CircBuffer( unsigned int size ):vec(size,0){
        VecSize = size;
        pos     = -1;//Pode dar asneira
        ins     = 0;
    }

    //Returns next elem and increments pos
    T next(  ){
        return vec[ (++pos)%VecSize ];
    }

    //Returns elem at relative position to the current elem
    T get( int RelPos ){
        RelPos += pos;
        RelPos = RelPos % VecSize;

        if( RelPos < 0 ){
            RelPos = pos + RelPos;
        }

        return vec[RelPos];
    }

    T insert( T val){
        vec[(ins++)%VecSize] = val;
        return val;
    }

};

class FIRFilter{
    public:
        std::vector<double> coef;
        unsigned int WinSize;


    FIRFilter(const std::vector<double> &Coefs) : coef(Coefs), WinSize(Coefs.size()) {}

/*
    //Processes the full buffer at once, 
    //Not used because is slow ...
    std::vector<double> ProcessSignal(const uint16_t sig[], unsigned int SigSize){
        std::vector<double> out(SigSize + WinSize - 1, 0.0);
        double aux;

        for (unsigned int pos = 0; pos < SigSize + WinSize - 1; ++pos){
            
            out[pos] = conv(pos,sig,coef,SigSize);
        }

        return out;
    }
*/
};

double conv( unsigned int pos, CircBuffer<double> sig,vector<double> coef/*, unsigned int SigSize*/ ){
    
    double aux = 0;
    for (unsigned int n = 0; n < coef.size(); ++n){
        //if (pos >= n && pos - n < SigSize){
            aux += sig.get(- n) * coef[n];
        //}
    }
    return aux;
}

#endif