#include <cmath>
#include <iostream>
#include <vector>
#include <MERSENNE_TWISTER.h>

using namespace std;

#define Assert(a,b) do { if (!(a)) { fprintf (stderr, "Assertion failed, file %s, line %d\n", __FILE__, __LINE__); fprintf (stderr, "Assertion: %s\n", #a); fprintf (stderr, "ERR: %s\n", b); exit (4); } } while (0)


// a random number generator
MERSENNE_TWISTER twister(123456);

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void writePPM(string filename, int xRes, int yRes, float* values)
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

float *genvalues(float prob, vector<int> colors)
{
  double randn;
  float* values = new float[500 * 500 * 3];

  for (int x = 0; x < 500 * 500; x++)
  {
    randn = twister.rand();
    if (randn < prob)
    {
      values[3 * x] = colors[0];
      values[3 * x + 1] = colors[1];
      values[3 * x + 2] = colors[2];
    }
    else 
    {
      values[3 * x] = colors[3];
      values[3 * x + 1] = colors[4];
      values[3 * x + 2] = colors[5];
    }
  }

  return values;

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  // Print out the command line args
  cout << " Command line arguments were: " << endl;
  for (int x = 0; x < argc; x++)
    cout << argv[x] << " ";
  cout << endl;

  // cout << argc ;
  // probability arg
  float prob;
  prob = atof(argv[1]);

  // try to convert them into ints
  Assert((argc==8),"Specify colors");

  vector<int> colors(6);
  for (int x = 0; x < 6; x++)
    colors[x] = atoi(argv[x+2]);


  fprintf(stdout, "%f", prob);

  float* values = genvalues(prob, colors);

  writePPM("noise.ppm", 500, 500, values);

}
