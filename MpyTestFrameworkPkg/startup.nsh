echo -off
vol -n MPTF

if %1 == "" then
  echo  "No argument specified.  Input 'startup.nsh -h' for usage info"
else
  if %1 == "-h" then
    echo "startup.nsh is the entry to access Micropython Test Framework for UEFI"
    echo "  startup.nsh [-h] [-a [ia32 | x64] [-d] [-t [script name]] [-s [sequence name]] ]"
    echo "  -h   Help info "
    echo "  -a   Load binary for ia32 platform or x64 platform. Besides -h, -a should be first parameter"
    echo "  -c   Run a Test Case in Script folder"
    echo "  -s   Run a Test Suite which has a sequence of Test Cases"
    echo "  -ss  Run a sequences of Test Suite"
  endif
endif

if %1 == "-a" then
  if %2 == "ia32" then
    load Bin\IA32\VirtualConsoleDxe.efi
    load Bin\IA32\MicroPythonDxe.efi
    Bin\IA32\micropython.efi -a Lib\scheduler.py %0 %1 %2 %3 %4 %5 %6 %7 %8
    goto START
  endif
  if %2 == "x64" then
    load Bin\X64\VirtualConsoleDxe.efi
    load Bin\X64\MicroPythonDxe.efi
    Bin\X64\micropython.efi -a Lib\scheduler.py %0 %1 %2 %3 %4 %5 %6 %7 %8
    goto START
  endif
  echo "Invalid argument. Input 'startup.nsh -h' for usage info"
endif

:START
echo "Launching Micropython Test Framework"
