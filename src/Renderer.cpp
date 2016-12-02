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
#include <vector>


Renderer::Renderer(void)
{  
  Ke[0] = Ke[1] = Kr[0] = Kr[1] = 0.04; // Extinction
  Ke[2] = Kr[3] = 0.9f; // Albedo
  Ke[3] = Kr[2] = 0.0f; // Albedo
  Ke[4] = Kr[4] = 0.0f; // Ambient
  specEmission[0] = 1.0f;
  specEmission[1] = 0.3f;
  specEmission[2] = 0.6f;
  specReflection[0] = 0.2f;
  specReflection[1] = 0.3f;
  specReflection[2] = 0.5f;
}

void Renderer::setCanvas(int w,int h)
{
  width  = w;
  height = h;
  
  // Set frame buffer.
  canvas = Image(width,height,3,sizeof(float));
  depth  = Image(width,height,1,sizeof(float));
  cleanCanvas(Vector3(0,0.5,0),1);
  
  // Set Camera.
  camera.setupScn(width,height,false);
}

void Renderer::cleanCanvas(Vector3 color,float d)
{
  ImCursor scan;
  scan.set(canvas);
  do for(int i=0;i<3;i++)
      ((float*)scan.ptr())[i]  = color[i];
  while(scan.moveNextElement());
  scan.set(depth);
  do *((float*)scan.ptr()) = d;
  while(scan.moveNextElement());
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
        
        // Only visible
        float   DaExtv = KeExt[1]*density[0]+KrExt[1]*density[1];
        Vector3 DaSctv = specEmission*(KeSctu*density[0]*radiance[0])+specReflection*(KrSctv*density[1]*radiance[1]);
        Vector3 DaAmbv = specEmission*density[0]*Ke[4]+specReflection*density[1]*Kr[4];

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

void Renderer::computeLightingVolume(const std::vector<Light> &lightList,float step)
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
    
    // Total radiance
    float radiance[2] = {};
    
    for(int k=0;k<lightList.size();k++)
    {
      
      float ru = lightList[k].ru;
      float rv = lightList[k].rv;
    
      // Source.
      Vector3 src = lightList[k].pos;
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
      float energy[2] = {ru,rv};
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
      for(int k=0;k<2;k++){
        energy[k] = energy[k] * ir;
        radiance[k] += energy[k];
        
      }
    }
    
    for(int k=0;k<2;k++)
        b_lighting.setvalue(uvw[0],uvw[1],uvw[2],radiance);

    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/numel);
  }
}

void drawTrianglePrivate(ImFloat &canvas,Triangle &tri,float fz);
std::vector<Triangle> checkTriangleAtCameraSpace(Triangle &tri);

