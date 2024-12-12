# 573Cache
In order to simulate our cache, use the following commands.
1) gcc matmul.c -o matmul.exe
2) valgrind --tool=lackey ./matmul.exe > mem_accesses.txt
3) g++ input.cpp -o input.exe
4) ./input.exe < mem_accesses.txt > addresses.txt
5) g++ main.cpp -o main.exe
6) ./main.exe < matrix.txt addresses.txt > output.txt
