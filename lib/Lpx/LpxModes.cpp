#include "LpxModes.h"

CLpxModes LpxModes;
bool ONE_TIME_PER_COMMAND;

bool CLpxModes::smartDelay(int ms, bool init, CLpxStrip *t)
{
  long startMs = millis(); //get the time

  //Serial.println(foo);
  while (millis() < (startMs + ms))
  {
    if (t->taskStateControl != init)
    {
      // Serial.println("here");
      return false;
    }
  }

  return true;
}

//NOTE: OFF
bool CLpxModes::off(CLpxStrip *t, bool o)
{
  if (o)
  {
    fill_solid(t->strand, t->strand_length, CRGB(0, 0, 0));
    t->showStrand();
  }

  return false;
}

//NOTE: SOLID
bool CLpxModes::solid(CLpxStrip *t, CLpxCommand c, bool o)
{
  if (o)
  {
    fill_solid(t->strand, t->strand_length, CRGB(c.primary[0], c.primary[1], c.primary[2]));
    t->showStrand();
  }

  return false;
}

//NOTE: RANDOM CLOUDY BLOBS
void CLpxModes::randomCloudyBlobs(CLpxStrip *t, CLpxCommand c, bool to)
{
  int blobCount = random16(3, t->strand_length / 7);
  int blobPos[blobCount];

  for (int i = 0; i < blobCount; i++)
  {
    blobPos[i] = random16(t->strand_length);
    fill_solid(t->strand + blobPos[i] - 1, 3, CRGB(c.primary[0], c.primary[1], c.primary[2]));
  }

  t->showStrand();

  for (int br = 0; br < random16(30, 50); br++)
  {
    nscale8(t->strand, t->strand_length, 255 - br);

    t->showStrand();
    LpxModes.smartDelay(c.delayMs, to, t);
  }
}

//NOTE: FLASH
void CLpxModes::flash(CLpxStrip *t, CLpxCommand c, bool to)
{
  fill_solid(t->strand, t->strand_length, CRGB(c.primary[0], c.primary[1], c.primary[2]));
  t->showStrand();
  LpxModes.smartDelay(c.delayMs, to, t);
  fill_solid(t->strand, t->strand_length, CRGB(0, 0, 0));
  t->showStrand();
  LpxModes.smartDelay(c.delayMs, to, t);
}

//NOTE: SWEEP
void CLpxModes::sweep(CLpxStrip target, byte r, byte g, byte b, int delayMs)
{
  //depreciated, will get rid of!
}

//NOTE: TWINKLE
void CLpxModes::randomTwinkle(CLpxStrip *target, CLpxCommand c, bool to)
{
  int blobCount = random16(3, target->strand_length / 5);
  int blobPos[blobCount];

  for (int i = 0; i < blobCount; i++)
  {
    blobPos[i] = random16(target->strand_length);
    fill_solid(target->strand + blobPos[i], 1, CRGB(c.primary[0], c.primary[1], c.primary[2]));
  }

  target->showStrand();

  for (int br = 0; br < random16(30, 50); br++)
  {
    nscale8(target->strand, target->strand_length, 255 - br);

    target->showStrand();
    LpxModes.smartDelay(c.delayMs, to, target);
  }
}

//NOTE: RANDOM TWINKLE
void CLpxModes::randomTwinkleRainbow(CLpxStrip *target, CLpxCommand c, bool to)
{
  int blobCount = random16(3, target->strand_length / 5);
  int blobPos[blobCount];

  for (int i = 0; i < blobCount; i++)
  {
    blobPos[i] = random16(target->strand_length);
    fill_solid(target->strand + blobPos[i], 1, CRGB(random16(255), random16(255), random16(255)));
  }

  target->showStrand();

  for (int br = 0; br < random16(30, 50); br++)
  {
    nscale8(target->strand, target->strand_length, 255 - br);

    target->showStrand();
    LpxModes.smartDelay(c.delayMs, to, target);
  }
}

//NOTE: RANDOM FLASH
void CLpxModes::randomFlash(CLpxStrip *target, CLpxCommand c, bool to)
{
  fill_solid(target->strand, target->strand_length, CRGB(random16(255), random16(255), random16(255)));
  target->showStrand();
  LpxModes.smartDelay(c.delayMs, to, target);
  fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));
  target->showStrand();
  LpxModes.smartDelay(c.delayMs, to, target);
}

