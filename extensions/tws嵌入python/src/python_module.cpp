/*
 * python_dispatch.cpp
 *
 *  Created on: 2016年6月3日
 *      Author: quentinxu
 */

#include "Python.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <dlfcn.h>

#include <string>
#include <list>
#include <iostream>

#include "python_module.h"
#include "pyhttp_object.h"

namespace web_solution{ namespace web_container_python{

using namespace web_solution::web_framework;
using namespace std;

const string loc = "pyservice";

int CPythonModule::Initialize(const char* pszVhostRootPath,
        const char* pszServiceConfigFile) {
    return 0;
}

int CPythonModule::Reset() {
    using namespace std;

    if (time(NULL) - m_lastupdate < 5) {
        return 0;
    }

    m_lastupdate = time(NULL);

    char sBuff[256] = { 0 };
    if (getcwd(sBuff, sizeof(sBuff)) == NULL) {
        cout << "getcwd failed" << endl;
        return -1;
    }
    string strBuff(sBuff);

    size_t i = strBuff.find_last_of("/");

    if (i == string::npos) {
        i = strBuff.size();
    }

    string strDir = strBuff.substr(0, i) + "/" + loc;

    /*    DIR* dir = opendir(strDir.c_str());
     
     if(dir == NULL)
     {
     return -1;
     }*/

    struct dirent **namelist;
    int n;

    list < string > lstName;

    n = scandir(strDir.c_str(), &namelist, 0, alphasort);
    if (n < 0) {
        cout << "scandir:" << strDir << " failed" << endl;
        return -1;
    } else {
        while (n--) {
            lstName.push_back(namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
    }

    for (auto it = lstName.begin(); it != lstName.end(); ++it) {
        cout << "check " << *it << endl;
        struct stat st;
        if (stat((strDir + "/" + *it).c_str(), &st) == -1
                || ((st.st_mode & S_IFMT) != S_IFREG) || it->length() < 3
                || it->substr(it->length() - 3, 3) != ".py") {
            continue;
        } else {
            if (m_mapModuleFile.find(*it) == m_mapModuleFile.end()) {
                CModuleInfo info;
                info.obj = PyImport_ImportModule(
                        (loc + "." + it->substr(0, it->length() - 3)).c_str());

                if (info.obj == NULL) {

                    PyObject *ptype, *pvalue, *ptraceback;
                    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                    PyObject_Print(ptype, stderr, 0);
                    PyObject_Print(pvalue, stderr, 0);
                    PyObject_Print(ptraceback, stderr, 0);

                    continue;
                }

                info.mtime = st.st_mtime;
                m_mapModuleFile[*it] = info;
            } else {
                if (m_mapModuleFile[*it].mtime != st.st_mtime) {
                    CModuleInfo info;
                    info.obj = PyImport_ReloadModule(m_mapModuleFile[*it].obj);
                    if (info.obj == NULL) {
                        cout << "PyImport_ReloadModule failed " << *it << endl;
                        PyObject *ptype, *pvalue, *ptraceback;
                        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                        PyObject_Print(ptype, stderr, 0);
                        PyObject_Print(pvalue, stderr, 0);
                        PyObject_Print(ptraceback, stderr, 0);

                        continue;
                    }
                    cout << "PyImport_ReloadModule" << *it << endl;
                    info.mtime = st.st_mtime;
                    m_mapModuleFile[*it] = info;
                }
            }
        }
    }

    return 0;
}

CPythonModule::CPythonModule() :
        m_lastupdate(0) {
    //add symbols
    cout << "CPythonModule::dlopen: "
            << dlopen("libpython2.6.so", RTLD_NOW | RTLD_GLOBAL) << endl;
    cout << (char*) dlerror() << endl;
    Py_Initialize();
    PyRun_SimpleString("import os\n"
            "import sys\n"
            "sys.path.append(os.path.dirname(os.getcwd()))\n");
    /* Error checking of pName left out */
    m_pMainModule = PyImport_ImportModule("bbcplatform.python_adaptor");
}

CPythonModule::~CPythonModule() {
    Py_Finalize();
}

int CPythonModule::Dispatch(const string& sController, const string& sAction,
        CActionParams& rParams, CWebViewData& rData, CWebSvcCntl& rCntl,
        string& sOutputBuffer) {

    using namespace pyhttp;

    if (m_pMainModule == NULL) {
        cerr << "m_pMainModule is NULL" << endl;
        return -1;
    }
    cout << "m_pMainModule != NULL" << endl;
    
    
    PyObject* pFunc = NULL;
    PyObject* pController = NULL;
    PyObject* pAction = NULL;
    PyObject* pParams = NULL;
    PyObject* pData = NULL;
    PyObject* pCntl = NULL;
    PyObject* pResult = NULL;
    
    try{
        
        pFunc = PyObject_GetAttrString(m_pMainModule, "invoke");
        if (pFunc == NULL)
            throw __LINE__;

        pController = PyString_FromString(sController.c_str());
        if (pController == NULL)
            throw __LINE__;

        pAction = PyString_FromString(sAction.c_str());
        if (pAction == NULL)
            throw __LINE__;

        pParams = (PyObject*) PyObject_NEW(CPyObjContainer<CActionParams>,
                GetActionParamsType());
        if (pParams == NULL)
            throw __LINE__;

        pData = (PyObject*) PyObject_NEW(CPyObjContainer<CWebViewData>,
                GetWebViewDataType());
        if (pData == NULL)
            throw __LINE__;

        pCntl = (PyObject*) PyObject_NEW(CPyObjContainer<CWebSvcCntl>,
                GetWebSvcCntlType());
        if (pCntl == NULL)
            throw __LINE__;
            
        ((CPyObjContainer<CActionParams>*) pParams)->ptr = &rParams;
        ((CPyObjContainer<CWebViewData>*) pData)->ptr = &rData;
        ((CPyObjContainer<CWebSvcCntl>*) pCntl)->ptr = &rCntl;

        pResult = PyObject_CallFunctionObjArgs(pFunc, pController,
                pAction, pParams, pData, pCntl, NULL);
        if (pResult == NULL)
            throw __LINE__;
    }
    catch (char* sLine){
        cerr << sLine << ": PyObject is NULL" << endl;
        
        Py_XDECREF(pFunc);
        Py_XDECREF(pController);
        Py_XDECREF(pAction);
        Py_XDECREF(pParams);
        Py_XDECREF(pData);
        Py_XDECREF(pCntl);
        
        PyErr_Print();
        
        return -1;
    }
    
    char* sBuffer = NULL;
    Py_ssize_t i = 0;
    PyString_AsStringAndSize(pResult, &sBuffer, &i);
    sOutputBuffer.assign(sBuffer, i);

    cout << "End Dispatch" << endl;

    Py_XDECREF(pFunc);
    Py_XDECREF(pController);
    Py_XDECREF(pAction);
    Py_XDECREF(pParams);
    Py_XDECREF(pData);
    Py_XDECREF(pCntl);
    
    return 0;
}


}
}

