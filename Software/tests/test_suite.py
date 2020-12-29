##
##
##
## Created: 12/26/20 07:00:00 AM
## Author :Jay Hamlin
##

import sys
import threading
import time
import os
import decimal

##    ["add","9999999999999999","9"],
##            99999999999999E+15
##            99999999999999E15
##
testCases = [
    ["comment","Addition",""],
    ["add","3.141592653589793","3.141592653589793"],
    ["add","123456.789","-16.345678"],
    ["add","-5.0E10","-167.45E8"],
    ["add","9999999999999999","9"],
    ["add","9999999999999999","0000000000000001"],
    ["comment","Subraction",""],
    ["sub","3.141592653589793","3.141592653589793"],
    ["sub","3.141592653589793","3.141592653589794"],
    ["sub","123456.789","-16.345678"],
    ["sub","-5.0E10","-167.45E8"],
    ["sub","0000000000000001","9999999999999999"],
    ["sub","9999999999999999","0000000000000001"],
    ["sub","11111E25","22222E28"],
    ["comment","Multiplication",""],
    ["mul","3.141592653589793","3.141592653589793"],
    ["mul","3.14159265E25","3.14159265E-25"],
    ["mul","123456.789","-16.345678"],
    ["mul","-5.0E10","-167.45E8"],
    ["mul","999999999999","999999999999"],
    ["mul","111111111111","22222222"],
    ["comment","Division",""],
    ["div","3.141592653589793","3.141592653589793"],
    ["div","123456.789","-16.345678"],
    ["div","-5.0E10","-167.45E8"],
    ["div","9999999999999998","9999999999999999"],
    ["div","111111111111","22222222"]
    ]
    
testCasesMultiply = [
    ["comment","Multiplication",""],
    ["mul","3.141592653589793","3.141592653589793"],
    ["mul","3.14159265E25","3.14159265E-25"],
    ["mul","123456.789","-16.345678"],
    ["mul","-5.0E10","-167.45E8"],
    ["mul","999999999999","999999999999"],
    ["mul","111111111111","22222222"],
    ["mul","222222222222","333333333333"],
    ["mul","2222222222","3333333333"],
    ["mul","22222222","33333333"],
    ["mul","2E20","3E40"]
    ]

def CompareAdds(output):
    parmList = output.split(",")
    xdec = decimal.Decimal(parmList[1])
    ydec = decimal.Decimal(parmList[2])
    tdec = decimal.Decimal(parmList[3])
    
    expectdec = xdec + ydec

    print("adds: x=",xdec," y=",ydec," t=",tdec," expect=",expectdec)


    return;

file = open('test_results.csv', 'w')
decimal.getcontext().prec = 16
a = decimal.Decimal('1.23')
b = decimal.Decimal('3.14')
print("decimal test=",a/b)

##for x in testCasesMultiply :
for x in testCases :
    if(x[0] is "comment"):
        print(x[1])
    else:
        cmnd ='./tests ' + x[0] + ' ' + x[1] + ' ' + x[2]
        stream = os.popen(cmnd,'r', 1)
        time.sleep(0.001)
        output = stream.read()
        file.write(output)
        sys.stdout.write(output)
        if(x[0]=="add"):
            CompareAdds(output)
            print("")

if(file):
    file.close()


