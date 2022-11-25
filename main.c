#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "hidapi.h"

hid_device *dev;

/*
 * The report returned is a 8 int list, ex:
 *
 * [0, 76, 72, 67, 88, 73, 0, 0, 2]

 * The first 5 in the list are a unique ID, in case there is more than one switch.

 * The last three seem to be reserved for the status of the relays. The status should
 * be interpreted in binary and in reverse order.  For example:

 * 2 = 00000010
 * This means that switch 1 is off and switch 2 is on, and all others are off.
 */
int read_status_mask(void)
{
	int ret;
	unsigned char buffer[10];

	memset(buffer, 0, sizeof(buffer));

	ret = hid_get_feature_report(dev, buffer, 9);
	if (ret == -1)
	{
		printf("read_status error\n");
		return 0;
	}

	return buffer[8] & 0xff;
}

int read_status(int chan)
{
	int mask = read_status_mask();

	return (mask & (1<<(chan-1)));
}

void power_off(int chan)
{
	int ret;
	unsigned char buffer[10];

	if (!read_status(chan))
		return;

	memset(buffer, 0, sizeof(buffer));

	buffer[0] = 0;
	buffer[1] = 0xfd;
	buffer[2] = chan;

	ret = hid_send_feature_report(dev, buffer, 9);
	if (ret == -1)
		printf("power on error\n");
}

void power_on(int chan)
{
	int ret;
	unsigned char buffer[10];

	if (read_status(chan))
		return;

	memset(buffer, 0, sizeof(buffer));

	buffer[0] = 0;
	buffer[1] = 0xff;
	buffer[2] = chan;

	ret = hid_send_feature_report(dev, buffer, 9);
	if (ret == -1)
		printf("power on error\n");
}

void power_toggle(int chan)
{
	if (read_status(chan))
		power_off(chan);
	else
		power_on(chan);
}

void power_on_all(void)
{
	int ret;
	unsigned char buffer[10];

	memset(buffer, 0, sizeof(buffer));

	buffer[0] = 0;
	buffer[1] = 0xfe;

	ret = hid_send_feature_report(dev, buffer, 9);
	if (ret == -1)
		printf("power on error\n");
}

void power_off_all(void)
{
	int ret;
	unsigned char buffer[10];

	memset(buffer, 0, sizeof(buffer));

	buffer[0] = 0;
	buffer[1] = 0xfc;

	ret = hid_send_feature_report(dev, buffer, 9);
	if (ret == -1)
		printf("power on error\n");
}

int main(void)
{
	dev = hid_open(0x16c0, 0x05df, NULL);
	if (!dev)
	{
		printf("cannot open hid device, %s\n", strerror(errno));
		return 0;
	}

	power_on_all();

	sleep(1);

	power_off_all();

	sleep(1);

	while (1)
	{
		int chan = rand()%8+1;

		printf("toggle %d\n", chan);

		power_toggle(chan);

		usleep(500*1000);
	}


	return 0;
}
