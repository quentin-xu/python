from a import *

@WebServer
class Base(object):
    @URL('/hello/world')
    def get(self, sController, sAction, rParams, rData, rCntl):
        return sController + sAction
