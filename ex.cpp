#include<iostream>
#include<stdio.h>
#include<string>

using namespace std;
class Cars
{
private:
         string color;
        static int count;
public:
         Cars(): color("red")
         {
             count++;
         }
         Cars(string c): color(c)
         {
             count++;
         }
         int getCount()
         {
             return count;
         }
};

int Cars::count = 0;


int main()
{
         Cars c1;
         cout << "Number of cars: " << c1.getCount() << endl;
         Cars c2("blue");
         cout << "Number of cars: " << c2.getCount() << endl;
         Cars c3("green");          
         cout << "Number of cars: " << c3.getCount() << endl;
return 0;
}