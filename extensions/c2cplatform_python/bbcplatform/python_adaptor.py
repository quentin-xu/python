import types


class WebServer(object):
    
    URLConfig = {}
    URLUnfinish = {}

    def __init__(self, cls):
        self.cls = cls
        for name,func in cls.__dict__.items():
            if func in WebServer.URLUnfinish:
                method = types.MethodType(func, None, cls)
                setattr(cls, name, method)
                WebServer.URLConfig[WebServer.URLUnfinish[func]] = (cls, method)

        WebServer.URLUnfinish = {}

    def __call__(self):
        return object.__new__(self.cls)



def URL(urlPattern):
    urlPattern = urlPattern.strip().strip('/').lower()
    def decorator(func):
        #if urlPattern not in WebServer.URLConfig and urlPattern not in WebServer.URLUnfinish:
        WebServer.URLUnfinish[func] = urlPattern

        return func

    return decorator


def invoke(sController, sAction, rParams, rData, rCntl):
    try:
        print url
        cls, method = WebServer.URLConfig[url.strip().strip('/').lower()]
        return method(cls(), sController, sAction, rParams, rData, rCntl)
    except Exception,e:
        print 'ERRRRRRRRRRRRROR:',e


