#include "VBF.hpp"

#include <cmath>
#include <cstdint>
#include <array>
#include <sstream>
#include <fstream>

#include <iostream>

void VBF::release(void)
{
  delete data;
  data = 0;
  numel = 0;
  channels = 0;
  width=height=0;
  du=dv=dw=0;
  ks=1; 
  kv=1;
}

bool VBF::set(std::uint16_t w,std::uint16_t h,std::uint16_t ch)
{
  // Check validity.
  bool invalid = false;
  invalid |= w  >= 1024 || w==0;
  invalid |= h  >= 1024 || h==0;
  invalid |= ch >= 5    || ch==0;
  if (invalid)
    return false;
  // Release old data.
  release();
  // Assign parameters.
  width     = w;
  height    = h;
  channels  = ch;
  numel     = width*width*height;
  
  du = channels;
  dw = du*width;
  dv = dw*width;
  
  // Allocate new data and initialize.
  data = new std::uint8_t[numel*channels];
  for(std::uint32_t i=0;i<numel*channels;i++)
    data[i] = 0;

  return true;
}

bool VBF::write(const char *filename)const
{
  std::ofstream file;
  file.exceptions(std::ifstream::failbit);
  file.open(filename,std::ostream::binary);
  file.write((char*)&width,sizeof(std::uint16_t));
  file.write((char*)&height,sizeof(std::uint16_t));
  file.write((char*)&channels,sizeof(std::uint16_t));
  file.write((char*)&ks,sizeof(float));
  file.write((char*)&kv,sizeof(float));
  file.write((char*)data,numel*channels*sizeof(std::uint8_t));
  file.close();
}

bool VBF::read(const char *filename)
{
  std::ifstream file;
  file.exceptions(std::ifstream::failbit|std::ifstream::failbit);
  file.open(filename,std::istream::binary);
  
  std::uint16_t w,h,ch;
  file.read((char*)&w,sizeof(std::uint16_t));
  file.read((char*)&h,sizeof(std::uint16_t));
  file.read((char*)&ch,sizeof(std::uint16_t));
  file.read((char*)&ks,sizeof(float));
  file.read((char*)&kv,sizeof(float));
  bool valid = set(w,h,ch);
  if (!valid)
    return false;
  file.read((char*)data,numel*channels*sizeof(std::uint8_t));
  file.close();
}

bool VBF::xyz2uvw(const float *xyz,float *uvw)const
{
  uvw[0] = (xyz[0]/ks + 1) * width / 2;
  uvw[1] = (xyz[1]/ks + (float)height/width) * width / 2;
  uvw[2] = (xyz[2]/ks + 1) * width / 2;
  return checkuvw(uvw);
}

void VBF::uvw2xyz(const float *uvw,float *xyz)const
{
  xyz[0] = (uvw[0] / width * 2 - 1) * ks;
  xyz[1] = (uvw[1] / width * 2 - (float)height/width) * ks;
  xyz[2] = (uvw[2] / width * 2 - 1) * ks;
}

bool VBF::checkuvw(const std::uint16_t *uvw)const
{
  return checkuvw(uvw[0],uvw[1],uvw[2]);
}

bool VBF::checkuvw(const float *uvw)const
{
  return checkuvw(uvw[0],uvw[1],uvw[2]);
}

bool VBF::checkuvw(std::uint16_t u,std::uint16_t v,std::uint16_t w)const
{
  return    u>=0 && u<(width-1)
         && v>=0 && v<(height-1)
         && w>=0 && w<(width-1);
}

bool VBF::checkuvw(float u,float v,float w)const
{
  return    u>=0 && u<(width-1)
         && v>=0 && v<(height-1)
         && w>=0 && w<(width-1);
}

