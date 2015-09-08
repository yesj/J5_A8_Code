/*****************************************************************************
 * webcam.c
 *
 * bc-cat module unit test - render textures from webcam
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *   
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *   
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <sys/time.h>
#include <linux/videodev2.h>

#ifdef GLES_20
#include <math.h>
#endif
#include <bc_cat.h>
#include "common.h"

typedef struct buf_info {
    int index;
    size_t length;
    char *start;
} buf_info_t;

#define NUM_BUFFERS         3
#define CAPTURE_DEVICE      "/dev/video3"
#define CAPTURE_NAME        "Capture"
#define MAX_BUFFERS         3


#define USBCAM_WIDTH        320
#define USBCAM_HEIGHT       240
#define USBCAM_DEF_PIX_FMT  V4L2_PIX_FMT_YUYV
#define YUV_PIXEL_FMT       BC_PIX_FMT_YUYV

#define CLEAR(x)            memset(&(x), 0, sizeof(x))

static char *cap_dev = NULL;
static buf_info_t cap_buf_info[NUM_BUFFERS];
static int bcdev_id = 0;
static int cap_fd = -1;
static tex_buffer_info_t buf_info;


int frame_init(bc_buf_params_t *p)
{
    int i, j, index;
    struct v4l2_requestbuffers reqbuf;
    struct v4l2_buffer buf;
    struct v4l2_capability capability;
    struct v4l2_input input;
    struct v4l2_format fmt;
    int retry = 0;

    if(!cap_dev)
        cap_dev = CAPTURE_DEVICE;

    /* Open the capture device */
    while (1) {
        if (gQuit == TRUE || retry >= 10)
            return -1;

        cap_fd = open(cap_dev, O_RDWR|O_NONBLOCK);
        if (cap_fd  <= 0) {
            printf("Cannot open capture device\n");
            printf("Please check the webcam... retry in 3 seconds...\n");

            sleep(3);
            retry++;
            continue;
        }
        break;
    }

    /* Get any active input */
    if (ioctl(cap_fd, VIDIOC_G_INPUT, &index) < 0) {
        perror("VIDIOC_G_INPUT");
        goto ERROR;
    }

    /* Enumerate input to get the name of the input detected */
    CLEAR(input);
    input.index = index;
    if (ioctl(cap_fd, VIDIOC_ENUMINPUT, &input) < 0) {
        perror("VIDIOC_ENUMINPUT");
        goto ERROR;
    }
	printf("INFO: current input is %s\n", input.name);
    index = 0;
    if (ioctl(cap_fd, VIDIOC_S_INPUT, &index) < 0) {
        perror("VIDIOC_S_INPUT");
        goto ERROR;
    }

    CLEAR(input);
    input.index = index;
    if (ioctl(cap_fd, VIDIOC_ENUMINPUT, &input) < 0) {
        perror("VIDIOC_ENUMINPUT");
        goto ERROR;
    }

    /* Check if the device is capable of streaming */
    if (ioctl(cap_fd, VIDIOC_QUERYCAP, &capability) < 0) {
        perror("VIDIOC_QUERYCAP");
        goto ERROR;
    }
    if (!capability.capabilities & V4L2_CAP_STREAMING) {
        printf("%s: Not capable of streaming\n", CAPTURE_NAME);
        goto ERROR;
    }

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cap_fd, VIDIOC_G_FMT, &fmt) < 0) {
        perror("VIDIOC_G_FMT");
        goto ERROR;
    }

    fmt.fmt.pix.width = USBCAM_WIDTH;
    fmt.fmt.pix.height = USBCAM_HEIGHT;
    fmt.fmt.pix.pixelformat = USBCAM_DEF_PIX_FMT;

    if (ioctl(cap_fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        goto ERROR;
    }

    if (ioctl(cap_fd, VIDIOC_G_FMT, &fmt) < 0) {
        perror("VIDIOC_G_FMT");
        goto ERROR;
    }

    if (fmt.fmt.pix.pixelformat != USBCAM_DEF_PIX_FMT) {
        printf("%s: Requested pixel format not supported\n", CAPTURE_NAME);
        goto ERROR;
    }

    /* Buffer allocation */
    CLEAR(reqbuf);
    reqbuf.count = NUM_BUFFERS;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cap_fd, VIDIOC_REQBUFS, &reqbuf) < 0) {
        perror("Cannot allocate memory");
        goto ERROR;
    }
    /*TODO check if NUM_BUFFERS got allocated*/

    for(i = 0; i < NUM_BUFFERS; i++)
        cap_buf_info[i].start = NULL;

    CLEAR(buf);

    for (i = 0; i < NUM_BUFFERS; i++) {
        buf.type = reqbuf.type;
        buf.index = i;
        buf.memory = reqbuf.memory;
        if (ioctl(cap_fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYCAP");
            goto ERROR1;
        }

        cap_buf_info[i].length = buf.length;
        cap_buf_info[i].index = i;
        cap_buf_info[i].start = mmap(NULL, buf.length,
                PROT_READ | PROT_WRITE, MAP_SHARED,
                cap_fd, buf.m.offset);

        if (cap_buf_info[i].start == MAP_FAILED) {
            printf("Cannot mmap = %d buffer\n", i);
            goto ERROR1;
        }

        if (ioctl(cap_fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            goto ERROR1;
        }
    }

    /* Start Streaming on capture device */
    index = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cap_fd, VIDIOC_STREAMON, &index) < 0) {
        perror("VIDIOC_STREAMON");
        goto ERROR1;
    }

    if (p) {
        p->count = 1;
        p->width = USBCAM_WIDTH;
        p->height = USBCAM_HEIGHT;
        p->fourcc = YUV_PIXEL_FMT;
        p->type = BC_MEMORY_MMAP;      
    }
    return 0;

