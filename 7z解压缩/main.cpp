#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "7z.h"
#include "7zFile.h"  
#include "7zTypes.h"
#include "7zCrc.h"
#include "Alloc.h"
#include "7zBuf.h"
#include "7zAlloc.h"

#define kInputBufSize ((size_t)1 << 18)

#define _UTF8_START(n) (0x100 - (1 << (7 - (n))))

#define _UTF8_RANGE(n) (((UInt32)1) << ((n) * 5 + 6))

#define _UTF8_HEAD(n, val) ((Byte)(_UTF8_START(n) + (val >> (6 * (n)))))
#define _UTF8_CHAR(n, val) ((Byte)(0x80 + (((val) >> (6 * (n))) & 0x3F)))

static int Buf_EnsureSize(CBuf *dest, size_t size)
{
    if (dest->size >= size)
        return 1;
    Buf_Free(dest, &g_Alloc);
    return Buf_Create(dest, size, &g_Alloc);
}

static size_t Utf16_To_Utf8_Calc(const UInt16 *src, const UInt16 *srcLim)
{
    size_t size = 0;
    for (;;)
    {
        UInt32 val;
        if (src == srcLim)
            return size;

        size++;
        val = *src++;

        if (val < 0x80)
            continue;

        if (val < _UTF8_RANGE(1))
        {
            size++;
            continue;
        }

        if (val >= 0xD800 && val < 0xDC00 && src != srcLim)
        {
            UInt32 c2 = *src;
            if (c2 >= 0xDC00 && c2 < 0xE000)
            {
                src++;
                size += 3;
                continue;
            }
        }

        size += 2;
    }
}

static Byte *Utf16_To_Utf8(Byte *dest, const UInt16 *src, const UInt16 *srcLim)
{
    for (;;)
    {
        UInt32 val;
        if (src == srcLim)
            return dest;

        val = *src++;

        if (val < 0x80)
        {
            *dest++ = (char)val;
            continue;
        }

        if (val < _UTF8_RANGE(1))
        {
            dest[0] = _UTF8_HEAD(1, val);
            dest[1] = _UTF8_CHAR(0, val);
            dest += 2;
            continue;
        }

        if (val >= 0xD800 && val < 0xDC00 && src != srcLim)
        {
            UInt32 c2 = *src;
            if (c2 >= 0xDC00 && c2 < 0xE000)
            {
                src++;
                val = (((val - 0xD800) << 10) | (c2 - 0xDC00)) + 0x10000;
                dest[0] = _UTF8_HEAD(3, val);
                dest[1] = _UTF8_CHAR(2, val);
                dest[2] = _UTF8_CHAR(1, val);
                dest[3] = _UTF8_CHAR(0, val);
                dest += 4;
                continue;
            }
        }

        dest[0] = _UTF8_HEAD(2, val);
        dest[1] = _UTF8_CHAR(1, val);
        dest[2] = _UTF8_CHAR(0, val);
        dest += 3;
    }
}

static SRes Utf16_To_Utf8Buf(CBuf *dest, const UInt16 *src, size_t srcLen)
{
    size_t destLen = Utf16_To_Utf8_Calc(src, src + srcLen);
    destLen += 1;
    if (!Buf_EnsureSize(dest, destLen))
        return SZ_ERROR_MEM;
    *Utf16_To_Utf8(dest->data, src, src + srcLen) = 0;
    return SZ_OK;
}

static SRes Utf16_To_Char(CBuf *buf, const UInt16 *s)
{
    unsigned len = 0;
    for (len = 0; s[len] != 0; len++);
    return Utf16_To_Utf8Buf(buf, s, len);
}

static std::string PrintString(const UInt16 *s)
{
    std::string strbuf;
    CBuf buf;
    SRes res;
    Buf_Init(&buf);
    res = Utf16_To_Char(&buf, s);
    if (res == SZ_OK)
        strbuf.append((const char *)buf.data, buf.size - 1);
    Buf_Free(&buf, &g_Alloc);
    return strbuf;
}

