/*
 *
 *  oFono - Open Source Telephony
 *
 *  Copyright (C) 2017  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/uio.h>
#include <linux/types.h>
#include <assert.h>

#include <ell/ell.h>

#include "drivers/mbimmodem/mbim.h"
#include "drivers/mbimmodem/mbim-message.h"
#include "drivers/mbimmodem/mbim-private.h"

struct message_data {
	uint32_t tid;
	const unsigned char *binary;
	size_t binary_len;
};

static const unsigned char message_binary_device_caps[] = {
	0x03, 0x00, 0x00, 0x80, 0x08, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0x89, 0xCC, 0x33,
	0xBC, 0xBB, 0x8B, 0x4F, 0xB6, 0xB0, 0x13, 0x3E, 0xC2, 0xAA, 0xE6, 0xDF,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00,
	0x60, 0x00, 0x00, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x9C, 0x00, 0x00, 0x00,
	0x3C, 0x00, 0x00, 0x00, 0x33, 0x00, 0x35, 0x00, 0x39, 0x00, 0x33, 0x00,
	0x33, 0x00, 0x36, 0x00, 0x30, 0x00, 0x35, 0x00, 0x30, 0x00, 0x30, 0x00,
	0x31, 0x00, 0x38, 0x00, 0x37, 0x00, 0x31, 0x00, 0x37, 0x00, 0x00, 0x00,
	0x46, 0x00, 0x49, 0x00, 0x48, 0x00, 0x37, 0x00, 0x31, 0x00, 0x36, 0x00,
	0x30, 0x00, 0x5F, 0x00, 0x56, 0x00, 0x31, 0x00, 0x2E, 0x00, 0x31, 0x00,
	0x5F, 0x00, 0x4D, 0x00, 0x4F, 0x00, 0x44, 0x00, 0x45, 0x00, 0x4D, 0x00,
	0x5F, 0x00, 0x30, 0x00, 0x31, 0x00, 0x2E, 0x00, 0x31, 0x00, 0x34, 0x00,
	0x30, 0x00, 0x38, 0x00, 0x2E, 0x00, 0x30, 0x00, 0x37, 0x00, 0x00, 0x00,
	0x58, 0x00, 0x4D, 0x00, 0x4D, 0x00, 0x37, 0x00, 0x31, 0x00, 0x36, 0x00,
	0x30, 0x00, 0x5F, 0x00, 0x56, 0x00, 0x31, 0x00, 0x2E, 0x00, 0x31, 0x00,
	0x5F, 0x00, 0x4D, 0x00, 0x42, 0x00, 0x49, 0x00, 0x4D, 0x00, 0x5F, 0x00,
	0x47, 0x00, 0x4E, 0x00, 0x53, 0x00, 0x53, 0x00, 0x5F, 0x00, 0x4E, 0x00,
	0x41, 0x00, 0x4E, 0x00, 0x44, 0x00, 0x5F, 0x00, 0x52, 0x00, 0x45, 0x00
};

static const struct message_data message_data_device_caps = {
	.tid		= 2,
	.binary		= message_binary_device_caps,
	.binary_len	= sizeof(message_binary_device_caps),
};

static const unsigned char message_binary_device_caps_query[] = {
	0x03, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0x89, 0xCC, 0x33,
	0xBC, 0xBB, 0x8B, 0x4F, 0xB6, 0xB0, 0x13, 0x3E, 0xC2, 0xAA, 0xE6, 0xDF,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const struct message_data message_data_device_caps_query = {
	.tid = 2,
	.binary = message_binary_device_caps_query,
	.binary_len = sizeof(message_binary_device_caps_query),
};

static const unsigned char message_binary_subscriber_ready_status[] = {
	0x03, 0x00, 0x00, 0x80, 0xB4, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0x89, 0xCC, 0x33,
	0xBC, 0xBB, 0x8B, 0x4F, 0xB6, 0xB0, 0x13, 0x3E, 0xC2, 0xAA, 0xE6, 0xDF,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00,
	0x44, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
	0x33, 0x00, 0x31, 0x00, 0x30, 0x00, 0x34, 0x00, 0x31, 0x00, 0x30, 0x00,
	0x32, 0x00, 0x32, 0x00, 0x37, 0x00, 0x39, 0x00, 0x32, 0x00, 0x33, 0x00,
	0x33, 0x00, 0x37, 0x00, 0x34, 0x00, 0x00, 0x00, 0x38, 0x00, 0x39, 0x00,
	0x30, 0x00, 0x31, 0x00, 0x34, 0x00, 0x31, 0x00, 0x30, 0x00, 0x34, 0x00,
	0x32, 0x00, 0x31, 0x00, 0x32, 0x00, 0x32, 0x00, 0x37, 0x00, 0x39, 0x00,
	0x32, 0x00, 0x33, 0x00, 0x33, 0x00, 0x37, 0x00, 0x34, 0x00, 0x37, 0x00,
	0x31, 0x00, 0x35, 0x00, 0x31, 0x00, 0x32, 0x00, 0x34, 0x00, 0x33, 0x00,
	0x31, 0x00, 0x30, 0x00, 0x35, 0x00, 0x39, 0x00, 0x36, 0x00, 0x00, 0x00
};

static const struct message_data message_data_subscriber_ready_status = {
	.tid		= 2,
	.binary		= message_binary_subscriber_ready_status,
	.binary_len	= sizeof(message_binary_subscriber_ready_status),
};

static const unsigned char message_binary_phonebook_read[] = {
	0x03, 0x00, 0x00, 0x80, 0x68, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4B, 0xF3, 0x84, 0x76,
	0x1E, 0x6A, 0x41, 0xDB, 0xB1, 0xD8, 0xBE, 0xD2, 0x89, 0xC2, 0x5B, 0xDB,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00,
	0x28, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x39, 0x00, 0x32, 0x00,
	0x31, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00,
	0x36, 0x00, 0x00, 0x00, 0x54, 0x00, 0x53, 0x00,
};

static const struct message_data message_data_phonebook_read = {
	.tid		= 2,
	.binary		= message_binary_phonebook_read,
	.binary_len	= sizeof(message_binary_phonebook_read),
};

static const unsigned char message_binary_sms_read_all_empty[] = {
	0x03, 0x00, 0x00, 0x80, 0x38, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x3f, 0xbe, 0xeb,
	0x14, 0xfe, 0x44, 0x67, 0x9f, 0x90, 0x33, 0xa2, 0x23, 0xe5, 0x6c, 0x3f,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const struct message_data message_data_sms_read_all_empty = {
	.tid		= 8,
	.binary		= message_binary_sms_read_all_empty,
	.binary_len	= sizeof(message_binary_sms_read_all_empty),
};

static const unsigned char message_binary_sms_read_all[] = {
	0x03, 0x00, 0x00, 0x80, 0xac, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x3f, 0xbe, 0xeb,
	0x14, 0xfe, 0x44, 0x67, 0x9f, 0x90, 0x33, 0xa2, 0x23, 0xe5, 0x6c, 0x3f,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x2f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0x1f, 0x00, 0x00, 0x00, 0x07, 0x91, 0x61, 0x63, 0x83, 0x84, 0x29, 0xf3,
	0x04, 0x0b, 0x91, 0x51, 0x21, 0x55, 0x30, 0x71, 0xf9, 0x00, 0x00, 0x71,
	0x11, 0x70, 0x91, 0x62, 0x65, 0x4a, 0x04, 0xd4, 0xf2, 0x9c, 0x0e, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0x21, 0x00, 0x00, 0x00, 0x07, 0x91, 0x61, 0x63, 0x83, 0x84, 0x29, 0xf2,
	0x04, 0x0b, 0x91, 0x51, 0x21, 0x55, 0x30, 0x71, 0xf9, 0x00, 0x00, 0x71,
	0x11, 0x80, 0x41, 0x70, 0x55, 0x4a, 0x06, 0xc6, 0xf7, 0x1b, 0x74, 0x2f,
	0x03, 0x00, 0x00, 0x00
};

static const struct message_data message_data_sms_read_all = {
	.tid		= 0xc,
	.binary		= message_binary_sms_read_all,
	.binary_len	= sizeof(message_binary_sms_read_all),
};

static const unsigned char message_binary_sms_send[] = {
	0x03, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x3F, 0xBE, 0xEB,
	0x14, 0xFE, 0x44, 0x67, 0x9F, 0x90, 0x33, 0xA2, 0x23, 0xE5, 0x6C, 0x3F,
	0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x0B, 0x91, 0x99, 0x99, 0x99, 0x99, 0x99, 0xF9, 0x00,
	0x00, 0x06, 0xC6, 0xF7, 0x5B, 0x1C, 0x96, 0x03
};

static const struct message_data message_data_sms_send = {
	.tid		= 34,
	.binary		= message_binary_sms_send,
	.binary_len	= sizeof(message_binary_sms_send),
};

static const unsigned char message_binary_device_subscribe_list[] = {
	0x03, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0x89, 0xCC, 0x33,
	0xBC, 0xBB, 0x8B, 0x4F, 0xB6, 0xB0, 0x13, 0x3E, 0xC2, 0xAA, 0xE6, 0xDF,
	0x13, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
	0x3C, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0xA2, 0x89, 0xCC, 0x33,
	0xBC, 0xBB, 0x8B, 0x4F, 0xB6, 0xB0, 0x13, 0x3E, 0xC2, 0xAA, 0xE6, 0xDF,
	0x05, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
	0x0C, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
	0x53, 0x3F, 0xBE, 0xEB, 0x14, 0xFE, 0x44, 0x67, 0x9F, 0x90, 0x33, 0xA2,
	0x23, 0xE5, 0x6C, 0x3F, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00
};

static const struct message_data message_data_device_subscribe_list = {
	.tid		= 0x1f,
	.binary		= message_binary_device_subscribe_list,
	.binary_len	= sizeof(message_binary_device_subscribe_list),
};

static const unsigned char message_binary_packet_service_notify[] = {
	0x07, 0x00, 0x00, 0x80, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa2, 0x89, 0xcc, 0x33,
	0xbc, 0xbb, 0x8b, 0x4f, 0xb6, 0xb0, 0x13, 0x3e, 0xc2, 0xaa, 0xe6, 0xdf,
	0x0a, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x80, 0xf0, 0xfa, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xe1, 0xf5, 0x05, 0x00, 0x00, 0x00, 0x00,
};

static const struct message_data message_data_packet_service_notify = {
	.tid		= 0,
	.binary		= message_binary_packet_service_notify,
	.binary_len	= sizeof(message_binary_packet_service_notify),
};

static void do_debug(const char *str, void *user_data)
{
	const char *prefix = user_data;

	l_info("%s%s", prefix, str);
}

static struct mbim_message *build_message(const struct message_data *msg_data)
{
	static const unsigned int frag_size = 64;
	struct mbim_message *msg;
	struct iovec *iov;
	size_t n_iov;
	unsigned int i;

	n_iov = align_len(msg_data->binary_len, frag_size) / frag_size;
	iov = l_new(struct iovec, n_iov);

	iov[0].iov_len = msg_data->binary_len < frag_size ?
					msg_data->binary_len - 20 :
					frag_size - 20;
	iov[0].iov_base = l_memdup(msg_data->binary + 20, iov[0].iov_len);

	if (n_iov == 1)
		goto done;

	for (i = 1; i < n_iov - 1; i++) {
		iov[i].iov_base = l_memdup(msg_data->binary + i * frag_size,
							frag_size);
		iov[i].iov_len = frag_size;
	}

	iov[i].iov_len = msg_data->binary_len - i * frag_size;
	iov[i].iov_base = l_memdup(msg_data->binary + i * frag_size,
							iov[i].iov_len);

done:
	msg = _mbim_message_build(msg_data->binary, iov, n_iov);
	assert(msg);

	return msg;
}

static bool check_message(struct mbim_message *message,
					const struct message_data *msg_data)
{
	size_t len;
	void *message_binary = _mbim_message_to_bytearray(message, &len);
	bool r = false;

	l_util_hexdump(false, msg_data->binary, msg_data->binary_len,
			do_debug, "[MSG] ");

	l_util_hexdump(true, message_binary, len, do_debug, "[MSG] ");

	assert(message_binary);
	if (len != msg_data->binary_len)
		goto done;

	r = memcmp(message_binary, msg_data->binary, len) == 0;
done:
	l_free(message_binary);
	return r;
}

static void parse_device_caps(const void *data)
{
	struct mbim_message *msg = build_message(data);
	uint32_t device_type;
	uint32_t cellular_class;
	uint32_t voice_class;
	uint32_t sim_class;
	uint32_t data_class;
	uint32_t sms_caps;
	uint32_t control_caps;
	uint32_t max_sessions;
	char *custom_data_class;
	char *device_id;
	char *firmware_info;
	char *hardware_info;
	bool r;

	r = mbim_message_get_arguments(msg, "uuuuuuuussss",
					&device_type, &cellular_class,
					&voice_class, &sim_class, &data_class,
					&sms_caps, &control_caps, &max_sessions,
					&custom_data_class, &device_id,
					&firmware_info, &hardware_info);
	assert(r);

	assert(device_type == 1);
	assert(cellular_class = 1);
	assert(voice_class == 1);
	assert(sim_class == 2);
	assert(data_class == 0x3f);
	assert(sms_caps == 0x3);
	assert(control_caps == 1);
	assert(max_sessions == 16);
	assert(custom_data_class == NULL);
	assert(device_id);
	assert(!strcmp(device_id, "359336050018717"));
	assert(firmware_info);
	assert(!strcmp(firmware_info, "FIH7160_V1.1_MODEM_01.1408.07"));
	assert(hardware_info);
	assert(!strcmp(hardware_info, "XMM7160_V1.1_MBIM_GNSS_NAND_RE"));

	l_free(custom_data_class);
	l_free(device_id);
	l_free(firmware_info);
	l_free(hardware_info);
	mbim_message_unref(msg);
}

static void build_device_caps(const void *data)
{
	const struct message_data *msg_data = data;
	bool r;
	struct mbim_message *message;
	struct mbim_message_builder *builder;
	uint32_t device_type = 1;
	uint32_t cellular_class = 1;
	uint32_t voice_class = 1;
	uint32_t sim_class = 2;
	uint32_t data_class = 0x3f;
	uint32_t sms_caps = 0x3;
	uint32_t control_caps = 1;
	uint32_t max_sessions = 16;

	message = _mbim_message_new_command_done(mbim_uuid_basic_connect,
							1, 0);
	assert(message);

	builder = mbim_message_builder_new(message);
	assert(builder);

	assert(mbim_message_builder_append_basic(builder, 'u', &device_type));
	assert(mbim_message_builder_append_basic(builder, 'u',
							&cellular_class));
	assert(mbim_message_builder_append_basic(builder, 'u', &voice_class));
	assert(mbim_message_builder_append_basic(builder, 'u', &sim_class));
	assert(mbim_message_builder_append_basic(builder, 'u', &data_class));
	assert(mbim_message_builder_append_basic(builder, 'u', &sms_caps));
	assert(mbim_message_builder_append_basic(builder, 'u', &control_caps));
	assert(mbim_message_builder_append_basic(builder, 'u', &max_sessions));

	assert(mbim_message_builder_append_basic(builder, 's', NULL));
	assert(mbim_message_builder_append_basic(builder, 's',
							"359336050018717"));
	assert(mbim_message_builder_append_basic(builder, 's',
					"FIH7160_V1.1_MODEM_01.1408.07"));
	assert(mbim_message_builder_append_basic(builder, 's',
					"XMM7160_V1.1_MBIM_GNSS_NAND_RE"));

	assert(mbim_message_builder_finalize(builder));
	mbim_message_builder_free(builder);

	_mbim_message_set_tid(message, msg_data->tid);
	assert(check_message(message, msg_data));
	mbim_message_unref(message);

	/* now try to build the same message using set_arguments */
	message = _mbim_message_new_command_done(mbim_uuid_basic_connect,
							1, 0);
	assert(message);
	r = mbim_message_set_arguments(message, "uuuuuuuussss",
					1, 1, 1, 2, 0x3f, 0x3, 1, 16,
					NULL, "359336050018717",
					"FIH7160_V1.1_MODEM_01.1408.07",
					"XMM7160_V1.1_MBIM_GNSS_NAND_RE");
	assert(r);

	_mbim_message_set_tid(message, msg_data->tid);
	assert(check_message(message, msg_data));
	mbim_message_unref(message);
}

