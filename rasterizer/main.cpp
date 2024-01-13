#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

#include "SETTINGS.h"

using namespace std;


// resolution of the field
int xRes = 800;
int yRes = 600;

// world bounding box
float _left = -12.0;
float _right = 12.0;
float _bottom = -12.0;
float _top = 12.0;

// float _far = 0.0;
float _far = 100.0;
// float _near = 12.0;
float _near = 1.0;

float fovy = 65.0;
float aspect = 4.0/3.0;

// eye position, gaze, up direction
// VEC3 eye = VEC3(0.2,0.2,1.0);
// VEC3 eye = VEC3(1.0,1.0,1.0);
VEC3 eye = VEC3(0.0,2.0,3.0);
VEC3 lookat = VEC3(0,0,0);
VEC3 updir = VEC3(0,1,0);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
VEC3 truncate(const VEC4& v)
{
  return VEC3(v[0], v[1], v[2]);
}
VEC4 extend(const VEC3& v)
{
  return VEC4(v[0], v[1], v[2], 1.0);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void readPPM(const char* filename, int xRes, int yRes, float*& vs)
{
  // try to open the file
  FILE *fp;
  fp = fopen(filename, "rb");
  if (fp == NULL)
  {
    cout << " Could not open file \"" << filename << "\" for reading." << endl;
    cout << " Make sure you're not trying to read from a weird location or with a " << endl;
    cout << " strange filename. Bailing ... " << endl;
    exit(0);
  }

  // get the dimensions
  unsigned char newline;
  fscanf(fp, "P6\n%d %d\n255%c", &xRes, &yRes, &newline);
  if (newline != '\n') {
    cout << " The header of " << filename << " may be improperly formatted." << endl;
    cout << " The program will continue, but you may want to check your input. " << endl;
  }
  int totalCells = xRes * yRes;

  // grab the pixel values
  unsigned char* pixels = new unsigned char[3 * totalCells];
  fread(pixels, 1, totalCells * 3, fp);

  // copy to a nicer data type
  vs = new float[3 * totalCells];
  for (int i = 0; i < 3 * totalCells; i++)
    vs[i] = pixels[i];

  // clean up
  delete[] pixels;
  fclose(fp);
  // cout << " Read in file " << filename << endl;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void writePPM(const char* filename, int& xRes, int& yRes, float*& values)
{
  // int rc1 = system ("cd out_imgs");
  // assert (rc1==0);

  
  int totalCells = xRes * yRes;
  unsigned char* pixels = new unsigned char[3 * totalCells];
  for (int i = 0; i < 3 * totalCells; i++)
    pixels[i] = values[i];

  FILE *fp;
  fp = fopen(filename, "wb");
  if (fp == NULL)
  {
    cout << " Could not open file \"" << filename << "\" for writing." << endl;
    cout << " Make sure you're not trying to write from a weird location or with a " << endl;
    cout << " strange filename. Bailing ... " << endl;
    exit(0);
  }

  fprintf(fp, "P6\n%d %d\n255\n", xRes, yRes);
  fwrite(pixels, 1, totalCells * 3, fp);
  fclose(fp);
  delete[] pixels;
  // int rc2 = system ("cd ..");
  // assert (rc2==0);
}

//////////////////////////////////////////////////////////////////////////////////
// build out a single square
//////////////////////////////////////////////////////////////////////////////////
void buildBigSquare(vector<VEC3>& vertices, vector<VEC3I>& indices, vector<VEC3>& colors)
{
  vertices.push_back(VEC3(1.0, 1.0, 1.5));
  vertices.push_back(VEC3( 11.0, 1.0, 1.5));
  vertices.push_back(VEC3(1.0,  11.0, 1.5));
  vertices.push_back(VEC3( 11.0,  11.0, 1.5));

  // front face
  indices.push_back(VEC3I(0, 1, 2));
  indices.push_back(VEC3I(2, 1, 3));
  colors.push_back(VEC3(1.0, 0.0, 0.0));
  colors.push_back(VEC3(0.0, 0.0, 1.0));
}

//////////////////////////////////////////////////////////////////////////////////
// build out a single square
//////////////////////////////////////////////////////////////////////////////////
void buildSquare(vector<VEC3>& vertices, vector<VEC3I>& indices, vector<VEC3>& colors)
{
  vertices.push_back(VEC3(-0.5, -0.5,  0.5));
  vertices.push_back(VEC3( 0.5, -0.5,  0.5));
  vertices.push_back(VEC3(-0.5,  0.5,  0.5));
  vertices.push_back(VEC3( 0.5,  0.5,  0.5));

  // front face
  indices.push_back(VEC3I(0, 1, 2));
  indices.push_back(VEC3I(2, 1, 3));
  colors.push_back(VEC3(1.0, 0.0, 0.0));
  colors.push_back(VEC3(0.0, 1.0, 0.0));
}

//////////////////////////////////////////////////////////////////////////////////
// build out a cube
//////////////////////////////////////////////////////////////////////////////////
void buildCube(vector<VEC3>& vertices, vector<VEC3I>& indices, vector<VEC3>& colors)
{
  vertices.push_back(VEC3(-0.5, -0.5,  0.5));
  vertices.push_back(VEC3( 0.5, -0.5,  0.5));
  vertices.push_back(VEC3(-0.5,  0.5,  0.5));
  vertices.push_back(VEC3( 0.5,  0.5,  0.5));
  vertices.push_back(VEC3(-0.5, -0.5, -0.5));
  vertices.push_back(VEC3( 0.5, -0.5, -0.5));
  vertices.push_back(VEC3(-0.5,  0.5, -0.5));
  vertices.push_back(VEC3( 0.5,  0.5, -0.5));

  // front face
  indices.push_back(VEC3I(0, 1, 2));
  indices.push_back(VEC3I(2, 1, 3));
  colors.push_back(VEC3(1.0, 0.0, 0.0));
  colors.push_back(VEC3(1.0, 0.0, 0.0));

  // back face
  indices.push_back(VEC3I(5, 4, 7));
  indices.push_back(VEC3I(7, 4, 6));
  colors.push_back(VEC3(0.0, 1.0, 0.0));
  colors.push_back(VEC3(0.0, 1.0, 0.0));

  // left face
  indices.push_back(VEC3I(4, 0, 6));
  indices.push_back(VEC3I(6, 0, 2));
  colors.push_back(VEC3(0.0, 0.0, 1.0));
  colors.push_back(VEC3(0.0, 0.0, 1.0));

  // right face
  indices.push_back(VEC3I(1, 5, 3));
  indices.push_back(VEC3I(3, 5, 7));
  colors.push_back(VEC3(0.0, 1.0, 1.0));
  colors.push_back(VEC3(0.0, 1.0, 1.0));

  // top face
  indices.push_back(VEC3I(2, 3, 6));
  indices.push_back(VEC3I(6, 3, 7));
  colors.push_back(VEC3(1.0, 1.0, 0.0));
  colors.push_back(VEC3(1.0, 1.0, 0.0));

  // bottom face
  indices.push_back(VEC3I(4, 5, 0));
  indices.push_back(VEC3I(0, 5, 1));
  colors.push_back(VEC3(1.0, 0.0, 1.0));
  colors.push_back(VEC3(1.0, 0.0, 1.0));
}

//////////////////////////////////////////////////////////////////////////////////
// build out a cube
//////////////////////////////////////////////////////////////////////////////////
void buildCubePerVertexColors(vector<VEC3>& vertices, vector<VEC3I>& indices, vector<VEC3>& colors)
{
  vertices.push_back(VEC3(-0.5, -0.5,  0.5));
  vertices.push_back(VEC3( 0.5, -0.5,  0.5));
  vertices.push_back(VEC3(-0.5,  0.5,  0.5));
  vertices.push_back(VEC3( 0.5,  0.5,  0.5));
  vertices.push_back(VEC3(-0.5, -0.5, -0.5));
  vertices.push_back(VEC3( 0.5, -0.5, -0.5));
  vertices.push_back(VEC3(-0.5,  0.5, -0.5));
  vertices.push_back(VEC3( 0.5,  0.5, -0.5));
  colors.push_back(VEC3(1.0, 0.0, 0.0));
  colors.push_back(VEC3(1.0, 0.0, 0.0));
  colors.push_back(VEC3(0.0, 1.0, 0.0));
  colors.push_back(VEC3(0.0, 1.0, 0.0));
  colors.push_back(VEC3(0.0, 0.0, 1.0));
  colors.push_back(VEC3(0.0, 0.0, 1.0));
  colors.push_back(VEC3(1.0, 1.0, 0.0));
  colors.push_back(VEC3(1.0, 1.0, 0.0));

  // front face
  indices.push_back(VEC3I(0, 1, 2));
  indices.push_back(VEC3I(2, 1, 3));

  // back face
  indices.push_back(VEC3I(5, 4, 7));
  indices.push_back(VEC3I(7, 4, 6));

  // left face
  indices.push_back(VEC3I(4, 0, 6));
  indices.push_back(VEC3I(6, 0, 2));

  // right face
  indices.push_back(VEC3I(1, 5, 3));
  indices.push_back(VEC3I(3, 5, 7));

  // top face
  indices.push_back(VEC3I(2, 3, 6));
  indices.push_back(VEC3I(6, 3, 7));

  // bottom face
  indices.push_back(VEC3I(4, 5, 0));
  indices.push_back(VEC3I(0, 5, 1));
}

void buildCubePerVertexUVs(vector<VEC3>& vertices, vector<VEC3I>& indices, vector<VEC2>& uvs)
{
  vertices.push_back(VEC3(-0.5, -0.5,  0.5));
  vertices.push_back(VEC3( 0.5, -0.5,  0.5));
  vertices.push_back(VEC3(-0.5,  0.5,  0.5));
  vertices.push_back(VEC3( 0.5,  0.5,  0.5));
  vertices.push_back(VEC3(-0.5, -0.5, -0.5));
  vertices.push_back(VEC3( 0.5, -0.5, -0.5));
  vertices.push_back(VEC3(-0.5,  0.5, -0.5));
  vertices.push_back(VEC3( 0.5,  0.5, -0.5));
  uvs.push_back(VEC2(0.0, 0.0));
  uvs.push_back(VEC2(0.5, 0.0));
  uvs.push_back(VEC2(0.0, 1.0));
  uvs.push_back(VEC2(0.5, 1.0));
  uvs.push_back(VEC2(0.0, 0.0));
  uvs.push_back(VEC2(1.0, 0.0));
  uvs.push_back(VEC2(0.5, 1.0));
  uvs.push_back(VEC2(1.0, 1.0));

  // front face
  indices.push_back(VEC3I(0, 1, 2));
  indices.push_back(VEC3I(2, 1, 3));

  // back face
  indices.push_back(VEC3I(5, 4, 7));
  indices.push_back(VEC3I(7, 4, 6));

  // left face
  indices.push_back(VEC3I(4, 0, 6));
  indices.push_back(VEC3I(6, 0, 2));

  // right face
  indices.push_back(VEC3I(1, 5, 3));
  indices.push_back(VEC3I(3, 5, 7));

  // top face
  indices.push_back(VEC3I(2, 3, 6));
  indices.push_back(VEC3I(6, 3, 7));

  // bottom face
  indices.push_back(VEC3I(4, 5, 0));
  indices.push_back(VEC3I(0, 5, 1));
}

MATRIX4 get_mvp (int xRes, int yRes)
{
  MATRIX4 mvp = MATRIX4::Zero();

  mvp(0,0) = xRes/2;
  mvp(0,3) = xRes/2;
  mvp(1,1) = yRes/2;
  mvp(1,3) = yRes/2;
  mvp(2,2) = 1.0;
  mvp(3,3) = 1.0;

  return mvp;
}

MATRIX4 get_mortho (float m_right, float m_left, 
                    float m_bottom, float m_top, 
                    float m_near, float m_far)
{
  MATRIX4 mortho = MATRIX4::Zero();

  mortho(0,0) = 2/(m_right-m_left);
  mortho(1,1) = 2/(m_top-m_bottom);
  mortho(2,2) = 2/(m_near-m_far);
  mortho(3,3) = 1;

  mortho(0,3) = -(m_right+m_left)/(m_right-m_left);
  mortho(1,3) = -(m_top+m_bottom)/(m_top-m_bottom);
  mortho(2,3) =  (m_near+m_far)/(m_near-m_far);

  return mortho;
}

MATRIX4 get_mcam (VEC3 _eye, VEC3 _lookat, VEC3 _updir)
{
  MATRIX4 mcam_a = MATRIX4::Zero();

  mcam_a(0,0) = 1;
  mcam_a(1,1) = 1;
  mcam_a(2,2) = 1;
  mcam_a(3,3) = 1;
  mcam_a(0,3) = -_eye[0];
  mcam_a(1,3) = -_eye[1];
  mcam_a(2,3) = -_eye[2];

  MATRIX4 mcam_b = MATRIX4::Zero();

  mcam_b(3,3) = 1;
  VEC3 gaze = _eye - _lookat;
  VEC3 w = gaze/gaze.norm();
  mcam_b(2,0) = w[0];
  mcam_b(2,1) = w[1];
  mcam_b(2,2) = w[2];

  VEC3 u = (_updir.cross(w))/(_updir.cross(w).norm());
  mcam_b(0,0) = u[0];
  mcam_b(0,1) = u[1];
  mcam_b(0,2) = u[2];

  VEC3 v = w.cross(u);
  mcam_b(1,0) = v[0];
  mcam_b(1,1) = v[1];
  mcam_b(1,2) = v[2];

  return mcam_b*mcam_a;
}

MATRIX4 get_mpp (float m_fovy, float m_aspect, float m_near, float m_far,
                 VEC3 _eye, VEC3 _lookat, VEC3 _updir)
{
  MATRIX4 mpp = MATRIX4::Zero();
  float pi = 3.141;
  float scale_y = 1/(tan((m_fovy*pi)/(2*180)));
  float scale_x = scale_y / m_aspect;
  mpp(0,0) = scale_x;
  mpp(1,1) = scale_y;
  mpp(2,2) = -(m_far)/(m_far-m_near);
  mpp(3,2) = -(m_far*m_near)/(m_far-m_near);
  mpp(2,3) = -1;

  return mpp;
}

vector<VEC3> scale_colors (vector<VEC3> colors)
{
  vector<VEC3> scaled_colors;
  int n = colors.size();

  for (int i = 0; i<n; i++)
  {
    scaled_colors.push_back( VEC3(int(255*colors[i][0]),  
                                  int(255*colors[i][1]), 
                                  int(255*colors[i][2])) );
  }
  return scaled_colors;
}

VEC4 perspective_divide (VEC4 vertex)
{
  VEC4 vertex_out;
  assert (vertex[3]!=0);
  vertex_out[0] = vertex[0]/vertex[3];
  vertex_out[1] = vertex[1]/vertex[3];
  vertex_out[2] = vertex[2]/vertex[3];
  vertex_out[3] = 1;

  return vertex_out;
}

int is_in_interior (VEC3 v0, VEC3 v1, VEC3 v2, VEC3 p)
{
  MATRIX3 m;
  m(0,0) = v0[0]; m(0,1) = v1[0]; m(0,2) = v2[0];
  m(1,0) = v0[1]; m(1,1) = v1[1]; m(1,2) = v2[1];
  m(2,0) = 1;     m(2,1) = 1;     m(2,2) = 1;    

  VEC3 p_f = VEC3(p[0],p[1],1);
  VEC3 l = (m.inverse())*p_f;

  if ( l[0]<=1 && l[0]>=0 && l[1]<=1 && l[1]>=0 && l[2]<=1 && l[2]>=0 )
    return 1;
  else  
    return 0;
}

VEC3 barycentric (VEC3 v0, VEC3 v1, VEC3 v2, VEC3 p)
{
  MATRIX3 m;
  m(0,0) = v0[0]; m(0,1) = v1[0]; m(0,2) = v2[0];
  m(1,0) = v0[1]; m(1,1) = v1[1]; m(1,2) = v2[1];
  m(2,0) = 1;     m(2,1) = 1;     m(2,2) = 1;    

  VEC3 p_f = VEC3(p[0],p[1],1);
  VEC3 l = (m.inverse())*p_f;

  return l;
}

float *txs;

void load_textures ()
{
  txs = new float[xRes * yRes * 3];
  readPPM("sheep.ppm", 800, 800, txs);
}

// float lookup_texture (VEC3 tx_coord)
// {
//   int x = int(800*tx_coord[0]);
//   int y = int(800*tx_coord[1]);
//   int index = x + 800*y;
//   return txs[index];
// }

VEC3 get_xfmd_txs (MATRIX4 mvp, MATRIX4 mp, MATRIX4 mcam, VEC2 uv1, VEC2 uv2, VEC2 uv3, VEC3 l_p)
{
  // VEC3 tfuv1 = truncate ( perspective_divide ( mvp * mp * mcam * ( extend(VEC3(uv1[0],uv1[1],0)) ) ) );
  // VEC3 tfuv2 = truncate ( perspective_divide ( mvp * mp * mcam * ( extend(VEC3(uv2[0],uv2[1],0)) ) ) );
  // VEC3 tfuv3 = truncate ( perspective_divide ( mvp * mp * mcam * ( extend(VEC3(uv3[0],uv3[1],0)) ) ) );

  // VEC3 tx_coord = l_p[0]*tfuv1 + l_p[1]*tfuv2 + l_p[2]*tfuv3;

  VEC2 tx_coord = l_p[0]*uv1 + l_p[1]*uv2 + l_p[2]*uv3;

  int x = int(800*tx_coord[0]);
  int y = int(800*tx_coord[1]);
  int index = x + 800*(800-y);
  VEC3 tx = VEC3(txs[3*index + 0], txs[3*index + 1], txs[3*index + 2]);
  return tx;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void gen_ppm(const char*);

int main(int argc, char** argv)
{

  int i = 0;
  
  for (i = 0 ; i < 100; i++)
  {
    std::string outname = "out_imgs/img_";
    std::string suffix = ".ppm";

    float dx = 2 * cos (0.1*i);
    float dz = 2 * sin (0.1*i);
    // eye = eye + VEC3(0.1*i,0,0);
    eye = VEC3(dx,2,dz);
    
    std::string i_s = std::to_string(i);
    std::string fname = (outname.append(i_s)).append(suffix);

    gen_ppm(fname.c_str());
  }

}

void gen_ppm(const char* out_file)
{

  vector<VEC3> vertices;
  vector<VEC3I> indices;
  vector<VEC3> colors; 
  vector<VEC2> uvs; 

  int vertex_based_coloring;
  vertex_based_coloring = 0;

  // get geometry - face-based coloring
  // buildSquare(vertices, indices, colors);
  // buildBigSquare(vertices, indices, colors);
  // buildCube(vertices, indices, colors);

  // need to use vertex-based coloring
  buildCubePerVertexColors(vertices, indices, colors);
  vertex_based_coloring = 1;

  // buildCubePerVertexUVs (vertices, indices, uvs);
  // load_textures();

  vector<VEC3> scaled_colors = scale_colors(colors); 

  int n_vert = vertices.size();
  int n_triangles = indices.size();

  // viewport 
  MATRIX4 mvp = get_mvp (xRes, yRes);
  // orthographic 
  MATRIX4 mortho = get_mortho (_right, _left, _bottom, _top, _near, _far);
  // perspective projection
  MATRIX4 mpp = get_mpp (fovy, aspect, _near, _far, eye, lookat, updir);
  // camera
  MATRIX4 mcam = get_mcam (eye, lookat, updir);

  vector<VEC3> vertices_1;
  for (int i = 0 ; i<n_vert ; i++)
  {
    vertices_1.push_back ( truncate ( perspective_divide ( mvp * mpp * mcam * ( extend(vertices[i]) ) ) ) );
    // vertices_1.push_back( truncate( mvp * mortho * mcam *( extend(vertices[i]) ) ) );
    // vertices_1.push_back( truncate( mvp * mortho *( extend(vertices[i]) ) ) );
  }

  float *img = new float[xRes * yRes * 3];
  float *zbuffer = new float[xRes * yRes];


  // initialize z-buffer to infinity
  for (int y = 0; y < yRes; y++)
  {
    for (int x = 0; x < xRes; x++)
    {
      int index = x + ((yRes-y) * xRes);
      // zbuffer[index] = std::numeric_limits<float>::min();
      zbuffer[index] = std::numeric_limits<float>::max();
    }
  }
  vector<VEC3> vertices_out = vertices_1;

  VEC3 v1, v2, v3;
  VEC3 l_p;
  float p_z;

  VEC3 tx_sheep;

  // rasterization
  // for ( int i = 0 ; i<6; i++)
  for ( int i = 0 ; i<n_triangles; i++)
  {
    // get the 3 vertices of the triangle
    v1 = vertices_out[indices[i][0]];
    v2 = vertices_out[indices[i][1]];
    v3 = vertices_out[indices[i][2]];
    for (int y = 0; y < yRes; y++)
    {
      for (int x = 0; x < xRes; x++)
      {
        // Flip vertically coz origin is top left of image
        // int index = x + y * xRes;
        int index = x + ((yRes-y) * xRes);
        VEC3 pt = VEC3(x,y,1);

        // if (int(vertices_mvp[i][0]) == x && int(vertices_mvp[i][1]) == y)
        // if ((int(v1[0])==x && int(v1[1])==y) || (int(v2[0])==x && int(v2[1])==y) || (int(v3[0])==x && int(v3[1])==y))
        if ( is_in_interior(v1,v2,v3,pt)==1 )
        {
          l_p = barycentric(v1,v2,v3,pt);
          p_z = l_p.dot(VEC3(v1[2],v2[2],v3[2]));
          // if (p_z > zbuffer[index])
          if (p_z < zbuffer[index])
          {
            zbuffer[index] = p_z;

            if ( vertex_based_coloring == 0 )
            {
            // face-based coloring
            img[3 * index + 0] = scaled_colors[i][0];
            img[3 * index + 1] = scaled_colors[i][1];
            img[3 * index + 2] = scaled_colors[i][2];

            // tx_sheep = get_xfmd_txs (mvp, mpp, mcam, uvs[indices[i][0]], uvs[indices[i][1]], uvs[indices[i][2]], l_p);
            // img[3 * index + 0] = tx_sheep[0];
            // img[3 * index + 1] = tx_sheep[1];
            // img[3 * index + 2] = tx_sheep[2];
            }
            else 
            {
            // vertex-based coloring
            img[3 * index + 0] = l_p.dot(VEC3(scaled_colors[indices[i][0]][0],
                                                scaled_colors[indices[i][1]][0],
                                                scaled_colors[indices[i][2]][0]));

            img[3 * index + 1] = l_p.dot(VEC3(scaled_colors[indices[i][0]][1],
                                                scaled_colors[indices[i][1]][1],
                                                scaled_colors[indices[i][2]][1]));

            img[3 * index + 2] = l_p.dot(VEC3(scaled_colors[indices[i][0]][2],
                                                scaled_colors[indices[i][1]][2],
                                                scaled_colors[indices[i][2]][2]));
            }
          }
        }
      }
    }
  }

  // write output
  writePPM(out_file, xRes, yRes, img);

  return;
}
