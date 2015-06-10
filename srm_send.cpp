/*
  Copyright (c) 2015 Shohei Aoki

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <cstring>
#include <libusb.h>

#define LIBUSB_ENDPOINT_OUT 0x01
#define LIBUSB_ENDPOINT_IN 0x82

using namespace std;

int main(int argc, char *argv[]) {
  if(argc!=2){
    cout << "Usage: srm_send <file.rml>" << endl;
    exit(1);
  }
  char* filename = argv[1];

  libusb_device **devs; 
  libusb_device_handle *dev_handle; 
  libusb_context *ctx = NULL; 
  int r; 
  r = libusb_init(&ctx); 
  if(r < 0) {
    cout<<"Init Error "<<r<<endl; 
    return 1;
  }
  libusb_set_debug(ctx, 3); 

  dev_handle = libusb_open_device_with_vid_pid(ctx, 0x0b75, 0x039f); 
  if(dev_handle == NULL)
    cout<<"Cannot open SRM-20"<<endl;
  else
    cout<<"SRM-20 opened"<<endl;

  int actual; 
  if(libusb_kernel_driver_active(dev_handle, 0) == 1) { 
    cout<<"Kernel Driver Active"<<endl;
    if(libusb_detach_kernel_driver(dev_handle, 0) == 0) 
      cout << "Kernel Driver Detached!" << endl;
  }
  r = libusb_claim_interface(dev_handle, 0); 
  if(r < 0) {
    cout << "Cannot claim interface" << endl;
    return 1;
  }
  cout << "Claimed interface" << endl;

  std::ifstream ifs(filename);
  if (ifs.fail())
  {
      std::cerr << "File open failed." << std::endl;
      return -1;
  }
  int begin = static_cast<int>(ifs.tellg());
  ifs.seekg(0, ifs.end);
  int end = static_cast<int>(ifs.tellg());
  int size = end - begin;
  ifs.clear();  
  ifs.seekg(0, ifs.beg);
  unsigned char *data= new unsigned char[size + 1];
  data[size] = '\0';  
  ifs.read((char *)data, size);

  cout << "Data: "<< data << endl; 
  cout << "Writing data..." << endl;
  cout << "Written bytes:" << strlen(reinterpret_cast<const char*>(data)) << endl;

  r = libusb_bulk_transfer(dev_handle, LIBUSB_ENDPOINT_OUT, data, strlen(reinterpret_cast<const char*>(data)), &actual, 0); 

  if(r == 0 && actual == strlen(reinterpret_cast<const char*>(data))) 
    cout<<"Write success"<<endl;
  else
    cout<<"Write error"<<endl;

  r = libusb_release_interface(dev_handle, 0); 
  if(r!=0) {
    cout<<"Cannot release interface"<<endl;
    return 1;
  }
  cout<<"Released interface"<<endl;

  libusb_close(dev_handle); 
  libusb_exit(ctx); 

  delete[] data; 
  return 0;
}

