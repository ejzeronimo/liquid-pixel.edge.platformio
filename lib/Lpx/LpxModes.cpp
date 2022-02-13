#include "LpxModes.h"

CLpxModes LpxModes;

void CLpxModes::off(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));

        if (delayCallback(-1)) {
            target->showStrand();
            target->oneTimePerStrand = true;
        } else {
            return;
        }
    }
}

void CLpxModes::solid(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        fill_solid(target->strand, target->strand_length, command.primary);

        if (delayCallback(-1)) {
            target->showStrand();
            target->oneTimePerStrand = true;
        } else {
            return;
        }
    }
}

void CLpxModes::chroma(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    for (int i = 0; i < 255; i++) {
        fill_rainbow(target->strand, target->strand_length, i);

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }
    }
}

void CLpxModes::flash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    fill_solid(target->strand, target->strand_length, command.primary);

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    if (!delayCallback(command.delayMs)) {
        return;
    }

    fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    if (!delayCallback(command.delayMs)) {
        return;
    }
}

void CLpxModes::randomFlash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    fill_solid(target->strand, target->strand_length, CRGB(random16(255), random16(255), random16(255)));

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    if (!delayCallback(command.delayMs)) {
        return;
    }
    fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    if (!delayCallback(command.delayMs)) {
        return;
    }
}

void CLpxModes::suddenFlash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        fill_solid(target->strand, target->strand_length, command.primary);

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }

        fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));

        if (delayCallback(-1)) {
            target->showStrand();
            target->oneTimePerStrand = true;
        } else {
            return;
        }
    }
}

void CLpxModes::lightning(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        fill_solid(target->strand, target->strand_length, command.primary);
        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(round(command.delayMs / 6))) {
            return;
        }

        fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));
        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(round(command.delayMs / 4))) {
            return;
        }

        fill_solid(target->strand, target->strand_length, command.primary);
        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(round(command.delayMs / 2))) {
            return;
        }

        fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));
        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        target->oneTimePerStrand = true;
    }
}

void CLpxModes::cloudyBlobs(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    // choose how many blobs we want
    int blobCount = random16(3, target->strand_length / 7);
    int blobPos[blobCount];

    // make the blobs
    for (int i = 0; i < blobCount; i++) {
        blobPos[i] = random16(target->strand_length);
        fill_solid(target->strand + blobPos[i] - 1, 3, command.primary);
    }

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    // dim out
    for (int br = 0; br < random16(30, 50); br++) {
        nscale8(target->strand, target->strand_length, 255 - br);

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }
    }
}

void CLpxModes::twinkle(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int blobCount = random16(3, target->strand_length / 5);
    int blobPos[blobCount];

    for (int i = 0; i < blobCount; i++) {
        blobPos[i] = random16(target->strand_length);
        fill_solid(target->strand + blobPos[i], 1, command.primary);
    }

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    for (int br = 0; br < random16(30, 50); br++) {
        nscale8(target->strand, target->strand_length, 255 - br);

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }
    }
}

void CLpxModes::randomTwinkle(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int blobCount = random16(3, target->strand_length / 5);
    int blobPos[blobCount];

    for (int i = 0; i < blobCount; i++) {
        blobPos[i] = random16(target->strand_length);
        fill_solid(target->strand + blobPos[i], 1, CRGB(random16(255), random16(255), random16(255)));
    }

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    for (int br = 0; br < random16(30, 50); br++) {
        nscale8(target->strand, target->strand_length, 255 - br);

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }
    }
}

void CLpxModes::theaterChase(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int i = 0;

    if (target->strand[0].r != 0 || target->strand[0].g != 0 || target->strand[0].b != 0) {
        i = 1;
    }

    fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));

    for (i; i < target->strand_length; i += 2) {
        fill_solid(target->strand + i, 1, command.primary);
    }

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    if (!delayCallback(command.delayMs)) {
        return;
    }
}

void CLpxModes::dualTheaterChase(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int i = 0;

    if (target->strand[0].g != 255) {
        i = 1;
    }

    fill_solid(target->strand, target->strand_length, command.primary);

    for (i; i < target->strand_length; i += 2) {
        fill_solid(target->strand + i, 1, CRGB(255, 0, 0));
    }

    if (delayCallback(-1)) {
        target->showStrand();
    } else {
        return;
    }

    if (!delayCallback(command.delayMs)) {
        return;
    }
}