ERROR1:
    for (j = 0; j < NUM_BUFFERS; j++)
        if (cap_buf_info[j].start)
            munmap(cap_buf_info[j].start, cap_buf_info[j].length);
ERROR:
    close(cap_fd);

    return -1;
}

/*****************************************************************************
 * return cap buffer virtual address
 *      NULL - buffer unavailable
 *        -1 - DQBUF failed
 ****************************************************************************/
char *frame_get(bc_buf_ptr_t *p)
{
    struct v4l2_buffer buf;

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    /* Dequeue capture buffer */
    if (ioctl(cap_fd, VIDIOC_DQBUF, &buf) < 0) {
        if (errno == EAGAIN)
            return NULL;
        perror("VIDIOC_DQBUF");
        return (char *)-1;
    }

    if (p)
        p->pa = 0;

    return cap_buf_info[buf.index].start;
}

int frame_restore(char *p)
{
    struct v4l2_buffer buf;
    int ii;

    /*dont bother with NULL pointer*/
    if (!p)
        return 0;

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    for (ii = 0; ii < NUM_BUFFERS; ii++)
        if (cap_buf_info[ii].start == p)
            break;

    /*buffer not found*/
    if (ii == NUM_BUFFERS)
        return -1;

    buf.index = ii;

    /* Queue capture buffer */
    if (ioctl(cap_fd, VIDIOC_QBUF, &buf) < 0) {
        perror("VIDIOC_DQBUF");
        return -1;
    }
    return 0;
}

void frame_cleanup(void)
{
    int i, a;

    a = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cap_fd, VIDIOC_STREAMOFF, &a))
        perror("VIDIOC_STREAMOFF");

    for (i = 0; i < NUM_BUFFERS; i++) {
        munmap(cap_buf_info[i].start, cap_buf_info[i].length);
        cap_buf_info[i].start = NULL;
    }
    close(cap_fd);
}


#ifdef GLES_20
void drawCube(int bufferindex)
{
    static float rot_x = 0.0;
    static float rot_y = 0.0;
    float sx, cx, sy, cy;
    int tex_sampler;
   
    /* rotate the cube */
    sx = sin(rot_x);
    cx = cos(rot_x);
    sy = sin(rot_y);
    cy = cos(rot_y);
   
    modelview[0] = cy;
    modelview[1] = 0;
    modelview[2] = -sy;
    modelview[4] = sy * sy;
    modelview[5] = cx;
    modelview[6] = cy * sx;
    modelview[8] = sy * cx;
    modelview[9] = -sx;
    modelview[10] = cx * cy;
   
    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    glUseProgram(program[0]);
   
    glUniformMatrix4fv(model_view_idx[0], 1, GL_FALSE, modelview);
    glUniformMatrix4fv(proj_idx[0], 1, GL_FALSE, projection);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 8);

    /* associate the stream texture */
    tex_sampler = glGetUniformLocation(program[1], "streamtexture");

    glUseProgram(program[1]);

    if (ptex_objs) {
        /* activate texture unit */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_STREAM_IMG, ptex_objs[bufferindex]);
   
        /* associate the sampler to a texture unit
         * 0 matches GL_TEXTURE0 */
        glUniform1i(tex_sampler, 0);
   
        glUniformMatrix4fv(model_view_idx[1], 1, GL_FALSE, modelview);
        glUniformMatrix4fv(proj_idx[1], 1, GL_FALSE, projection);

        glDrawArrays (GL_TRIANGLE_STRIP, 0, 8);
        glDrawArrays (GL_TRIANGLE_STRIP, 8, 8);
    }

    eglSwapBuffers(dpy, surface);

    rot_x += 0.01;
    rot_y += 0.01;
}
#else

