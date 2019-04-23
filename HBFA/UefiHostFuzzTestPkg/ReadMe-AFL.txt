How to run AFL with UefiHostTestPkg in OS?
==============
AFL:
Prepare AFL in Linux
1)	goto http://lcamtuf.coredump.cx/afl/
2)	Download http://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz
3)	"mv afl-latest.tgz /home/<user name>/Env"
4)	"cd /home/<user name>/Env"
5)	"tar xzvf afl-latest.tgz"
6)	"cd afl-2.5.2b"
7)	"make"
8)	sudo bash -c 'echo core >/proc/sys/kernel/core_pattern'
9)	cd /sys/devices/system/cpu/
10)	sudo bash -c 'echo performance | tee cpu*/cpufreq/scaling_governor'
11)	For more document:
	  afl-2.52b\docs\QuickStartGuide.txt
	  afl-2.52b\docs\README
12)	Add below content at the end of ~/.bashrc:
	  export AFL_PATH=<AFL_PATH>
	  export PATH=$PATH:$AFL_PATH
	For example:
	  export AFL_PATH=/home/<user name>/Env/afl-2.52b
	  export PATH=$PATH:$AFL_PATH
13)	"reboot"

Prepare AFL in Windows
1)	goto https://github.com/googleprojectzero/winafl
2)	clone it (Note: Please use version d501350f02147860604b5d755960fe3fc201653a)
3)	read the instruction in README.md
4)	download DynamoRIO release 6.2.0-2 from https://github.com/DynamoRIO/dynamorio/wiki/Downloads
5)	setup AFL_PATH=<...>\winafl, DRIO_PATH=<...>\DynamoRIO-Windows-6.2.0-2
6)	if you want to build AFL from source, please download CMAKE (https://cmake.org/download/) (optional)
	mkdir build32
	cd build32
	cmake -G"Visual Studio 14 2015" .. -DDynamoRIO_DIR=%DRIO_PATH%\cmake
	cmake --build . --config Debug
	or
	mkdir build64
	cd build64
	cmake -G"Visual Studio 14 2015 Win64" .. -DDynamoRIO_DIR=%DRIO_PATH%\cmake
	cmake --build . --config Debug



Build EDKII test case in Linux
1)	"cat HBFA/UefiHostFuzzTestPkg/Conf/tools_def.customized >> Conf/tools_def.txt"
2)	"build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -a IA32 -t AFL"
3)	"mkdir testcase_dir"
4)	"mkdir /dev/shm/findings_dir"
5)	"cp HBFA/UefiHostFuzzTestCasePkg/Seed/<SPECIFIC_SEED_FOLDER>/xxx.bin testcase_dir"
	NOTE: <SPECIFIC_SEED_FOLDER> mapping list please refer to HBFA/UefiHostFuzzTestCasePkg/Seed/readme.txt

Build EDKII test case in Windows
1)	"build -p UefiHostFuzzTestCasePkg\UefiHostFuzzTestCasePkg.dsc -a IA32 -t VS2015x86"
2)	"mkdir in"
3)	"mkdir out"
4)	"cp HBFA/UefiHostFuzzTestCasePkg\Seed\<SPECIFIC_SEED_FOLDER>\xxx.bin in"
	NOTE: <SPECIFIC_SEED_FOLDER> mapping list please refer to HBFA\UefiHostFuzzTestCasePkg\Seed\readme.txt



Run AFL in Linux
Note:'/dev/shm' is tmpfs, so 'findings_dir' will disappear after reboot system, please backup test result before reboot system.
1)	"afl-fuzz -i testcase_dir -o /dev/shm/findings_dir Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/IA32/TestPartition @@"
2)	You will see something like below. Have fun!

Run AFL in Windows
1)	Copy xxx.exe and xxx.pdb to the same dir as winafl\bin32 or winafl\bin64, for example: TestPartition.exe and TestPartition.pdb. (NOTE: xxx.pdb must be copied)
2)	cd %AFL_PATH%\bin32
	afl-fuzz.exe -i in -o out -D %DRIO_PATH%\bin32 -t 20000 -- -coverage_module xxx.exe -fuzz_iterations 1000 -target_module xxx.exe -target_method main -nargs 2 -- xxx.exe @@ 
	or
	cd %AFL_PATH%\bin64
	afl-fuzz.exe -i in -o out -D %DRIO_PATH%\bin64 -t 20000 -- -coverage_module xxx.exe -fuzz_iterations 1000 -target_module xxx.exe -target_method main -nargs 2 -- xxx.exe @@ 
3)	You will see similar output, although it is slower than Linux.


                    american fuzzy lop 2.52b (TestPartition)

lq process timing qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqwq overall results qqqqqk
x        run time : 0 days, 0 hrs, 0 min, 5 sec        x  cycles done : 40     x
x   last new path : none yet (odd, check syntax!)      x  total paths : 1      x
x last uniq crash : none seen yet                      x uniq crashes : 0      x
x  last uniq hang : none seen yet                      x   uniq hangs : 0      x
tq cycle progress qqqqqqqqqqqqqqqqqqqqwq map coverage qvqqqqqqqqqqqqqqqqqqqqqqqu
x  now processing : 0 (0.00%)         x    map density : 0.00% / 0.00%         x
x paths timed out : 0 (0.00%)         x count coverage : 1.00 bits/tuple       x
tq stage progress qqqqqqqqqqqqqqqqqqqqnq findings in depth qqqqqqqqqqqqqqqqqqqqu
x  now trying : havoc                 x favored paths : 1 (100.00%)            x
x stage execs : 164/256 (64.06%)      x  new edges on : 1 (100.00%)            x
x total execs : 11.8k                 x total crashes : 0 (0 unique)           x
x  exec speed : 2092/sec              x  total tmouts : 0 (0 unique)           x
tq fuzzing strategy yields qqqqqqqqqqqvqqqqqqqqqqqqqqqwq path geometry qqqqqqqqu
x   bit flips : 0/32, 0/31, 0/29                      x    levels : 1          x
x  byte flips : 0/4, 0/3, 0/1                         x   pending : 0          x
x arithmetics : 0/224, 0/204, 0/68                    x  pend fav : 0          x
x  known ints : 0/8, 0/18, 0/10                       x own finds : 0          x
x  dictionary : 0/0, 0/0, 0/0                         x  imported : n/a        x
x       havoc : 0/11.0k, 0/0                          x stability : 100.00%    x
x        trim : 100.00%/29, 0.00%                     tqqqqqqqqqqqqqqqqqqqqqqqqj
^Cqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj          [cpu000: 70%]



