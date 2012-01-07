package com.bookofbrilliantthings.totg.itoa;

import java.io.IOException;
import java.io.Writer;

public class Integer
{
    private final static char digit[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    public static void writeTo(Writer writer, int i)
        throws IOException
    {
        if (i == 0)
        {
            writer.write('0');
            return;
        }

        /*
         * This local buffer is never passed anywhere, so we should expect escape
         * analysis to detect it can be reclaimed on function exit.
         */
        final char buf[] = new char[32];
        int idx = 32;
        boolean negative = false;

        if (i < 0)
        {
            i = -i;
            negative = true;
        }

        for(; i > 0; i /= 10)
            buf[--idx] = digit[i % 10];

        if (negative)
            buf[--idx] = '-';

        /* write the characters out one at a time to avoid passing the char array */
        for(idx = 32 - idx; idx != 0; --idx)
            writer.write(buf[idx]);
    }
}
