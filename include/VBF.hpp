#ifndef VBF_H_
#define VBF_H_

#include <cstdint>
#include <cmath>

// Square cuboid.

class VBF
{
public:
  
  //
  VBF(void){data=0;release();}
  VBF(std::uint16_t w,std::uint16_t h,std::uint16_t ch){data=0;set(w,h,ch);}
  ~VBF(void){release();}
  
  // Configurate the VBF.
  bool set(std::uint16_t w,std::uint16_t h,std::uint16_t ch);
  
  // Free the data memory and clear the parameters.
  void release(void);
  
  // Write the block into a file.
  bool write(const char *filename)const;
  
  // Read the block from a file.
  bool read(const char *filename);
  
  // Access data.
  std::uint8_t* access(std::uint32_t idx)const{return data+idx*du;}
  std::uint8_t* access(std::uint16_t u,std::uint16_t v,std::uint16_t w)const{return data+u*du+v*dv+w*dw;}
  //std::uint8_t& code(std::uint16_t u,std::uint16_t v,std::uint16_t w,std::uint16_t ch){return ptr(u,v,w)[ch];}
  //const std::uint8_t& code(std::uint16_t u,std::uint16_t v,std::uint16_t w,std::uint16_t ch)const{return ptr(u,v,w)[ch];}
  
  bool query(const float *xyz,float *dst)const;
  bool query(float x,float y,float z,float *dst)const;
  
  bool getvalue(std::uint16_t u,std::uint16_t v,std::uint16_t w,float *dst)const;
  bool setvalue(std::uint16_t u,std::uint16_t v,std::uint16_t w,float *src);
  bool setvalue(std::uint32_t idx,float *src);
  void getcoord(std::uint16_t u,std::uint16_t v,std::uint16_t w,float *dst)const;
  void getcoord(std::uint32_t idx,uint16_t* uvw,float *xyz=0)const;
  
  std::uint32_t getNumel(void){return numel;}
  std::uint16_t getWidth(void){return width;}
  std::uint16_t getHeight(void){return height;}
  
  float setKs(float k){ks=k;}
  float getKs(void)const{return ks;}
  float setKv(float k){kv=k;}
  float getKv(void)const{return kv;}
  
  void preview(const char* filename,int nslice)const;
  
    
//private:
  
  // Convert between world coords and block coords.
  bool xyz2uvw(const float *xyz,float *uvw)const;
  void uvw2xyz(const float *uvw,float *xyz)const;
  
  // Return whether the coords are in bound.
  bool checkuvw(const float *uvw)const;
  bool checkuvw(const std::uint16_t *uvw)const;
  bool checkuvw(std::uint16_t u,std::uint16_t v,std::uint16_t w)const;
  bool checkuvw(float u,float v,float w)const;
  
  // Convert between voxel codewords and point values.
  float cw2pt(std::uint8_t cw)const{return (float)cw*kv/255.0f;}
  std::uint8_t pt2cw(float pt)const{return (std::uint8_t)std::round(std::fmin(std::fmax(pt/kv,0),1)*255);}
  
  // Linear interpolation.
  float linterp(float v1,float v2,float a)const{return (1-a)*v1+a*v2;}

  // Data.
  std::uint8_t *data;
  // Width of the block.
  std::uint32_t width;  // x,z
  // Height of the block.
  std::uint32_t height; // y
  // Number of voxels in total.
  std::uint32_t numel;
  // Number of channels.
  std::uint16_t channels;
  // Stride of each dimension.
  std::uint32_t du;
  std::uint32_t dv;
  std::uint32_t dw;
  //
  float ks;
  float kv;
  
private:
}; 

#endif