static void build_device_caps_query(const void *data)
{
	const struct message_data *msg_data = data;
	struct mbim_message *message;

	message = mbim_message_new(mbim_uuid_basic_connect, 1,
						MBIM_COMMAND_TYPE_QUERY);
	assert(message);
	assert(mbim_message_set_arguments(message, ""));

	_mbim_message_set_tid(message, msg_data->tid);
	assert(check_message(message, msg_data));
	mbim_message_unref(message);
}

static void parse_subscriber_ready_status(const void *data)
{
	struct mbim_message *msg = build_message(data);
	uint32_t ready_state;
	char *imsi;
	char *iccid;
	uint32_t ready_info;
	uint32_t n_phone_numbers;
	char *phone_number;
	struct mbim_message_iter array;
	bool r;

	r = mbim_message_get_arguments(msg, "ussuas",
					&ready_state, &imsi, &iccid,
					&ready_info,
					&n_phone_numbers, &array);
	assert(r);

	assert(ready_state == 1);
	assert(imsi);
	assert(!strcmp(imsi, "310410227923374"));
	assert(iccid);
	assert(!strcmp(iccid, "89014104212279233747"));
	assert(ready_info == 0);

	assert(n_phone_numbers == 1);
	assert(mbim_message_iter_next_entry(&array, &phone_number));

	assert(phone_number);
	assert(!strcmp(phone_number, "15124310596"));
	l_free(phone_number);

	assert(!mbim_message_iter_next_entry(&array, &phone_number));

	l_free(imsi);
	l_free(iccid);
	mbim_message_unref(msg);
}

