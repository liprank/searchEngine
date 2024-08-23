#include <iostream>

using std::cout;
int main(){
    int a = 10;
    auto out = [&a]{
        auto in = [&a]{
            a = 5;
        };
        in();
    };

    out();
    cout << a << "\n";
}