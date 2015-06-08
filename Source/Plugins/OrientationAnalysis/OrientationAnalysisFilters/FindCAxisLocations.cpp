/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "FindCAxisLocations.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCAxisLocations::FindCAxisLocations() :
  AbstractFilter(),
  m_QuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_CAxisLocationsArrayName(DREAM3D::CellData::CAxisLocation),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_CAxisLocations(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCAxisLocations::~FindCAxisLocations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCAxisLocations::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Required Information", FilterParameter::Uncategorized));
  parameters.push_back(FilterParameter::New("Element Quaternions", "QuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getQuatsArrayPath(), FilterParameter::Uncategorized, ""));
  parameters.push_back(SeparatorFilterParameter::New("Created Information", FilterParameter::Uncategorized));
  parameters.push_back(FilterParameter::New("C-Axis Locations", "CAxisLocationsArrayName", FilterParameterWidgetType::StringWidget, getCAxisLocationsArrayName(), FilterParameter::Uncategorized, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCAxisLocations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCAxisLocationsArrayName(reader->readString("CAxisLocationsArrayName", getCAxisLocationsArrayName() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindCAxisLocations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CAxisLocationsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCAxisLocations::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> cDims(1, 4);
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(m_QuatsArrayPath.getDataContainerName(), m_QuatsArrayPath.getAttributeMatrixName(), getCAxisLocationsArrayName() );
  m_CAxisLocationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CAxisLocationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CAxisLocations = m_CAxisLocationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCAxisLocations::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCAxisLocations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalPoints = m_QuatsPtr.lock()->getNumberOfTuples();

  QuatF q1 = QuaternionMathF::New();
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float g1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float g1t[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float caxis[3] = {0.0f, 0.0f, 1.0f};
  float c1[3] = {0.0f, 0.0f, 0.0f};

  size_t index = 0;
  for (size_t i = 0; i < totalPoints; i++)
  {
    index = 3 * i;
    QuaternionMathF::Copy(quats[i], q1);
    FOrientArrayType om(9);
    FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
    om.toGMatrix(g1);
    // transpose the g matricies so when caxis is multiplied by it
    // it will give the sample direction that the caxis is along
    MatrixMath::Transpose3x3(g1, g1t);
    MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
    // normalize so that the magnitude is 1
    MatrixMath::Normalize3x1(c1);
    if (c1[2] < 0) { MatrixMath::Multiply3x1withConstant(c1, -1); }
    m_CAxisLocations[index] = c1[0];
    m_CAxisLocations[index + 1] = c1[1];
    m_CAxisLocations[index + 2] = c1[2];
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindCAxisLocations::newFilterInstance(bool copyFilterParameters)
{
  FindCAxisLocations::Pointer filter = FindCAxisLocations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCAxisLocations::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCAxisLocations::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCAxisLocations::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCAxisLocations::getHumanLabel()
{ return "Find C-Axis Locations"; }
