#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <libusb.h>

#define LIBUSB_ENDPOINT_OUT 0x01
#define LIBUSB_ENDPOINT_IN 0x82

using namespace std;

int main() {
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

  std::ifstream ifs("zero.rml");
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
  cout << "sizeof(data)" << sizeof(data) << endl;
  cout << "strlen(data)" << strlen(reinterpret_cast<const char*>(data)) << endl;

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
