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

#include "c2cent/library/log/CWWLog2.h"

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
        cerr << "getcwd failed" << endl;
	C2C_WW_LOG_ERR(-1, "getcwd failed");
        return -1;
    }
    string strBuff(sBuff);

    size_t i = strBuff.find_last_of("/");

    if (i == string::npos) {
        i = strBuff.size();
    }

    string strDir = strBuff.substr(0, i) + "/" + loc;

    struct dirent **namelist;
    int n;

    list < string > lstName;

    n = scandir(strDir.c_str(), &namelist, 0, alphasort);
    if (n < 0) {
        cerr << "scandir:" << strDir << " failed" << endl;
	C2C_WW_LOG_ERR(-1, "scandir failed dir:%s", strDir.c_str());
        return -1;
    } else {
        while (n--) {
            lstName.push_back(namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
    }

    for (auto it = lstName.begin(); it != lstName.end(); ++it) {
        C2C_WW_LOG("check file %s", it->c_str());
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

		C2C_WW_LOG("PyImport_ImportModule  %s", it->c_str());
                if (info.obj == NULL) {
		  
                    PyObject *ptype, *pvalue, *ptraceback;
                    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                    PyObject_Print(ptype, stderr, 0);
                    PyObject_Print(pvalue, stderr, 0);
                    PyObject_Print(ptraceback, stderr, 0);
		    
		    cerr << "PyImport_ImportModule failed " << *it << endl;
		    C2C_WW_LOG_ERR(-1, "PyImport_ImportModule failed %s", it->c_str());
                    continue;
                }

                info.mtime = st.st_mtime;
                m_mapModuleFile[*it] = info;
            } else {
                if (m_mapModuleFile[*it].mtime != st.st_mtime) {
                    CModuleInfo info;
                    info.obj = PyImport_ReloadModule(m_mapModuleFile[*it].obj);
                    if (info.obj == NULL) {
                        cerr << "PyImport_ReloadModule failed " << *it << endl;
			C2C_WW_LOG_ERR(-1, "PyImport_ReloadModule failed %s", it->c_str());
                        PyObject *ptype, *pvalue, *ptraceback;
                        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                        PyObject_Print(ptype, stderr, 0);
                        PyObject_Print(pvalue, stderr, 0);
                        PyObject_Print(ptraceback, stderr, 0);

                        continue;
                    }
		    C2C_WW_LOG("PyImport_ReloadModule %s", it->c_str());
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
            "sys.path.append(os.path.dirname(os.getcwd()))\n"
	    "sys.path.append(os.path.dirname(os.getcwd()) + '/pyservice')\n");
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
	C2C_WW_LOG_ERR(-1, "m_pMainModule is NULL sController:%s sAction:%s", sController.c_str(), sAction.c_str());
        return -1;
    }
    
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
    catch (int iLine){
        cerr << "PyObject is NULL at line:" << iLine << endl;
	C2C_WW_LOG_ERR(-1, "PyObject is NULL at line:%i sController:%s sAction:%s", iLine, sController.c_str(), sAction.c_str());

        Py_XDECREF(pFunc);
        Py_XDECREF(pController);
        Py_XDECREF(pAction);
        Py_XDECREF(pParams);
        Py_XDECREF(pData);
        Py_XDECREF(pCntl);
        
        PyErr_Print();
        
        return -1;
    }
    

    if(PyString_Check(pResult) || PyUnicode_Check(pResult)){
        char* sBuffer = NULL;
        Py_ssize_t i = 0;
	PyString_AsStringAndSize(pResult, &sBuffer, &i);
	sOutputBuffer.assign(sBuffer, i);
    }

    C2C_WW_LOG("End Dispatch sController:%s sAction:%s", sController.c_str(), sAction.c_str());

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

