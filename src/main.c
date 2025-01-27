// Copyright (c) Kuba Szczodrzyński 2025-1-25.

#include "include.h"

int create_can() {
	int sfd = (int)socket(AF_CAN, SOCK_RAW, CAN_RAW);
	if (sfd == -1)
		SOCK_ERROR("socket()", return -1);

	struct ifreq ifr = {
		.ifr_name = "can0",
	};
	if (ioctl(sfd, SIOCGIFINDEX, &ifr) != 0)
		SOCK_ERROR("ioctl(SIOCGIFINDEX)", return -1);

	struct sockaddr_can addr = {
		.can_family	 = AF_CAN,
		.can_ifindex = ifr.ifr_ifindex,
	};
	if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
		SOCK_ERROR("bind()", return -1);

	return sfd;
}

int main() {
	int sfd = -1;
	if (db_connect(DATABASE_FILE) == NULL)
		goto error;

	// process unsaved trips
	db_process_trips();

	sfd = create_can();
	if (sfd == -1)
		goto error;
	LT_I("Socket opened");

	record_t record = {0};
	record_reset(&record);

	int counter				  = 0;
	unsigned int engine_speed = 0;
	while (1) {
		struct can_frame can_frame;
		ssize_t len = read(sfd, &can_frame, sizeof(can_frame));
		if (len <= 0)
			return 1;

		frame_t frame;
		if (!frame_parse(&can_frame, &frame))
			continue;
		// frame_print(&frame);

		if (frame.type == FRAME_BSI_FAST) {
			if ((bool)engine_speed != (bool)frame.bsi_fast.engine_speed) {
				// engine starts/stops - save and reset the current record
				db_save_record(&record);
				record_print(&record);
				record_reset(&record);
				// save latest dist/fuel readings
				unsigned int dist_raw = frame.bsi_fast.dist * 10;
				unsigned int fuel_raw = frame.bsi_fast.fuel * 80;
				record.dist_last	  = dist_raw;
				record.fuel_last	  = fuel_raw;
			}
			engine_speed = frame.bsi_fast.engine_speed;
		}

		// avoid processing records if the engine is not running
		if (engine_speed == 0)
			continue;
		// otherwise aggregate frame data into the current record
		record_append(&record, &frame);

		if ((record.end.time - record.start.time) >= 60 * 1000) {
			// save and reset records every 1 min
			db_save_record(&record);
			record_print(&record);
			record_reset(&record);
		}

		if ((counter++ % 10) == 0)
			record_print(&record);
	}

	db_process_trips();

	return 0;

error:
	db_close();
	close(sfd);
	return 1;
}
