/*=============================================================================

   Name     : flumorereader.cpp

   System   : FME Plug-in SDK

   Language : C++

   Purpose  : IFMEReader method implementations

         Copyright (c) 1994 - 2017, Safe Software Inc. All rights reserved.

   Redistribution and use of this sample code in source and binary forms, with 
   or without modification, are permitted provided that the following 
   conditions are met:
   * Redistributions of source code must retain the above copyright notice, 
     this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation 
     and/or other materials provided with the distribution.

   THIS SAMPLE CODE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SAMPLE CODE, EVEN IF 
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

// Include Files
#include "flumorereader.h"
#include "flumorepriv.h"

#include <fmestring.h>
#include <igeometrytools.h>
#include <ilogfile.h>
#include <fmemap.h>
#include <isession.h>
#include <ifeature.h>
#include <vector>
#include <tuple>

// These are initialized externally when a reader object is created so all
// methods in this file can assume they are ready to use.
IFMELogFile* FLUMOREReader::gLogFile = NULL;
IFMEMappingFile* FLUMOREReader::gMappingFile = NULL;
IFMECoordSysManager* FLUMOREReader::gCoordSysMan = NULL;
IFMESession* gFMESession = NULL;

//===========================================================================
// Constructor
FLUMOREReader::FLUMOREReader(const char* readerTypeName, const char* readerKeyword)
:
   readerTypeName_(readerTypeName),
   readerKeyword_(readerKeyword),
   dataset_(""),
   coordSys_(""),
   fmeGeometryTools_(NULL)
{
}


//===========================================================================
// Open
FME_Status FLUMOREReader::open(const char* datasetName, const IFMEStringArray& parameters)
{
   // Get geometry tools
   fmeGeometryTools_ = gFMESession->getGeometryTools();
   FMEString::setSession(gFMESession);
   dataset_ = datasetName;

   // -----------------------------------------------------------------------
   // Add additional setup here
   // -----------------------------------------------------------------------

   // Log an opening reader message
   gLogFile->logMessageString((kMsgOpeningReader + dataset_).c_str());

   // -----------------------------------------------------------------------
   // Open the dataset here, e.g. inputFile.open(dataSetName, ios::in);
   // -----------------------------------------------------------------------

   // Read the mapping file parameters if there is one specified.
   if (parameters.entries() < 1)
   {
      // We are in "open to read data features" mode.
      readParametersDialog();
   }

   return FME_SUCCESS;
}

//===========================================================================
// Abort
FME_Status FLUMOREReader::abort()
{
   // -----------------------------------------------------------------------
   // Add any special actions to shut down a reader not finished reading
   // data; e.g. log a message or send an email. 
   // -----------------------------------------------------------------------

   close();
   return FME_SUCCESS;
}

//===========================================================================
// Close
FME_Status FLUMOREReader::close()
{
   // -----------------------------------------------------------------------
   // Perform any closing operations / cleanup here; e.g. close opened files
   // -----------------------------------------------------------------------

   // Log that the reader is done
   gLogFile->logMessageString((kMsgClosingReader + dataset_).c_str());

   return FME_SUCCESS;
}

int32_t _outer_iterator = 0;
int32_t _inner_iterator = 0;
bool initialized = false;

std::vector<std::tuple<_DataRowFLUMOREArray, const char*>> __parserResult_dataRows;

//===========================================================================
// Read
FME_Status FLUMOREReader::read(IFMEFeature& feature, FME_Boolean& endOfFile)
{
    if (!initialized) {
        parserResult = Parser_getSimulationFileData(dataset_.c_str());
        initialized = true;
        __parserResult_dataRows.reserve(parserResult.array->len);
        for (int _i = 0; _i < parserResult.array->len; ++_i) {
            gLogFile->logMessageString((kMsgClosingReader + dataset_).c_str());
            Parser* __parserResult_array_element = g_array_index(parserResult.array, DataTableFLUMORE*, _i);
            _DataRowFLUMOREArray dataRows = DataTableFLUMORE_get_data(__parserResult_array_element);
            let date = DataTableFLUMORE_get_time(__parserResult_array_element);
            __parserResult_dataRows.push_back(std::make_tuple(dataRows, date));
            //mono_embeddinator_destroy_object(__parserResult_array_element);
        }
    }
    endOfFile = FME_FALSE;
    if (parserResult.array->len > _outer_iterator) {
        _DataRowFLUMOREArray dataRows = std::get<_DataRowFLUMOREArray>(__parserResult_dataRows[_outer_iterator]);
        //FLUMOREReader::gLogFile->logMessageString("reading");

        DataRowFLUMORE* __parserResult_dataRows_array_element = g_array_index(dataRows.array, DataRowFLUMORE*, _inner_iterator);

        let id = DataRowFLUMORE_get_id(__parserResult_dataRows_array_element);
        let h = DataRowFLUMORE_get_h(__parserResult_dataRows_array_element);
        let vres = DataRowFLUMORE_get_vres(__parserResult_dataRows_array_element);
        let wsp = DataRowFLUMORE_get_wsp(__parserResult_dataRows_array_element);
        let x = DataRowFLUMORE_get_x(__parserResult_dataRows_array_element);
        let y = DataRowFLUMORE_get_y(__parserResult_dataRows_array_element);
        let z = DataRowFLUMORE_get_z(__parserResult_dataRows_array_element);
        let date = std::get<const char*>(__parserResult_dataRows[_outer_iterator]);

        //mono_embeddinator_destroy_object(__parserResult_dataRows_array_element);

        feature.setAttribute("id", (FME_Int32)id);
        feature.setAttribute("h", h);
        feature.setAttribute("vres", vres);
        feature.setAttribute("wsp", wsp);
        feature.setAttribute("x", x);
        feature.setAttribute("y", y);
        feature.setAttribute("z", z);
        feature.setAttribute("date", date);
        feature.setFeatureType("FLUMORE");

        if (_inner_iterator + 1 == dataRows.array->len)
        {
            ++_outer_iterator;
            _inner_iterator = 0;
        }
        else
        {
            ++_inner_iterator;
        }
    }
    /*else if (parserResult.array->len == _outer_iterator) 
    {
        ++_outer_iterator;
    }*/
    else {
        endOfFile = FME_TRUE;
    }
    // Log the feature
    gLogFile->logFeature(feature);
    return FME_SUCCESS;
}

