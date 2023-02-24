#include <iostream>
#include "canvas.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
   Canvas drawer(640, 380);
   drawer.begin(POINTS);
   drawer.background(0, 255, 0);
   drawer.end();
   drawer.save("stars.png");
   return 0;
}