static void build_subscriber_ready_status(const void *data)
{
	const struct message_data *msg_data = data;
	bool r;
	struct mbim_message *message;

	message = _mbim_message_new_command_done(mbim_uuid_basic_connect,
							2, 0);
	assert(message);

	r = mbim_message_set_arguments(message, "ussuas",
			1, "310410227923374", "89014104212279233747", 0,
			1, "15124310596");
	assert(r);

	_mbim_message_set_tid(message, msg_data->tid);
	assert(check_message(message, msg_data));
	mbim_message_unref(message);
}

static void parse_phonebook_read(const void *data)
{
	struct mbim_message *msg = build_message(data);
	uint32_t n_items;
	struct mbim_message_iter array;
	uint32_t index;
	char *number;
	char *name;
	bool r;

	r = mbim_message_get_arguments(msg, "a(uss)", &n_items, &array);
	assert(r);

	assert(n_items == 1);
	assert(mbim_message_iter_next_entry(&array, &index, &number, &name));
	assert(index == 3);
	assert(number);
	assert(!strcmp(number, "921123456"));
	assert(name);
	assert(!strcmp(name, "TS"));
	l_free(number);
	l_free(name);

	assert(!mbim_message_iter_next_entry(&array, &index, &number, &name));
	mbim_message_unref(msg);
}

