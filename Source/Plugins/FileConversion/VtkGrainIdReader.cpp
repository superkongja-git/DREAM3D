/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VtkGrainIdReader.h"

#include <map>

#include "MXA/Common/LogTime.h"

#include "DREAM3DLib/Common/Constants.h"


#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkGrainIdReader::VtkGrainIdReader() :
m_GrainIdScalarName(DREAM3D::VTK::GrainIdScalarName)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkGrainIdReader::~VtkGrainIdReader()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::parseFloat3V(const char* input, float* output, float defaultValue)
{
  char text[256];
  int n = sscanf(input, "%s %f %f %f", text, &(output[0]), &(output[1]), &(output[2]) );
  if (n != 4)
  {
    output[0] = output[1] = output[2] = defaultValue;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::parseInt3V(const char* input, int* output, int defaultValue)
{
  char text[256];
  int n = sscanf(input, "%s %d %d %d", text, &(output[0]), &(output[1]), &(output[2]) );
  if (n != 4)
  {
    output[0] = output[1] = output[2] = defaultValue;
    return -1;
  }
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::nonPrintables(char* buf, size_t bufSize)
{
  int n = 0;
  for (size_t i = 0; i < bufSize; ++i)
  {
    if (buf[i] < 33 && buf[i] > 0) { n++; }
  }
  return n;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t VtkGrainIdReader::parseByteSize(char text[256])
{

  char cunsigned_char [64] = "unsigned_char";
  char cchar [64] = "char";
  char cunsigned_short [64] = "unsigned_short";
  char cshort [64] = "short";
  char cunsigned_int [64] = "unsigned_int";
  char cint [64] = "int";
  char cunsigned_long [64] = " unsigned_long";
  char clong [64] = "long";
  char cfloat [64] = "float";
  char cdouble [64] = " double";

  if (strcmp(text, cunsigned_char) == 0 ) { return 1;}
  if (strcmp(text, cchar) == 0 ) { return 1;}
  if (strcmp(text, cunsigned_short) == 0 ) { return 2;}
  if (strcmp(text, cshort) == 0 ) { return 2;}
  if (strcmp(text, cunsigned_int) == 0 ) { return 4;}
  if (strcmp(text, cint) == 0 ) { return 4;}
  if (strcmp(text, cunsigned_long) == 0 ) { return 8;}
  if (strcmp(text, clong) == 0 ) { return 8;}
  if (strcmp(text, cfloat) == 0 ) { return 4;}
  if (strcmp(text, cdouble) == 0 ) { return  8;}
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::ignoreData(std::ifstream &in, int byteSize, char* text, int xDim, int yDim, int zDim)
{
  char cunsigned_char [64] = "unsigned_char";
  char cchar [64] = "char";
  char cunsigned_short [64] = "unsigned_short";
  char cshort [64] = "short";
  char cunsigned_int [64] = "unsigned_int";
  char cint [64] = "int";
  char cunsigned_long [64] = " unsigned_long";
  char clong [64] = "long";
  char cfloat [64] = "float";
  char cdouble [64] = " double";
  int err = 0;
  if (strcmp(text, cunsigned_char) == 0 ) {
    err |= skipVolume<unsigned char>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cchar) == 0 ) { err |= skipVolume<char>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_short) == 0 ) { err |= skipVolume<unsigned short>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cshort) == 0 ) {err |= skipVolume<short>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_int) == 0 ) { err |= skipVolume<unsigned int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cint) == 0 ) { err |= skipVolume<int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_long) == 0 ) { err |= skipVolume<unsigned long long int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, clong) == 0 ) { err |= skipVolume<long long int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cfloat) == 0 ) { err |= skipVolume<float>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cdouble) == 0 ) { err |= skipVolume<double>(in, byteSize, xDim, yDim, zDim);}
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::readHeader()
{

  int err = 0;
  if (getFileName().empty() == true)
  {
    std::stringstream ss;
    ss << "Input filename was empty";
    setErrorMessage(ss.str());
    return -1;
  }

  std::ifstream instream;
  instream.open(getFileName().c_str(), std::ios_base::binary);
  if (!instream.is_open())
  {
    std::stringstream ss;
    ss << "Vtk file could not be opened: " << getFileName();
    setErrorMessage(ss.str());
    return -1;
  }
  char buf[kBufferSize];
  instream.getline(buf, kBufferSize); // Read Line 1 - VTK Version Info
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 2 - User Comment
  setComment(std::string(buf));
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 3 - BINARY or ASCII
  std::string fileType(buf);
  if (fileType.find("BINARY", 0) == 0)
  {
    setFileIsBinary(true);
  }
  else if (fileType.find("ASCII", 0) == 0)
  {
    setFileIsBinary(false);
  }
  else
  {
    err = -1;
    std::stringstream ss;
    ss << "The file type of the VTK legacy file could not be determined. It should be ASCII' or 'BINARY' and should appear on line 3 of the file.";
    setErrorMessage(ss.str());
    return err;
  }
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 4 - Type of Dataset
  {
    char text[256];
    int n = sscanf(buf, "%s %s", text, &(text[16]) );
    if (n < 2)
    {
      std::stringstream ss;
      ss << "Error Reading the type of data set. Was expecting 2 fields but got " << n;
      setErrorMessage(ss.str());
      return -1;
    }
    std::string dataset(&(text[16]));
    setDatasetType(dataset);
  }

  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 5 which is the Dimension values
  int dims[3];
  err = parseInt3V(buf, dims, 0);
  setDimensions(dims);

#if 0
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 6 which is the Origin values
  float origin[3];
  err = parseFloat3V(buf, origin, 0.0f);
  setOrigin(origin);

  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize);// Read Line 7 which is the Scaling values
  float resolution[3];
  err = parseFloat3V(buf, resolution, 1.0f);
  setResolution(resolution);

  ::memset(buf, 0, kBufferSize);