void CLpxModes::rain(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int trailLen = 1; // round(pixelLen/delayMs);
    int offset = 4 + trailLen;
    int k;
    for (int q = 0; q < offset; q++) {
        for (int i = 0; i < (target->strand_length + trailLen + 1); i = i + offset) {
            if ((i + q) >= 0 && (i + q) < target->strand_length) {
                target->strand[i + q] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
            }

            for (int w = 0; w < trailLen; w++) {
                k = w + 1;
                int rf = round(command.primary[0] / (k * k));
                int gf = round(command.primary[1] / (k * k));
                int bf = round(command.primary[2] / (k * k));
                int curpos = i + q - k;

                if (curpos >= 0 && curpos < target->strand_length) {
                    target->strand[curpos] = CRGB(rf, gf, bf);
                }

                if ((i + q - (trailLen + 1)) >= 0 && (i + q - (trailLen + 1)) < target->strand_length) {
                    target->strand[i + q - (trailLen + 1)] = CRGB(0, 0, 0);
                }

                if (!delayCallback(2)) {
                    return;
                }
            }
        }

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }
    }
}

void CLpxModes::fadeIn(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        CRGB temp[target->strand_length];

        for (int i = 0; i < target->strand_length; i++) {
            temp[i] = command.primary;
        }

        for (int i = 0; i < 255; i++) {
            nblend(target->strand, temp, target->strand_length, 25);

            if (!delayCallback(command.delayMs)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        target->oneTimePerStrand = true;
    }
}

void CLpxModes::fadeOut(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        for (int i = 0; i < 255; i++) {
            fadeToBlackBy(target->strand, target->strand_length, 25);

            if (!delayCallback(command.delayMs)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        target->oneTimePerStrand = true;
    }
}

void CLpxModes::breath(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    for (int i = 0; i < 255; i += 5) {
        fill_solid(target->strand, target->strand_length, command.primary);
        fadeLightBy(target->strand, target->strand_length, 255 - i);

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }
    }

    for (int i = 0; i < 255; i += 5) {
        fill_solid(target->strand, target->strand_length, command.primary);
        fadeLightBy(target->strand, target->strand_length, i);

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }
    }
}

void CLpxModes::randomBreath(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    CRGB rand = CRGB(random16(255), random16(255), random16(255));

    for (int i = 0; i < 255; i += 5) {
        fill_solid(target->strand, target->strand_length, rand);
        fadeLightBy(target->strand, target->strand_length, 255 - i);

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }
    }

    for (int i = 0; i < 255; i += 5) {
        fill_solid(target->strand, target->strand_length, rand);
        fadeLightBy(target->strand, target->strand_length, i);

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if (delayCallback(-1)) {
            target->showStrand();
        } else {
            return;
        }
    }
}

