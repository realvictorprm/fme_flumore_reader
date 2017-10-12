/*=============================================================================

   Name     : flumoreentrypoints.cpp

   System   : FME Plug-in SDK

   Language : C++

   Purpose  : Entry points for the DLL

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

#ifdef WIN32
#define _WIN32_WINNT 0x601 
#include <Windows.h>
#include <sstream>
#endif

#include <fmedllexport.h>
#include <fmetypes.h>
#include <fmeread.h>
#include <fmewrt.h>
#include <sstream>
#include <string>
#include <ilogfile.h>
#include <strsafe.h>
#include <filesystem>

using namespace std;

//FME_DLLEXPORT_C BOOL APIENTRY DllMain(HANDLE hModule,
//    DWORD  ul_reason_for_call,
//    LPVOID lpReserved)
//{
//
//    return TRUE;
//}

//=====================================================================
// Forward Declarations
class IFMESession;

//=====================================================================
// Global Variables
IFMESession* gFMESession;

HMODULE handle = NULL;

typedef void(*AcceptSessionSignature) (IFMESession*);
typedef const char* (*ApiVersionSignature) ();
void (*acceptSession) (IFMESession*); 
const char* (*apiVersion) ();
FME_MsgNum (*initialize) (IFMEServiceManager&);
FME_MsgNum (*createReader) (IFMELogFile& logFile,
    IFMEMappingFile& mappingFile,
    IFMECoordSysManager& coordSysMan,
    IFMEReader*& reader,
    const char* readerTypeName,
    const char* readerKeyword);
FME_MsgNum (*destroyReader)(IFMEReader*& reader);


DWORD printLastError(const char* fnctName) {
    auto err = GetLastError();
    if (err != 0) {
        LPVOID lpMsgBuf, lpDisplayBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);
        auto lpszFunction = fnctName;
        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
            (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
        StringCchPrintf((LPTSTR)lpDisplayBuf,
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("%s failed with error %d: %s.\nPlease contaxt your System administrator for support."),
            lpszFunction, err, lpMsgBuf);
        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK | MB_ICONERROR);

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
    }/*else
        MessageBoxW(NULL, L"Successfully executed operation!", fnctName, MB_OK);*/
    return err;
}

namespace fs = std::experimental::filesystem;

FME_DLLEXPORT_C BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle,
    IN DWORD     nReason,
    IN LPVOID    Reserved)
{
    BOOLEAN bSuccess = TRUE;
    DWORD err;
    auto currentPath = std::experimental::filesystem::current_path();
    auto firstIncludePath =
        fs::path("plugins/flumore_importer_plugin/lib");
    auto secondIncludePath =
        fs::path("plugins/flumore_importer_plugin");
    auto thirdIncludePath =
        fs::path("plugins");
    auto modulePath =
        fs::path("plugins/flumore_importer_plugin/flumore_core.dll");
    fs::path workingPath;
    switch (nReason)
    {
    case DLL_PROCESS_ATTACH:
        SetErrorMode(0);
        SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        err = printLastError(std::string("load library ").append((workingPath / modulePath).string()).c_str());
        wchar_t full_path[MAX_PATH];

        GetModuleFileNameW(NULL, full_path, MAX_PATH);
        workingPath = fs::path(full_path).remove_filename();
#if DEBUG
        MessageBoxW(NULL, L"Trying to load library", L"Debug output", MB_OK | MB_ICONINFORMATION);
#endif
        AddDllDirectory((workingPath / firstIncludePath).generic_wstring().data());
        AddDllDirectory((workingPath / secondIncludePath).generic_wstring().data());
        AddDllDirectory((workingPath / thirdIncludePath).generic_wstring().data());
        handle = LoadLibraryW((workingPath / modulePath).generic_wstring().data());
        err = printLastError(std::string("load library ").append((workingPath / modulePath).string()).c_str());
        if (handle != nullptr && err == 0) {
#if DEBUG
            MessageBoxW(NULL, L"Successfully loaded libraries!", L"Info", MB_OK | MB_ICONINFORMATION);
#endif            
            acceptSession = (AcceptSessionSignature)GetProcAddress(handle, TEXT("FME_acceptSession"));
            printLastError("GetProcAddress(handle, TEXT(\"FME_acceptSession\"))");
            apiVersion = (decltype(apiVersion))GetProcAddress(handle, TEXT("FME_apiVersion"));
            printLastError("GetProcAddress(handle, TEXT(\"FME_apiVersion\"))");
            createReader = (decltype(createReader))GetProcAddress(handle, TEXT("FME_createReader"));
            printLastError("GetProcAddress(handle, TEXT(\"FME_createReader\"))");
            destroyReader = (decltype(destroyReader))GetProcAddress(handle, TEXT("FME_destroyReader"));
            printLastError("GetProcAddress(handle, TEXT(\"FME_destroyReader\"))");
            initialize = (decltype(initialize))GetProcAddress(handle, TEXT("FME_initialize"));
            printLastError("GetProcAddress(handle, TEXT(\"FME_initialize\"))");
        }
        else {
            bSuccess = FALSE; 
        }
        break;
    default:
    case DLL_PROCESS_DETACH:

        break;
    };

    return bSuccess;
}