void Renderer::drawSkybox(Image &image)
{
  float SIZE = 50;
  Vector3 cameraPosition;
  camera.getPosition(cameraPosition.ptr());
  
  // Vertex List.
  Vector3 vertexList[8];
  vertexList[0] = Vector3(-.5,-.5,-.5) * SIZE + cameraPosition;
  vertexList[1] = Vector3(-.5,-.5, .5) * SIZE + cameraPosition;
  vertexList[2] = Vector3( .5,-.5, .5) * SIZE + cameraPosition;
  vertexList[3] = Vector3( .5,-.5,-.5) * SIZE + cameraPosition;
  vertexList[4] = Vector3(-.5, .5,-.5) * SIZE + cameraPosition;
  vertexList[5] = Vector3(-.5, .5, .5) * SIZE + cameraPosition;
  vertexList[6] = Vector3( .5, .5, .5) * SIZE + cameraPosition;
  vertexList[7] = Vector3( .5, .5,-.5) * SIZE + cameraPosition;
  
  // UV List.
  Vector3 uvList[10];
  uvList[0] = Vector3(0.00f,1.0f,0);
  uvList[1] = Vector3(0.25f,1.0f,0);
  uvList[2] = Vector3(0.50f,1.0f,0);
  uvList[3] = Vector3(0.75f,1.0f,0);
  uvList[4] = Vector3(0.00f,0.0f,0);
  uvList[5] = Vector3(0.25f,0.0f,0);
  uvList[6] = Vector3(0.50f,0.0f,0);
  uvList[7] = Vector3(0.75f,0.0f,0);
  uvList[8] = Vector3(1.00f,1.0f,0);
  uvList[9] = Vector3(1.00f,0.0f,0);
  
  int triangleList[12][6] = {
    { 0,1,3,0,1,5 }, { 3,1,2,4,1,5 }, { 4,5,7,3,2,7 }, { 7,5,6,7,2,6 },
    { 1,5,6,1,5,6 }, { 1,2,6,1,2,6 }, { 3,7,0,3,7,8 }, { 4,7,0,9,7,8 },
    { 0,1,4,0,1,4 }, { 4,5,1,4,5,1 }, { 2,3,7,2,3,7 }, { 2,6,7,2,6,7 }};
    
    // Draw all 12 triangles.
  for(int k=0; k<12 ;k++)
  {
    Triangle tri;
    for(int i=0;i<3;i++)
    {
      tri.vertex[i] = vertexList[triangleList[k][i]];
      tri.uv[i]     = uvList[triangleList[k][i+3]];
      tri.tex       = image;
    }
    drawTriangle(tri);
    /*
    std::vector<Triangle> tris_to_be_drawn = checkTriangleAtCameraSpace(tri);
    for(int i=0;i<tris_to_be_drawn.size();i++)
    {
      for(int j=0;j<3;j++)
        camera.projectXsc(tris_to_be_drawn[i].vertex[j].ptr(),tris_to_be_drawn[i].vertex[j].ptr());
      drawTrianglePrivate(canvas,tris_to_be_drawn[i],camera.fz);
    }*/
  }
}


void Renderer::drawTriangle(Triangle &tri)
{
  for(int i=0;i<3;i++)
    camera.projectXcw(tri.vertex[i].ptr(),tri.vertex[i].ptr());
  std::vector<Triangle> tris_to_be_drawn = checkTriangleAtCameraSpace(tri);
  for(int i=0;i<tris_to_be_drawn.size();i++)
  {
    for(int j=0;j<3;j++)
      camera.projectXsc(tris_to_be_drawn[i].vertex[j].ptr(),tris_to_be_drawn[i].vertex[j].ptr());
    drawTrianglePrivate(canvas,tris_to_be_drawn[i],camera.fz);
  }
}

Vector3 getZIntersect(const Vector3 &start, const Vector3 &end) {
  Vector3 slope = start - end;
  float t = - start[2]/slope[2];
  Vector3 result;
  result[2] = 0;
  result[0] = start[0] + slope[0] * t;
  result[1] = start[1] + slope[1] * t;
  return result;
}

