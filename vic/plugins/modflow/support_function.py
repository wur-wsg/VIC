import calendar
import shutil
from datetime import datetime
from dateutil.relativedelta import relativedelta

def startdate(current_date):
    startyear, startmonth, startday = current_date.year, current_date.month, current_date.day
    return startyear, startmonth, startday

def enddate(current_date):
    _, lastday = calendar.monthrange(current_date.year, current_date.month)
    endyear, endmonth, endday = current_date.year, current_date.month, lastday
    return endyear, endmonth, endday
def statefiledate(current_date):
    stateyear, statemonth, stateday = (current_date + relativedelta(months=1)).year, (current_date + relativedelta(months=1)).month, 1
    return stateyear, statemonth, stateday

def init_datestr(current_date):
    init_date = current_date
    init_datestr = current_date.strftime('%Y%m%d')  
    return init_date,init_datestr
