# os_final

# Instruction
1. Clone the NachOS source code and cross compiler on github (Refer to Environment & NachOS Installation file)
    git clone https://github.com/joshhsieh1999/2024_OS_Final.git
2. Decompress file
    tar -zxf <Compressed-File>
3. Switch to the code folder
    cd nachos-4.0-final
4. Compile
    make clean
    make
5. Test your implementation with different p1, bt1, p2, bt2
    userprog/nachos -epb <execute file> <p1> <bt1> -epb <execute file> <p2> <bt2> -d z
    ex:
    userprog/nachos -epb test/hw2_test1 40 5000 -epb test/hw2_test2 80 4000 -d z
    You should see the results as below:
    other test case:
    userprog/nachos -epb test/hw2_test1 40 5000 -epb test/hw2_test2 40 4000 -d z
    userprog/nachos -epb test/hw2_test1 90 5000 -epb test/hw2_test2 100 4000 -d z

# Install NachOS and dependency 
I. Open a terminal (ctrl + alt + t), it should be at home directory (cd ~).
II. Install C-Shell and g++ 
• sudo apt-get install csh 
• sudo apt-get install g++ 
III. Download nachos and cross compiler on github 
• cd ~/Desktop 
• git clone https://github.com/joshhsieh1999/2024_OS_Final.git 
IV. Unzip nachos and cross compiler 
• cd ~/Desktop/2024_OS_Final
• sudo tar -zxf nachos-4.0-final.tar.gz • sudo tar -zxf mips-decstation.linux-xgcc.tar.gz 
V. Move cross compiler to /usr/local 
• mv ~/Desktop/2024_OS_Final/usr/local/nachos /usr/local 
VI. Test cross compiler by compiling “test” directory 
• cd ~/Desktop/2024_OS_Final/nachos-4.0-final/code/test 
• make clean (You have to add “clean” command into Makefile) 
• make 

# Compile (nachos won’t be able to correctly compile until you finish some functions within. Refer to the error messages during compiling)
• cd ~/nachos-master-nachos-4.0-final/nachos-4.0-final/code
• make clean
• make

# Test (This is the example for official version of nachos. Our project won’t be able to successfully build before implementing some functions!)
• cd userprog
• ./nachos -e ../test/test1
