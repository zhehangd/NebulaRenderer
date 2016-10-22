#include "VBF.hpp" 

#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

int main(int argc,const char **argv)
{
  
  cout.precision(2);
  
  int width  = 4*2;
  int height = 3*2;
  
  VBF volume;
  volume.set(width,height,3);
  
  for(int v=0;v<height;v++){
    for(int w=0;w<width;w++){
      for(int u=0;u<width;u++)
      {
        std::uint8_t  val = rand()%255;
        for(int k=0;k<3;k++)
          volume.access(u,v,w)[k] = val;
        cout<<setw(3)<<(int)val<<" ";
      }
      cout<<"\n";
    }
    cout<<"--------------------\n";
  }
  volume.preview("PREV-1.ppm",6);
  
  width  *= 50;
  height *= 50;
  
  VBF sample;
  sample.set(width,height,3);
  //sample.setKs(1.1f);
  for(int v=0;v<height;v++){
    for(int w=0;w<width;w++){
      for(int u=0;u<width;u++)
      {
        float rgb[3];
        float xyz[3];
        float voluvw[3];
        
        sample.getcoord(u,v,w,xyz);
        volume.xyz2uvw(xyz,voluvw);
        volume.query(xyz,rgb);
        sample.setvalue(u,v,w,rgb);
        
        if(v==0 && w==0 && u==0){
          cout<<"SRT"<<endl;
          cout<<xyz[0]<<", "<<xyz[1]<<", "<<xyz[2]<<endl;
          cout<<voluvw[0]<<", "<<voluvw[1]<<", "<<voluvw[2]<<endl;
        }
        if(v==height-1 && w==width-1 && u==width-1){
          cout<<"END"<<endl;
          cout<<xyz[0]<<", "<<xyz[1]<<", "<<xyz[2]<<endl;
          cout<<voluvw[0]<<", "<<voluvw[1]<<", "<<voluvw[2]<<endl;
        }
      }
    }
  }
  sample.preview("PREV-2.ppm",6);
  
  
  return 0;
}
