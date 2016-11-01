#include <iostream>
#include <string>
using namespace std;
typedef struct test{
    int x, y;
    string str;
    void print_x(){
        cout << this->x << endl;
    };
    void print_str(){
        cout << this->str << endl;
    };
}testt;


int main(void){
    enum state{
        none, jack, temp
    }state_;
    cout << sizeof(none) << endl;
    cout << sizeof(state) << endl;
    cout << sizeof(state_) << endl;
    testt ttt;
    ttt.print_x();
    ttt.print_str();
}