static void build_phonebook_read(const void *data)
{
	const struct message_data *msg_data = data;
	bool r;
	struct mbim_message *message;

	message = _mbim_message_new_command_done(mbim_uuid_phonebook, 2, 0);
	assert(message);

	r = mbim_message_set_arguments(message, "a(uss)", 1,
			3, "921123456", "TS");
	assert(r);

	_mbim_message_set_tid(message, msg_data->tid);
	assert(check_message(message, msg_data));
	mbim_message_unref(message);
}

static void parse_sms_read_all(const void *data)
{
	struct mbim_message *msg = build_message(data);
	uint32_t format;
	uint32_t n_sms;
	struct mbim_message_iter array;
	struct mbim_message_iter bytes;
	uint32_t index;
	uint32_t status;
	uint32_t pdu_len;
	uint8_t pdu[176];
	uint32_t i = 0;
	uint32_t j = 0;

	assert(mbim_message_get_arguments(msg, "ua(uuay)",
						&format, &n_sms, &array));

	assert(format == 0);

	i = 0;

	while (mbim_message_iter_next_entry(&array, &index, &status,
							&pdu_len, &bytes)) {
		i += 1;
		j = 0;

		while (mbim_message_iter_next_entry(&bytes, pdu + j))
			j += 1;

		assert(j == pdu_len);
	}

	assert(i == n_sms);

	mbim_message_unref(msg);
}

