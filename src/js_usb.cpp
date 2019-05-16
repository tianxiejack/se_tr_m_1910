/*
 * js_usb.cpp
 *
 *  Created on: 2019年5月7日
 *      Author: yd
 */
#include "js_usb.hpp"

CjoyStick::CjoyStick()
{
	g_usb_handle = NULL;
	memset(jos_data, 0, sizeof(jos_data));
	init();
}

CjoyStick::~CjoyStick()
{

}

int CjoyStick::init()
{
	int rv;
	user_device.idProduct = USB_PRODUCT_ID;
	user_device.idVendor =  USB_VENDOR_ID ;
	user_device.bInterfaceClass = LIBUSB_CLASS_HID ;
	user_device.bInterfaceSubClass = LIBUSB_CLASS_HID ;
	user_device.bmAttributes = LIBUSB_TRANSFER_TYPE_INTERRUPT ;
	user_device.dev = NULL;
	libusb_init(NULL);

	rv = get_device_descriptor(&dev_desc,&user_device);
	if(rv < 0) {
		printf("*** get_device_descriptor failed! \n");
		return -1;
	}
	printf("get_device_descriptor  rv = %d \n",rv);
	rv = get_device_endpoint(&dev_desc,&user_device);
	if(rv < 0) {
		printf("*** get_device_endpoint failed! rv:%d \n",rv);
		return -1;
	}
	printf("get_device_endpoint  rv = %d \n",rv);
	g_usb_handle = libusb_open_device_with_vid_pid(NULL, user_device.idVendor, user_device.idProduct);
	if(g_usb_handle == NULL) {
		printf("*** Permission denied or Can not find the USB board (Maybe the USB driver has not been installed correctly), quit!\n");
		return -1;
	}
		rv = libusb_claim_interface(g_usb_handle,user_device.bInterfaceNumber);
		if(rv < 0) {
			rv = libusb_detach_kernel_driver(g_usb_handle,user_device.bInterfaceNumber);
			if(rv < 0) {
				printf("*** libusb_detach_kernel_driver failed! rv%d\n",rv);
				return -1;
			}
			rv = libusb_claim_interface(g_usb_handle,user_device.bInterfaceNumber);
			if(rv < 0)
			{
				printf("*** libusb_claim_interface failed! rv%d\n",rv);
				return -1;
			}
		}
		return 0;
}

unsigned char* CjoyStick::JoystickProcess()
{
	int rv;
	int length;
	int data;
	rv =  libusb_interrupt_transfer(g_usb_handle,user_device.bInEndpointAddress,jos_data,8,&length,10000000);
	if(rv < 0)
	{
		printf("*** bulk_transfer failed!   rv = %d \n",rv);
		//return (unsigned char*)-1;
	}
#if 0
	else
	{
		for(int i = 0; i<length; i++)
			printf("jos[%d]=%x  ", i, jos_data[i]);
		putchar(10);
	}
#endif
	return jos_data;
}

int CjoyStick::get_device_endpoint(struct libusb_device_descriptor *dev_desc,struct userDevice *user_device)
{
	/*3.get device endpoint that you need */
	int rv = -2;
	int i,j,k;
	struct libusb_config_descriptor *conf_desc;
	u_int8_t isFind = 0;
	for (i=0; i< dev_desc->bNumConfigurations; i++)
	{
		if(user_device->dev != NULL)
			rv = libusb_get_config_descriptor(user_device->dev,i,&conf_desc);
		if(rv < 0) {
			printf("*** libusb_get_config_descriptor failed! \n");
			return -1;
		}
		for (j=0; j< conf_desc->bNumInterfaces; j++)
		{
			for (k=0; k < conf_desc->interface[j].num_altsetting; k++)
			{
				if( conf_desc->interface[j].altsetting[k].bInterfaceClass==user_device->bInterfaceClass )
				{
					if(match_with_endpoint(&(conf_desc->interface[j].altsetting[k] ), user_device ))
					{
						user_device->bInterfaceNumber = conf_desc->interface[j].altsetting[k].bInterfaceNumber;
						libusb_free_config_descriptor(conf_desc);
						rv = 0;
						return rv;
					}
				}
			}
		}
	}
	return -2;  //don't find user device
}

int CjoyStick::get_device_descriptor(struct libusb_device_descriptor *dev_desc,struct userDevice *user_device)
{
	/*2.get device descriptor*/
	int rv = -2;
	ssize_t cnt;
	int i = 0;

	libusb_device **devs;
	libusb_device *dev;

	cnt = libusb_get_device_list(NULL, &devs); //check the device number
	if (cnt < 0)
		return (int) cnt;

	while ((dev = devs[i++]) != NULL) {
		rv = libusb_get_device_descriptor(dev,dev_desc);
		if(rv < 0) {
			printf("*** libusb_get_device_descriptor failed! i:%d \n",i);
			return -1;
		}
		if(dev_desc->idProduct==user_device->idProduct &&dev_desc->idVendor==user_device->idVendor)
		{
			user_device->dev = dev;
			user_device->devs = devs;
			rv = 0;
			break;
		}
	}
	return rv;
}

int CjoyStick::match_with_endpoint(const struct libusb_interface_descriptor * interface, struct userDevice *user_device)
{
	int i;
	int ret=0;
	for(i=0;i<interface->bNumEndpoints;i++)
	{
		if((interface->endpoint[i].bmAttributes&0x03)==user_device->bmAttributes)   //transfer type :bulk ,control, interrupt
		{
				if(interface->endpoint[i].bEndpointAddress&0x80)					//out endpoint & in endpoint
				{
					ret|=1;
					user_device->bInEndpointAddress = interface->endpoint[i].bEndpointAddress;
				}
				else
				{
					//ret|=2;
					user_device->bOutEndpointAddress = interface->endpoint[i].bEndpointAddress;
				}
		}

	}
	if(ret==3)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