int make_sure_dir_exist(const char* desPath)
{
    char* p = strchr((char *)desPath, '/');
    while (p)
    {
        int c = *(p + 1);
        *(p + 1) = '\0';
        if (access(desPath, F_OK) == -1)
        {
            if (mkdir(desPath, 0755) < 0)
            {
                return -1;
            }
        }
        *(p + 1) = (char)c;
        p = strchr(p + 1, '/');
    }
    return 0;
}

int main()
{
    std::string srcfile = "/home/7ztest/test.7z";
    std::string dstpath = "/home/7ztest/";

    CFileInStream archiveStream;
    CLookToRead2 lookStream;
    ISzAlloc allocImp = g_Alloc;
    ISzAlloc allocTempImp = g_Alloc;
    SRes res = SZ_OK;

    if (InFile_Open(&archiveStream.file, srcfile.c_str()))
    {
        printf("cant not open input file: %s\n", srcfile.c_str());
        return -1;
    }

    FileInStream_CreateVTable(&archiveStream);
    LookToRead2_CreateVTable(&lookStream, False);
    lookStream.buf = NULL;

    lookStream.buf = (Byte *)ISzAlloc_Alloc(&allocImp, kInputBufSize);
    if (lookStream.buf)
    {
        lookStream.bufSize = kInputBufSize;
        lookStream.realStream = &archiveStream.vt;
        LookToRead2_Init(&lookStream);

        CrcGenerateTable();

        CSzArEx db;
        SzArEx_Init(&db);

        Byte *outBuffer = 0;
        size_t outBufferSize = 0;
        UInt32 blockIndex = 0xFFFFFFFF;

        res = SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp);
        if (res == SZ_OK)
        {
            for (int i = db.NumFiles - 1; i >= 0; i--)
            {
                /* 判断是文件or文件夹 */
                unsigned isDir = SzArEx_IsDir(&db, i);
                size_t tempSize = SzArEx_GetFileNameUtf16(&db, i, NULL);    
                UInt16 *temp = (UInt16 *)SzAlloc(NULL, tempSize * sizeof(temp[0]));
                SzArEx_GetFileNameUtf16(&db, i, temp);

                /* 组装全路径 */
                std::string outpath = dstpath + PrintString(temp);

                if (isDir == 0)
                {
                    printf("Extracting\t%s\n", outpath.c_str());
                    size_t offset = 0;
                    size_t outSizeProcessed = 0;
                    res = SzArEx_Extract(&db, &lookStream.vt, i, &blockIndex, &outBuffer, &outBufferSize, &offset, &outSizeProcessed, &allocImp, &allocTempImp);
                    if (res != SZ_OK) break;

                    CSzFile outFile;
                    size_t processedSize = outSizeProcessed;
                    if (OutFile_Open(&outFile, outpath.c_str()))
                    {
                        res = SZ_ERROR_FAIL;
                        break;
                    }

                    if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
                    {
                        res = SZ_ERROR_FAIL;
                        break;
                    }

                    if (File_Close(&outFile))
                    {
                        res = SZ_ERROR_FAIL;
                        break;
                    }
                }
                else
                {
                    outpath += "/";
                    printf("Extracting\t%s\n", outpath.c_str());
                    if (0 > make_sure_dir_exist(outpath.c_str()))
                    {
                        res = SZ_ERROR_FAIL;
                        break;
                    }
                }
                SzFree(NULL, temp);
            }
        }
        ISzAlloc_Free(&allocImp, outBuffer);
        SzArEx_Free(&db, &allocImp);
    }
    else
    {
        res = SZ_ERROR_MEM;
    }

    ISzAlloc_Free(&allocImp, lookStream.buf);
    File_Close(&archiveStream.file);

    if (res == SZ_OK)
        printf("Everything is Ok\n");
    return 1;
}
