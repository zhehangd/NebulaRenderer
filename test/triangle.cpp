#include "Camera.hpp"
#include "Image.hpp"
#include "Primitive.hpp"
#include "Math.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

struct Light
{
  Vector3 pos;
  Vector3 color;
};

struct Triangle
{
  Image   tex;
  Vector3 uv[3]; // ignore the 3rd component.
  Vector3 vertex[3];
};

struct Global
{
  ImFloat  canvas;
  ImFloat  depth;
  Camera camera;
  std::vector<Light> lightList;
  
}global;

void drawTriangle(Triangle &tri)
{
  // Project to the camera space.
  for(int i=0;i<3;i++)
    global.camera.projectXcw(tri.vertex[i].ptr(),tri.vertex[i].ptr());
  // =================
  // If some vertices are behind the camera,
  //   we need to find a way to get only the visible part.
  //   this involves splitting the triangle into several subtriangles.
  //   .....
  // =================
  // Here we just ignore this problem.
  
  for(int i=0;i<3;i++)
    global.camera.projectXsc(tri.vertex[i].ptr(),tri.vertex[i].ptr());
  
  // Attributes to interpolate..
  const int attLength = 3;
  float attList[3][attLength];
  for(int i=0;i<3;i++)
  {
    float *att = attList[i];
    att[0] = tri.vertex[i][2];
    att[1] = tri.uv[i][0];
    att[2] = tri.uv[i][1];
  }
  
  TriInterp interp;
  interp.setup(tri.vertex[0].ptr(),tri.vertex[1].ptr(),tri.vertex[2].ptr(),
            attList[0],attList[1],attList[2],attLength);
  
  int yMin = max((int)floor(interp.bbox[1]),0);
  int yMax = max(min((int)ceil(interp.bbox[3]),global.canvas.height-1),0);
  int xMin = max((int)floor(interp.bbox[0]),0);
  int xMax = max(min((int)ceil(interp.bbox[2]),global.canvas.width-1),0);

  for (unsigned int r = yMin; r <= yMax; r++){
    for (unsigned int c = xMin; c <= xMax; c++){
      if (interp.evaluate(c,r))
      {
        // Interpolate the attributes
        float attributes[3];
        interp.interpolate(c,r,attributes);
        
        // Depth Test.
        float z   = (float)(attributes[0]);
        float *dz = ((float*)global.depth.ptr(r,c));
        if(*dz<z)
          continue;
        *dz = z;
        
        //std::cout<<"[z]"<<z<<std::endl;
        
        // UV
        float uv[2];
        uv[0] = attributes[1];
        uv[1] = attributes[2];
        
        // Perspective Correction
        // ---------------
        // ...
        // -------------
        
        // UV to image coordinate.
        uv[0] *= tri.tex.width;
        uv[1] *= tri.tex.height;
        
        // Color
        float color[3] = {1,1,1};
        if (tri.tex.test(uv[1],uv[0]))
        {
          //std::cout<<uv[0]<<" "<<uv[1]<<std::endl;
          
          // No interpolation.
          unsigned char *texp = (unsigned char*)tri.tex.ptr(uv[1],uv[0]);
          for(int i=0;i<3;i++)
            color[i] = (float)texp[i]/255;
        }
        // Shade
        float *pixel = (float*)global.canvas.ptr(r,c);
        for(int i=0;i<3;i++)
          pixel[i] = min(color[i],1.0f);
      }
    }
  }
  /*
  
  
  Vector3 line[2] = {{0,0,-2},{0,0,2}};
  cam.projectXsw(line[0].ptr(),line[0].ptr());
  cam.projectXsw(line[1].ptr(),line[1].ptr());
  
  
  Bresenham linInterp;
  float att1[4] = {1.0f,0.0f,0.0f,line[0][2]};
  float att2[4] = {0.0f,0.0f,1.0f,line[1][2]};
  linInterp.setup(line[0][0],line[0][1],line[1][0],line[1][1],att1,att2,4);
  
  int x,y;
  float att[4];
  while(linInterp.next(&x,&y,att))
  {
    if(x<0 || x>=canvas.width || y<0 || y>=canvas.height)
      continue;
    unsigned short z   = (unsigned short)(att[3]*mdepth);
    unsigned short *dz = ((unsigned short*)depth.ptr(y,x));
    if(*dz<z)
      continue;
    *dz = z;
    unsigned char *pixel = (unsigned char *)canvas.ptr(y,x);
    for(int i=0;i<3;i++)
      pixel[i] = (unsigned char)(att[i]*255);
  }
  
  */
}