void drawCube(int bufferindex)
{
    static GLfloat m_fAngleX = 0.0f;
    static GLfloat m_fAngleY = 0.0f;

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_STREAM_IMG);

    glPushMatrix();

    // Rotate the cube model
    glRotatef(m_fAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(m_fAngleY, 0.0f, 1.0f, 0.0f);

    glTexBindStreamIMG(bcdev_id, bufferindex);
/*    glBindTexture(GL_TEXTURE_STREAM_IMG, bufferindex);*/

    // Enable 3 types of data
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Set pointers to the arrays
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glNormalPointer(GL_FLOAT, 0, cube_normals);
    glTexCoordPointer(2, GL_FLOAT, 0, cube_tex_coords);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 8);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glPopMatrix();

    glDisable(GL_TEXTURE_STREAM_IMG);
    eglSwapBuffers(dpy, surface);

    m_fAngleX += 0.25f;
    m_fAngleY += 0.25f;
}
#endif

void usage(char *arg)
{
    printf("Usage:\n"
           "    %s [-b <id> | -c <dev> | -p | -w <width> | -t <height> | -h]\n"
           "\t-b - bc device id [default: 0]\n"
           "\t-c - capture device [default: /dev/video3]\n"
           "\t-p - enable profiling\n"
           "\t-w - width of texture buffer in pixel, multiple of 8 (for ES3.x)\n"
           "\t     or 32 (for ES2.x)\n"
           "\t-t - height of texture buffer in pixel\n"
           "\t-h - print this message\n\n", arg);
}