std::vector<Triangle> checkTriangleAtCameraSpace(Triangle &tri) {
  std::vector<Triangle> tris_to_be_drawn;

  // Check if the vertices are behind camera
  int num_vertices_behind = 0;
  std::vector<int> indices_behind;
  std::vector<int> indices_front;
  for (int i = 0; i < 3; i++) {
    if (tri.vertex[i][2] > 0) {
      num_vertices_behind++;
      indices_behind.emplace_back(i);
    }
    else {
      indices_front.emplace_back(i);
    }
  }

  // For UV and z interpolation
  Vector3 uvz[3];
  for (int i = 0; i < 3; i++) {
    uvz[i]    = tri.uv[i];
    uvz[i][2] = tri.vertex[i][2];
  }
  
  if (num_vertices_behind == 0) {
    tris_to_be_drawn.emplace_back(tri);
  }
  else if (num_vertices_behind == 1) {
    Vector3 vertex_intersect0 = getZIntersect(tri.vertex[indices_behind[0]],
                                              tri.vertex[indices_front[0]]);
    Vector3 vertex_intersect1 = getZIntersect(tri.vertex[indices_behind[0]],
                                              tri.vertex[indices_front[1]]);
    // UV interpolation
    Vector3 uv_intersect0 = getZIntersect(uvz[indices_behind[0]], uvz[indices_front[0]]);
    Vector3 uv_intersect1 = getZIntersect(uvz[indices_behind[0]], uvz[indices_front[1]]);
    Triangle new_tri;
    new_tri.tex = tri.tex;
    new_tri.vertex[0] = vertex_intersect0;
    new_tri.vertex[1] = vertex_intersect1;
    new_tri.vertex[2] = tri.vertex[indices_front[0]];
    new_tri.uv[0] = uv_intersect0;
    new_tri.uv[1] = uv_intersect1;
    new_tri.uv[2] = tri.uv[indices_front[0]];
    tris_to_be_drawn.emplace_back(new_tri);

    tri.vertex[indices_behind[0]] = vertex_intersect1;
    tri.uv[indices_behind[0]] = uv_intersect1;
    tris_to_be_drawn.emplace_back(tri);
  } else if (num_vertices_behind == 2) {
    Vector3 vertex_intersect0 = getZIntersect(tri.vertex[indices_front[0]],
                                              tri.vertex[indices_behind[0]]);
    Vector3 vertex_intersect1 = getZIntersect(tri.vertex[indices_front[0]],
                                              tri.vertex[indices_behind[1]]);
    // UV interpolation
    Vector3 uv_intersect0 = getZIntersect(uvz[indices_front[0]], uvz[indices_behind[0]]);
    Vector3 uv_intersect1 = getZIntersect(uvz[indices_front[0]], uvz[indices_behind[1]]);
    tri.vertex[indices_behind[0]] = vertex_intersect0;
    tri.vertex[indices_behind[1]] = vertex_intersect1;
    tri.uv[indices_behind[0]] = uv_intersect0;
    tri.uv[indices_behind[1]] = uv_intersect1;
    tris_to_be_drawn.emplace_back(tri);
  }
  return tris_to_be_drawn;
}


void drawTrianglePrivate(ImFloat &canvas,Triangle &tri,float fz)
{
  // Attributes to interpolate..
  const int attLength = 3;
  float attList[3][attLength];
  for(int i=0;i<3;i++)
  {
    float *att = attList[i];
    att[0] = tri.vertex[i][2];
    att[1] = tri.uv[i][0]/(att[0]/(fz-att[0])+1);
    att[2] = tri.uv[i][1]/(att[0]/(fz-att[0])+1);
  }
  TriInterp interp;
  interp.setup(tri.vertex[0].ptr(),tri.vertex[1].ptr(),tri.vertex[2].ptr(),
            attList[0],attList[1],attList[2],attLength);
  int yMin = std::max((int)std::floor(interp.bbox[1]),0);
  int yMax = std::max(std::min((int)ceil(interp.bbox[3]),canvas.height-1),0);
  int xMin = std::max((int)std::floor(interp.bbox[0]),0);
  int xMax = std::max(std::min((int)ceil(interp.bbox[2]),canvas.width-1),0);
  for (unsigned int r = yMin; r <= yMax; r++){
    for (unsigned int c = xMin; c <= xMax; c++){
      if (interp.evaluate(c,r))
      {
        // Interpolate the attributes
        float attributes[3];
        interp.interpolate(c,r,attributes);
        // z and uv
        float z   = (float)(attributes[0]);
        float uv[2];
        uv[0] = attributes[1];
        uv[1] = attributes[2];
        // Perspective Correction
        uv[0]*=(z/(fz-z)+1);
        uv[1]*=(z/(fz-z)+1);
        // UV to image coordinate.
        uv[0] *= tri.tex.width;
        uv[1] *= tri.tex.height;
        
        // Color
        unsigned char color[3]={0,0,0};
        tri.tex.interp(uv[1],uv[0],color);
        // Shade
        float *pixel = (float*)canvas.ptr(r,c);
        for(int i=0;i<3;i++)
          pixel[i] = std::min((float)color[i]/255.0f,1.0f);
      }
    }
  }
}