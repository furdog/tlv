#include <stdio.h>

#define TLV_IMPLEMENTATION
#include "tlv.h"

/** Example widget types */
enum gui_widget_type {
	GUI_WIDGET_TYPE_BOX,
	GUI_WIDGET_TYPE_TEXT
};

/** Example widget box */
struct gui_widget_box {
	uint16_t x; /**< x position */
	uint16_t y; /**< y position */
	uint16_t w; /**< w width */
	uint16_t h; /**< h height */
};

/** Example widget text */
struct gui_widget_text {
	uint16_t x; /**< x position */
	uint16_t y; /**< y position */

	const char *text; /**< some text */
};

/** Pushes box widget into tlv buffer */
bool gui_widget_box_push(struct tlv_buf *self, struct gui_widget_box *w)
{
	struct tlv_entry entry;

	tlv_entry_init(&entry, GUI_WIDGET_TYPE_BOX,
		       sizeof(struct gui_widget_box), w);
	return tlv_buf_push(self, &entry);
}

/** Pushes text widget into tlv buffer */
bool gui_widget_text_push(struct tlv_buf *self, struct gui_widget_text *w)
{
	struct tlv_entry entry;

	tlv_entry_init(&entry, GUI_WIDGET_TYPE_TEXT,
		       sizeof(struct gui_widget_text), w);
	return tlv_buf_push(self, &entry);
}

/** Prints widgets */
void gui_widget_parse_print(struct tlv_buf *self, struct tlv_entry *entry)
{
	assert(self && entry);

	switch (entry->tag) {
	case GUI_WIDGET_TYPE_BOX: {
		struct gui_widget_box w;
		memcpy(&w, entry->val, entry->len);
		printf("box: x:%u, y:%u, w:%u, h:%u\n", w.x, w.y, w.w, w.h);
		break;
	}

	case GUI_WIDGET_TYPE_TEXT: {
		struct gui_widget_text w;
		memcpy(&w, entry->val, entry->len);
		printf("box: x:%u, y:%u, text:%s\n", w.x, w.y, w.text);
		break;
	}

	default:
		printf("Unknown widget type: %u\n", entry->tag);
		break;
	}
}

int main()
{
	struct tlv_entry entry;

	/* TLV header size && occupied memory counter. */
	size_t occupied_mem = 0u;

	/* Widgets */
	struct gui_widget_box  box  = {50, 50, 100, 100};
	struct gui_widget_text text = {55, 55, "Hello Box!"};

	/* Some random value */
	uint8_t some_val = 137;

	/* Create and initialize TLV buffer that will store widgets */
	struct tlv_buf widgets;
	uint8_t widget_data[80u];
	tlv_buf_init(&widgets, widget_data, 80u);
	assert(tlv_buf_get_free_mem_size(&widgets) == 80u);
	assert(tlv_buf_get_occupied_mem_size(&widgets) == 0u);

	/* Put some random value into widgets buffer (just for test) */
	tlv_entry_init(&entry, some_val, sizeof(some_val), &some_val);
	tlv_buf_push(&widgets, &entry);

	occupied_mem += TLV_TAGLEN_SIZE + sizeof(some_val);
	assert(tlv_buf_get_occupied_mem_size(&widgets) == occupied_mem);

	/* Put actual widgets */
	gui_widget_box_push(&widgets, &box);
	occupied_mem += TLV_TAGLEN_SIZE + sizeof(box);
	assert(tlv_buf_get_occupied_mem_size(&widgets) == occupied_mem);

	gui_widget_text_push(&widgets, &text);
	occupied_mem += TLV_TAGLEN_SIZE + sizeof(text);
	assert(tlv_buf_get_occupied_mem_size(&widgets) == occupied_mem);

	text.y += 16;
	text.text = "modified!";
	gui_widget_text_push(&widgets, &text);
	occupied_mem += TLV_TAGLEN_SIZE + sizeof(text);
	assert(tlv_buf_get_occupied_mem_size(&widgets) == occupied_mem);

	/* Pop widgets and parse and print their values */
	while (tlv_buf_pop(&widgets, &entry) == true) {
		gui_widget_parse_print(&widgets, &entry);
	}

	/* Try again */
	tlv_buf_reset_head(&widgets);
	while (tlv_buf_pop(&widgets, &entry) == true) {
		gui_widget_parse_print(&widgets, &entry);
	}

	/* No widgets must be stored at this point */
	if (tlv_buf_pop(&widgets, &entry) == false) {
		printf("No widgets stored!\n");
	}

	assert(tlv_buf_get_occupied_mem_size(&widgets) == occupied_mem);

	tlv_buf_reset(&widgets);
	assert(tlv_buf_get_occupied_mem_size(&widgets) == 0u);

	return 0;
}
