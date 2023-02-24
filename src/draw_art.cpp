#include <iostream>
#include "canvas.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
   Canvas drawer(640, 380);
   drawer.begin(POINTS, true);
   drawer.background(0, 20, 40);
   drawer.color(0, 0, 20);
   drawer.draw_rectangle(640, 40, 320, 360);
   drawer.color(255, 255, 255);
   int i0 = 0;
   for (int i = i0; i < 320; i += 15) {
	   for (int j = 0; j < 640; j += 15) {
		   if (i0 % 3 == 0) {
			   drawer.vertex(j, i);
		   }
		   i0++;
	   }
   }
   drawer.draw_circle(50, 320, 70);
   drawer.end();
   drawer.save("stars.png");
   return 0;
}