bool featureRead = false;

//===========================================================================
// readSchema
FME_Status FLUMOREReader::readSchema(IFMEFeature& feature, FME_Boolean& endOfSchema)
{
    feature.setAttribute("fme_geometry{0}", "flumore_none");
    feature.setAttribute("id");
    feature.setAttribute("h");
    feature.setAttribute("vres");
    feature.setAttribute("wsp");
    feature.setAttribute("x");
    feature.setAttribute("y");
    feature.setAttribute("z");
    feature.setAttribute("date");
    feature.setFeatureType("FLUMORE");
   endOfSchema = FME_Boolean(featureRead);
   featureRead = true;
   
   return FME_SUCCESS;
}

FME_Boolean FLUMOREReader::getProperties(const char * propertyCategory, IFMEStringArray & values)
{
    return FME_FALSE;
}

//===========================================================================
// readParameterDialog

void FLUMOREReader::readParametersDialog()
{
   FMEString paramValue;
   if (gMappingFile->fetchWithPrefix(readerKeyword_.c_str(), readerTypeName_.c_str(), kSrcMyFormatParamTag, *paramValue))
   {
      // A parameter value has been found, so set the values.
      myFormatParameter_ = paramValue->data();

      // Let's log to the user that a parameter value has been specified.
      string paramMsg = (kMyFormatParamTag + myFormatParameter_).c_str();
      gLogFile->logMessageString(paramMsg.c_str(), FME_INFORM);
   }
   else
   {
      // Log that no parameter value was entered.
      gLogFile->logMessageString(kMsgNoMyFormatParam, FME_INFORM);
   }
}


//===========================================================================
// Destructor
FLUMOREReader::~FLUMOREReader()
{
    /*for (int i = 0; i < __parserResult_dataRows.size(); i++) {
        let dataRow = __parserResult_dataRows[i];
        mono_embeddinator_array
        g_array_free(dataRow.array, true);
    }*/
    close();
}