#ifndef FLUMORE_READER_H
#define FLUMORE_READER_H
/*=============================================================================

   Name     : flumorereader.h

   System   : FME Plug-in SDK

   Language : C++

   Purpose  : Declaration of FLUMOREReader

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

#include <fmeread.h>
#include <sstream>
#include <string>
#include <ImportSimulationData.h>
#include "Utils.hpp"

using namespace std;

// Forward declarations
class IFMEFeature;
class IFMELogFile;
class IFMEGeometryTools;

// The reader ID assigned by Safe Software for this module
const FME_UInt32 kReaderId = 87062;

class FLUMOREReader : public IFMEReader
{

public:

   // Please refer to the IFMEWriter documentation for more information in
   // FME_DEV_HOME/pluginbuilder/cpp/apidoc/classIFMEReader.html
   // -----------------------------------------------------------------------
   // Constructor
   // The readerTypeName is the name used in the following contexts:
   // - the name of the format's .db file in the formatsinfo folder
   // - the format short name for the format within the .db file
   // - FORMAT_NAME in the metafile
   // 
   // The readerKeyword is a unique identifier for this reader instance.
   // It is usually set by the READER_KEYWORD in the mapping file.
   // 
   // Since there can be multiple instances of a readerTypeName within
   // an FME session, the readerKeyword must be used when fetching 
   // context information from FME.
   FLUMOREReader(const char* readerTypeName, const char* readerKeyword);

   // -----------------------------------------------------------------------
   // Destructor
   virtual ~FLUMOREReader();

   // -----------------------------------------------------------------------
   // open()
   virtual FME_Status open(const char* datasetName, const IFMEStringArray& parameters);

   // -----------------------------------------------------------------------
   // abort()
   virtual FME_Status abort();

   // -----------------------------------------------------------------------
   // close()
   virtual FME_Status close();

   // -----------------------------------------------------------------------
   // id()
   virtual FME_UInt32 id() const { return kReaderId; }

   // -----------------------------------------------------------------------
   // read()
   virtual FME_Status read(IFMEFeature& feature, FME_Boolean& endOfFile);

   // -----------------------------------------------------------------------
   // readSchema()
   virtual FME_Status readSchema(IFMEFeature& feature, FME_Boolean& endOfSchema);

   virtual FME_Boolean getProperties(const char* propertyCategory, IFMEStringArray & values);

   // -----------------------------------------------------------------------
   // Insert additional public methods here
   // -----------------------------------------------------------------------


   // Data members

   // A pointer to an IFMELogFile object that allows the plug-in to log messages
   // to the FME log file. Initialized externally after the plug-in object is created.
   static IFMELogFile* gLogFile;

   // A pointer to an IFMEMappingFile object that allows the plug-in to access
   // information from the mapping file. It is initialized externally after the
   // plug-in object is created.
   static IFMEMappingFile* gMappingFile;

   // A pointer to an IFMECoordSysManager object that allows the plug-in to retrieve
   // and define coordinate systems. It is initialized externally after the plug-in
   // object is created.
   static IFMECoordSysManager* gCoordSysMan;

   // -----------------------------------------------------------------------
   // Insert additional public data members here
   // -----------------------------------------------------------------------


private:

   // -----------------------------------------------------------------------
   // Copy constructor
   FLUMOREReader(const FLUMOREReader&);

   // -----------------------------------------------------------------------
   // Assignment operator
   FLUMOREReader &operator=(const FLUMOREReader&);

   // -----------------------------------------------------------------------
   // readParametersDialog
   //
   // This method doesn't do anything too interesting.  Its main purpose
   // is to illustrate how to read in the values entered in a parameters dialog.
   // In the Multi - Channel MyFormat reader case, the values read from here
   // are logged. The values logged in the dataset are not used for anything
   // other than demonstration purposes.
   // A parameters dialog acts as the bridge between a mapping file and the
   // reader / writer.It should contain any configurable parameters
   // defined for a reader / writer.
   //
   // Please refer to the IFMEMappingFile documentation for more information in
   // FME_DEV_HOME/pluginbuilder/cpp/apidoc/classIFMEMappingFile.html
   void readParametersDialog();

   // -----------------------------------------------------------------------
   // Insert additional private methods here
   // -----------------------------------------------------------------------


   // Data members

   // The value specified for the READER_TYPE in the mapping file.
   // It is also specified for the FORMAT_NAME in the metafile.
   // Initialized in the constructor to the value passed in by the FME.
   string readerTypeName_;

   // Typically the same as the readerTypeName_, but it could be different
   // if a value was specified for READER_KEYWORD in the FME mapping file.
   // Initialized in the constructor to the value passed in by the FME.
   string readerKeyword_;

   // Stores the full path of the dataset that the plug-in is reading from.
   // Initialized to an empty string in the constructor and set to the value
   // passed in by the FME in the open() method.
   string dataset_;

   // Stores the coordinate system of all the features in the file being read.
   // Initialized to an empty string. Not used in this implementation.
   string coordSys_;

   // A pointer to an IFMEGeometryTools object which is used to create and
   // manipulate geometries.
   IFMEGeometryTools* fmeGeometryTools_;

   // The parameters value used for reading the dataset.
   string myFormatParameter_;

   _DataTableFLUMOREArray parserResult;

   // -----------------------------------------------------------------------
   // Insert additional private data members here
   // -----------------------------------------------------------------------

};

#endif