void CLpxModes::fallingStars(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int trailLen = 10;

    for (int pos = 0; pos < (target->strand_length + trailLen); pos++) {
        if (pos < target->strand_length) {
            target->strand[pos] = command.primary;
        }

        for (int i = 1; i < trailLen; i++) {
            int rf = round(command.primary.r / (i * i));
            int gf = round(command.primary.g / (i * i));
            int bf = round(command.primary.b / (i * i));

            int curpos = pos - i;

            if (curpos >= 0 && curpos < target->strand_length) {
                target->strand[curpos] = CRGB(rf, gf, bf);
            }

            if (!delayCallback(2)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if ((pos - trailLen) >= 0 && (pos - trailLen) < target->strand_length) {
            target->strand[pos - trailLen] = CRGB(0, 0, 0);
        }
    }
}

void CLpxModes::pong(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int trailLen = 6;

    for (int pos = 0; pos < (target->strand_length); pos++) {
        if (pos < target->strand_length) {
            target->strand[pos] = command.primary;
        }

        for (int i = 1; i < trailLen; i++) {
            int rf = round(command.primary.r / i);
            int gf = round(command.primary.g / i);
            int bf = round(command.primary.b / i);
            int curpos = pos - i;
            if (curpos >= 0 && curpos < target->strand_length) {
                target->strand[curpos] = CRGB(rf, gf, bf);
            }

            if (!delayCallback(2)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if ((pos - trailLen) >= 0 && (pos - trailLen) < target->strand_length) {
            target->strand[pos - trailLen] = CRGB(0, 0, 0);
        }
    }

    for (int pos = target->strand_length; pos > 0; pos--) {
        if (pos < target->strand_length) {
            target->strand[pos] = command.primary;
        }

        for (int i = 1; i < trailLen; i++) {
            int rf = round(command.primary.r / i);
            int gf = round(command.primary.g / i);
            int bf = round(command.primary.b / i);
            int curpos = pos + i;
            if (curpos >= 0 && curpos < target->strand_length) {
                target->strand[curpos] = CRGB(rf, gf, bf);
            }

            if (!delayCallback(2)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if ((pos + trailLen) >= 0 && (pos + trailLen) < target->strand_length) {
            target->strand[pos + trailLen] = CRGB(0, 0, 0);
        }
    }
}

void CLpxModes::waterfall(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        int i = 0;

        while (i <= target->strand_length) {
            fill_solid(target->strand, i, command.primary);
            i++;

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }

            if (!delayCallback(command.delayMs)) {
                return;
            }
        }

        target->oneTimePerStrand = true;
    }
}

void CLpxModes::rainbowWaterfall(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    if (!target->oneTimePerStrand) {
        int i = 0;

        while (i <= target->strand_length) {
            fill_rainbow(target->strand, i, 0);
            i++;

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }

            if (!delayCallback(command.delayMs)) {
                return;
            }
        }

        target->oneTimePerStrand = true;
    }
}

void CLpxModes::waves(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int trailLen = 10;
    int swellLen = 2;

    for (int pos = 0; pos < (target->strand_length + trailLen); pos++) {
        for (int i = 1; i < swellLen; i++) {
            int rs = round(command.primary.r / (i * i));
            int gs = round(command.primary.g / (i * i));
            int bs = round(command.primary.b / (i * i));

            int prepos = pos + i;

            if (prepos >= 0 && prepos < target->strand_length) {
                target->strand[prepos] = CRGB(rs, gs, bs);
            }

            if (!delayCallback(2)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        if (pos < target->strand_length) {
            target->strand[pos] = command.primary;
        }

        for (int i = 1; i < trailLen; i++) {
            int rf = round(command.primary.r / (float)((i * i) / 4));
            int gf = round(command.primary.g / (float)((i * i) / 4));
            int bf = round(command.primary.b / (float)((i * i) / 4));

            int curpos = pos - i;

            if (curpos >= 0 && curpos < target->strand_length) {
                target->strand[curpos] = CRGB(rf, gf, bf);
            }

            if (!delayCallback(2)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }

        if ((pos - (4 * trailLen)) >= 0 && (pos - (4 * trailLen)) < target->strand_length) {
            target->strand[pos - (4 * trailLen)] = CRGB(0, 0, 0);
        }
    }
}

void CLpxModes::levels(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback) {
    int trailLen = target->strand_length;
    int maxheight = round(target->strand_length / random16(1, 5));

    for (int pos = -1; pos <= maxheight; pos++) {
        if (pos < target->strand_length) {
            target->strand[pos] = command.primary;
        }

        for (int i = 1; i < trailLen; i++) {
            int rf = round(command.primary.r / i);
            int gf = round(command.primary.g / i);
            int bf = round(command.primary.b / i);
            int curpos = pos + i;

            if (curpos >= 0 && curpos < target->strand_length) {
                target->strand[curpos] = CRGB(rf, gf, bf);
            }

            if (!delayCallback(2)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }
    }

    for (int pos = maxheight; pos >= 0; pos--) {
        if (pos < target->strand_length) {
            target->strand[pos] = command.primary;
        }

        for (int i = 1; i < trailLen; i++) {
            int rf = round(command.primary.r / i);
            int gf = round(command.primary.g / i);
            int bf = round(command.primary.b / i);

            int curpos = pos + i;

            if (curpos >= 0 && curpos < target->strand_length) {
                target->strand[curpos] = CRGB(rf, gf, bf);
            }

            if (!delayCallback(2)) {
                return;
            }

            if (delayCallback(-1)) {
                target->showStrand();
            } else {
                return;
            }
        }

        if (!delayCallback(command.delayMs)) {
            return;
        }
    }
}
