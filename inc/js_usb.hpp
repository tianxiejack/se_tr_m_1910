/*
 * js_usb.hpp
 *
 *  Created on: 2019年5月7日
 *      Author: yd
 */

#ifndef JS_USB_HPP_
#define JS_USB_HPP_

#include <iostream>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <libusb.h>

using namespace std;

#define USB_VENDOR_ID 0x11FF
#define USB_PRODUCT_ID 0x3331

#define BULK_ENDPOINT_OUT 1
#define BULK_ENDPOINT_IN  2

struct userDevice{
	uint16_t idVendor;
	uint16_t idProduct;
	uint8_t  bInterfaceClass;
	uint8_t  bInterfaceSubClass;
	uint8_t  bmAttributes;
	libusb_device *dev;
	libusb_device **devs;
	u_int8_t bInEndpointAddress;
	u_int8_t bOutEndpointAddress;
	uint8_t  bInterfaceNumber;
};

class CjoyStick
{
public:
	CjoyStick();
	~CjoyStick();
	unsigned char* JoystickProcess();
private:
	unsigned char jos_data[8];
	libusb_device_handle* g_usb_handle;
	struct userDevice user_device;
	struct libusb_device_descriptor dev_desc;
	int init();
	void JsProcess();
	int get_device_descriptor(struct libusb_device_descriptor *dev_desc,struct userDevice *user_device);
	int get_device_endpoint(struct libusb_device_descriptor *dev_desc,struct userDevice *user_device);
	int match_with_endpoint(const struct libusb_interface_descriptor * interface, struct userDevice *user_device);
};



#endif /* JS_USB_HPP_ */
