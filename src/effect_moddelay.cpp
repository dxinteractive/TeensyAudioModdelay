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

#include "effect_moddelay.h"

void AudioEffectModDelay::update(void)
{
    Serial.print("maxblocks:");
    Serial.print(maxblocks);
    Serial.print("\t");

    audio_block_t *modulation, *output;
    uint32_t head, tail, count, index, offset;
    int32_t x;
    uint16_t xa, shift, i;
    int16_t ya, yb;

    // grab incoming data and put it into the queue
    head = headindex;
    tail = tailindex;
    if (++head >= MODDELAY_QUEUE_SIZE) head = 0;
    if (head == tail) {
        release(queue[tail]);
        if (++tail >= MODDELAY_QUEUE_SIZE) tail = 0;
    }
    queue[head] = receiveReadOnly(0);
    modulation = receiveReadOnly(1);
    if(!modulation) return;

    Serial.print("write[");
    Serial.print(head);
    Serial.print("]\t");

    headindex = head;

    // discard unneeded blocks from the queue
    if (head >= tail) {
        count = head - tail;
    } else {
        count = MODDELAY_QUEUE_SIZE + head - tail;
    }
    if (count > maxblocks) {
        count -= maxblocks;
        do {
            Serial.print("release[");
            Serial.print(tail);
            Serial.print("]\t");

            release(queue[tail]);
            queue[tail] = NULL;
            if (++tail >= MODDELAY_QUEUE_SIZE) tail = 0;
        } while (--count > 0);
    }
    tailindex = tail;

    // transmit the delayed outputs using queue data
    output = allocate();
    if(!output) {
        release(modulation);
        return;
    }

    shift = 10;

    for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // lerp from http://coranac.com/tonc/text/fixed.htm
        // x is shifted position with modulation
        x = ((position + i) << shift) + modulation->data[i];
        if(x < 0) {
            x = 0;
        } 
        // if x is bigger than the biggest thing then x = the biggest thing
        xa = x >> shift;
        index = xa / AUDIO_BLOCK_SAMPLES;
        offset = xa % AUDIO_BLOCK_SAMPLES;

        if (head >= index) {
            index = head - index;
        } else {
            index = MODDELAY_QUEUE_SIZE + head - index;
        }

        if(i == 0) {
            Serial.print("position:");
            Serial.print(position);
            Serial.print("\t");
            Serial.print("x:");
            Serial.print(x);
            Serial.print("\t");
            Serial.print("xa:");
            Serial.print(xa);
            Serial.print("\t");
            Serial.print("index:");
            Serial.print(index);
            Serial.print("\t");
            Serial.print("offset:");
            Serial.print(offset);
            Serial.print("\t");
        }

        if(!queue[index]) {
            output->data[i] = 0;
            continue;
        }

        // uninterpolated
        //output->data[i] = queue[index]->data[offset];

        ya = queue[index]->data[offset];
        if(offset == AUDIO_BLOCK_SAMPLES - 1) {
            yb = queue[index + 1]->data[0];
        } else {
            yb = queue[index]->data[offset + 1];
        }

        output->data[i] = ya + ((yb - ya) * (x - (xa << shift)) >> shift);
    }

    transmit(output);
    release(output);
    release(modulation);

    /*
    if (offset == 0) {
        // delay falls on the block boundary
        if (queue[index]) {
            transmit(queue[index]);
        }
    } else {

        // delay requires grabbing data from 2 blocks
        output = allocate();
        if (output) {
            dst = output->data;
            if (index > 0) {
                prev = index - 1;
            } else {
                prev = MODDELAY_QUEUE_SIZE-1;
            }
            if (queue[prev]) {
                end = queue[prev]->data + AUDIO_BLOCK_SAMPLES;
                src = end - offset;
                while (src < end) {
                    *dst++ = *src++; // TODO: optimize
                }
            } else {
                end = dst + offset;
                while (dst < end) {
                    *dst++ = 0;
                }
            }
            end = output->data + AUDIO_BLOCK_SAMPLES;
            if (queue[index]) {
                src = queue[index]->data;
                while (dst < end) {
                    *dst++ = *src++; // TODO: optimize
                }
            } else {
                while (dst < end) {
                    *dst++ = 0;
                }
            }
            transmit(output);
            release(output);
        }
    }*/

   
}

