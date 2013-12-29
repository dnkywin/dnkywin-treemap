#include<iostream>
#include "dnkywin_augmented_treemap.h"

template < typename T >
struct gcd{
  T operator() (const T& x, const T& y) const {
    T a = x;
    T b = y;
    while(b){
        T c = a%b;
        a = b;
        b = c;
    }
    return a;
  }
};

int main(){
    augmented_treemap<int,int,gcd<int> > tester;
    for (int i=0;i<106000;i++) tester.set(i,10*i);
    for (int i=1;i<106000;i+=2) tester.erase(i);
    std::cout<<tester.query(0,106000)<<"\n";
    std::cout<<tester.size()<<"\n";

    augmented_treemap<int,long long int> tester2;
    for (int i=10;i>=0;i--) tester2.set(i*i, 10-i);

    for (augmented_treemap<int,long long int>::iterator it = tester2.begin(); it!= tester2.end(); ++it){
        std::cout<<(it->second)<<"\n";
    }
    
    augmented_treemap<int,long long int>::iterator it1 = tester2.lower_bound(4);
    std::cout<<(it1->first)<<"\n";
    augmented_treemap<int,long long int>::iterator it2 = tester2.lower_bound(5);
    std::cout<<(it2->first)<<"\n";
    
    augmented_treemap<int,long long int>::iterator it3 = tester2.upper_bound(4);
    std::cout<<(it3->first)<<"\n";
    augmented_treemap<int,long long int>::iterator it4 = tester2.upper_bound(5);
    std::cout<<(it4->first)<<"\n";
}
