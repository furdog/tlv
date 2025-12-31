/**
 * @file tlv.h
 * @brief Type-Length-Value (TLV) Data Serializer (Hardware-Agnostic)
 *
 * This file contains the software implementation of the TLV logic.
 * The design is hardware-agnostic, requiring an external adaptation layer
 * for hardware interaction.
 *
 * **Conventions:**
 * C89, Linux kernel style, MISRA, rule of 10, No hardware specific code,
 * only generic C and some binding layer. Be extra specific about types.
 *
 * Scientific units where posible at end of the names, for example:
 * - timer_10s (timer_10s has a resolution of 10s per bit)
 * - power_150w (power 150W per bit or 0.15kw per bit)
 *
 * Keep variables without units if they're unknown or not specified or hard
 * to define with short notation.
 *
 * ```LICENSE
 * Copyright (c) 2025 furdog <https://github.com/furdog>
 *
 * SPDX-License-Identifier: 0BSD
 * ```
 *
 * Be free, be wise and take care of yourself!
 * With best wishes and respect, furdog
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/** Entry which used to store data in TLV buffer. Not stored explicitly,
 *  but used by other methods. */ 
struct tlv_entry {
	uint8_t tag; /**< User specified data type */
	uint8_t len; /**< Entry Data length (255 bytes max) */
	void   *val; /**< User specified data */
};

/** Buffer data structure used to store TLV entries */ 
struct tlv_buf {
	uint8_t *_mem; /**< Pointer to buffer memory */ 
	size_t   _cap; /**< Memory capacity */

	size_t _head_idx; /**< Increments on pop  */
	size_t _tail_idx; /**< Increments on push */
};

/** Initialize TLV buffer. Provide pointer to uint8_t memory to be
 *  used by TLV entries and buffer capacity in bytes */
void tlv_buf_init(struct tlv_buf *self, uint8_t *mem, const size_t cap)
{
	self->_mem = mem;
	self->_cap = cap;

	self->_head_idx = 0u;
	self->_tail_idx = 0u;
}

/** Initialize TLV entry to be used by PUSH operation */
void tlv_entry_init(struct tlv_entry *self,
		    const uint8_t tag, const uint8_t len, void *val)
{
	self->tag = tag;
	self->len = len;
	self->val = val;
}

/** Push(FIFO) tag, len and val to TLV buffer.
 *  Returns (bool) true on success */
bool tlv_buf_push(struct tlv_buf *self, struct tlv_entry *entry)
{
	bool result = false;

	size_t header_size = sizeof(entry->tag) + sizeof(entry->len);

	size_t entry_size   = header_size + entry->len;
	size_t new_tail_idx = self->_tail_idx + entry_size;

	if (new_tail_idx < self->_cap) {
		self->_mem[self->_tail_idx] = entry->tag;
		self->_tail_idx += sizeof(entry->tag);

		self->_mem[self->_tail_idx] = entry->len;
		self->_tail_idx += sizeof(entry->len);

		(void)memcpy(&self->_mem[self->_tail_idx], entry->val,
			     entry->len);
		self->_tail_idx = new_tail_idx;

		result = true;
	}

	return result;
}

/** Pop(FIFO) entry from TLV buffer, which contains tag, len and val.
 *  Returns (bool) true on success */
bool tlv_buf_pop(struct tlv_buf *self, struct tlv_entry *entry)
{
	bool result = false;

	size_t header_size = sizeof(entry->tag) + sizeof(entry->len);

	if ((self->_head_idx + header_size) <= self->_tail_idx) {
		entry->tag = self->_mem[self->_head_idx];
		self->_head_idx += sizeof(entry->tag);

		entry->len = self->_mem[self->_head_idx];
		self->_head_idx += sizeof(entry->len);

		entry->val = &self->_mem[self->_head_idx];
		self->_head_idx += entry->len;

		result = true;
	}

	return result;
}

/** Empties TLV buffer */
void tlv_buf_flush(struct tlv_buf *self)
{
	self->_head_idx = 0u;
	self->_tail_idx = 0u;
}
