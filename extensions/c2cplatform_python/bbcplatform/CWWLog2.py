#coding:utf8
'''
Created on 2016年8月8日

@author: quentinxu
'''

import sys

try:
    import CWWLog
except Exception,e:
    CWWLog = None
    print e


WLOG_CLOSE = CWWLog.WLOG_CLOSE if CWWLog else 0x0
WLOG_OPEN = CWWLog.WLOG_OPEN if CWWLog else 0x1
WLOG_KEY_OPEN = CWWLog.WLOG_KEY_OPEN if CWWLog else 0x2
WLOG_ERR_OPEN = CWWLog.WLOG_ERR_OPEN if CWWLog else 0x4
WLOG_PER_OPEN = CWWLog.WLOG_PER_OPEN if CWWLog else 0x8
WLOG_RUN_OPEN = CWWLog.WLOG_RUN_OPEN if CWWLog else 0x10
WLOG_DEG_OPEN = CWWLog.WLOG_DEG_OPEN if CWWLog else 0x20
WLOG_ALL_OPEN = CWWLog.WLOG_ALL_OPEN if CWWLog else 0xFF




def QuickInitForCGI(logName, baseLogFilePath, isCanRemote, logSwitch):
    if CWWLog:
        CWWLog.QuickInitForCGI(logName, baseLogFilePath, isCanRemote, logSwitch)


def C2C_WW_LOG(format, *args):
    if CWWLog:
        CWWLog.LogRunMsg(format % args)


def C2C_WW_LOG_ERR(errno, format, *args):
    if CWWLog:
        frame = sys._getframe().f_back
        CWWLog.LogErrMsg(('FILE:%s LN:%i FUN:%s EN:%i EM:' + format) % ((frame.f_code.co_filename, frame.f_lineno, frame.f_code.co_name, errno) + args))



def C2C_WW_LOG_DEBUG(format, *args):
    if CWWLog:
        frame = sys._getframe().f_back
        CWWLog.LogDebugMsg(('FILE:%s LN:%i FUN:%s EM:' + format) % ((frame.f_code.co_filename, frame.f_lineno, frame.f_code.co_name) + args))



def C2C_WW_LOG_KEY(format, *args):
    if CWWLog:
        CWWLog.LogKeyMsg(format % args)


def C2C_WW_LOG_PERFORM(format, *args):
    if CWWLog:
        CWWLog.LogPerformMsg(format % args)