#endif



  instream.close();
  return err;

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::parseCoordinateLine(const char* input, int &value)
{
  char text[256];
  char text1[256];
  int i = 0;
  int n = sscanf(input, "%s %d %s", text, &i, text1);
  if (n != 3)
  {
    value = -1;
    return -1;
  }
  value = i;
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::readLine(std::istream &in, char* buf, int bufSize)
{

  bool readAnotherLine = true;
  size_t gcount = in.gcount();
  while ( readAnotherLine == true && in.gcount() != 0) {
    // Zero out the buffer
    ::memset(buf, 0, bufSize);
    // Read a line up to a '\n' which will catch windows and unix line endings but
    // will leave a trailing '\r' at the end of the string
    in.getline(buf, kBufferSize);
    gcount = in.gcount();
    if (gcount > 1 && buf[in.gcount()-2] == '\r')
    {
      buf[in.gcount()-2] = 0;
    }
    int len = strlen(buf);
    int np = nonPrintables(buf, bufSize);
    if (len != np)
    {
      readAnotherLine = false;
    }

  }
  return static_cast<int>(in.gcount());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::readGrainIds()
{
  int err = 0;
  
  err = readHeader();
  if (err < 0) { return err; }

  std::string filename = getFileName();
  std::ifstream instream;
  instream.open(filename.c_str(), std::ios_base::binary);
  if (!instream.is_open())
  {
    std::stringstream ss;
    ss << logTime() << " vtk file could not be opened: " << filename << std::endl;
    setErrorMessage(ss.str());
    return -1;
  }

  char buf[kBufferSize];
  for (int i = 0; i < 5; ++i)
  {
    instream.getline(buf, kBufferSize);
  }

  int dims[3];
  getDimensions(dims);


  int dim = 0;
  // Now parse the X, coordinates.
 // ::memset(buf, 0, kBufferSize);
  err = readLine(instream, buf, kBufferSize);
  err = parseCoordinateLine(buf, dim);
  if (err < 0 || dim != dims[0])
  {
    std::stringstream ss;
    ss << "x dimension does not match expected dimension: " << dim << " <--> " << dims[0];
    setErrorMessage(ss.str());
    return -1;
  }
  float xscale = 1.0f;
  err = skipVolume<float>(instream, 4, dim, 1, 1, xscale);

  // Now parse the Y coordinates.
 // ::memset(buf, 0, kBufferSize);
  err = readLine(instream, buf, kBufferSize);
  err = parseCoordinateLine(buf, dim);
  if (err < 0 || dim != dims[1])
  {
    std::stringstream ss;
    ss << "y dimension does not match expected dimension: " << dim << " <--> " << dims[1];
    setErrorMessage(ss.str());
    return -1;
  }
  float yscale = 1.0f;
  err = skipVolume<float>(instream, 4, 1, dim, 1, yscale);
 
  // Now parse the Z coordinates.
//  ::memset(buf, 0, kBufferSize);
  err = readLine(instream, buf, kBufferSize);
  err = parseCoordinateLine(buf, dim);
  if (err < 0 || dim != dims[2])
  {
    std::stringstream ss;
    ss << "z dimension does not match expected dimension: " << dim << " <--> " << dims[2];
    setErrorMessage(ss.str());
    return -1;
  }
  float zscale = 1.0f;
  err = skipVolume<float>(instream, 4, 1, 1, dim, zscale);
  if (err < 0)
  {
    std::stringstream ss;
    ss << "Error skipping Volume section of VTK file.";
    return err;
  }
  // This makes a very bad assumption that the Rectilinear grid has even spacing
  // along each axis which it does NOT have to have. Since this class is specific
  // to the DREAM.3D package this is a safe assumption.
  setResolution(xscale, yscale, zscale);


  // Now we need to search for the 'GrainID' and
  char text1[kBufferSize];
  ::memset(text1, 0, kBufferSize);
  char text2[kBufferSize];
  ::memset(text2, 0, kBufferSize);
  char text3[kBufferSize];
  ::memset(text3, 0, kBufferSize);
  int fieldNum = 0;
  bool needGrainIds = true;

  std::string scalarName;
  int typeByteSize = 0;

  //size_t index = 0;
  //Cell Data is one less in each direction
  setDimensions(dims[0] -1, dims[1] -1, dims[2] -1);
  getDimensions(dims);
  size_t totalVoxels = dims[0] * dims[1] * dims[2];
  DataArray<int>::Pointer grainIds = DataArray<int>::CreateArray(totalVoxels);
  grainIds->SetName("GrainIds");
  readLine(instream, buf, kBufferSize);

 // int i = 0;
  while (needGrainIds == true)
  {
    readLine(instream, buf, kBufferSize);
    ::memset(text1, 0, kBufferSize);
    ::memset(text2, 0, kBufferSize);
    ::memset(text3, 0, kBufferSize);
    int n = sscanf(buf, "%s %s %s %d", text1, text2, text3, &fieldNum);
    if (n != 4)
    {
      std::stringstream ss;
      ss << "Error reading SCALARS header section of VTK file.";
      setErrorMessage(ss.str());
      return -1;
    }
    scalarName = std::string(text2);
    typeByteSize = parseByteSize(text3);
    if (typeByteSize == 0)
    {
      return -1;
    }

    readLine(instream, buf, kBufferSize); // Read Line 11

    // Check to make sure we are reading the correct set of scalars and if we are
    // NOT then read all this particular Scalar Data and try again

    if (m_GrainIdScalarName.compare(scalarName) == 0)
    {
    //  std::map<int, int> grainIdMap;
      if (getFileIsBinary() == true)
      {
        // Splat 0xAB across the entire array. that way if the read messes up we
        //  can more easily diagnose the problem.
        ::memset(grainIds->GetPointer(0), 0xAB, sizeof(int) * totalVoxels);
        instream.read(reinterpret_cast<char*> (grainIds->GetPointer(0)), sizeof(int) * totalVoxels);
        int t;
        // We need to Byte Swap (Possibly) from the Big Endian format stored by
        // the vtk binary file into what ever system we are running.
        for (size_t i = 0; i < totalVoxels; ++i)
        {
          t = grainIds->GetValue(i);
          MXA::Endian::FromBigToSystem::convert<int>(t);
          grainIds->SetValue(i, t);
        }
      }
      else // ASCII VTK File
      {
        int grain_index = -1;
        for (size_t i = 0; i < totalVoxels; ++i)
        {
          instream >> grain_index;
          grainIds->SetValue(i, grain_index);
       //   grainIdMap[grain_index]++;
        }
      }
      needGrainIds = false;
    }
    else
    {
        ignoreData(instream, typeByteSize, text3, dims[0], dims[1], dims[2]);
    }

  }
  setGrainIds(grainIds);
  instream.close();
  return err;
}
