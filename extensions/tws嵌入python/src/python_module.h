/*
 * python_dispatch.h
 *
 *  Created on: 2016年6月3日
 *      Author: quentinxu
 */

#ifndef WEB_SOLUTION_WEB_PYTHON_SRC_PYTHON_MODULE_H_
#define WEB_SOLUTION_WEB_PYTHON_SRC_PYTHON_MODULE_H_


#include "Python.h"


#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <map>


#include "webframework3/action_invocation.h"
#include "webframework3/web_view.h"
#include "webframework3/web_svc_cntl.h"

//class _object;

namespace web_solution{ namespace web_container_python{

using namespace web_solution::web_framework;


class CPythonModule
{
public:
    CPythonModule();
    ~CPythonModule();
    
    int Reset();
    int Initialize(const char* pszVhostRootPath, const char* pszServiceConfigFile);    
    int Dispatch(const std::string& sController, const std::string& sAction, CActionParams& rParams, CWebViewData& rData, CWebSvcCntl& rCntl, std::string& sOutputBuffer);

private:
    
    struct CModuleInfo
    {
        PyObject* obj;
        time_t mtime;
    };
    
    std::map<std::string, CModuleInfo> m_mapModuleFile;
    PyObject* m_pMainModule;
    time_t m_lastupdate;
};
}
}



#endif /* WEB_SOLUTION_WEB_PYTHON_SRC_PYTHON_MODULE_H_ */