int main(int argc, char *argv[])
{
    int bcfd = -1; 
    char bcdev_name[] = "/dev/bccatX";
    BCIO_package ioctl_var;
    bc_buf_params_t buf_param;
    bc_buf_ptr_t buf_pa;

    unsigned long buf_paddr[MAX_BUFFERS];
    char *buf_vaddr[MAX_BUFFERS] = { MAP_FAILED };
    char *frame = NULL;
    int buf_size = 0;
    int c, idx, ret = -1;
    char opts[] = "c:pw:t:b:h";

    int   ii;
    int   frame_w, frame_h;
    int   min_w = 0, min_h = 0;;
    int   cp_offset = 0;

    struct timeval tvp, tv, tv0 = {0,0};
    unsigned long tdiff = 0;
    unsigned long fcount = 0;

    for (;;) {
        c = getopt_long(argc, argv, opts, (void *)NULL, &idx);

        if (-1 == c)
            break;

        switch (c) {
            case 0:
                break;
            case 'b':
                bcdev_id = atoi(optarg) % 10;
                break;
            case 'c':
                cap_dev = optarg;
                printf("INFO: capture device is %s\n", cap_dev);
                break;
            case 'p':
                profiling = TRUE;
                printf("INFO: profiling enabled\n");
                break;
            case 'w':
                min_w = atoi(optarg);
                break;
            case 't':
                min_h = atoi(optarg);
                break;
            default:
                usage(argv[0]);
                return 0;
        }
    }

    signal(SIGINT, signalHandler);

    if (frame_init(&buf_param))
        return -1;

    bcdev_name[strlen(bcdev_name)-1] = '0' + bcdev_id;

    if ((bcfd = open(bcdev_name, O_RDWR|O_NDELAY)) == -1) {
        printf("ERROR: open %s failed\n", bcdev_name);
        goto err_ret;
    }

    frame_w = buf_param.width;
    frame_h = buf_param.height;

    if (min_w > 0 && !(min_w % 8))
        buf_param.width = min_w;

    if (min_h > 0)
        buf_param.height = min_h;
 
    if (ioctl(bcfd, BCIOREQ_BUFFERS, &buf_param) != 0) {
        printf("ERROR: BCIOREQ_BUFFERS failed\n");
        goto err_ret;
    }

    if (ioctl(bcfd, BCIOGET_BUFFERCOUNT, &ioctl_var) != 0) {
        goto err_ret;
    }

    if (ioctl_var.output == 0) {
        printf("ERROR: no texture buffer available\n");
        goto err_ret;
    }

    /* for BC_MEMORY_USERPTR, BCIOSET_BUFFERPHYADDR must be called
     * before init IMG_extensions in initTexExt()*/
    if (buf_param.type == BC_MEMORY_USERPTR) {
        for (idx = 0; idx < buf_param.count; idx++) {
            while ((frame = frame_get(&buf_pa)) == NULL) { }

            if (frame == (char *)-1)
                goto err_ret;

            if (ioctl(bcfd, BCIOSET_BUFFERPHYADDR, &buf_pa) != 0) {
                frame_restore(frame);
                printf("ERROR: BCIOSET_BUFFERADDR[%d]: failed (0x%lx)\n",
                       buf_pa.index, buf_pa.pa);
                goto err_ret;
            }

            if (frame_restore(frame))
                goto err_ret;
        }
    }

    if (initEGL(buf_param.count)) {
        printf("ERROR: init EGL failed\n");
        goto err_ret;
    }
 
    if ((ret = initTexExt(bcdev_id, &buf_info)) < 0) {
        printf("ERROR: initTexExt() failed [%d]\n", ret);
        goto err_ret;
    }

    if (buf_info.n > MAX_BUFFERS) {
        printf("ERROR: number of texture buffer exceeds the limit\n");
        goto err_ret;
    }

    /*FIXME calc stride instead of 2*/
    buf_size = buf_info.w * buf_info.h * 2;
    min_w    = buf_info.w < frame_w ? buf_info.w : frame_w;
    min_h    = buf_info.h < frame_h ? buf_info.h : frame_h;

    if (buf_info.h > frame_h)
        cp_offset = (buf_info.h - frame_h) * buf_info.w;

    if (buf_info.w > frame_w)
        cp_offset += buf_info.w - frame_w;

    if (buf_param.type == BC_MEMORY_MMAP) {
        for (idx = 0; idx < buf_info.n; idx++) {
            ioctl_var.input = idx;

            if (ioctl(bcfd, BCIOGET_BUFFERPHYADDR, &ioctl_var) != 0) {
                printf("ERROR: BCIOGET_BUFFERADDR failed\n");
                goto err_ret;
            }

            buf_paddr[idx] = ioctl_var.output;
            buf_vaddr[idx] = (char *)mmap(NULL, buf_size,
                              PROT_READ | PROT_WRITE, MAP_SHARED,
                              bcfd, buf_paddr[idx]);

            if (buf_vaddr[idx] == MAP_FAILED) {
                printf("ERROR: mmap failed\n");
                goto err_ret;
            }
        }
    }

    ret = 0;
    idx = 0;

    if (profiling == TRUE) {
        gettimeofday(&tvp, NULL);
        tv0 = tvp;
    }

    while (!gQuit) {
#ifdef USE_SOLID_PATTERN
        usleep(1000 * 1000);
#endif
        frame = frame_get(&buf_pa);

        if (frame == (char *) -1)
            break;

        if (frame) {
            if (buf_param.type == BC_MEMORY_MMAP) {
                for (ii = 0; ii < min_h; ii++)
                      /*FIXME calc stride instead of 2*/
                    memcpy(buf_vaddr[idx] + buf_info.w * 2 * ii + cp_offset,
                           frame + frame_w * 2 * ii, min_w * 2);
            }
            else    /*buf_param.type == BC_MEMORY_USERPTR*/
                idx = buf_pa.index;
        }

        drawCube(idx);

        if (frame_restore(frame))
            break;

#ifdef X11
        if (doX11Events())
            gQuit = TRUE;
#endif

        idx = (idx + 1) % buf_info.n;

        if (profiling == FALSE)
            continue;

        gettimeofday(&tv, NULL);

        fcount++;

        if (!(fcount % 60)) {
            tdiff = (unsigned long)(tv.tv_sec*1000 + tv.tv_usec/1000 -
                                tvp.tv_sec*1000 - tvp.tv_usec/1000);
            if (tdiff < 1800)   /*print fps every 2 sec*/
                continue;

            fprintf(stderr, "\rAvg FPS: %ld",
                    fcount / (tv.tv_sec - tv0.tv_sec));
            tvp = tv;
        }
    }
    printf("\n");

err_ret:
    if (buf_param.type == BC_MEMORY_MMAP) {
        for (idx = 0; idx < buf_info.n; idx++) {
            if (buf_vaddr[idx] != MAP_FAILED)
                munmap(buf_vaddr[idx], buf_size);
        }
    }
    if (bcfd > -1)
        close(bcfd);

    deInitEGL(buf_info.n);
#ifdef X11
    deInitX11();
#endif
    frame_cleanup();

    printf("done\n");
    return ret;
}

