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

typedef struct vmod_geoip_db_type {
    GeoIP    *ipv4;
    GeoIP    *ipv6;
} GeoipDB;

static void
cleanup_db(GeoipDB *db)
{
	if (db->ipv4)
		GeoIP_delete(db->ipv4);

	if (db->ipv6)
		GeoIP_delete(db->ipv6);

	free(db);
}

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
		pp->priv = malloc(sizeof(GeoipDB));
		AN(pp->priv);

		GeoipDB *db = (GeoipDB *)pp->priv;
		db->ipv4 = 0;
		db->ipv6 = 0;

		pp->free = (vmod_priv_free_f *)cleanup_db;

		db->ipv4 = GeoIP_open_type(GEOIP_COUNTRY_EDITION, GEOIP_MMAP_CACHE);
		AN(db->ipv4);
		GeoIP_set_charset(db->ipv4, GEOIP_CHARSET_UTF8);

		db->ipv6 = GeoIP_open_type(GEOIP_COUNTRY_EDITION_V6, GEOIP_MMAP_CACHE);
		AN(db->ipv6);
		GeoIP_set_charset(db->ipv6, GEOIP_CHARSET_UTF8);
	}

	return (0);
}

static const char *
vmod_country_code_by_addr(GeoipDB *gi, const char *ip)
{
	if (strchr(ip, ':') != NULL)
		return GeoIP_country_code_by_addr_v6(gi->ipv6, ip);
	else
		return GeoIP_country_code_by_addr(gi->ipv4, ip);
}

static const char *
vmod_country_name_by_addr(GeoipDB *gi, const char *ip)
{
	if (strchr(ip, ':') != NULL)
		return GeoIP_country_name_by_addr_v6(gi->ipv6, ip);
	else
		return GeoIP_country_name_by_addr(gi->ipv4, ip);
}

static const char *
vmod_region_name_by_addr(GeoipDB *gi, const char *ip)
{
	GeoIPRegion *gir;
	const char *region = NULL;

	if (strchr(ip, ':') != NULL)
		gir = GeoIP_region_by_addr_v6(gi->ipv6, ip);
	else
		gir = GeoIP_region_by_addr(gi->ipv4, ip);
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
GEOIP_PROPERTY(country_code, vmod_country_code_by_addr);
GEOIP_PROPERTY(country_name, vmod_country_name_by_addr);
GEOIP_PROPERTY(region_name, vmod_region_name_by_addr);
#undef GEOIP_PROPERTY
