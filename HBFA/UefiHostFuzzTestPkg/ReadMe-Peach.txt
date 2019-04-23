How to run Peach with UefiHostTestPkg in OS?
==============
Peach:
Prepare Peach in Linux
1)  goto http://www.peach.tech/resources/peachcommunity/, read the content.
2)  install mono-complete "sudo apt-get install mono-complete"
3)  goto https://sourceforge.net/projects/peachfuzz/, "Project Activity"
    Download https://sourceforge.net/projects/peachfuzz/files/Peach/3.1/peach-3.1.124-linux-x86-release.zip/download
    or https://sourceforge.net/projects/peachfuzz/files/Peach/3.1/peach-3.1.124-linux-x86_64-release.zip/download
4)  "cp peach-3.1.124-linux-x86_64-release.zip /home/<user name>/Env/peach"
5)  "cd /home/<user name>/Env/peach"
6)  "unzip peach-3.1.124-linux-x86_64-release.zip"
7)  Add below content at the end of ~/.bashrc:
      export PEACH_PATH=<PEACH_PATH>
    For example:
      export PEACH_PATH=/home/<user name>/Env/peach
8)  try "peach samples/HelloWorld.xml"
    If you find the screen has lots output, that means the peach runs successfully.

Prepare Peach in Windows
1)  goto http://www.peach.tech/resources/peachcommunity/, read the content.
2)  download windows pre-build binary.

Write EDKII Peach PIT file
1)  Read http://community.peachfuzzer.com/v3/PeachQuickStart.html
2)  Write PIT file in "HBFA/UefiHostFuzzTestCasePkg/TestCase/XXX/PeachDataModel/TestXXX.xml"
2.1) Define DataModel.
  <DataModel name="VARIABLE_STORE_HEADER">
    <Blob name="Signature" length="16" value="782cf3aa7b949a43a1802e144ec37792" valueType="hex" mutable="false"/>
    <Number name="Size" size="32" value="00001000" valueType="hex"/>
    ......
  </DataModel>

2.2) Define Executable and Arguments
    <Monitor class="LinuxDebugger">
      <Param name="Executable" value="<...>/Build/UefiHostFuzzTestCasePkg/DEBUG_GCC5/IA32/TestUsb" />
      <Param name="Arguments" value="fuzzfile.bin" />
    </Monitor>

Build Test App.
1) Build all test with GCC5 and CLANG8 tool chain.
   (some test will use GCC5 and some test will use CLANG8)

Run Peach
1)  "peach HBFA/UefiHostFuzzTestCasePkg/TestCase/XXX/PeachDataModel/TestXXX.xml"
2)  You will see screen has lots of output. Have fun.

=============================================
[2052,-,-] Performing iteration
[*] Fuzzing: TheDataModel.DataElement_0
[*] Mutator: DataElementDuplicateMutator
Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!
[2053,-,-] Performing iteration
[*] Fuzzing: TheDataModel.DataElement_0
[*] Mutator: StringMutator
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
[2055,-,-] Performing iteration
[*] Fuzzing: TheDataModel.DataElement_0
[*] Mutator: StringMutator
134
