#include "logservice/logger.h"
#include "webframework3/action_invocation.h"
#include "webframework3/http_request.h"
#include "webframework3/http_response.h"
#include "webframework3/http_response_process.h"
#include "webframework3/web_view.h"
#include "webframework3/web_svc_cntl.h"
#include <sstream>
#include <string.h>
#include <sys/time.h>
#include "libc2c/c2c_errno_define.h"
//#include "c2cplatform/library/util/cgi_itil.h"  
#include "app_platform/cgi_stub_cntl.h"

#include "python_module.h"


#ifndef _RUN_AS_APACHE_CGI_
#ifdef __cplusplus
extern "C"
{
#endif
	
    int WebServiceEntryProcess2(const char* pszVhostRootPath,
        const char* pszServiceConfigFile,
        web_solution::comm::CHttpRequest2* pHttpRequest, 
        web_solution::comm::CHttpResponse* pHttpResponse);
	
#ifdef __cplusplus
}
#endif

#include "webframework3/web_service_stat.h"
web_solution::web_framework::CWebServiceStat g_oStat;

#endif

// log
#include "c2cplatform/c2cent/library/log/CWTimeElf2.h"

using namespace c2cent::library::wwlog;
CWWLog g_AdaptorLog;

using namespace web_solution::web_framework;
using namespace web_solution::web_container_python;
//using namespace c2cent::web_service;


int InterceptorInitialize(const char* pszVhostRootPath, const char* pszServiceConfigFile) 
{       
	//注册拦截器
	/* 应基础业务需求暂时屏蔽之
	CActionInvocation* pActionInvocation = CActionInvocation::GetInstance();
	CPythonSvcInterceptor* pInterceptor = new CPythonSvcInterceptor;
	pActionInvocation->SetInterceptor(pInterceptor, NULL);
	*/

	return 0;
}   

int MyWebSvcDispatch(
		CActionParams& rActionParams,
		CWebViewData& rData,
		CWebSvcCntl& rCntl,
		CPythonModule& rSvc,
    web_solution::comm::CHttpRequest2& rHttpRequest, 
    web_solution::comm::CHttpResponse& rHttpResponse)
{
	//TODO: 监控

    CActionInvocation* pActionInvocation = CActionInvocation::GetInstance();
	std::string sController;
	std::string sAction;
	
	int iRetCode = pActionInvocation->ParseActionName(
		rHttpRequest.GetMethodType(),
		rHttpRequest.GetFileName(), 
		rHttpRequest.GetQueryString(), 
		rHttpRequest.GetPostInfo(), 
		rHttpRequest.GetContentType(),
		rHttpRequest.GetCookie(),
		sController, sAction, rActionParams);
	if(iRetCode != 0)
	{
		web_solution::web_container::CHttpResponseProcess::Instance()->DoNotFoundResponse(
				rHttpRequest, rHttpResponse);
		g_oStat.UpdateStatValue(CWebServiceStat::ERR_PKG_SENT, 1);
		g_AdaptorLog.LogErrMsg(
				"No match! ParseActionName failed(RetCode:%d) filename[%s]. err_info[%s]", 
				iRetCode, rHttpRequest.GetFileName().c_str(), pActionInvocation->GetLastErrMsg().c_str());

		//CGI_ITIL->AddErr("default");
		return -1;
	}

	uint32_t dwWaitTime = static_cast<uint32_t>(time(NULL)) - rHttpRequest.GetRequestTime(); // 单位:秒
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "%u", dwWaitTime);
	rActionParams.AddVal("TWSWaitTime", buf);
	struct timeval tvTime;
	::gettimeofday(&tvTime, NULL);
	//uint32_t dwTimestampBegin = (tvTime.tv_sec - BASE_SECONDS) * 1000 + tvTime.tv_usec / 1000;

	// Set TWS basic params
	rActionParams.AddVal("TWSClientIP", rHttpRequest.GetClientIp());
	rActionParams.AddVal("TWSFileName", rHttpRequest.GetFileName());
	rActionParams.AddVal("TWSQueryString", rHttpRequest.GetQueryString());
	rActionParams.AddVal("TWSPostInfo", rHttpRequest.GetPostInfo());
	rActionParams.AddVal("TWSContentType", rHttpRequest.GetContentType());
	rActionParams.AddVal("TWSCookie", rHttpRequest.GetCookie());
	rActionParams.AddVal("TWSUserAgent", rHttpRequest.GetUserAgent());
	rActionParams.AddVal("TWSReferer", rHttpRequest.GetReferer());
	//rActionParams.AddVal("TWSOrigin", rHttpRequest.GetOrigin());
	rActionParams.AddVal("TWSQueryString", rHttpRequest.GetQueryString());
	std::stringstream oss;
	oss << rHttpRequest.GetClientPort();
	rActionParams.AddVal("TWSClientPort", oss.str());
	if(0x03 == rHttpRequest.GetMethodType())
		rActionParams.AddVal("TWSHttpMethod", "POST");
	else if(0x04 == rHttpRequest.GetMethodType())
		rActionParams.AddVal("TWSHttpMethod", "OPTIONS");
	else
		rActionParams.AddVal("TWSHttpMethod", "GET");

	rCntl.SetResult(200);

    web_solution::web_framework::CActionParams rParams;
    CCGI_STUB_CNTL->setCallerName("PythonSvc:Invoke");
    rSvc.Reset();
    std::string sOutputBuffer;
    iRetCode = rSvc.Dispatch(sController, sAction, rActionParams, rData, rCntl, sOutputBuffer);
    if(iRetCode != 0)
    {	
      //CGI_ITIL->AddErr("invoke");
        g_oStat.UpdateStatValue(CWebServiceStat::ERR_PKG_SENT, 1);
        web_solution::web_container::CHttpResponseProcess::Instance()->DoBadRequestResponse(
                rHttpRequest, rHttpResponse);
        g_AdaptorLog.LogErrMsg("RenderInvoke fail.");
        rCntl.SetResult(400);
        return -2;
    }

    /// 显示视图数据
    rHttpResponse.SetResult(rCntl.GetResult());
    rHttpResponse.SetUpdateTime(rCntl.GetUpdateTime());
    rHttpResponse.SetWebCacheExpireTime(rCntl.GetWebCacheExpireTime());
    rHttpResponse.SetBrowserCacheExpireTime(rCntl.GetBrowserCacheExpireTime());
    rHttpResponse.SetEtags(rCntl.GetEtags());
    rHttpResponse.SetMimeType(rCntl.GetMimeType());
    rHttpResponse.SetMimeEncoding(rCntl.GetMimeEncoding());
    rHttpResponse.SetUseWebCache(rCntl.GetUseWebCache());
    rHttpResponse.SetContentEncoding(rCntl.GetContentEncoding());

	// Clean CRLF
	std::string sLocation = rCntl.GetLocation();
	std::string::size_type pos = sLocation.find("\015\012");
	if(pos != std::string::npos)
		sLocation.erase(pos, sLocation.length());

	rHttpResponse.SetLocation(sLocation);
	std::string sSetCookie;
	rData.OutputCookie(sSetCookie);
	rHttpResponse.SetCookie(sSetCookie);
    rHttpResponse.SetResponseContent(sOutputBuffer);

	return 0;
}

