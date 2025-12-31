#include "tlv.h"
#include <stdio.h>

/* Various log entry types */
enum my_log_entry_types {
	MY_LOG_ENTRY_TYPE1,
	MY_LOG_ENTRY_TYPE2
};

/* Various log entry structures */
struct my_log_entry {
	const char *label;

	uint16_t some_u16_data;

	uint32_t timestamp;
};

struct my_log_entry2 {
	uint16_t some_u16_data;
};

int main()
{
	struct tlv_buf   jour;
	struct tlv_entry entry;

	/* Prepare user defined log entries */
	struct my_log_entry mle_1;
	struct my_log_entry mle_2;

	struct my_log_entry2 mle2_1;
	struct my_log_entry2 mle2_2;

	uint8_t journal_data[80u];
	tlv_buf_init(&jour, journal_data, 80u);

	mle_1.label = "my log entry";
	mle_1.some_u16_data = 1337;
	mle_1.timestamp = 999;

	mle2_1.some_u16_data = 0xCAFE;

	tlv_entry_init(&entry, MY_LOG_ENTRY_TYPE2, sizeof(mle2_2), &mle2_1);
	tlv_buf_push(&jour, &entry);

	tlv_entry_init(&entry, MY_LOG_ENTRY_TYPE1, sizeof(mle_1), &mle_1);
	tlv_buf_push(&jour, &entry);

	if ((tlv_buf_pop(&jour, &entry) == true) &&
	     entry.tag == MY_LOG_ENTRY_TYPE2) {
		memcpy(&mle2_2, entry.val, entry.len);

		printf("Type 2 entry: %X\n", mle2_2.some_u16_data);
	}

	if ((tlv_buf_pop(&jour, &entry) == true) &&
	     entry.tag == MY_LOG_ENTRY_TYPE1) {
		memcpy(&mle_2, entry.val, entry.len);

		printf("[%u] %s: %u\n", mle_2.timestamp, mle_2.label,
			mle_2.some_u16_data);
	}

	if (tlv_buf_pop(&jour, &entry) == false) {
		printf("nah\n");
	}

	return 0;
}
