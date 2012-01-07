package com.bookofbrilliantthings.totg.itoa;

import java.io.IOException;
import java.io.Writer;

public class NullWriter
    extends Writer
{
    private boolean closed;

    public NullWriter()
    {
        closed = false;
    }

    @Override
    public void close()
        throws IOException
    {
        if (closed)
            throw new IOException("NullWriter has already been closed");

        closed = true;
    }

    @Override
    public void flush()
        throws IOException
    {
        if (closed)
            throw new IOException("NullWriter has been closed");
    }

    @Override
    public void write(char[] arg0, int arg1, int arg2)
        throws IOException
    {
        if (closed)
            throw new IOException("NullWriter has been closed");
    }
}
