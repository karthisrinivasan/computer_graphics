#include <cmath>
#include <iostream>
#include <vector>
#include <MERSENNE_TWISTER.h>

using namespace std;

// a random number generator
MERSENNE_TWISTER twister(123456);

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
void writePPM(const string& filename, const int xRes, const int yRes, const float* values)
{
  // copy to the data type PPM expects
  int totalCells = xRes * yRes;
  unsigned char* pixels = new unsigned char[3 * totalCells];
  for (int i = 0; i < 3 * totalCells; i++)
    pixels[i] = values[i];

  // try to open the file
  FILE *fp;
  fp = fopen(filename.c_str(), "wb");
  if (fp == NULL)
  {
    cout << " Could not open file \"" << filename.c_str() << "\" for writing." << endl;
    cout << " Make sure you're not trying to write from a weird location or with a " << endl;
    cout << " strange filename. Bailing ... " << endl;
    exit(0);
  }

  // write what PPM expects 
  fprintf(fp, "P6\n%d %d\n255\n", xRes, yRes);
  fwrite(pixels, 1, totalCells * 3, fp);

  // clean up
  fclose(fp);
  delete[] pixels;
  cout << " Wrote out file " << filename.c_str() << endl;
}

///////////////////////////////////////////////////////////////////////
// Example of parsing command line arguments
///////////////////////////////////////////////////////////////////////
void parseArgs(int& argc, char**& argv)
{
  // Print out the command line args
  cout << " Command line arguments were: " << endl;
  for (int x = 0; x < argc; x++)
    cout << argv[x] << " ";
  cout << endl;

  // try to convert them into ints
  vector<int> commandInts(argc);
  for (int x = 0; x < argc; x++)
    commandInts[x] = atoi(argv[x]);

  cout << " I tried to convert each into an integer and got: " << endl;
  for (int x = 0; x < argc; x++)
    cout << commandInts[x] << " ";
  cout << endl;

  // try to convert them into doubles
  vector<double> commandDoubles(argc);
  for (int x = 0; x < argc; x++)
    commandDoubles[x] = atof(argv[x]);

  cout << " I tried to convert each into double and got: " << endl;
  for (int x = 0; x < argc; x++)
    cout << commandDoubles[x] << " ";
  cout << endl;
}

int avg5x5 (int x, int y, int xRes, int yRes, float*& pic)
{
  float avg = 0;
  int i, j;
  int index = x + y * xRes;

  // int x_lb = (x<2) ? -x : -2;

  int x_lb = -(min(2,x));
  int y_lb = -(min(2,y));

  int x_ub = min(2,xRes-x);
  int y_ub = min(2,yRes-y);

  for (j = y_lb; j <= y_ub ; j++)
    {
    for (i = x_lb; i <= x_ub ; i++)
      {
        avg = avg + pic[3*(index+i+j*xRes)];
      }
    }

  avg = 0.04*avg;
  return avg;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  int xRes, yRes;
  float* values = NULL;

  if (argc != 2)
  {
    cout << " Expecting args " << argv[0] << " <input filename> " << endl;
    return 0;
  }

  readPPM(argv[1], xRes, yRes, values);

  // stomp off everything except red
  for (int y = 0; y < yRes; y++)
    for (int x = 0; x < xRes; x++)
    {
      int index = x + y * xRes;
      values[3 * index    ] = avg5x5(x, y, xRes, yRes, values);
      values[3 * index + 1] = avg5x5(x, y, xRes, yRes, values);
      values[3 * index + 2] = avg5x5(x, y, xRes, yRes, values);

      // values[3 * index + 1] = 0;
      // values[3 * index + 2] = 0;
    }
  
  writePPM("filtered.ppm", xRes, yRes, values);

  return 0;
}
