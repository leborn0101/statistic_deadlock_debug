import sys
import os
import fnmatch

global_arr= []
dirName = ''
def put_key(key, value, index):
    #print key
    if len(global_arr) <= index:
        global_arr.append({})
    if key in global_arr[index]:
        global_arr[index][key].append(value)
    else:
        global_arr[index][key] = [value]

def do_some_operation(fileName, index):
    result = False
    for line in open(fileName):
        line = line.strip()
        arr = line.split(":")
        put_key(arr[1], arr[0], index)
        result = True
    return result

def pre_deal_per_file():
    for per_file in global_arr:
        for key in per_file:
            tmp = 0
            for value in per_file[key]:
                tmp += int(value)
            tmp /= len(per_file[key])
            per_file[key] = {"value": tmp, "flag" : True}
#    print global_arr

def average():
    if(len(global_arr) > 0):
        for key in global_arr[0]:
            print (key)
            avgOfValue = 0
            for i in range(len(global_arr)):
                avgOfValue += global_arr[i][key]["value"]
            avgOfValue /= len(global_arr)
            avgconfValue = 0
            for i in range(len(global_arr)):
               avgconfValue += (global_arr[i][key]["value"] - avgOfValue)**2
            avgconfValue /= len(global_arr)
            for i in range(len(global_arr)):
                if (global_arr[i][key]["value"] - avgOfValue)**2 > avgconfValue:
                    global_arr[i][key]["flag"] = False
    #    print global_arr

def count_flag(index):
    cnt = 0
    for key in global_arr[index].keys():
        if global_arr[index][key]["flag"] == True:
            cnt += 1
    return cnt

def write_file(fileName, result):
    with open('/'.join([dirName, fileName]), 'a') as fileObj:
        fileObj.write(result + "\n")

def write_cov(fileName):
    for i in range(len(global_arr)):
        with open('/'.join([dirName, fileName]), 'a') as fileObj:
            fileObj.write(str(i+1) + " ")

def write_result():
    for i in range(len(global_arr)):
        if count_flag(i) > len(global_arr[i]) * 0.888:
            result = "%d pass" %(i+1)
        else:
            result = "%d fail" %(i+1)
        write_file("result",result)
def write_gcov():
    for i in range(len(global_arr)):
        fileName = str(i+1) + ".gcov"
        os.remove('/'.join([dirName, fileName]))
        print ("write before")
        with open('/'.join([dirName, fileName]), 'a') as fileObj:
            for key in global_arr[i].keys():
                result = "    %5d:%5d:\n" %(global_arr[i][key]["value"], int(key))
                fileObj.write(result);
def main():
    global dirName;
    dirName = sys.argv[1]
    dirName = os.path.abspath(dirName)
    index = 0
    for fileName in os.listdir(dirName):
        if fnmatch.fnmatch(fileName, "*.svr"):
            fileNameNew = str(index + 1) + ".gcov"
            os.rename('/'.join([dirName, fileName]), '/'.join([dirName,fileNameNew]))
            result = do_some_operation('/'.join([dirName, fileNameNew]), index)
            if result:
                index += 1
        else:
            print ("error: no svr file")
    pre_deal_per_file()
    write_gcov()
    average()
#    print global_arr
    if os.path.exists('/'.join([dirName, "result"])):
        os.remove('/'.join([dirName, "result"]))
    write_result()
    if os.path.exists('/'.join([dirName, "cov"])):
        os.remove('/'.join([dirName, "cov"]))
    write_cov("cov")

# if __name__ == "__main__":
main()
