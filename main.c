#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <usb.h>

//Define SRM-20
#define USB_VENDOR 0x0B75
#define USB_PRODUCT 0x039F
//#define TIMEOUT (5*1000)
#define TIMEOUT (1000)
#define ENDPOINT_IN 0x82
#define ENDPOINT_OUT 0x01

int SendRml(usb_dev_handle *dh, char *rmlfile){
  FILE *fp;
  char buf[128];
  int result;

  fp = fopen(rmlfile,"r");
  if(fp==NULL){
    printf("rml file not found\n");
    return -1;
  }
  while(fgets(buf,128,fp) != NULL){
    if (strchr(buf, '\n') != NULL) {
       buf[strlen(buf) - 1] = '\0';
    }
    printf("*****start sending********\n");
    printf("sending rml: %s\n",buf);
    printf("number of bytes: %d\n",strlen(buf));
    /*

    result = usb_bulk_write(dh, ENDPOINT_OUT, buf, strlen(buf), TIMEOUT);
    if(result < 0){
      printf("rml write error\n");
      return -1;
    } else {
      printf("%d bytes sent.\n",result);
    }
    printf("*****end sending********\n");
    */
  }

  fclose(fp);
  return 0;
}

/* Init USB */
struct usb_bus *USB_init()
{
  usb_init();
  usb_find_busses();
  usb_find_devices();
  return(usb_get_busses());
}

/* Find USB device  */
struct usb_device *USB_find(struct usb_bus *busses, struct usb_device *dev)
{
  struct usb_bus *bus;
  for(bus=busses; bus; bus=bus->next){
    for(dev=bus->devices; dev; dev=dev->next) {
      if( (dev->descriptor.idVendor==USB_VENDOR) && (dev->descriptor.idProduct==USB_PRODUCT) ){
        return( dev );
      }
    }
  }
  return( NULL );
}

/* USB Open */
struct usb_dev_handle *USB_open(struct usb_device *dev)
{
  struct usb_dev_handle *udev = NULL;

  udev=usb_open(dev);
  if( (udev=usb_open(dev))==NULL ){
    fprintf(stderr,"usb_open Error.(%s)\n",usb_strerror());
    exit(1);
  }

  if( usb_set_configuration(udev,dev->config->bConfigurationValue)<0 ){
    if( usb_detach_kernel_driver_np(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
      fprintf(stderr,"usb_set_configuration Error.\n");
      fprintf(stderr,"usb_detach_kernel_driver_np Error.(%s)\n",usb_strerror());
      printf("Could not complete configuration. Try with sudo.\n");
      exit(-1);
    }
  }

  if( usb_claim_interface(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
    if( usb_detach_kernel_driver_np(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
      fprintf(stderr,"usb_claim_interface Error.\n");
      fprintf(stderr,"usb_detach_kernel_driver_np Error.(%s)\n",usb_strerror());
    }
  }

  if( usb_claim_interface(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
    fprintf(stderr,"usb_claim_interface Error.(%s)\n",usb_strerror());
  }

  return(udev);
}

/* USB Close */
void USB_close(struct usb_dev_handle *dh)
{
  if(usb_release_interface(dh, 0)){
    fprintf(stderr,"usb_release_interface() failed. (%s)\n",usb_strerror());
  }
  if( usb_close(dh)<0 ){
    fprintf(stderr,"usb_close Error.(%s)\n",usb_strerror());
  }
}

/* USB altinterface */
void USB_altinterface(struct usb_dev_handle *dh,int tyep)
{
  if(usb_set_altinterface(dh,tyep)<0)
  {
    fprintf(stderr,"Failed to set altinterface %d: %s\n", 1,usb_strerror());
    USB_close(dh);
  }
}

int main()
{
  struct usb_bus *bus;
  struct usb_device *dev;
  usb_dev_handle *dh;

  /* Initialize */
    bus=USB_init();
    dev=USB_find(bus,dev);
    if( dev==NULL ){
      fprintf(stderr,"Device not found\n");
      exit(1); 
    }
  printf("Initialization done.\n");
  /*-------------*/
    /* Device Open */
    /*-------------*/
    dh=USB_open(dev);
    if( dh==NULL ){ exit(2); }
  printf("SRM-20 opened.\n");

  char *rmlfile = "zero.rml";
  SendRml(dh, rmlfile);

  printf("SRM-20 closed.\n");
  USB_close(dh);

  return 0;
}

