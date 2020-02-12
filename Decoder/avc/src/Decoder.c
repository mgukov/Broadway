#include "H264SwDecApi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

/* Debug prints */
#define DEBUG(argv) printf argv

#include "opttarget.h"
#include "yuv_rgb.h"

u32 broadwayInit();

u32 broadwayDecode();

void broadwayExit();

H264SwDecInst decInst;
H264SwDecInput decInput;
H264SwDecOutput decOutput;
H264SwDecPicture decPicture;
H264SwDecInfo decInfo;

u32 picDecodeNumber;
u32 picDisplayNumber;
u32 picSize;

typedef struct {
    u32 length;
    u8 *buffer;
    u8 *pos;
    u8 *end;
} Stream;

Stream broadwayStream;

u32 rgb = 0;

void streamInit(Stream *stream, u32 length) {
    stream->buffer = stream->pos = (u8 *) malloc(sizeof(u8) * length);
    stream->length = length;
    stream->end = stream->buffer + length;
}

void playStream(u32 id, Stream *stream) {
    decInput.pStream = stream->buffer;
    decInput.dataLen = stream->length;
    do {
        u32 ret = broadwayDecode(id);
    } while (decInput.dataLen > 0);
}

extern void broadwayOnHeadersDecoded();

extern void broadwayOnPictureDecoded(u32 id, u8 *buffer, u32 width, u32 height);


u8 *broadwayCreateStream(u32 length) {
    streamInit(&broadwayStream, length);
    return broadwayStream.buffer;
}

void broadwayPlayStream(u32 id, u32 length) {
    broadwayStream.length = length;
    playStream(id, &broadwayStream);
}

u32 broadwayInit(u32 _rgb) {
    rgb = _rgb;
    H264SwDecRet ret;
#ifdef DISABLE_OUTPUT_REORDERING
    u32 disableOutputReordering = 1;
#else
    u32 disableOutputReordering = 0;
#endif

    /* Initialize decoder instance. */
    ret = H264SwDecInit(&decInst, disableOutputReordering);
    if (ret != H264SWDEC_OK) {
        emscripten_log(EM_LOG_ERROR, "DECODER INITIALIZATION FAILED");
        broadwayExit();
        return -1;
    }
    picDecodeNumber = picDisplayNumber = 1;

    emscripten_log(EM_LOG_CONSOLE, "init avc, rgb: %d", rgb);
    return 0;
}


u32 broadwayDecode(u32 id) {
    decInput.picId = picDecodeNumber;

    H264SwDecRet ret = H264SwDecDecode(decInst, &decInput, &decOutput);

    switch (ret) {
        case H264SWDEC_HDRS_RDY_BUFF_NOT_EMPTY:
            /* Stream headers were successfully decoded, thus stream information is available for query now. */
            ret = H264SwDecGetInfo(decInst, &decInfo);
            if (ret != H264SWDEC_OK) {
                return -1;
            }

            picSize = decInfo.picWidth * decInfo.picHeight;
            picSize = (3 * picSize) / 2;

            broadwayOnHeadersDecoded();

            decInput.dataLen -= decOutput.pStrmCurrPos - decInput.pStream;
            decInput.pStream = decOutput.pStrmCurrPos;
            break;

        case H264SWDEC_PIC_RDY_BUFF_NOT_EMPTY:
            /* Picture is ready and more data remains in the input buffer,
             * update input structure.
             */
            decInput.dataLen -= decOutput.pStrmCurrPos - decInput.pStream;
            decInput.pStream = decOutput.pStrmCurrPos;

            /* fall through */

        case H264SWDEC_PIC_RDY:
            //if (ret == H264SWDEC_PIC_RDY) {
            decInput.dataLen = 0;
            //}

            /* Increment decoding number for every decoded picture */
            picDecodeNumber++;


            while (H264SwDecNextPicture(decInst, &decPicture, 0) == H264SWDEC_PIC_RDY) {
                // printf(" Decoded Picture Decode: %d, Display: %d, Type: %s\n", picDecodeNumber, picDisplayNumber, decPicture.isIdrPicture ? "IDR" : "NON-IDR");

                /* Increment display number for every displayed picture */
                picDisplayNumber++;

#ifndef EMIT_IMAGE_ASAP

                if (rgb == 0) {
                    broadwayOnPictureDecoded(id, (u8 *) decPicture.pOutputPicture, decInfo.picWidth, decInfo.picHeight);
                } else {

                    u32 stride = decInfo.picWidth;
                    u32 stride2 = stride/2;

                    u32 ylen = stride * decInfo.picHeight;
                    u32 uvlen = stride2 * (decInfo.picHeight / 2);

                    u8 *y = ((u8 *) decPicture.pOutputPicture);
                    u8 *u = ((u8 *) decPicture.pOutputPicture) + ylen;
                    u8 *v = ((u8 *) decPicture.pOutputPicture) + ylen + uvlen;


                    u8 rgb[decInfo.picWidth * decInfo.picHeight * 4];
                    yuv420_rgb32_std(decInfo.picWidth, decInfo.picHeight, y, u, v, stride, stride2, rgb,
                                     stride * 4, YCBCR_601);
                    broadwayOnPictureDecoded(id, rgb, decInfo.picWidth, decInfo.picHeight);
                }
#endif
            }
            break;

        case H264SWDEC_STRM_PROCESSED:
        case H264SWDEC_STRM_ERR:
            /* Input stream was decoded but no picture is ready, thus get more data. */
            decInput.dataLen = 0;
            break;

        default:
            break;
    }
    return ret;
}

void broadwayExit() {
}

u8 *broadwayCreateStreamBuffer(u32 size) {
    u8 *buffer = (u8 *) malloc(sizeof(u8) * size);
    if (buffer == NULL) {
        DEBUG(("UNABLE TO ALLOCATE MEMORY\n"));
    }
    return buffer;
}

u32 broadwayGetMajorVersion() {
    return H264SwDecGetAPIVersion().major;
}

u32 broadwayGetMinorVersion() {
    return H264SwDecGetAPIVersion().minor;
}

