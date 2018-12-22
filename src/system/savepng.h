#ifndef _SDL_SAVEPNG
#define _SDL_SAVEPNG
/*
 * SDL_SavePNG -- libpng-based SDL_Surface writer.
 *
 * This code is free software, available under zlib/libpng license.
 * http://www.libpng.org/pub/png/src/libpng-LICENSE.txt
 */

#ifdef __cplusplus
extern "C" { /* This helps CPP projects that include this header */
#endif

/*
 * Save an SDL_Surface as a PNG file.
 *
 * Returns 0 success or -1 on failure, the error message is then retrievable
 * via SDL_GetError().
 */
	SDL_SavePNG_RW(surface, SDL_RWFromFile(file, "wb"), 1)

/*
 * Save an SDL_Surface as a PNG file, using writable RWops.
 * 
 * surface - the SDL_Surface structure containing the image to be saved
 * dst - a data stream to save to
 * freedst - non-zero to close the stream after being written
 *
 * Returns 0 success or -1 on failure, the error message is then retrievable
 * via SDL_GetError().
 */

/*
 * Return new SDL_Surface with a format suitable for PNG output.
 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

#define SDL_SavePNG(surface, file) \

extern SDL_Surface *SDL_PNGFormatAlpha(SDL_Surface *src);
extern int SDL_SavePNG_RW(SDL_Surface *surface, SDL_RWops *rw, int freedst);
#define SDL_SavePNG(surface, file) \
