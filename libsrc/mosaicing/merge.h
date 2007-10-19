/* Declarations for code shared between im_lrmerge() and im_tbmerge().
 */

/*

    Copyright (C) 1991-2003 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/* Number of entries in blend table. As a power of two as well, for >>ing.
 */
#define BLEND_SHIFT (10)
#define BLEND_SIZE (1<<BLEND_SHIFT)

/* How much we scale the int version up by.
 */
#define BLEND_SCALE (4096)

/* How many bits of precision we keep for transformations in im_affine().
 */
#define TRANSFORM_SHIFT (5)
#define TRANSFORM_SCALE (1<<TRANSFORM_SHIFT)

/* How many bits of precision we keep for interpolation.
 */
#define INTERPOL_SHIFT (13)
#define INTERPOL_SCALE (1<<INTERPOL_SHIFT)

/* Keep state for each call in one of these.
 */
typedef struct _Overlapping {
	IMAGE *ref;			/* Arguments */
	IMAGE *sec;
	IMAGE *out;
	int dx, dy;
	int mwidth;

	/* Ref and sec images, overlap, output area. We normalise these, so
	 * that the output image is always positioned at (0,0) - ie. all these
	 * coordinates are in output image space.
	 */
	Rect rarea;
	Rect sarea;
	Rect overlap;
	Rect oarea;
	int blsize;			/* Max blend length */
	int flsize;			/* first/last cache size */

	/* Sections of ref and sec which we use in output, excluding 
	 * overlap area.
	 */
	Rect rpart;
	Rect spart;

	/* Overlap start/end cache 
	 */
	GMutex *fl_lock;		/* Need to lock on build */
	int *first, *last;

	/* Blend function.
	 */
	int (*blend)();
} Overlapping;

/* Keep per-thread state here.
 */
typedef struct _MergeInfo {
	REGION *rir;			/* Two input regions */
	REGION *sir;

	float *from1;			/* IM_CODING_LABQ buffers */
	float *from2;
	float *merge;
} MergeInfo;

/* Params for similarity transformation.
 */
typedef struct {
	Rect iarea;			/* Area in input we can use */
	Rect oarea;			/* Area in output we generate */
	double a, b, c, d;		/* Transform */
	double dx, dy;

	double ia, ib, ic, id;		/* Inverse of matrix abcd */
} Transformation;

/* Functions shared between lr and tb.
 */
extern double *im__coef1;
extern double *im__coef2;
extern int *im__icoef1;
extern int *im__icoef2;
int im__make_blend_luts();

int im__attach_input( REGION *or, REGION *ir, Rect *area );
int im__copy_input( REGION *or, REGION *ir, Rect *area, Rect *reg );
Overlapping *im__build_mergestate( IMAGE *ref, IMAGE *sec, IMAGE *out, 
	int dx, int dy, int mwidth );
void *im__start_merge( IMAGE *out, void *, void * );
int im__merge_gen( REGION *or, void *seq, void *a, void * );
int im__stop_merge( void *seq, void *, void * );
void im__black_region( REGION *reg );
int im__extract_area( IMAGE *in, IMAGE *out,
	int x, int y, int w, int h );
int im__extract_band( IMAGE *in, IMAGE *out, int band );
int im__lrmerge( IMAGE *ref, IMAGE *sec, IMAGE *out, 
	int dx, int dy, int mwidth );
int im__tbmerge( IMAGE *ref, IMAGE *sec, IMAGE *out, 
	int dx, int dy, int mwidth );
int im__lrmerge1( IMAGE *ref, IMAGE *sec, IMAGE *out,
	double a, double b, double dx, double dy, 
	int mwidth );
int im__tbmerge1( IMAGE *ref, IMAGE *sec, IMAGE *out,
	double a, double b, double dx, double dy, 
	int mwidth );

/* similarity() stuff, used by mosaic1.
 */
int im__affine( IMAGE *in, IMAGE *out, Transformation *trn );

void im__transform_init( Transformation *trn );
int im__transform_calc_inverse( Transformation *trn );
int im__transform_isidentity( Transformation *trn );
void im__transform_forward( Transformation *trn, 
	double x, double y, double *ox, double *oy );
void im__transform_inverse( Transformation *trn, 
	double x, double y, double *ox, double *oy );
void im__transform_set_area( Transformation * );
int im__transform_add( Transformation *in1, Transformation *in2, 
	Transformation *out );
void im__transform_print( Transformation *trn );