//NOTE: CHROMA
void CLpxModes::chroma(CLpxStrip *target, CLpxCommand c, bool to)
{
  for (int i = 0; i < 255; i++)
  {
    fill_rainbow(target->strand, target->strand_length, i);
    target->showStrand();
    LpxModes.smartDelay(c.delayMs, to, target);
  }
}

//NOTE: THEATER CHASE
void CLpxModes::theaterChase(CLpxStrip *target, CLpxCommand c, bool to)
{
  int i = 0;
  if (target->strand[0].r != 0 || target->strand[0].g != 0 || target->strand[0].b != 0)
  {
    i = 1;
  }
  fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));
  for (i; i < target->strand_length; i += 2)
  {
    fill_solid(target->strand + i, 1, CRGB(c.primary[0], c.primary[1], c.primary[2]));
  }
  target->showStrand();
  LpxModes.smartDelay(c.delayMs, to, target);
}

//NOTE: FADE IN
bool CLpxModes::fadeIn(CLpxStrip *target, CLpxCommand c, bool o, bool to)
{
  if (o)
  {
    CRGB temp[target->strand_length];
    for (int i = 0; i < target->strand_length; i++)
    {
      temp[i] = CRGB(c.primary[0], c.primary[1], c.primary[2]);
    }

    for (int i = 0; i < 255; i++)
    {
      nblend(target->strand, temp, target->strand_length, 25);
      LpxModes.smartDelay(c.delayMs, to, target);
      target->showStrand();
    }
  }
  return false;
}

//NOTE: FADE OUT
bool CLpxModes::fadeOut(CLpxStrip *target, CLpxCommand c, bool o, bool to)
{
  if (o)
  {
    for (int i = 0; i < 255; i++)
    {
      fadeToBlackBy(target->strand, target->strand_length, 25);
      LpxModes.smartDelay(c.delayMs, to, target);
      target->showStrand();
    }
  }
  return false;
}

//NOTE: SUDDEN FLASH
bool CLpxModes::sudden(CLpxStrip *target, CLpxCommand c, bool o, bool to)
{
  if (o)
  {
    fill_solid(target->strand, target->strand_length, CRGB(c.primary[0], c.primary[1], c.primary[2]));
    target->showStrand();
    LpxModes.smartDelay(c.delayMs, to, target);
    fill_solid(target->strand, target->strand_length, CRGB(0, 0, 0));
    target->showStrand();
    ONE_TIME_PER_COMMAND = false;
  }

  return false;
}

//NOTE: RANDOM BREATH
void CLpxModes::randomBreath(CLpxStrip *target, CLpxCommand command, bool to)
{
  CRGB rand = CRGB(random16(255), random16(255), random16(255));
  for (int i = 0; i < 255; i += 5)
  {
    fill_solid(target->strand, target->strand_length, rand);
    fadeLightBy(target->strand, target->strand_length, 255 - i);
    target->showStrand();
    LpxModes.smartDelay(command.delayMs, to, target);
  }
  for (int i = 0; i < 255; i += 5)
  {
    //done
    fill_solid(target->strand, target->strand_length, rand);
    fadeLightBy(target->strand, target->strand_length, i);
    target->showStrand();
    LpxModes.smartDelay(command.delayMs, to, target);
  }
}

//NOTE: BREATH
void CLpxModes::rgbFadeInAndOut(CLpxStrip *target, CLpxCommand command, bool to)
{
  //we need to get the vars and set them here
  for (int i = 0; i < 255; i += 5)
  {
    fill_solid(target->strand, target->strand_length, CRGB(command.primary[0], command.primary[1], command.primary[2]));
    fadeLightBy(target->strand, target->strand_length, 255 - i);
    target->showStrand();
    LpxModes.smartDelay(command.delayMs, to, target);
  }
  for (int i = 0; i < 255; i += 5)
  {
    fill_solid(target->strand, target->strand_length, CRGB(command.primary[0], command.primary[1], command.primary[2]));
    fadeLightBy(target->strand, target->strand_length, i);
    target->showStrand();
    LpxModes.smartDelay(command.delayMs, to, target);
  }
}