bool VBF::query(const float *xyz,float *dst)const
{
  float fuvw[3] = {};
  float cuvw[3] = {};
  float auvw[3] = {};
  if(xyz2uvw(xyz,auvw)==false)
  {
    for(int i=0;i<channels;i++)
      dst[i] = 0.0f;
    return false;
  }
  
  for(int i=0;i<3;i++)
  {
    fuvw[i] = std::floor(auvw[i]);
    cuvw[i] = std::ceil( auvw[i]);
    auvw[i] = auvw[i] - fuvw[i];
  }
  
  //std::uint8_t *m = access(fuvw[0],fuvw[1],fuvw[2]);
  //for(int i=0;i<3;i++)
  //  dst[i] = cw2pt(m[i]);
  
  //return true;

  std::uint8_t* ptr[8] = {
    access(fuvw[0],fuvw[1],fuvw[2]), access(cuvw[0],fuvw[1],fuvw[2]),
    access(fuvw[0],fuvw[1],cuvw[2]), access(cuvw[0],fuvw[1],cuvw[2]),
    access(fuvw[0],cuvw[1],fuvw[2]), access(cuvw[0],cuvw[1],fuvw[2]),
    access(fuvw[0],cuvw[1],cuvw[2]), access(cuvw[0],cuvw[1],cuvw[2]),
  };
  
  //  

  for(int i=0;i<channels;i++)
  {
    float v0w0 = linterp(cw2pt(ptr[0][i]),cw2pt(ptr[1][i]),auvw[0]);
    float v0w1 = linterp(cw2pt(ptr[2][i]),cw2pt(ptr[3][i]),auvw[0]);
    float v1w0 = linterp(cw2pt(ptr[4][i]),cw2pt(ptr[5][i]),auvw[0]);
    float v1w1 = linterp(cw2pt(ptr[6][i]),cw2pt(ptr[7][i]),auvw[0]);
    float v0   = linterp(v0w0,v0w1,auvw[2]);
    float v1   = linterp(v1w0,v1w1,auvw[2]);
    float v    = linterp(v0,v1,auvw[1]);
    dst[i] = v;
  }
  return true;
}

bool VBF::query(float x,float y,float z,float *dst)const
{
  float xyz[3] = {x,y,z};
  return query(xyz,dst);
} 

bool VBF::getvalue(std::uint16_t u,std::uint16_t v,std::uint16_t w,float *dst)const
{
  if(checkuvw(u,v,w)==false)
  {
    for(int i=0;i<channels;i++)
      dst[i] = 0.0f;
    return false;
  }
  std::uint8_t *ptr = access(u,v,w);
  for(int i=0;i<channels;i++)
    dst[i] = cw2pt(ptr[i]);
  return true;
}

bool VBF::setvalue(std::uint32_t idx,float *src)
{
  if(idx>=getNumel())
    return false;
  std::uint8_t *ptr = access(idx);
  for(int i=0;i<channels;i++)
    ptr[i] = pt2cw(src[i]);
  return true;
}

bool VBF::setvalue(std::uint16_t u,std::uint16_t v,std::uint16_t w,float *src)
{
  if(checkuvw(u,v,w)==false)
    return false;
  std::uint8_t *ptr = access(u,v,w);
  for(int i=0;i<channels;i++)
    ptr[i] = pt2cw(src[i]);
  return true;
}

void VBF::getcoord(std::uint16_t u,std::uint16_t v,std::uint16_t w,float *dst)const
{
  float uvw[3] = {(float)u,(float)v,(float)w};
  uvw2xyz(uvw,dst);
}

void VBF::getcoord(std::uint32_t idx,uint16_t* uvw,float *xyz)const
{
  uvw[1] = (idx / (width*width));  idx = idx % (width*width);
  uvw[2] = (idx / width);          idx = idx % width;
  uvw[0] = (idx);
  if( xyz != 0)
    getcoord(uvw[0],uvw[1],uvw[2],xyz);
}

void VBF::preview(const char* filename,int nslice)const
{
  nslice = (nslice  <1) ? 1 : nslice;
  int ch = (channels>3) ? 3 : channels;
  
  std::ostringstream ss;
  ss<<"P6 "<<width<<" "<<width*nslice<<" 255\r";
  
  std::ofstream file;
  file.exceptions(std::ofstream::failbit|std::ofstream::failbit);
  file.open(filename,std::ostream::binary);
  file.write(ss.str().c_str(),ss.str().size());
  
  for(std::uint16_t i=0;i<nslice;i++){
    std::uint16_t v = i*(height-1)/(nslice-1);
    for(std::uint16_t w=0;w<width;w++){
      for(std::uint16_t u=0;u<width;u++){
        std::uint8_t *p = access(u,v,w);
        std::uint8_t val[3] = {};
        for(int k=0;k<ch;k++)
          val[k] = p[k];
        file.write((char*)val,3);
      }
    }
  }
  file.close();
}