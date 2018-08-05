import os
import subprocess
import shutil
import subprocess
import sys

def error_p(message):
    print 'Error. ' + message

def check_environment():
    try:
        global wp 
        wp = os.environ['WORKSPACE']
    except:
        error_p('Please set environment variable WORKSPACE as edk2 root')        
        return False
    
    # Check the environment
    if 'JAVA_HOME' not in os.environ:
        error_p('Please install java and set \'JAVA_HOME\' as environment variable')
        return False

    if 'M2_HOME' not in os.environ:
        error_p('Please install maven fisrstly')
        return False

    global origin
    origin = os.path.split(os.path.abspath(__file__))[0]

    if not os.path.exists(origin + '/Report' + '/resources' + '/js' + '/Chart.bundle.min.js'):
        error_p('Please download Chart.bundle.min.js to MpyTestFrameworkPkg/Report/resources/js folder')
        return False

    if not os.path.exists(origin + '/Report' + '/resources' + '/js' + '/jquery-3.3.1.js'):
        error_p('Please download jquery-3.3.1.jsChart.bundle.min.js to MpyTestFrameworkPkg/Report/resources/js folder')
        return False
    
    return True

def  generate_target():
    # Create target folder
    if os.path.isdir(os.path.join(wp , 'Build')) == False:
        os.mkdir(os.path.join(wp + 'Build'))

    target = os.path.join(wp, 'Build','MpyTest')
    mpython_lib = os.path.join(os.path.dirname(origin),'MicroPythonPkg/MicroPythonDxe/Lib/Uefi')
    mpython_bin = os.path.join(wp,'Build','MicroPythonPkg')

    if os.path.isdir(target) == True:
        shutil.rmtree(target)

    if not os.path.exists(mpython_bin) or os.listdir(mpython_bin) == []:
        error_p('Please build MicroPythonPkg firstly')
        return

    if len(sys.argv) != 3:
        error_p('''Please set Build target in one of [DEBUG RELEASE NOOPT] and set Build tool chain in one of [VS2013 VS2015 VS2017]
        It should be consistent as what you used to build MicroPythonPkg
        Sample can be "setup.py VS2015 DEBUG"''')
        return
    
    if sys.argv[1] not in ['VS2013', 'VS2015', 'VS2017'] or sys.argv[2] not in ['DEBUG', 'RELEASE', 'NOOPT']:
        error_p('''Please set Build target in one of [DEBUG RELEASE NOOPT] and set Build tool chain in one of [VS2013 VS2015 VS2017]
        It should be consistent as what you used to build MicroPythonPkg
        Sample can be "setup.py VS2015 DEBUG"''')
        return
    
    match = False
    binary_target = ""

    for item in os.listdir(mpython_bin):
        if sys.argv[1] in item and sys.argv[2] in item:
            match = True
            binary_target = item
                
    if match == False:
        error_p('Micropython binary not found. Please build MicroPythonPkg firstly.')
        return

    os.mkdir(target)
    os.mkdir(target + '/Bin')

    arch = ['IA32', 'X64']

    for arch_item in arch:
        darch = os.path.join(mpython_bin, binary_target, arch_item)
        if os.path.isdir(darch):
            os.mkdir(target + '/Bin/' + arch_item)
            for item in os.listdir(darch):
                if '.efi' in item:
                    shutil.copy2(darch + '/' + item, target + '/Bin/' + arch_item)

    os.mkdir(os.path.join(target,'Log'))

    shutil.copy2(origin + "/startup.nsh", target)
    shutil.copy2(origin + "/README.md", target)
    shutil.copy2(origin + "/setup.py", target)
    shutil.copytree(origin + '/Config', target + '/Config')
    shutil.copytree(origin + '/Doc', target + '/Doc')
    shutil.copytree(origin + '/Lib', target + '/Lib')
    for item in os.listdir(mpython_lib):
        s = os.path.join(mpython_lib, item)
        shutil.copy2(s, target + '/Lib')
    shutil.copytree(origin + '/Report', target + '/Report')
    shutil.copytree(origin + '/Scripts', target + '/Scripts')
    shutil.copytree(origin + '/Tools', target + '/Tools')
    os.chdir(target + '/Tools/ReportGenerator')
    p = subprocess.Popen(r'mvn.cmd package')
    p.wait()
    shutil.copy2(target + '/Tools/ReportGenerator/target/ReportGenerator-1.0-SNAPSHOT-jar-with-dependencies.jar', target + '/Tools')

# Main procedure
if check_environment() == True:
    generate_target()