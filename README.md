# os_final

# Instruction
1. Clone the NachOS source code and cross compiler on github (Refer to Environment & NachOS Installation file)<br>
    git clone https://github.com/joshhsieh1999/2024_OS_Final.git<br>
2. Decompress file<br>
    tar -zxf <Compressed-File><br>
3. Switch to the code folder<br>
    cd nachos-4.0-final<br>
4. Compile<br>
    make clean<br>
    make<br>
5. Test your implementation with different p1, bt1, p2, bt2<br>
    userprog/nachos -epb <execute file> <p1> <bt1> -epb <execute file> <p2> <bt2> -d z<br>
    ex:<br>
    userprog/nachos -epb test/hw2_test1 40 5000 -epb test/hw2_test2 80 4000 -d z<br>
    You should see the results as below:<br>
    other test case:<br>
    userprog/nachos -epb test/hw2_test1 40 5000 -epb test/hw2_test2 40 4000 -d z<br>
    userprog/nachos -epb test/hw2_test1 90 5000 -epb test/hw2_test2 100 4000 -d z<br>

# Install NachOS and dependency 
I. Open a terminal (ctrl + alt + t), it should be at home directory (cd ~).<br>

II. Install C-Shell and g++ <br>
• sudo apt-get install csh <br>
• sudo apt-get install g++ <br>

III. Download nachos and cross compiler on github <br>
• cd ~/Desktop <br>
• git clone https://github.com/joshhsieh1999/2024_OS_Final.git <br>

IV. Unzip nachos and cross compiler <br>
• cd ~/Desktop/2024_OS_Final<br>
• sudo tar -zxf nachos-4.0-final.tar.gz <br>
• sudo tar -zxf mips-decstation.linux-xgcc.tar.gz <br>

V. Move cross compiler to /usr/local <br>
• mv ~/Desktop/2024_OS_Final/usr/local/nachos /usr/local <br>

VI. Test cross compiler by compiling “test” directory <br>
• cd ~/Desktop/2024_OS_Final/nachos-4.0-final/code/test <br>
• make clean (You have to add “clean” command into Makefile) <br>
• make <br>

# Compile 
(nachos won’t be able to correctly compile until you finish some functions within. Refer to the error messages during compiling)<br>
• cd ~/nachos-master-nachos-4.0-final/nachos-4.0-final/code<br>
• make clean<br>
• make<br>

# Test 
(This is the example for official version of nachos. Our project won’t be able to successfully build before implementing some functions!)<br>
• cd userprog<br>
• ./nachos -e ../test/test1<br>