//=====================================================================
// This method is used to reliably get a session for use by this
// plug-in.
//
FME_DLLEXPORT_C void FME_acceptSession(IFMESession* fmeSession)
{
    acceptSession(fmeSession);
    printLastError("acceptSession(fmeSession)");
   // This is a pointer only and does not take ownership.
    gFMESession = fmeSession;
}

//=====================================================================
// This method returns the version of the FME API that this plug-in
// was compiled with. The value returned here is a constant defined in
// fmetypes.h and should not be changed.
//
// If this plug-in is compiled with a beta build of FME, compatibility
// issues may arise with subsequent builds of FME. To prevent crashes,
// FME will not load a plug-in compiled against a beta if there is a
// mismatch in the FME API version. To resolve this issue, re-compile
// this plug-in with the version of FME that will run this plug-in.
//
// It is recommended that this plug-in is compiled with a release
// version of FME before being distributed for use. As long as this
// plug-in is compiled with a release version of FME, there should
// be no compatibility issues when running this plug-in on any
// subsequent build of FME.
//
// A plug-in compiled with a release version of FME may generate
// a warning when used with subsequent builds of FME, but the plug-in
// will still operate correctly.
//
FME_DLLEXPORT_C const char* FME_apiVersion()
{
    return apiVersion(); //
}

//=====================================================================
// This method is called by the FME to initialize the plug-in.
//
FME_DLLEXPORT_C FME_MsgNum FME_initialize(IFMEServiceManager& serviceManager)
{
    return initialize(serviceManager);;
}


// --------------------------------------------------------------------
// Reader methods -- remove the following two methods if you are not
// implementing a reader
// --------------------------------------------------------------------

//=====================================================================
// This method is called by the FME when the reader is required. It
// instantiates an object that implements the IFMEReader interface and
// returns a pointer to that object as an output parameter. Note that
// this pointer is not "remembered" by this method; once the FME has
// finished using the object, it will pass the pointer to
// FME_destroyReader() so the reader object can be deleted.
//
FME_DLLEXPORT_C FME_MsgNum FME_createReader( IFMELogFile& logFile,
                                       IFMEMappingFile& mappingFile,
                                       IFMECoordSysManager& coordSysMan,
                                       IFMEReader*& reader,
                                       const char* readerTypeName,
                                       const char* readerKeyword )
{
    logFile.logMessageString("Trying to load external libraries");
    auto ret = createReader(logFile, mappingFile, coordSysMan, reader, readerTypeName, readerKeyword);;
    logFile.logMessageString("reader was created");

    return ret;
}

//=====================================================================
// This method is called by the FME when the reader is no longer
// required. It deletes the reader object pointed to by the input
// parameter.
//
FME_DLLEXPORT_C FME_MsgNum FME_destroyReader(IFMEReader*& reader)
{
    return destroyReader(reader);
}
 


// --------------------------------------------------------------------
// Writer methods -- remove the following two methods if you are not
// implementing a writer
// --------------------------------------------------------------------

//=====================================================================
// This method is called by the FME when the writer is required. It
// instantiates an object that implements the IFMEWriter interface and
// returns a pointer to that object as an output parameter. Note that
// this pointer is not "remembered" by this method; once the FME has
// finished using the object, it will pass the pointer to
// FME_destroyWriter() so the reader object can be deleted.
//
//FME_DLLEXPORT_C FME_MsgNum FME_createWriter( IFMELogFile& logFile,
//                                       IFMEMappingFile& mappingFile,
//                                       IFMECoordSysManager& coordSysMan,
//                                       IFMEWriter*& writer,
//                                       const char* writerTypeName,
//                                       const char* writerKeyword )
//{
//    logFile.logMessageString("creating writer");
//   //writer = new FLUMOREWriter(writerTypeName, writerKeyword);
//
//   //FLUMOREWriter::gLogFile      = &logFile;     // create pointer to log file
//   //FLUMOREWriter::gMappingFile  = &mappingFile; // create pointer to mapping file
//   //FLUMOREWriter::gCoordSysMan  = &coordSysMan; // create pointer to coordinate system manager
//   
//   return FME_SUCCESS;
//}
//
////=====================================================================
//// This method is called by the FME when the writer is no longer
//// required. It deletes the writer object pointed to by the input
//// parameter.
////
//FME_DLLEXPORT_C FME_MsgNum FME_destroyWriter(IFMEWriter*& writer)
//{
//    if(writer != nullptr)
//        delete writer;
//    writer = nullptr;
//    return FME_SUCCESS;
//}

