#include "Renderer.hpp"
#include "Primitive.hpp"
#include "Camera.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "VBF.hpp"

#include <iostream>
#include <iomanip>

#include <algorithm>
#include <cstdint>
#include <cmath>



void Renderer::setup(int w,int h)
{
  width  = w;
  height = h;
  
  // Set frame buffer.
  canvas = Image(width,height,3,sizeof(float));
  depth  = Image(width,height,1,sizeof(float));
  ImCursor scan;
  scan.set(canvas);
  do *((float*)scan.ptr())  = 0.0f;
  while(scan.moveNextElement());
  scan.set(depth);
  do *((float*)scan.ptr())   = 1.0f;
  while(scan.moveNextElement());
  
  // Set Camera.
  camera.setupScn(width,height,false);
}

void Renderer::drawLine(Vector3 srt,Vector3 end,Vector3 color)
{
  for(int i=0;i<3;i++)
  {
    color[i] = std::fmax(color[i],0);
    color[i] = std::fmin(color[i],1);
  }
  camera.projectXcw(srt.ptr(),srt.ptr());
  camera.projectXcw(end.ptr(),end.ptr());
  
  // Trim the line.
  if ( srt[2] > end[2] )
    std::swap(srt,end);

  if ( srt[2] > 0 )
    return;
  if ( end[2] > 0 )
  {
    end[0] = (srt[0]*end[2]-end[0]*srt[2])/(end[2]-srt[2]);
    end[1] = (srt[1]*end[2]-end[1]*srt[2])/(end[2]-srt[2]);
    end[2] = 0;
  }

  camera.projectXsc(srt.ptr(),srt.ptr());
  camera.projectXsc(end.ptr(),end.ptr());
  
  // To Add: trim by the canvas boundary.
  
  Bresenham linInterp;
  linInterp.setup(srt[0],srt[1],end[0],end[1],&srt[2],&end[2],1);
  
  int x,y;
  float att[1];
  while(linInterp.next(&x,&y,att))
  {
    if(x<0 || x>=width || y<0 || y>=height)
      continue;
    float z   = att[0];
    float *dz = (float*)depth.ptr(y,x);
    if(*dz<z)
      continue;
    *dz = z;
    float *pixel = (float*)canvas.ptr(y,x);
    for(int i=0;i<3;i++)
      pixel[i] = color[i];
  }
};

void Renderer::drawCube(float radius,int tick,Vector3 color)
{
  for(int i=0;i<3;i++)
  {
    Vector3 srt,end,incx,incy;
    srt[i] = -1;
    end[i] =  1;
    incx[(i+1)%3] = 1;
    incy[(i+2)%3] = 1;
    srt *= radius;
    end *= radius;
    for(int y=0;y<=tick;y++){
      for(int x=0;x<=tick;x++)
      {
        float kx = x*(2*radius)/tick - radius;
        float ky = y*(2*radius)/tick - radius;
        Vector3 offset = incx*kx + incy*ky;
        Vector3 a = srt + offset;
        Vector3 b = end + offset;
        drawLine(a,b,color);
      }
    }
  }
}

void Renderer::drawVolume(float ke,float ka,float step)
{
  Vector3 spec[2] = {{1.0f,0.3f,0.6f},{0.4f,0.5f,0.8f}};

  //
  // K[e/r/a][Ext/Sct][a/u/v]
  // [e/r/a]   emission, reflection, all.
  // [Ext/Sct] extinction, scattering.
  // [a/u/v]   all,ultraviolet,visible.
  
  // Extinction coefficients.
  float KeExt[2] = {2.0f,0.6f};
  float KrExt[2] = {2.0f,0.6f};
  // Albedo values.
  float Ae[2] = {0.8,0.1};
  float Ar[2] = {0.0,0.6};
  
  float KeSctu = KeExt[1]*Ae[0];
  float KrSctv = KrExt[1]*Ar[1];
  
  for (unsigned int r = 0; r < height; r++){
    for (unsigned int c = 0; c < width; c++){
      float *pixel = (float*)canvas.ptr(r,c);
      float *dz    = (float*)depth.ptr(r,c);

      // rayEnd is at the camera plane.
      // raySrt is at where the ray is cast.
      Vector3 rayEnd(c,r,0);
      Vector3 raySrt(c,r,*dz);
      camera.projectInvXsw(raySrt.ptr(),raySrt.ptr());
      camera.projectInvXsw(rayEnd.ptr(),rayEnd.ptr());

      // Direction of the marching.
      Vector3 rayDir = normalize(rayEnd-raySrt);
      // Compute the distance.
      float dist = (rayEnd-raySrt).norm();
      // Compute the number of steps.
      int   nstep = std::ceil(dist / step);
      // Adjust the step length based on the step number.
      float fstep = dist / nstep;
      // Adjust the starting point so we sample at the center of each step.
      raySrt  = raySrt + rayDir * fstep/2;

      //
      bool isInVolume = false;
      Vector3 energy;
      for(int i=0;i<3;i++)
        energy[i] = pixel[i];
      for(int i=0;i<nstep;i++)
      {
        Vector3 rayPos = raySrt + rayDir * i * fstep;
        
        float value[2];
        float light[2];
        if(b_material.query((rayPos).ptr(),value))
        {
          isInVolume = true;
          b_lighting.query((rayPos).ptr(),light);

          // Only visible
          float   DaExtv = KeExt[1]*value[0]+KrExt[1]*value[1];
          Vector3 DaSctv = spec[0]*(KeSctu*value[0]*light[0])+spec[1]*(KrSctv*value[1]*light[1]);
          
          energy *= (1 - DaExtv*fstep);
          energy +=      DaSctv*fstep;
        }
        else
          if(isInVolume)
            break;
      }
      for(int k=0;k<3;k++)
        pixel[k] = energy[k]*2;
    }
    if( r % 10 == 0 )
      std::cout<<"progress: "<<std::setw(3)<<r<<"/"<<std::setw(3)<<height<<"\r"<<std::flush;
  }
}

bool Renderer::loadVolumeMaterial(const char* filename)
{
  return loadVolume(b_material,filename);
}

bool Renderer::loadVolumeLighting(const char* filename)
{
  return loadVolume(b_lighting,filename);
}

bool Renderer::loadVolume(VBF &volume,const char* filename)
{
  return volume.read(filename);
}

void Renderer::setVolumeScale(float ks)
{
  b_material.setKs(ks);
  b_lighting.setKs(ks);
}