static const uint8_t sms_pdu[] = {
	0x00, 0x01, 0x00, 0x0B, 0x91, 0x99, 0x99, 0x99, 0x99, 0x99,
	0xF9, 0x00, 0x00, 0x06, 0xC6, 0xF7, 0x5B, 0x1C, 0x96, 0x03
};

static void parse_sms_send(const void *data)
{
	struct mbim_message *msg = build_message(data);
	uint32_t format;
	uint32_t pdu_len;
	struct mbim_message_iter pdu;
	struct mbim_message_iter databuf;
	uint8_t buf[182];
	uint8_t b;
	int i;

	assert(mbim_message_get_arguments(msg, "ud", &format, "ay", &databuf));

	assert(format == 0);

	assert(mbim_message_iter_next_entry(&databuf, &pdu_len, &pdu));
	assert(pdu_len == 20);

	i = 0;
	while (mbim_message_iter_next_entry(&pdu, &b))
		buf[i++] = b;

	assert(i == 20);
	assert(!memcmp(buf, sms_pdu, i));

	mbim_message_unref(msg);
}

static void build_sms_send(const void *data)
{
	const struct message_data *msg_data = data;
	struct mbim_message *message;

	message = mbim_message_new(mbim_uuid_sms,
					MBIM_CID_SMS_SEND,
					MBIM_COMMAND_TYPE_SET);
	assert(message);
	assert(mbim_message_set_arguments(message, "ud", 0,
					"ay", sizeof(sms_pdu), sms_pdu));
	_mbim_message_set_tid(message, msg_data->tid);
	assert(check_message(message, msg_data));
	mbim_message_unref(message);
}

