/*
 * Modulated delay for Teensy 3.X audio
 *
 * Copyright (c) 2017 Damien Clarke, http://damienclarke.me
 * Based heavily on effect_delay by Paul Stoffregen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef effect_moddelay_h_
#define effect_moddelay_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"

#define MODDELAY_QUEUE_SIZE  20 // temporarily changed from 117

class AudioEffectModDelay : public AudioStream
{
public:
    AudioEffectModDelay() : AudioStream(2, inputQueueArray) {
        headindex = 0;
        tailindex = 0;
        maxblocks = 0;
        memset(queue, 0, sizeof(queue));
    }
    void delay(float milliseconds) {
        if (milliseconds < 0.0) milliseconds = 0.0;
        uint32_t n = (milliseconds*(AUDIO_SAMPLE_RATE_EXACT/1000.0))+0.5;
        uint32_t nmax = AUDIO_BLOCK_SAMPLES * (MODDELAY_QUEUE_SIZE-1);
        if (n > nmax) n = nmax;
        uint32_t blks = (n + (AUDIO_BLOCK_SAMPLES-1)) / AUDIO_BLOCK_SAMPLES + 1;
        position = n;
        if (blks > maxblocks) maxblocks = blks;

        // temporary
        maxblocks = 20;
    }
    virtual void update(void);
private:
    uint8_t headindex;    // head index (incoming) data in quueu
    uint8_t tailindex;    // tail index (outgoing) data from queue
    uint8_t maxblocks;    // number of blocks needed in queue
    uint16_t position;    // # of sample delay
    audio_block_t *queue[MODDELAY_QUEUE_SIZE];
    audio_block_t *inputQueueArray[2];
};

#endif