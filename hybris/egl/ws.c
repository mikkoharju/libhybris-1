/*
 * Copyright (C) 2013 libhybris
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ws.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static struct ws_module *ws = NULL;

static void _init_ws()
{
	if (ws == NULL)
	{
		char ws_name[2048];
		char *egl_platform;

		// Mesa uses EGL_PLATFORM for its own purposes.
		// Add HYBRIS_EGLPLATFORM to avoid the conflicts
		egl_platform=getenv("HYBRIS_EGLPLATFORM");

		if (egl_platform == NULL)
			egl_platform=getenv("EGL_PLATFORM");

		if (egl_platform == NULL)
			egl_platform = DEFAULT_EGL_PLATFORM;

		snprintf(ws_name, 2048, PKGLIBDIR "eglplatform_%s.so", egl_platform);

		void *wsmod = (void *) dlopen(ws_name, RTLD_LAZY);
		if (wsmod==NULL)
		{
			fprintf(stderr, "ERROR: %s\n\t%s\n", ws_name, dlerror());
			assert(0);
		}
		ws = dlsym(wsmod, "ws_module_info");
		assert(ws != NULL);
		ws->init_module(&hybris_egl_interface);
	}
}


struct _EGLDisplay *ws_GetDisplay(EGLNativeDisplayType display)
{
	_init_ws();
	return ws->GetDisplay(display);
}

void ws_Terminate(struct _EGLDisplay *dpy)
{
	_init_ws();
	ws->Terminate(dpy);
}

EGLNativeWindowType ws_CreateWindow(EGLNativeWindowType win, struct _EGLDisplay *display)
{
	_init_ws();
	return ws->CreateWindow(win, display);
}

void ws_DestroyWindow(EGLNativeWindowType win)
{
	_init_ws();
	return ws->DestroyWindow(win);
}

__eglMustCastToProperFunctionPointerType ws_eglGetProcAddress(const char *procname)
{
	_init_ws();
	return ws->eglGetProcAddress(procname);
}

void ws_passthroughImageKHR(EGLContext *ctx, EGLenum *target, EGLClientBuffer *buffer, const EGLint **attrib_list)
{
	_init_ws();
	return ws->passthroughImageKHR(ctx, target, buffer, attrib_list);
}

const char *ws_eglQueryString(EGLDisplay dpy, EGLint name, const char *(*real_eglQueryString)(EGLDisplay dpy, EGLint name))
{
	_init_ws();
	return ws->eglQueryString(dpy, name, real_eglQueryString);
}

void ws_prepareSwap(EGLDisplay dpy, EGLNativeWindowType win, EGLint *damage_rects, EGLint damage_n_rects)
{
	_init_ws();
	if (ws->prepareSwap)
		ws->prepareSwap(dpy, win, damage_rects, damage_n_rects);
}

void ws_finishSwap(EGLDisplay dpy, EGLNativeWindowType win)
{
	_init_ws();
	if (ws->finishSwap)
		ws->finishSwap(dpy, win);
}

void ws_setSwapInterval(EGLDisplay dpy, EGLNativeWindowType win, EGLint interval)
{
	_init_ws();
	if (ws->setSwapInterval)
		ws->setSwapInterval(dpy, win, interval);
}

#define HYBRIS_MAKE_TOKEN(a, b, c, d) ((((unsigned int) a) << 24) \
                                      | (((unsigned int) b) << 16) \
                                      | (((unsigned int) c) << 8) \
                                      | ((unsigned int) d))

#define HYBRIS_EGL_IMAGE_ID HYBRIS_MAKE_TOKEN('H', 'I', 'M', 'G')

struct egl_image *egl_image_create()
{
    struct egl_image *img = (struct egl_image *) malloc(sizeof(struct egl_image));
    img->__type_token = HYBRIS_EGL_IMAGE_ID;
    return img;
}

int egl_image_sanitycheck(struct egl_image *image)
{
    if (image && image->__type_token == HYBRIS_EGL_IMAGE_ID)
        return 1;
    return 0;
}


// vim:ts=4:sw=4:noexpandtab
