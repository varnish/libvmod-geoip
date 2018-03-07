/**
 * libvmod-geoip - varnish interface to MaxMind's GeoIP library
 * GeoIP API: http://www.maxmind.com/app/c
 *
 * See file README.rst for usage instructions
 *
 * This code is licensed under a MIT-style License, see file LICENSE
*/

#include <stdlib.h>
#include <GeoIP.h>

#include <cache/cache.h>

#ifndef VRT_H_INCLUDED
#  include <vrt.h>
#endif

#ifndef VDEF_H_INCLUDED
#  include <vdef.h>
#endif

#include <vcl.h>
#include "vrt_obj.h"

#include "vcc_if.h"

int
vmod_event(VRT_CTX, struct vmod_priv *pp, enum vcl_event_e evt)
{

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	if (pp->priv == NULL) {
		xxxassert(evt == VCL_EVENT_LOAD);

		/* The README says:
		 * If GEOIP_MMAP_CACHE doesn't work on a 64bit machine, try
		 * adding * the flag "MAP_32BIT" to the mmap call. MMAP is not
		 * avail for WIN32.
		 */
		pp->priv = GeoIP_new(GEOIP_MMAP_CACHE);
		AN(pp->priv);
		pp->free = (vmod_priv_free_f *)GeoIP_delete;
		GeoIP_set_charset((GeoIP *)pp->priv, GEOIP_CHARSET_UTF8);
	}

	return (0);
}

static const char *
vmod_region_name_by_addr(GeoIP *gi, const char *ip)
{
	GeoIPRegion *gir;
	const char *region = NULL;

	gir = GeoIP_region_by_addr(gi, ip);
	if (gir == NULL)
		return (NULL);

	region = GeoIP_region_name_by_code(gir->country_code, gir->region);
	GeoIPRegion_delete(gir);
	return (region);
}

#define GEOIP_PROPERTY(prop, func)					\
	VCL_STRING							\
	vmod_##prop(VRT_CTX, struct vmod_priv *pp, VCL_STRING ip)	\
	{								\
		const char *str = NULL;					\
									\
		CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);			\
		AN(pp->priv);						\
									\
		if (ip)							\
			str = func(pp->priv, ip);			\
		if (str == NULL)					\
			str = "Unknown";				\
		return (str);						\
	}
GEOIP_PROPERTY(country_code, GeoIP_country_code_by_addr);
GEOIP_PROPERTY(country_name, GeoIP_country_name_by_addr);
GEOIP_PROPERTY(region_name, vmod_region_name_by_addr);
#undef GEOIP_PROPERTY
