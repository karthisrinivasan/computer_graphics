#include "COLOR_FIELD_2D.h"
#include <assert.h>

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D::COLOR_FIELD_2D(const int& rows, const int& cols) :
  _xRes(rows), _yRes(cols)
{
  _totalCells = _xRes * _yRes;
  _data = new VEC3F[_totalCells];

  for (int x = 0; x < _totalCells; x++)
    _data[x].setZero();
}

COLOR_FIELD_2D::COLOR_FIELD_2D(const COLOR_FIELD_2D& m) :
  _xRes(m.xRes()), _yRes(m.yRes())
{
  _totalCells = _xRes * _yRes;
  _data = new VEC3F[_totalCells];

  for (int x = 0; x < _totalCells; x++)
    _data[x] = m[x];
}

COLOR_FIELD_2D::COLOR_FIELD_2D() :
  _xRes(0), _yRes(0), _totalCells(0), _data(NULL)
{
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D::~COLOR_FIELD_2D()
{
  delete[] _data;
}
  
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::clear()
{
  for (int x = 0; x < _totalCells; x++)
    _data[x].setZero();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::writePPM(const string& filename) const
{
  FILE *fp;
  unsigned char* pixels = new unsigned char[3 * _totalCells];

  for (int x = 0; x < _totalCells; x++)
  {
    pixels[3 * x] = 255 * _data[x][0];
    pixels[3 * x + 1] = 255 * _data[x][1];
    pixels[3 * x + 2] = 255 * _data[x][2];
  }

  fp = fopen(filename.c_str(), "wb");
  fprintf(fp, "P6\n%d %d\n255\n", _xRes, _yRes);
  fwrite(pixels, 1, _totalCells * 3, fp);
  fclose(fp);
  delete[] pixels;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::normalize()
{
  float maxFound = 0.0;
  float minFound = _data[0].norm();
  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
    {
      maxFound = (_data[x][y] > maxFound) ? _data[x][y] : maxFound;
      minFound = (_data[x][y] < minFound) ? _data[x][y] : minFound;
    }

  float range = 1.0 / (maxFound - minFound);
  for (int x = 0; x < _totalCells; x++)
  {
    for (int y = 0; y < 3; y++)
      _data[x][y] -= minFound;
    _data[x] *= range;
  }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::abs()
{
  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
      _data[x][y] = fabs(_data[x][y]);

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::resizeAndWipe(int xRes, int yRes)
{
  if (_xRes == xRes && _yRes == yRes)
  {
    clear();
    return;
  }

  if (_data)
    delete[] _data;

  _xRes = xRes;
  _yRes = yRes;
  _totalCells = _xRes * _yRes;

  _data = new VEC3F[_xRes * _yRes];
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    _data[x].setConstant(alpha);

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator*=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] *= alpha;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator/=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] /= alpha;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator+=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
      _data[x][y] += alpha;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator-=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);
  for (int x = 0; x < _totalCells; x++)
    _data[x] -= input[x];

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator+=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);
  for (int x = 0; x < _totalCells; x++)
    _data[x] += input[x];

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator*=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);

  for (int x = 0; x < _totalCells; x++)
  {
    _data[x][0] *= input[x][0];
    _data[x][1] *= input[x][1];
    _data[x][2] *= input[x][2];
  }

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator/=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);

  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
      if (fabs(input[x][y]) > 1e-6)
        _data[x][y] /= input[x][y];
      else
        _data[x][y] = 0;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator*(const COLOR_FIELD_2D& A, const float alpha)
{
  COLOR_FIELD_2D final(A);
  final *= alpha;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator/(const COLOR_FIELD_2D& A, const float alpha)
{
  COLOR_FIELD_2D final(A);
  final /= alpha;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator+(const COLOR_FIELD_2D& A, const COLOR_FIELD_2D& B)
{
  COLOR_FIELD_2D final(A);
  final += B;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator-(const COLOR_FIELD_2D& A, const COLOR_FIELD_2D& B)
{
  COLOR_FIELD_2D final(A);
  final -= B;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator+(const COLOR_FIELD_2D& A, const float alpha)
{
  COLOR_FIELD_2D final(A);
  final += alpha;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator*(const float alpha, const COLOR_FIELD_2D& A)
{
  return A * alpha;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator+(const float alpha, const COLOR_FIELD_2D& A)
{
  return A + alpha;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator=(const COLOR_FIELD_2D& A)
{
  resizeAndWipe(A.xRes(), A.yRes());

  for (int x = 0; x < _totalCells; x++)
    _data[x] = A[x];

  return *this;
}

///////////////////////////////////////////////////////////////////////
// sum of all entries
///////////////////////////////////////////////////////////////////////
VEC3F COLOR_FIELD_2D::sum()
{
  VEC3F total;
  for (int x = 0; x < _totalCells; x++)
    total += _data[x];

  return total;
}

///////////////////////////////////////////////////////////////////////
// take the log
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::log(float base)
{
  float scale = 1.0 / std::log(base);
  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
      _data[x][y] = std::log(_data[x][y]) * scale;
}

///////////////////////////////////////////////////////////////////////
// get the min of the field
///////////////////////////////////////////////////////////////////////
VEC3F COLOR_FIELD_2D::min()
{
  assert(_xRes > 0);
  assert(_yRes > 0);
  VEC3F final = _data[0];

  for (int i = 0; i < _xRes * _yRes; i++)
    for (int j = 0; j < 3; j++)
      final[j] = (_data[i][j] < final[j]) ? _data[i][j] : final[j];

  return final;
}

///////////////////////////////////////////////////////////////////////
// get the max of the field
///////////////////////////////////////////////////////////////////////
VEC3F COLOR_FIELD_2D::max()
{
  assert(_xRes > 0);
  assert(_yRes > 0);
  VEC3F final = _data[0];

  for (int i = 0; i < _xRes * _yRes; i++)
    for (int j = 0; j < 3; j++)
      final[j] = (_data[i][j] > final[j]) ? _data[i][j] : final[j];

  return final;
}
