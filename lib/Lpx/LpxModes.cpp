#include "LpxModes.h"

CLpxModes LpxModes;
bool ONE_TIME_PER_COMMAND;

boolean CLpxModes::smartDelay(int ms, CLpxStrip t)
{
  long startMs = millis(); //get the time
  while (millis() < (startMs + ms))
  {
    delayMicroseconds(10); //do nothing?

    //yield();

    // TaskHandle_t h = (TaskHandle_t)xSemaphoreGetMutexHolder(t.strandSem);
    // if (!(h == t.taskHandle || h == 0x0))
    // {
    //   Serial.println("here");
    //   printf("%p\n", h);

    //   return false; //back out to get new command
    // }

    // if(t.ownSem)
    // {
    //   Serial.println("here");
    //   return false;
    // }
  }
  //delay(ms);
  return true;
}

//NOTE: OFF
void CLpxModes::off(CLpxStrip t)
{
  fill_solid(t.strand, t.strand_length, CRGB(0, 0, 0));
  t.showStrand();
}

//NOTE: SOLID
void CLpxModes::solid(CLpxStrip t, CLpxCommand c)
{
  fill_solid(t.strand, t.strand_length, CRGB(c.primary[0], c.primary[1], c.primary[2]));
  t.showStrand();
}

//NOTE: RANDOM CLOUDY BLOBS
void CLpxModes::randomCloudyBlobs(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int blobCount = random16(3, target.strand_length / 7);
  int blobPos[blobCount];

  for (int i = 0; i < blobCount; i++)
  {
    blobPos[i] = random16(target.strand_length);
    fill_solid(target.strand + blobPos[i] - 1, 3, CRGB(r, g, b));
  }

  target.showStrand();

  for (int br = 0; br < random16(30, 50); br++)
  {
    nscale8(target.strand, target.strand_length, 255 - br);

    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: FLASH
void CLpxModes::flash(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  fill_solid(target.strand, target.strand_length, CRGB(r, g, b));
  target.showStrand();
  LpxModes.smartDelay(delayMs, target);
  fill_solid(target.strand, target.strand_length, CRGB(0, 0, 0));
  target.showStrand();
  LpxModes.smartDelay(delayMs, target);
}

//NOTE: SWEEP
void CLpxModes::sweep(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  //depreciated, will get rid of!
}

//NOTE: TWINKLE
void CLpxModes::randomTwinkle(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int blobCount = random16(3, target.strand_length / 5);
  int blobPos[blobCount];

  for (int i = 0; i < blobCount; i++)
  {
    blobPos[i] = random16(target.strand_length);
    fill_solid(target.strand + blobPos[i], 1, CRGB(r, g, b));
  }

  target.showStrand();

  for (int br = 0; br < random16(30, 50); br++)
  {
    nscale8(target.strand, target.strand_length, 255 - br);

    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: RANDOM TWINKLE
void CLpxModes::randomTwinkleRainbow(CLpxStrip target, int delayMs)
{
  int blobCount = random16(3, target.strand_length / 5);
  int blobPos[blobCount];

  for (int i = 0; i < blobCount; i++)
  {
    blobPos[i] = random16(target.strand_length);
    fill_solid(target.strand + blobPos[i], 1, CRGB(random16(255), random16(255), random16(255)));
  }

  target.showStrand();

  for (int br = 0; br < random16(30, 50); br++)
  {
    nscale8(target.strand, target.strand_length, 255 - br);

    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: RANDOM FLASH
void CLpxModes::randomFlash(CLpxStrip target, int delayMs)
{
  fill_solid(target.strand, target.strand_length, CRGB(random16(255), random16(255), random16(255)));
  target.showStrand();
  LpxModes.smartDelay(delayMs, target);
  fill_solid(target.strand, target.strand_length, CRGB(0, 0, 0));
  target.showStrand();
  LpxModes.smartDelay(delayMs, target);
}

//NOTE: THEATER CHASE
void CLpxModes::theaterChase(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int i = 0;
  if (target.strand[0].r != 0 || target.strand[0].g != 0 || target.strand[0].b != 0)
  {
    i = 1;
  }
  fill_solid(target.strand, target.strand_length, CRGB(0, 0, 0));
  for (i; i < target.strand_length; i += 2)
  {
    fill_solid(target.strand + i, 1, CRGB(r, g, b));
  }
  target.showStrand();
  LpxModes.smartDelay(delayMs, target);
}

//NOTE: CHROMA
void CLpxModes::chroma(CLpxStrip target, int delayMs)
{
  for (int i = 0; i < 255; i++)
  {
    fill_rainbow(target.strand, target.strand_length, i);
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: FADE IN
void CLpxModes::fadeIn(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  CRGB temp[target.strand_length];
  for (int i = 0; i < target.strand_length; i++)
  {
    temp[i] = CRGB(r, g, b);
  }

  for (int i = 0; i < 255; i++)
  {
    nblend(target.strand, temp, target.strand_length, 25);
    LpxModes.smartDelay(delayMs, target);
    target.showStrand();
  }
}

//NOTE: FADE OUT
void CLpxModes::fadeOut(CLpxStrip target, int delayMs)
{

  for (int i = 0; i < 255; i++)
  {
    fadeToBlackBy(target.strand, target.strand_length, 25);
    LpxModes.smartDelay(delayMs, target);
    target.showStrand();
  }
}

//NOTE: SUDDEN FLASH
void CLpxModes::sudden(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  if (ONE_TIME_PER_COMMAND)
  {
    fill_solid(target.strand, target.strand_length, CRGB(r, g, b));
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
    fill_solid(target.strand, target.strand_length, CRGB(0, 0, 0));
    target.showStrand();
    ONE_TIME_PER_COMMAND = false;
  }
}

//NOTE: RANDOM BREATH
void CLpxModes::randomBreath(CLpxStrip target, int delayMs)
{
  CRGB rand = CRGB(random16(255), random16(255), random16(255));
  for (int i = 0; i < 255; i += 5)
  {
    fill_solid(target.strand, target.strand_length, rand);
    fadeLightBy(target.strand, target.strand_length, 255 - i);
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
  for (int i = 0; i < 255; i += 5)
  {
    fill_solid(target.strand, target.strand_length, rand);
    fadeLightBy(target.strand, target.strand_length, i);
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: BREATH
void CLpxModes::rgbFadeInAndOut(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  for (int i = 0; i < 255; i += 5)
  {
    fill_solid(target.strand, target.strand_length, CRGB(r, g, b));
    fadeLightBy(target.strand, target.strand_length, 255 - i);
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
  for (int i = 0; i < 255; i += 5)
  {
    fill_solid(target.strand, target.strand_length, CRGB(r, g, b));
    fadeLightBy(target.strand, target.strand_length, i);
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: FALLING STARS -- redo at some point
void CLpxModes::fallingStars(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int trailLen = 10;
  for (int pos = 0; pos < (target.strand_length + trailLen); pos++)
  {
    if (pos < target.strand_length)
    {
      target.strand[pos] = CRGB(r, g, b);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(r / (i * i));
      int gf = round(g / (i * i));
      int bf = round(b / (i * i));
      int curpos = pos - i;
      if (curpos >= 0 && curpos < target.strand_length)
      {
        target.strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, target);
      target.showStrand();
    }
    LpxModes.smartDelay(delayMs, target);
    if ((pos - trailLen) >= 0 && (pos - trailLen) < target.strand_length)
    {
      target.strand[pos - trailLen] = CRGB(0, 0, 0);
    }
  }
}

//NOTE: CHRISTMAS CHASE -- kill at some point
void CLpxModes::xmasChase(CLpxStrip target, int delayMs)
{
  int i = 0;
  if (target.strand[0].g != 255)
  {
    i = 1;
  }
  fill_solid(target.strand, target.strand_length, CRGB(0, 255, 0));
  for (i; i < target.strand_length; i += 2)
  {
    fill_solid(target.strand + i, 1, CRGB(255, 0, 0));
  }
  target.showStrand();
  LpxModes.smartDelay(delayMs, target);
}

//NOTE: PONG -- redo at some point
void CLpxModes::pong(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int trailLen = 6;
  for (int pos = 0; pos < (target.strand_length); pos++)
  {
    if (pos < target.strand_length)
    {
      target.strand[pos] = CRGB(r, g, b);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(r / i);
      int gf = round(g / i);
      int bf = round(b / i);
      int curpos = pos - i;
      if (curpos >= 0 && curpos < target.strand_length)
      {
        target.strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, target);
      target.showStrand();
    }
    LpxModes.smartDelay(delayMs, target);
    if ((pos - trailLen) >= 0 && (pos - trailLen) < target.strand_length)
    {
      target.strand[pos - trailLen] = CRGB(0, 0, 0);
    }
  }
  for (int pos = target.strand_length; pos > 0; pos--)
  {
    if (pos < target.strand_length)
    {
      target.strand[pos] = CRGB(r, g, b);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(r / i);
      int gf = round(g / i);
      int bf = round(b / i);
      int curpos = pos + i;
      if (curpos >= 0 && curpos < target.strand_length)
      {
        target.strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, target);
      target.showStrand();
    }
    LpxModes.smartDelay(delayMs, target);
    if ((pos + trailLen) >= 0 && (pos + trailLen) < target.strand_length)
    {
      target.strand[pos + trailLen] = CRGB(0, 0, 0);
    }
  }
}

//NOTE: WATERFALL
void CLpxModes::waterfall(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int i = 0;
  while (i <= target.strand_length)
  {
    fill_solid(target.strand, i, CRGB(r, g, b));
    i++;
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: WATERFALL RAINBOW --add to options
void CLpxModes::waterfallRainbow(CLpxStrip t, CLpxCommand c)
{
  int i = 0;
  while (i <= t.strand_length)
  {
    fill_rainbow(t.strand, i, 0);
    i++;
    t.showStrand();
    if (!LpxModes.smartDelay(c.delayMs, t))
    {
      break;
    }
  }
}

//NOTE: LIGHTNING
void CLpxModes::lightning(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  if (ONE_TIME_PER_COMMAND)
  {
    //solid(target, r, g, b);
    LpxModes.smartDelay(round(delayMs / 6), target); // how long on
    //solid(target, 0, 0, 0);
    LpxModes.smartDelay(round(delayMs / 4), target); // how long off
    //solid(target, r, g, b);
    LpxModes.smartDelay(round(delayMs / 2), target); // how long on
    //solid(target, 0, 0, 0);
    ONE_TIME_PER_COMMAND = false;
  }
}

//NOTE: WAVES -- redo at some point
void CLpxModes::waves(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int trailLen = 10;
  int swellLen = 2;
  for (int pos = 0; pos < (target.strand_length + trailLen); pos++)
  {
    for (int i = 1; i < swellLen; i++)
    {
      int rs = round(r / (i * i));
      int gs = round(g / (i * i));
      int bs = round(b / (i * i));
      int prepos = pos + i;
      if (prepos >= 0 && prepos < target.strand_length)
      {
        target.strand[prepos] = CRGB(rs, gs, bs);
      }
      LpxModes.smartDelay(2, target);
      target.showStrand();
    }
    if (pos < target.strand_length)
    {
      target.strand[pos] = CRGB(r, g, b);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(r / (float)((i * i) / 4));
      int gf = round(g / (float)((i * i) / 4));
      int bf = round(b / (float)((i * i) / 4));
      int curpos = pos - i;
      if (curpos >= 0 && curpos < target.strand_length)
      {
        target.strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, target);
      target.showStrand();
    }
    LpxModes.smartDelay(delayMs, target);
    if ((pos - (4 * trailLen)) >= 0 && (pos - (4 * trailLen)) < target.strand_length)
    {
      target.strand[pos - (4 * trailLen)] = CRGB(0, 0, 0);
    }
  }
}

//NOTE: LEVELS -- redo at some point
void CLpxModes::levels(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int trailLen = target.strand_length;
  int maxheight = round(target.strand_length / random16(1, 5));
  for (int pos = -1; pos <= maxheight; pos++)
  {
    if (pos < target.strand_length)
    {
      target.strand[pos] = CRGB(r, g, b);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(r / i);
      int gf = round(g / i);
      int bf = round(b / i);
      int curpos = pos + i;
      if (curpos >= 0 && curpos < target.strand_length)
      {
        target.strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, target);
      target.showStrand();
    }
    LpxModes.smartDelay(delayMs, target);
  }
  for (int pos = maxheight; pos >= 0; pos--)
  {
    if (pos < target.strand_length)
    {
      target.strand[pos] = CRGB(r, g, b);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(r / i);
      int gf = round(g / i);
      int bf = round(b / i);
      int curpos = pos + i;
      if (curpos >= 0 && curpos < target.strand_length)
      {
        target.strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, target);
      target.showStrand();
    }
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: RAIN -- redo at some point
void CLpxModes::rain(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  int trailLen = 1; //round(pixelLen/delayMs);
  int offset = 4 + trailLen;
  int k;
  for (int q = 0; q < offset; q++)
  {
    for (int i = 0; i < (target.strand_length + trailLen + 1); i = i + offset)
    {
      if ((i + q) >= 0 && (i + q) < target.strand_length)
      {
        target.strand[i + q] = CRGB(r, g, b);
      }
      for (int w = 0; w < trailLen; w++)
      {
        k = w + 1;
        int rf = round(r / (k * k));
        int gf = round(g / (k * k));
        int bf = round(b / (k * k));
        int curpos = i + q - k;
        if (curpos >= 0 && curpos < target.strand_length)
        {
          target.strand[curpos] = CRGB(rf, gf, bf);
        }
        if ((i + q - (trailLen + 1)) >= 0 && (i + q - (trailLen + 1)) < target.strand_length)
        {
          target.strand[i + q - (trailLen + 1)] = CRGB(0, 0, 0);
        }
        LpxModes.smartDelay(2, target);
      }
    }
    target.showStrand();
    LpxModes.smartDelay(delayMs, target);
  }
}

//NOTE: SOUNDSYNC
void CLpxModes::soundsync(CLpxStrip target, byte r, byte g, byte b)
{
  //strip.setBrightness(sound);
  //strip.show();
  //  for ( int i = 0; i <= pixelLen; i++)
  //  {
  //    strip.setPixelColor(i, r, g, b);
  //    strip.show();
  //  }
}