static void build_device_subscribe_list(const void *data)
{
	const struct message_data *msg_data = data;
	struct mbim_message *message;

	message = mbim_message_new(mbim_uuid_basic_connect,
					MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST,
					MBIM_COMMAND_TYPE_SET);

	assert(message);
	assert(mbim_message_set_arguments(message, "av", 2,
					"16yuuuuuu", mbim_uuid_basic_connect, 5,
					MBIM_CID_SIGNAL_STATE,
					MBIM_CID_REGISTER_STATE,
					MBIM_CID_CONNECT,
					MBIM_CID_SUBSCRIBER_READY_STATUS,
					MBIM_CID_PACKET_SERVICE,
					"16yuuu", mbim_uuid_sms, 2,
					MBIM_CID_SMS_READ,
					MBIM_CID_SMS_MESSAGE_STORE_STATUS));

	_mbim_message_set_tid(message, msg_data->tid);
	assert(check_message(message, msg_data));
	mbim_message_unref(message);
}

static void parse_packet_service_notify(const void *data)
{
	struct mbim_message *msg = build_message(data);
	uint32_t nw_error;
	uint32_t state;
	uint32_t data_class;
	uint64_t uplink;
	uint64_t downlink;

	assert(mbim_message_get_arguments(msg, "uuutt",
						&nw_error, &state, &data_class,
						&uplink, &downlink));

	assert(nw_error == 0);
	assert(state == 2);
	assert(data_class == MBIM_DATA_CLASS_LTE);
	assert(uplink == 50000000);
	assert(downlink == 100000000);

	mbim_message_unref(msg);
}

int main(int argc, char *argv[])
{
	l_test_init(&argc, &argv);

	l_test_add("Device Caps (parse)",
			parse_device_caps, &message_data_device_caps);
	l_test_add("Device Caps (build)",
			build_device_caps, &message_data_device_caps);

	l_test_add("Device Caps Query (build)", build_device_caps_query,
					&message_data_device_caps_query);

	l_test_add("Subscriber Ready Status (parse)",
			parse_subscriber_ready_status,
			&message_data_subscriber_ready_status);
	l_test_add("Subscriber Ready Status (build)",
			build_subscriber_ready_status,
			&message_data_subscriber_ready_status);

	l_test_add("Phonebook Read (parse)", parse_phonebook_read,
			&message_data_phonebook_read);
	l_test_add("Phonebook Read (build)", build_phonebook_read,
			&message_data_phonebook_read);

	l_test_add("SMS Read All [Empty] (parse)", parse_sms_read_all,
			&message_data_sms_read_all_empty);
	l_test_add("SMS Read All [1] (parse)", parse_sms_read_all,
			&message_data_sms_read_all);

	l_test_add("SMS Send (parse)", parse_sms_send,
			&message_data_sms_send);
	l_test_add("SMS Send (build)", build_sms_send,
			&message_data_sms_send);

	l_test_add("Device Subscribe List (build)", build_device_subscribe_list,
			&message_data_device_subscribe_list);

	l_test_add("Packet Service Notify (parse)", parse_packet_service_notify,
			&message_data_packet_service_notify);

	return l_test_run();
}
