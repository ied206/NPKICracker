# NPKICracker
NPKI Bruteforcing Tool written for [Inc0gnito Hacking Conference](http://inc0gnito.com/) 2015.

Written with OpenMP and C.

## Note
This project is written for academic purpose and may be incomplete.  
I do not provide any warranty with result.  
OpenCL support is not available currently, please do not use it.  
Any bug report or suggestions, contributions would be appreciated.

# Usage
Open terminal and specify arguments.  
If NPKICracker is launched without any arguments, it will print detailed help message.
- Argument format :
```
./NPKICracker -f <PrivateKey> -m <MinPWLen> -M <MaxPWLen> [Optionial...]
```
- Example 1 :  
-> NPKICracker will brurteforce signPri.key, using password of length 10-12, compsited of charecters from charset.txt.
```
$ ./NPKICracker -f NPKI/signPri.key -m 10 -M 12 -c charset.txt
```
- Example 2 :  
-> NPKICracker will brurteforce signPri.key, starting from password '000b0gn!4o'.
```
$ ./NPKICracker -f NPKI/signPri.key -m 10 -M 10 -c charset.txt -i '000b0gn\!4o'
```

# Compile
This program is built under this environment:
- CodeBlocks
- gcc (on Linux)
- MinGW-w64 (on Windows)

## Using Code::Blocks
1. Open `NPKICracker.cbp` using CodeBlocks
2. Open 'Projects' tab in Management panel (usually at left of screen)
3. Right-click project `NPKICracker` and select 'Build Options'
4. Choose your favorite compiler, usually 'GNU GCC Compiler', then press OK button
5. Set Build targer to 'Release', then build project.
6. Binary will be generated in `bin\Release\` directory.

## Using MinGW-w64 and make (Windows)
1\. Add MinGW-w64 toolchain to PATH. Make sure `gcc, g++, ar, windres, mingw32-make` can be called in console.  
2\. Build project with mingw32-make.
```
Type this in source directory's root:
> mingw32-make
```

3\. Binary will be compiled under `bin` folder.

## Using make (Linux, macOS)
1\. Install make and compiler.  
    gcc or compatible compiler such as clang is recommended.  
```
For example, in Debian based Linux distribution:
$ sudo apt install build-essential
```

2\. Build project with make.
```
Type this in source directory's root:
> make
```

3\. Binary will be compiled under `bin` folder.

# Credits
Hajin Jang (@ied206, aka joveler), Junhyuk Lee (@leehj10003)   
NPKICracker is licensed under MIT License.  
SEED Code from [KISA](https://seed.kisa.or.kr/iwt/ko/sup/EgovSeedInfo.do)  