//NOTE: FALLING STARS -- redo at some point
void CLpxModes::fallingStars(CLpxStrip *target, CLpxCommand command, bool to)
{
  int trailLen = 10;
  for (int pos = 0; pos < (target->strand_length + trailLen); pos++)
  {
    if (pos < target->strand_length)
    {
      target->strand[pos] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(command.primary[0] / (i * i));
      int gf = round(command.primary[1] / (i * i));
      int bf = round(command.primary[2] / (i * i));
      int curpos = pos - i;
      if (curpos >= 0 && curpos < target->strand_length)
      {
        target->strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, to, target);
      target->showStrand();
    }
    LpxModes.smartDelay(command.delayMs, to, target);
    if ((pos - trailLen) >= 0 && (pos - trailLen) < target->strand_length)
    {
      target->strand[pos - trailLen] = CRGB(0, 0, 0);
    }
  }
}

//NOTE: CHRISTMAS CHASE -- kill at some point
void CLpxModes::xmasChase(CLpxStrip *target, CLpxCommand command, bool to)
{
  int i = 0;
  if (target->strand[0].g != 255)
  {
    i = 1;
  }
  fill_solid(target->strand, target->strand_length, CRGB(0, 255, 0));
  for (i; i < target->strand_length; i += 2)
  {
    fill_solid(target->strand + i, 1, CRGB(255, 0, 0));
  }
  target->showStrand();
  LpxModes.smartDelay(command.delayMs, to, target);
}

//NOTE: PONG -- redo at some point
void CLpxModes::pong(CLpxStrip *target, CLpxCommand command, bool to)
{
  int trailLen = 6;
  for (int pos = 0; pos < (target->strand_length); pos++)
  {
    if (pos < target->strand_length)
    {
      target->strand[pos] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(command.primary[0] / i);
      int gf = round(command.primary[1] / i);
      int bf = round(command.primary[2] / i);
      int curpos = pos - i;
      if (curpos >= 0 && curpos < target->strand_length)
      {
        target->strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, to, target);
      target->showStrand();
    }
    LpxModes.smartDelay(command.delayMs, to, target);
    if ((pos - trailLen) >= 0 && (pos - trailLen) < target->strand_length)
    {
      target->strand[pos - trailLen] = CRGB(0, 0, 0);
    }
  }
  for (int pos = target->strand_length; pos > 0; pos--)
  {
    if (pos < target->strand_length)
    {
      target->strand[pos] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(command.primary[0] / i);
      int gf = round(command.primary[1] / i);
      int bf = round(command.primary[2] / i);
      int curpos = pos + i;
      if (curpos >= 0 && curpos < target->strand_length)
      {
        target->strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, to, target);
      target->showStrand();
    }
    LpxModes.smartDelay(command.delayMs, to, target);
    if ((pos + trailLen) >= 0 && (pos + trailLen) < target->strand_length)
    {
      target->strand[pos + trailLen] = CRGB(0, 0, 0);
    }
  }
}

//NOTE: WATERFALL
bool CLpxModes::waterfall(CLpxStrip *t, CLpxCommand c, bool o, bool to)
{
  if (o)
  {
    int i = 0;
    while (i <= t->strand_length)
    {
      fill_solid(t->strand, i, CRGB(c.primary[0], c.primary[1], c.primary[2]));
      i++;
      t->showStrand();
      LpxModes.smartDelay(c.delayMs, to, t);
    }
  }

  return false;
}

//NOTE: WATERFALL RAINBOW --add to options
bool CLpxModes::waterfallRainbow(CLpxStrip *t, CLpxCommand c, bool o, bool to)
{
  if (o)
  {
    int i = 0;
    while (i <= t->strand_length)
    {
      fill_rainbow(t->strand, i, 0);
      i++;
      t->showStrand();
      if (!LpxModes.smartDelay(c.delayMs, to, t))
      {
        break;
      }
    }
  }

  return false;
}

//NOTE: LIGHTNING
bool CLpxModes::lightning(CLpxStrip *t, CLpxCommand c, bool o, bool to)
{
  if (o)
  {
    solid(t, c, o);
    LpxModes.smartDelay(round(c.delayMs / 6), to, t); // how long on
    off(t, o);
    LpxModes.smartDelay(round(c.delayMs / 4), to, t); // how long off
    solid(t, c, o);
    LpxModes.smartDelay(round(c.delayMs / 2), to, t); // how long on
    off(t, o);
  }

  return false;
}

