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

#include "vcl.h"
#include "vrt.h"
#include "vrt_obj.h"
#include "cache/cache.h"

#include "vcc_if.h"

// The default string in case the GeoIP lookup fails
#define GI_UNKNOWN_STRING "Unknown"

int __match_proto__(vmod_event_f)
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

VCL_STRING
vmod_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp,
    VCL_STRING ip)
{
	const char *country = NULL;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	AN(pp->priv);

	if (ip)
		country = GeoIP_country_code_by_addr((GeoIP *)pp->priv, ip);

	return (WS_Copy(ctx->ws, (country ? country : GI_UNKNOWN_STRING), -1));
}

VCL_STRING
vmod_ip_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp,
    VCL_IP ip)
{
	return (vmod_country_code(ctx, pp, VRT_IP_string(ctx, ip)));
}

VCL_STRING
vmod_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp,
    VCL_STRING ip)
{
	const char *country = NULL;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	AN(pp->priv);

	if (ip)
		country = GeoIP_country_name_by_addr((GeoIP *)pp->priv, ip);

	return (WS_Copy(ctx->ws, (country ? country : GI_UNKNOWN_STRING), -1));
}

VCL_STRING
vmod_ip_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp,
    VCL_IP ip)
{
	return (vmod_country_name(ctx, pp, VRT_IP_string(ctx, ip)));
}

VCL_STRING
vmod_region_name(const struct vrt_ctx *ctx, struct vmod_priv *pp,
    VCL_STRING ip)
{
	GeoIPRegion *gir;
	const char *region = NULL;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	AN(pp->priv);

	if (ip) {
		if ((gir = GeoIP_region_by_addr((GeoIP *)pp->priv, ip))) {
			region =
			    GeoIP_region_name_by_code(gir->country_code,
			    gir->region);
			// TODO: is gir * a local copy or the actual record?
			GeoIPRegion_delete(gir);
		}
	}

	return (WS_Copy(ctx->ws, (region ? region : GI_UNKNOWN_STRING), -1));
}

VCL_STRING
vmod_ip_region_name(const struct vrt_ctx *ctx, struct vmod_priv *pp,
    VCL_IP ip)
{
	return (vmod_region_name(ctx, pp, VRT_IP_string(ctx, ip)));
}