int WebServiceEntryProcess2(const char* pszVhostRootPath,
        const char* pszServiceConfigFile,
        web_solution::comm::CHttpRequest2* pHttpRequest, 
        web_solution::comm::CHttpResponse* pHttpResponse)
{
	/// 进行初始化
	static bool bMyWebSvcInit = false;
	static CPythonModule* pMyWebSvc = new CPythonModule();

	if(strcasecmp("PerformTimer", pHttpRequest->GetHost().c_str()) == 0)
	{
		g_oStat.Initialize("PythonSvc", (pHttpRequest->GetSeqNo() > 0));
		g_oStat.ShowStat();
			return 0;
	}

	pHttpResponse->SetMimeType("text/html; charset=utf8");

    CActionInvocation* pActionInvocation = CActionInvocation::GetInstance();

	if(!bMyWebSvcInit)
	{
		std::string sLoggerName = "default_svc";
		pActionInvocation->SetLoggerName(sLoggerName);
		
		g_AdaptorLog.QuickInitForCGI("PythonSvc_adaptor");

		if(InterceptorInitialize(pszVhostRootPath, pszServiceConfigFile) != 0)
		{
			g_AdaptorLog.LogErrMsg("InterceptorInitialize() fail.");
			web_solution::web_container::CHttpResponseProcess::Instance()->DoSvrInternalErrResponse(
					*pHttpRequest, *pHttpResponse);
			return -1;
		}

		if(pMyWebSvc->Initialize(pszVhostRootPath, pszServiceConfigFile) != 0)
		{
			g_AdaptorLog.LogErrMsg("Attention: pMyWebSvc->Initialize() fail.");
			web_solution::web_container::CHttpResponseProcess::Instance()->DoSvrInternalErrResponse(
					*pHttpRequest, *pHttpResponse);
			return -1;
		}
		bMyWebSvcInit = true;
	}

	/// 依次调用拦截器before
	int iRetCode = 0;
	void* pParam = reinterpret_cast<void*>(pHttpRequest);
	if((iRetCode = pActionInvocation->InvokeInterceptorBefore(pParam)) != 0)
		g_AdaptorLog.LogErrMsg("Attention: pActionInvocation->InvokeBeforeInterceptor fail. RetCode = %d", iRetCode);

	/// 命令分发
	CActionParams oActionParams;
	CWebViewData oData;
	CWebSvcCntl oCntl;
    oCntl.SetUseWebCache(pHttpResponse->GetUseWebCache());
    oCntl.SetUpdateTime(pHttpResponse->GetUpdateTime());
    oCntl.SetWebCacheExpireTime(pHttpResponse->GetWebCacheExpireTime());
    oCntl.SetBrowserCacheExpireTime(pHttpResponse->GetBrowserCacheExpireTime());
    oCntl.SetEtags(pHttpResponse->GetEtags());
    oCntl.SetMimeType(pHttpResponse->GetMimeType());
    oCntl.SetMimeEncoding(pHttpResponse->GetMimeEncoding());
    oCntl.SetContentEncoding(pHttpResponse->GetContentEncoding());
    MyWebSvcDispatch(oActionParams, oData, oCntl, *pMyWebSvc, *pHttpRequest, *pHttpResponse);

	/// 依次调用拦截器after
	if((iRetCode = pActionInvocation->InvokeInterceptorAfter(NULL)) != 0)
		g_AdaptorLog.LogErrMsg("Attention: pActionInvocation->InvokeAfterInterceptor fail. RetCode = %d", iRetCode);

    return 0;
}









