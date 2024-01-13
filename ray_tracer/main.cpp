#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

#include "SETTINGS.h"

using namespace std;

const float pi = 3.141;
// resolution of the field
int xRes = 800;
int yRes = 600;

int phong_exp = 10;
// world bounding box
// float _left = -12.0;
// float _right = 12.0;
// float _bottom = -12.0;
// float _top = 12.0;

// float _far = 0.0;
// float _far = 100.0;
// float _near = 12.0;
float _near = 1.0;

float fovy = 65.0;
// float fovy = 32.5;
float aspect = 4.0/3.0;
// float fovx = fovy * aspect;

// eye position, gaze, up direction
// VEC3 eye = VEC3(0.2,0.2,1.0);
// VEC3 eye = VEC3(1.0,1.0,1.0);
VEC3 eye = VEC3(0.0,0.0,0.0);
VEC3 lookat = VEC3(0,0,1);
VEC3 updir = VEC3(0,1,0);


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void readPPM(const string& filename, int& xRes, int& yRes, float*& values)
{
  // try to open the file
  FILE *fp;
  fp = fopen(filename.c_str(), "rb");
  if (fp == NULL)
  {
    cout << " Could not open file \"" << filename.c_str() << "\" for reading." << endl;
    cout << " Make sure you're not trying to read from a weird location or with a " << endl;
    cout << " strange filename. Bailing ... " << endl;
    exit(0);
  }

  // get the dimensions
  unsigned char newline;
  fscanf(fp, "P6\n%d %d\n255%c", &xRes, &yRes, &newline);
  if (newline != '\n') {
    cout << " The header of " << filename.c_str() << " may be improperly formatted." << endl;
    cout << " The program will continue, but you may want to check your input. " << endl;
  }
  int totalCells = xRes * yRes;

  // grab the pixel values
  unsigned char* pixels = new unsigned char[3 * totalCells];
  fread(pixels, 1, totalCells * 3, fp);

  // copy to a nicer data type
  values = new float[3 * totalCells];
  for (int i = 0; i < 3 * totalCells; i++)
    values[i] = pixels[i];

  // clean up
  delete[] pixels;
  fclose(fp);
  cout << " Read in file " << filename.c_str() << endl;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void writePPM(const string& filename, int& xRes, int& yRes, float*& values)
{
  int totalCells = xRes * yRes;
  unsigned char* pixels = new unsigned char[3 * totalCells];
  for (int i = 0; i < 3 * totalCells; i++)
    pixels[i] = values[i];

  FILE *fp;
  fp = fopen(filename.c_str(), "wb");
  if (fp == NULL)
  {
    cout << " Could not open file \"" << filename.c_str() << "\" for writing." << endl;
    cout << " Make sure you're not trying to write from a weird location or with a " << endl;
    cout << " strange filename. Bailing ... " << endl;
    exit(0);
  }

  fprintf(fp, "P6\n%d %d\n255\n", xRes, yRes);
  fwrite(pixels, 1, totalCells * 3, fp);
  fclose(fp);
  delete[] pixels;
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

void buildSpheres(vector<VEC3>& centers, vector<float>& radii, vector<VEC3>& colors)
{
  centers.push_back(VEC3(-3.5, 0, 10));
  centers.push_back(VEC3(3.5, 0, 10));
  centers.push_back(VEC3(0, -1000, 10));

  radii.push_back(3);
  radii.push_back(3);
  radii.push_back(997);

  colors.push_back(VEC3(1, 0.25, 0.25));
  colors.push_back(VEC3(0.25, 0.25, 1));
  colors.push_back(VEC3(0.5, 0.5, 0.5));

}

void buildSpheres1(vector<VEC3>& centers, vector<float>& radii, vector<VEC3>& colors)
{
  centers.push_back(VEC3(-3.5, 0, 10));
  centers.push_back(VEC3(3.5, 0, 10));
  centers.push_back(VEC3(0, -1000, 10));

  radii.push_back(3);
  radii.push_back(3);
  radii.push_back(997);

  colors.push_back(VEC3(1, 0.25, 0.25));
  colors.push_back(VEC3(0.25, 0.25, 1));
  colors.push_back(VEC3(0.5, 0.5, 0.5));
  for ( int i = 0 ; i < 20 ; i++)
  {
    for ( int j = 0 ; j < 10 ; j++)
    {
      centers.push_back(VEC3(-20+2*i,-2+2*j,20));
      radii.push_back(1);
      colors.push_back(VEC3(1, 1, 1));
    }
  }
}

void buildLights(vector<VEC3>& positions, vector<VEC3>& colors)
{
  positions.push_back(VEC3(10, 3, 5));
  positions.push_back(VEC3(-10, 3, 7.5));

  colors.push_back(VEC3(1, 1, 1));
  colors.push_back(VEC3(0.5, 0, 0));
}

// !! assumes normalized dir !!
float get_nearest_intersection(VEC3 center, float radius, VEC3 dir)
{
  // float epsilon = 1e-4;
  float t;
  VEC3 emc = eye - center;
  float discr = (dir.dot(emc) * dir.dot(emc)) - (emc.dot(emc) - radius*radius);

  if ( discr < 0 ) 
    return std::numeric_limits<float>::max();
  else 
    return (-dir.dot(emc) - sqrt(discr));

}

float get_nearest_intersection_shadow (VEC3 center, float radius, VEC3 dir, VEC3 base)
{
  // float epsilon = 1e-4;
  float t;
  VEC3 emc = base - center;
  float discr = (dir.dot(emc) * dir.dot(emc)) - (emc.dot(emc) - radius*radius);

  if ( discr < 0 ) 
    return std::numeric_limits<float>::max();
  else 
    return (-dir.dot(emc) - sqrt(discr));

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
// Sphere 0, center = (-3.5, 0, 10), radius = 3, color = (1, 0.25, 0.25)
// Sphere 1, center = (3.5, 0, 10), radius = 3, color = (0.25, 0.25, 1)
// Sphere 2, center = (0, -1000, 10), radius = 997, color = (0.5, 0.5, 0.5)

// Light 0, position = (10, 3, 5), color = (1, 1, 1)
// Light 1, position = (-10, 3, 7.5), color = (0.5, 0, 0)

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  // compute vectors
  VEC3 gaze = lookat - eye;

  vector<VEC3> centers;
  vector<float> radii;
  vector<VEC3> colors; 

  vector<VEC3> light_positions;
  vector<VEC3> light_colors; 

  buildLights (light_positions, light_colors);
  buildSpheres1 (centers, radii, colors);
  int n_lights = 2;
  vector<VEC3> scaled_colors = scale_colors (colors);
  int n_spheres = radii.size();

  // assume eye at origin, gaze along z
  float rb = (_near * aspect * tan ( 0.5 * fovy * pi / 180 ));
  float lb = -rb;
  float ub = (_near * tan ( 0.5 * fovy * pi / 180 ));
  float db = -ub;

  printf("right boundary: %f\n", rb);
  printf("down boundary: %f\n", db);
  printf("left boundary: %f\n", lb);
  printf("up boundary: %f\n", ub);

  float *img = new float[xRes * yRes *3]; 

  for (int y = 0; y < yRes; y++)
    {
      for (int x = 0; x < xRes; x++)
      {
        int index = x + ((yRes-y) * xRes);

        VEC3 pix_loc = VEC3((lb + (rb-lb)*(x+0.5)/xRes),(db + (ub-db)*(y+0.5)/yRes),_near);
        VEC3 ray_dir = pix_loc - eye;
        ray_dir.normalize();

        // compute intersection
        int closest_sphere = -1;
        float t = std::numeric_limits<float>::max() - 1;
        VEC3 normal;
        VEC3 intxn_pt;
        for (int sphere_index = 0 ; sphere_index < n_spheres ; sphere_index++)
        {
          float t_new = get_nearest_intersection (centers[sphere_index], radii[sphere_index], ray_dir);
          if ( t_new < t && (eye+t*ray_dir)[2] > _near && t_new > 0) 
          { 
            // if (closest_sphere > -1) printf ("intersection found already, %f, %f\n", t_new, t);
            t = t_new;
            closest_sphere = sphere_index;
            intxn_pt = eye + t*ray_dir;
            normal = intxn_pt - centers[closest_sphere];
            normal.normalize();
          }
        }

        img [3 * index + 0] = 0;
        img [3 * index + 1] = 0;
        img [3 * index + 2] = 0;
        
        if (closest_sphere > -1)
        {
          for ( int light_index = 0 ; light_index < n_lights ; light_index++ )
          {
            VEC3 ldir = light_positions[light_index] - intxn_pt;
            ldir.normalize();
  
            // check if point is 'in the shade' wrt this light source
            int shade = 0;
            for (int sphere_index = 0 ; sphere_index < n_spheres ; sphere_index++)
            {
              float t1 = get_nearest_intersection_shadow (centers[sphere_index], radii[sphere_index], ldir, intxn_pt);
              if ( t1 < std::numeric_limits<float>::max() && t1 > 1e-4 && sphere_index != 2 ) 
              {
                // printf ("got here\n");
                shade = 1;
                break;
              }
            }

            if ( shade == 0 )
            {
            // diffuse shading
            img [3 * index + 0] += light_colors[light_index][0] * colors[closest_sphere][0] * max(0.0,normal.dot(ldir));
            img [3 * index + 1] += light_colors[light_index][1] * colors[closest_sphere][1] * max(0.0,normal.dot(ldir));
            img [3 * index + 2] += light_colors[light_index][2] * colors[closest_sphere][2] * max(0.0,normal.dot(ldir));
          
            // phong shading
            VEC3 vdir = -t*ray_dir;
            vdir.normalize();
            VEC3 h = vdir+ldir;
            h.normalize();
            img [3 * index + 0] += light_colors[light_index][0] * 0.5 * pow(max(0.0,normal.dot(h)),phong_exp);
            img [3 * index + 1] += light_colors[light_index][1] * 0.5 * pow(max(0.0,normal.dot(h)),phong_exp);
            img [3 * index + 2] += light_colors[light_index][2] * 0.5 * pow(max(0.0,normal.dot(h)),phong_exp);
            }
          }

          img [3 * index + 0] = min (float(1.0), img [3 * index + 0]);
          img [3 * index + 1] = min (float(1.0), img [3 * index + 1]);
          img [3 * index + 2] = min (float(1.0), img [3 * index + 2]);

          img [3 * index + 0] *= 255;
          img [3 * index + 1] *= 255;
          img [3 * index + 2] *= 255;
          // img [3 * index + 0] = scaled_colors[closest_sphere][0];
          // img [3 * index + 1] = scaled_colors[closest_sphere][1];
          // img [3 * index + 2] = scaled_colors[closest_sphere][2];

          }

        // img [3 * index + 0] = 255*ray_dir(0);
        // img [3 * index + 1] = 255*ray_dir(1);
        // img [3 * index + 2] = 0;
      }
    }

  writePPM ("out.ppm", xRes, yRes, img);
  // writePPM ("out1.ppm", xRes, yRes, img);

  return 0;
}
