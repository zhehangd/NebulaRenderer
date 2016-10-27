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


Renderer::Renderer(void)
{  
  Ke[0] = Ke[1] = Kr[0] = Kr[1] = 0.04; // Extinction
  Ke[2] = Kr[3] = 0.9f; // Albedo
  Ke[3] = Kr[2] = 0.0f; // Albedo
  Ke[4] = Kr[4] = 0.0f; // Ambient
}

void Renderer::setCanvas(int w,int h)
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

void Renderer::drawOrigin(float radius)
{
  for(int i=0;i<3;i++)
  {
    Vector3 srt,end,color;
    end[i] = radius;
    color[i] = 1;
    drawLine(srt,end,color);
  }
}

void Renderer::drawVolume(float step)
{
  Vector3 spec[2] = {{1.0f,0.3f,0.6f},{0.2f,0.3f,0.5f}};
  //Vector3 spec[2] = {{1.0f,0.0f,0.0f},{0.0f,0.0f,1.0f}};

  //
  // K[e/r/a][Ext/Sct][a/u/v]
  // [e/r/a]   emission, reflection, all.
  // [Ext/Sct] extinction, scattering.
  // [a/u/v]   all,ultraviolet,visible.
  
  // Extinction coefficients.
  float KeExt[2] = {Ke[0],Ke[1]};
  float KrExt[2] = {Kr[0],Kr[1]};
  // Albedo values.
  float Ae[2] = {Ke[2],Ke[3]};
  float Ar[2] = {Kr[2],Kr[3]};
  
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
      bool hasInVolume = false;
      Vector3 energy;
      for(int i=0;i<3;i++)
        energy[i] = pixel[i];
      for(int i=0;i<nstep;i++)
      {
        // Move a step.
        Vector3 rayPos = raySrt + rayDir * i * fstep;
        //
        float density[2];
        float radiance[2];
        
        // 
        bool  flag = true;
        flag &= b_material.query((rayPos).ptr(),density);
        flag &= b_lighting.query((rayPos).ptr(),radiance);
        if(flag == false)
          if(hasInVolume)
            break;
          else
            continue;
        else
          hasInVolume = true;

        //density[0] = std::exp(2.3026f*density[0]);
        //density[1] = std::exp(2.3026f*density[1]);
        
        // Only visible
        float   DaExtv = KeExt[1]*density[0]+KrExt[1]*density[1];
        Vector3 DaSctv = spec[0]*(KeSctu*density[0]*radiance[0])+spec[1]*(KrSctv*density[1]*radiance[1]);
        Vector3 DaAmbv = spec[0]*density[0]*Ke[4]+spec[1]*density[1]*Kr[4];
        
        energy *= std::exp(-DaExtv*fstep);
        energy += (DaSctv+DaAmbv)*fstep;

      }
      
      for(int i=0;i<3;i++)
        pixel[i] = energy[i];
    }
    if( r % 10 == 0 )
      std::cout<<"progress: "<<std::setw(3)<<r<<"/"<<std::setw(3)<<height<<"\r"<<std::flush;
  }
}

void Renderer::computeLightingVolume(Vector3 src,float Ru,float Rv,float step)
{
  b_lighting.set(b_material.width,b_material.height,2);
  b_lighting.setKs(b_material.getKs());
  b_lighting.setKv(3.0f);

  int numel = b_lighting.getNumel();
  for(int i=0;i<numel;i++)
  {
    Vector3  xyz;
    uint16_t uvw[3];
    b_material.getcoord(i,uvw,xyz.ptr());
    
    // Destination.
    Vector3 dst = xyz;
    // Direction of the marching.
    Vector3 dir = normalize(dst-src);
    // Compute the distance.
    float dist = (dst-src).norm();
    // Compute the number of steps.
    int   nstep = std::ceil(dist / step);
    // Adjust the step length based on the step number.
    float fstep = dist / nstep;
    // Adjust the starting point so we sample at the center of each step.
    Vector3 pos  = src + dir * fstep/2;
    
    // Extinction coefficients.
    float KeExt[2] = {Ke[0],Ke[1]};
    float KrExt[2] = {Kr[0],Kr[1]};
    
    // This is the radiance of the star.
    // The first component represents ultraviolet radiance strength.
    // The second component represents visible radiance strength.
    float energy[2] = {Ru,Rv};
    for(int i=0;i<nstep;i++)
    {
      pos += dir * fstep;
 
      Vector3 value;
      if(b_material.query(pos.ptr(),value.ptr())==false)
        continue;

      //value[0] = std::exp(2.3026f*value[0]);
      //value[1] = std::exp(2.3026f*value[1]);
      
      float DaExt[2];
      DaExt[0] = KeExt[0]*(value[0])+KrExt[0]*(value[1]);
      DaExt[1] = KeExt[1]*(value[0])+KrExt[1]*(value[1]);

      for(int k=0;k<2;k++)
        energy[k] = energy[k] * std::exp(-DaExt[k]*fstep);
    }

    float ir = b_lighting.getKs() / (dist + b_lighting.getKs()/10);
    for(int k=0;k<2;k++)
      energy[k] = energy[k] * ir;

    for(int k=0;k<2;k++)
      b_lighting.setvalue(uvw[0],uvw[1],uvw[2],energy);

    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/numel);
  }
}
