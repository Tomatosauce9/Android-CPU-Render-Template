// --------------------------------------------------------------------------------
// Blend2D rendering backend for Nuklear
// by GlideCode (https://github.com/glide-code)
//
// Useful links:
// Nuklear : https://github.com/Immediate-Mode-UI/Nuklear
// Blend2D : https://github.com/blend2d/blend2d
//
// Version: 1.0.0
// - Basic rendering capabilities
// - No support for multi coloured rectangles
// --------------------------------------------------------------------------------

#ifndef NUKLEAR_BLEND2D_H
#define NUKLEAR_BLEND2D_H

#include <blend2d.h>
#ifndef NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_FIXED_TYPES
#endif // NK_INCLUDE_FIXED_TYPES
#include "nuklear.h"


// --------------------------------------------------------------------------------
// Initialize Blend2D rendering backend
//
// Parameters:
// font : reference to a BLFont object with a valid font
//
// Return:
// It returns an nk_context struct on success
// --------------------------------------------------------------------------------

NK_API struct nk_context nk_blend2d_init(BLFont& font);

// --------------------------------------------------------------------------------
// Renders Nuklear UI components on the given BLContext object
//
// Parameters:
// nctx : pointer to an nk_context struct that holds the current context data to be
//        rendered
// bctx : reference to a BLContext object where the data is to be rendered
//
// Return:
// It doesn't return anything. It's obvious from the 'void' type, isn't it? :)
// --------------------------------------------------------------------------------

NK_API void nk_blend2d_render(struct nk_context* nctx, BLContext& bctx);
NK_API BLFont nk_zh_font_init();

#endif // NUKLEAR_BLEND2D_H
