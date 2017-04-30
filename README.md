# Virtualbox Wrapper for SERVANT on Windows
SERVANT uses Virtualbox for VM management.  
Since g++ can not use MSCOM properly, this wrapper use localhost socket to connect SERVANT Base and Virtualbox.  

## Dependencies
* Virtualbox SDK  
* asio-1.10.6  

## Build
* Put asio in this relative path, or just edit CMakeLists.txt:  
  ../servant-dependencies/asio-1.10.6/  
* Put Virtualbox SDK Files except VirtualBox.h in ./src/lib:  
  VirtualBox.tlb  
  VirtualBox_i.c
* Put Virtualbox.h in ./src/include

## Note
* This wrapper is Windows-dependent.  
* This project is still under heavy development.  