//NOTE: WAVES -- redo at some point
void CLpxModes::waves(CLpxStrip *target, CLpxCommand command, bool to)
{
  int trailLen = 10;
  int swellLen = 2;
  for (int pos = 0; pos < (target->strand_length + trailLen); pos++)
  {
    for (int i = 1; i < swellLen; i++)
    {
      int rs = round(command.primary[0] / (i * i));
      int gs = round(command.primary[1] / (i * i));
      int bs = round(command.primary[2] / (i * i));
      int prepos = pos + i;
      if (prepos >= 0 && prepos < target->strand_length)
      {
        target->strand[prepos] = CRGB(rs, gs, bs);
      }
      LpxModes.smartDelay(2, to, target);
      target->showStrand();
    }
    if (pos < target->strand_length)
    {
      target->strand[pos] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(command.primary[0] / (float)((i * i) / 4));
      int gf = round(command.primary[1] / (float)((i * i) / 4));
      int bf = round(command.primary[2] / (float)((i * i) / 4));
      int curpos = pos - i;
      if (curpos >= 0 && curpos < target->strand_length)
      {
        target->strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, to, target);
      target->showStrand();
    }
    LpxModes.smartDelay(command.delayMs, to, target);
    if ((pos - (4 * trailLen)) >= 0 && (pos - (4 * trailLen)) < target->strand_length)
    {
      target->strand[pos - (4 * trailLen)] = CRGB(0, 0, 0);
    }
  }
}

//NOTE: LEVELS -- redo at some point
void CLpxModes::levels(CLpxStrip *target, CLpxCommand command, bool to)
{
  int trailLen = target->strand_length;
  int maxheight = round(target->strand_length / random16(1, 5));
  for (int pos = -1; pos <= maxheight; pos++)
  {
    if (pos < target->strand_length)
    {
      target->strand[pos] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(command.primary[0] / i);
      int gf = round(command.primary[1] / i);
      int bf = round(command.primary[2] / i);
      int curpos = pos + i;
      if (curpos >= 0 && curpos < target->strand_length)
      {
        target->strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, to, target);
      target->showStrand();
    }
    LpxModes.smartDelay(command.delayMs, to, target);
  }
  for (int pos = maxheight; pos >= 0; pos--)
  {
    if (pos < target->strand_length)
    {
      target->strand[pos] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
    }
    for (int i = 1; i < trailLen; i++)
    {
      int rf = round(command.primary[0] / i);
      int gf = round(command.primary[1] / i);
      int bf = round(command.primary[2] / i);
      int curpos = pos + i;
      if (curpos >= 0 && curpos < target->strand_length)
      {
        target->strand[curpos] = CRGB(rf, gf, bf);
      }
      LpxModes.smartDelay(2, to, target);
      target->showStrand();
    }
    LpxModes.smartDelay(command.delayMs, to, target);
  }
}

//NOTE: RAIN -- redo at some point
void CLpxModes::rain(CLpxStrip *target, CLpxCommand command, bool to)
{
  int trailLen = 1; //round(pixelLen/delayMs);
  int offset = 4 + trailLen;
  int k;
  for (int q = 0; q < offset; q++)
  {
    for (int i = 0; i < (target->strand_length + trailLen + 1); i = i + offset)
    {
      if ((i + q) >= 0 && (i + q) < target->strand_length)
      {
        target->strand[i + q] = CRGB(command.primary[0], command.primary[1], command.primary[2]);
      }
      for (int w = 0; w < trailLen; w++)
      {
        k = w + 1;
        int rf = round(command.primary[0] / (k * k));
        int gf = round(command.primary[1] / (k * k));
        int bf = round(command.primary[2] / (k * k));
        int curpos = i + q - k;
        if (curpos >= 0 && curpos < target->strand_length)
        {
          target->strand[curpos] = CRGB(rf, gf, bf);
        }
        if ((i + q - (trailLen + 1)) >= 0 && (i + q - (trailLen + 1)) < target->strand_length)
        {
          target->strand[i + q - (trailLen + 1)] = CRGB(0, 0, 0);
        }
        LpxModes.smartDelay(2, to, target);
      }
    }
    target->showStrand();
    LpxModes.smartDelay(command.delayMs, to, target);
  }
}

//NOTE: SOUNDSYNC
void CLpxModes::soundsync(CLpxStrip *target, CLpxCommand command, bool to)
{
  //strip.setBrightness(sound);
  //strip.show();
  //  for ( int i = 0; i <= pixelLen; i++)
  //  {
  //    strip.setPixelColor(i, r, g, b);
  //    strip.show();
  //  }
}