void drawLine(Vector3 srt,Vector3 end,Vector3 color)
{
  for(int i=0;i<3;i++)
  {
    color[i] = std::fmax(color[i],0);
    color[i] = std::fmin(color[i],1);
  }
  global.camera.projectXcw(srt.ptr(),srt.ptr());
  global.camera.projectXcw(end.ptr(),end.ptr());
  
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
  
  //
  global.camera.projectXsc(srt.ptr(),srt.ptr());
  global.camera.projectXsc(end.ptr(),end.ptr());
  
  // To Add: trim by the canvas boundary.
  
  Bresenham linInterp;
  linInterp.setup(srt[0],srt[1],end[0],end[1],&srt[2],&end[2],1);
  
  int x,y;
  float att[1];
  while(linInterp.next(&x,&y,att))
  {
    if(x<0 || x>=global.canvas.width || y<0 || y>=global.canvas.height)
      continue;
    float z   = att[0];
    float *dz = (float*)global.depth.ptr(y,x);
    if(*dz<z)
      continue;
    *dz = z;
    float *pixel = (float*)global.canvas.ptr(y,x);
    for(int i=0;i<3;i++)
      pixel[i] = color[i];
  }
}


int main(void)
{
  float SIZE = 100;
  float DISTANCE = 110;
  
  // Initialize the canvas and the depth buffer.
  const int width  = 640;
  const int height = 480;
  global.canvas = ImFloat(width,height,3,IMAGE_F32);
  global.depth  = ImFloat(width,height,1,IMAGE_F32);
  global.canvas = global.canvas * 0;
  global.depth  = global.depth  * 0 + 1;

  // Set up the camera.
  global.camera.setupExt(-48,30,DISTANCE);
  global.camera.setupInt(2,1.2*(SIZE+DISTANCE));
  global.camera.setupScn(width,height,false);
  
  // Set up the lights.
  Light light1;
  light1.pos   = Vector3(1,1,0);
  light1.color = Vector3(1,0.8,0.8);
  Light light2;
  light2.pos   = Vector3(0,1,1);
  light2.color = Vector3(0.8,0.8,1.0);
  global.lightList.push_back(light1);
  global.lightList.push_back(light2);
  

  // Vertex List.
  Vector3 vertexList[8];
  vertexList[0] = Vector3(-.5,-.5,-.5) * SIZE;
  vertexList[1] = Vector3(-.5,-.5, .5) * SIZE;
  vertexList[2] = Vector3( .5,-.5, .5) * SIZE;
  vertexList[3] = Vector3( .5,-.5,-.5) * SIZE;
  vertexList[4] = Vector3(-.5, .5,-.5) * SIZE;
  vertexList[5] = Vector3(-.5, .5, .5) * SIZE;
  vertexList[6] = Vector3( .5, .5, .5) * SIZE;
  vertexList[7] = Vector3( .5, .5,-.5) * SIZE;
  
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
  
  // Normal List.
  Vector3 normalList[6];
  normalList[0] = Vector3( 1, 0, 0);
  normalList[1] = Vector3(-1, 0, 0);
  normalList[2] = Vector3( 0, 1, 0);
  normalList[3] = Vector3( 0,-1, 0);
  normalList[4] = Vector3( 0, 0, 1);
  normalList[5] = Vector3( 0, 0,-1);
  
  // Trangle List.
  // First three are vertex indices, the last three are uv
  int triangleList[12][6] = {
    { 0,1,3,0,1,5 }, { 3,1,2,4,1,5 }, { 4,5,7,3,2,7 }, { 7,5,6,7,2,6 },
    { 1,5,6,1,5,6 }, { 1,2,6,1,2,6 }, { 3,7,0,3,7,8 }, { 4,7,0,9,7,8 },
    { 0,1,4,0,1,4 }, { 4,5,1,4,5,1 }, { 2,3,7,2,3,7 }, { 2,6,7,2,6,7 }};
  int lineList[12][2] = {
    {0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}};
  // Read the texture
  Image texture;
  if (imread("test/skybox.ppm",texture)==false)
    return 1;
  
    
  // Draw all 12 triangles.
  for(int k=0; k<12 ;k++)
  {
    Triangle tri;
    for(int i=0;i<3;i++)
    {
      tri.vertex[i] = vertexList[triangleList[k][i]];
      tri.uv[i]     = uvList[triangleList[k][i+3]];
      tri.tex       = texture;
    }
    std::cout<<"[input]"<<k<<std::endl;
    drawTriangle(tri);
    
  }
  
  // Draw 12 lines.
  for(int i=0;i<12;i++)
  {
    Vector3 srt = vertexList[lineList[i][0]];
    Vector3 end = vertexList[lineList[i][1]];
    Vector3 color(1,1,1);
    drawLine(srt,end,color);
  }
  imwrite(global.canvas,"triangle.ppm");
  
  return 0;
}; 
