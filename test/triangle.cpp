#include "Camera.hpp"
#include "Image.hpp"
#include "Primitive.hpp"
#include "Math.hpp"

#include <iostream>
#include <iomanip>
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

void drawTriangle(Triangle &tri);
vector<Triangle> checkTriangleAtCameraSpace(Triangle &tri);


void putTriangle(Triangle &tri)
{
  
  for(int i=0;i<3;i++)
    global.camera.projectXcw(tri.vertex[i].ptr(),tri.vertex[i].ptr());

  vector<Triangle> tris_to_be_drawn = checkTriangleAtCameraSpace(tri);
  
  for(int i=0;i<tris_to_be_drawn.size();i++)
  {
    std::cout<<"TRI-"<<i<<std::endl;
    std::cout<<"Vcam-0   "<<tri.vertex[0]<<std::endl;
    std::cout<<"Vcam-1   "<<tri.vertex[1]<<std::endl;
    std::cout<<"Vcam-2   "<<tri.vertex[2]<<std::endl;
    std::cout<<"UV-0       "<<tri.uv[0]<<std::endl;
    std::cout<<"UV-1       "<<tri.uv[1]<<std::endl;
    std::cout<<"UV-2       "<<tri.uv[2]<<std::endl;
    for(int j=0;j<3;j++)
      global.camera.projectXsc(tris_to_be_drawn[i].vertex[j].ptr(),tris_to_be_drawn[i].vertex[j].ptr());
    
    std::cout<<"Vscr-0   "<<tri.vertex[0]<<std::endl;
    std::cout<<"Vscr-1   "<<tri.vertex[1]<<std::endl;
    std::cout<<"Vscr-2   "<<tri.vertex[2]<<std::endl;
    std::cout<<"UV-0       "<<tri.uv[0]<<std::endl;
    std::cout<<"UV-1       "<<tri.uv[1]<<std::endl;
    std::cout<<"UV-2       "<<tri.uv[2]<<std::endl;
    
    drawTriangle(tris_to_be_drawn[i]);
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

vector<Triangle> checkTriangleAtCameraSpace(Triangle &tri) {
  vector<Triangle> tris_to_be_drawn;

  // Check if the vertices are behind camera
  std::cout<<"Checking the number of vertices behind the camera: ";
  int num_vertices_behind = 0;
  vector<int> indices_behind;
  vector<int> indices_front;
  for (int i = 0; i < 3; i++) {
    if (tri.vertex[i][2] > 0) {
      num_vertices_behind++;
      indices_behind.emplace_back(i);
    }
    else {
      indices_front.emplace_back(i);
    }
  }
  std::cout << num_vertices_behind << std::endl;

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


void drawTriangle(Triangle &tri)
{
  // Attributes to interpolate..
  const int attLength = 3;
  float attList[3][attLength];
  for(int i=0;i<3;i++)
  {
    float *att = attList[i];
    att[0] = tri.vertex[i][2];
    att[1] = tri.uv[i][0]/(att[0]/(global.camera.fz-att[0])+1);
    att[2] = tri.uv[i][1]/(att[0]/(global.camera.fz-att[0])+1);
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
        uv[0]*=(z/(global.camera.fz-z)+1);
        uv[1]*=(z/(global.camera.fz-z)+1);
        // -------------
        
        // UV to image coordinate.
        uv[0] *= tri.tex.width;
        uv[1] *= tri.tex.height;
        
        // Color
        unsigned char color[3]={0,0,0};
        tri.tex.interp(uv[1],uv[0],color);
        // Shade
        float *pixel = (float*)global.canvas.ptr(r,c);
        for(int i=0;i<3;i++)
          pixel[i] = min((float)color[i]/255.0f,1.0f);
      }
    }
  }
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
  float DISTANCE = 250;
  
  // Initialize the canvas and the depth buffer.
  const int width  = 640;
  const int height = 480;
  global.canvas = ImFloat(width,height,3,IMAGE_F32);
  global.depth  = ImFloat(width,height,1,IMAGE_F32);
  global.canvas = global.canvas * 0;
  global.depth  = global.depth  * 0 + 1;

  // Set up the camera.
  //
  float cams[]={0,0,0};
  float camd[]={1,0,1};
  float camu[]={0,1,0};
  //global.camera.setupExt(cams,camd,camu);
  global.camera.setupExt(cams,80,0);
  //global.camera.setupExt(-45,35.264,150);
  global.camera.setupInt(1,1.2*(SIZE+DISTANCE));
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
    
    std::cout<<"======================================"<<std::endl;
    std::cout<<"Drawing Triangle "<<std::setw(2)<<k<<std::endl;
    std::cout<<"Vertex-0   "<<tri.vertex[0]<<std::endl;
    std::cout<<"Vertex-1   "<<tri.vertex[1]<<std::endl;
    std::cout<<"Vertex-2   "<<tri.vertex[2]<<std::endl;
    std::cout<<"UV-0       "<<tri.uv[0]<<std::endl;
    std::cout<<"UV-1       "<<tri.uv[1]<<std::endl;
    std::cout<<"UV-2       "<<tri.uv[2]<<std::endl;
    putTriangle(tri);
    
  }
  
  // Draw 12 lines.
  for(int i=0;i<0;i++)
  {
    Vector3 srt = vertexList[lineList[i][0]];
    Vector3 end = vertexList[lineList[i][1]];
    Vector3 color(1,1,1);
    drawLine(srt,end,color);
  }
  imwrite(global.canvas,"triangle.ppm");
  
  return 0;
}; 
