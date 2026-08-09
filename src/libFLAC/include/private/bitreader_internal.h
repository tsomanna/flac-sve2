/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2000-2009  Josh Coalson
 * Copyright (C) 2011-2025  Xiph.Org Foundation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLAC__PRIVATE__BITREADER_INTERNAL_H
#define FLAC__PRIVATE__BITREADER_INTERNAL_H

#include "FLAC/ordinals.h"
#include "bitreader.h"
#include "bitmath.h"
#include "share/endswap.h"

/* Things should be fastest when this matches the machine word size */
/* WATCHOUT: if you change this you must also change the following #defines down to COUNT_ZERO_MSBS2 below to match */
/* WATCHOUT: there are a few places where the code will not work unless brword is >= 32 bits wide */
/*           also, some sections currently only have fast versions for 4 or 8 bytes per word */

#if (ENABLE_64_BIT_WORDS == 0)

typedef FLAC__uint32 brword;
#define FLAC__BYTES_PER_WORD 4		/* sizeof brword */
#define FLAC__BITS_PER_WORD 32
#define FLAC__WORD_ALL_ONES ((FLAC__uint32)0xffffffff)
/* SWAP_BE_WORD_TO_HOST swaps bytes in a brword (which is always big-endian) if necessary to match host byte order */
#if WORDS_BIGENDIAN
#define SWAP_BE_WORD_TO_HOST(x) (x)
#else
#define SWAP_BE_WORD_TO_HOST(x) ENDSWAP_32(x)
#endif
/* counts the # of zero MSBs in a word */
#define COUNT_ZERO_MSBS(word) FLAC__clz_uint32(word)
#define COUNT_ZERO_MSBS2(word) FLAC__clz2_uint32(word)

#else

typedef FLAC__uint64 brword;
#define FLAC__BYTES_PER_WORD 8		/* sizeof brword */
#define FLAC__BITS_PER_WORD 64
#define FLAC__WORD_ALL_ONES ((FLAC__uint64)FLAC__U64L(0xffffffffffffffff))
/* SWAP_BE_WORD_TO_HOST swaps bytes in a brword (which is always big-endian) if necessary to match host byte order */
#if WORDS_BIGENDIAN
#define SWAP_BE_WORD_TO_HOST(x) (x)
#else
#define SWAP_BE_WORD_TO_HOST(x) ENDSWAP_64(x)
#endif
/* counts the # of zero MSBs in a word */
#define COUNT_ZERO_MSBS(word) FLAC__clz_uint64(word)
#define COUNT_ZERO_MSBS2(word) FLAC__clz2_uint64(word)

#endif

struct FLAC__BitReader {
	/* any partially-consumed word at the head will stay right-justified as bits are consumed from the left */
	/* any incomplete word at the tail will be left-justified, and bytes from the read callback are added on the right */
	brword *buffer;
	uint32_t capacity; /* in words */
	uint32_t words; /* # of completed words in buffer */
	uint32_t bytes; /* # of bytes in incomplete word at buffer[words] */
	uint32_t consumed_words; /* #words ... */
	uint32_t consumed_bits; /* ... + (#bits of head word) already consumed from the front of buffer */
	uint32_t read_crc16; /* the running frame CRC */
	uint32_t crc16_offset; /* the number of words in the current buffer that should not be CRC'd */
	uint32_t crc16_align; /* the number of bits in the current consumed word that should not be CRC'd */
	FLAC__bool read_limit_set; /* whether reads are limited */
	uint32_t read_limit; /* the remaining size of what can be read */
	uint32_t last_seen_framesync; /* the location of the last seen framesync, if it is in the buffer, in bits from front of buffer */
	FLAC__BitReaderReadCallback read_callback;
	void *client_data;
};

#endif /* FLAC__PRIVATE__BITREADER_INTERNAL_H */