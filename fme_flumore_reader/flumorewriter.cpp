/*=============================================================================

   Name     : FLUMOREWriter.cpp

   System   : FME Plug-in SDK

   Language : C++

   Purpose  : IFMEWriter method implementations

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
#include "flumorewriter.h"
#include "flumorepriv.h"
#include "geometryvisitor.h"

#include <ifeature.h>
#include <igeometry.h>
#include <igeometrytools.h>
#include <ilogfile.h>
#include <isession.h>
#include <fmestring.h>
#include <ImportSimulationData.h>

// These are initialized externally when a writer object is created so all
// methods in this file can assume they are ready to use.
IFMELogFile* FLUMOREWriter::gLogFile = NULL;
IFMEMappingFile* FLUMOREWriter::gMappingFile = NULL;
IFMECoordSysManager* FLUMOREWriter::gCoordSysMan = NULL;
extern IFMESession* gFMESession;

//===========================================================================
// Constructor
FLUMOREWriter::FLUMOREWriter(const char* writerTypeName, const char* writerKeyword)
:
   writerTypeName_(writerTypeName),
   writerKeyword_(writerKeyword),
   dataset_(""),
   fmeGeometryTools_(NULL),
   visitor_(NULL),
   schemaFeature_(NULL)
{
}

//===========================================================================
// Destructor
FLUMOREWriter::~FLUMOREWriter()
{
   close();
}

//===========================================================================
// Open
FME_Status FLUMOREWriter::open(const char* datasetName, const IFMEStringArray& parameters)
{
   // Perform setup steps before opening file for writing

   // Get geometry tools
   fmeGeometryTools_ = gFMESession->getGeometryTools();

   // Set the session on FMEString[Array]s.  The session set here was retrieved
   // when this plug-in was loaded.  Consequently, subsequent calls to this
   // method will use the same session, so we can make multiple calls to
   // setSession without worrying.
   FMEString::setSession(gFMESession);
   FMEStringArray::setSession(gFMESession);

   Parser r;
   // Create visitor to visit feature geometries
   visitor_ = new GeometryVisitor(fmeGeometryTools_, gFMESession);

   dataset_ = datasetName;

   schemaFeature_ = gFMESession->createFeature();

   // -----------------------------------------------------------------------
   // Add additional setup here
   // -----------------------------------------------------------------------

   // Log an opening writer message
   string msgOpeningWriter = kMsgOpeningWriter + dataset_;
   gLogFile->logMessageString(msgOpeningWriter.c_str());

   // Fetch all the schema features and add the DEF lines.
   FLUMOREWriter::addDefLineToSchema(parameters);
   

   // Write the schema information to the file. In this template,
   // since we are not writing to a file we will log the schema information
   // instead.
   //gLogFile->logFeature(*schemaFeature_, FME_INFORM, 20);

   // -----------------------------------------------------------------------
   // Open the dataset here
   // e.g. outputFile_.open(dataset_.c_str(), ios::out|ios::trunc);
   // -----------------------------------------------------------------------

   return FME_SUCCESS;
}

//===========================================================================
// Abort
FME_Status FLUMOREWriter::abort()
{
   // -----------------------------------------------------------------------
   // Add any special actions to shut down a writer not finished writing
   // data. For example, if your format requires a footer at the end of a
   // file, write it here.
   // -----------------------------------------------------------------------

   close();
   return FME_SUCCESS;
}

//===========================================================================
// Close
FME_Status FLUMOREWriter::close()
{
   // -----------------------------------------------------------------------
   // Perform any closing operations / cleanup here; e.g. close opened files
   // -----------------------------------------------------------------------

   // Delete the visitor
   if (visitor_)
   {
      delete visitor_;
   }
   visitor_ = NULL;

   if (schemaFeature_)
   {
      gFMESession->destroyFeature(schemaFeature_);
      schemaFeature_ = NULL;
   }

   // Log that the writer is done
   gLogFile->logMessageString((kMsgClosingWriter + dataset_).c_str());

   return FME_SUCCESS;
}

//===========================================================================
// Write
FME_Status FLUMOREWriter::write(const IFMEFeature& feature)
{
   // Log the feature
   // gLogFile->logFeature(feature);

   // -----------------------------------------------------------------------
   // The feature type and the attributes can be extracted from the feature
   // at this point.
   // -----------------------------------------------------------------------

   // Extract the geometry from the feature
   const IFMEGeometry* geometry = (const_cast<IFMEFeature&>(feature)).getGeometry();
   FME_Status badNews = geometry->acceptGeometryVisitorConst(*visitor_);
   if (badNews)
   {
      // There was an error in writing the geometry
      gLogFile->logMessageString(kMsgWriteError);
      return FME_FAILURE;
   }

   // -----------------------------------------------------------------------
   // Perform your write operations here
   // -----------------------------------------------------------------------

   return FME_SUCCESS;
}

//===========================================================================
// Add DEF Line to the Schema Feature
void FLUMOREWriter::addDefLineToSchema(const IFMEStringArray& parameters)
{
   // Get the feature type.
   const IFMEString* paramValue;

   paramValue = parameters.elementAt(0);

   // Set it on the schema feature.
   schemaFeature_->setFeatureType(paramValue->data());

   string attrName;
   string attrType;
   for (FME_UInt32 i = 1; i < parameters.entries(); i += 2)
   {
      // Grab the attribute name and type
      paramValue = parameters.elementAt(i);
      attrName = paramValue->data();

      paramValue = parameters.elementAt(i + 1);
      attrType = paramValue->data();
      // Add the attribute name and type pair to the schema feature.
      schemaFeature_->setSequencedAttribute(attrName.c_str(), attrType.c_str());
   }
}
