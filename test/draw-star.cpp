#include "Image.hpp"

void drawStar(ImFloat image, float brightness, float x, float y)
{
  float *ptr = (float*)image.ptr(y,x);
  for(int i=0;i<3;i++)
  {
    (ptr+0)[i] += 1.0f;
    (ptr+image.cdx/image.elength)[i] += 1.0f;
    (ptr-image.cdx/image.elength)[i] += 1.0f;
    (ptr+image.cdy/image.elength)[i] += 1.0f;
    (ptr-image.cdy/image.elength)[i] += 1.0f;
  }
}



int main(int argc,const char **argv)
{
  ImFloat image(640,480,3);
  
  drawStar(image,0.5, 50,240);
  drawStar(image,1.0,150,240);
  drawStar(image,2.0,250,240);
  drawStar(image,3.0,350,240);
  drawStar(image,4.0,450,240);
  drawStar(image,5.0,550,240);
  
  imwrite(image,"draw-star.ppm");
  
  
  return 0;
}