/* Pi-hole: A black hole for Internet advertisements
*  (c) 2019 Pi-hole, LLC (https://pi-hole.net)
*  Network-wide ad blocking via your own hardware.
*
*  FTL Engine
*  API JSON macros
*
*  This file is copyright under the latest version of the EUPL.
*  Please see LICENSE file for your rights under this license. */

#include "../cJSON/cJSON.h"
#include "http.h"

// Provides a compile-time flag for JSON formatting
// This should never be needed as all modern browsers
// tyoically contain a JSON explorer
#ifdef JSON_FORMATTED
/* Examplary output:
{
	"queries in database":	70,
	"database filesize":	49152,
	"SQLite version":	"3.30.1"
}
*/
#define JSON_FORMATTER(object) cJSON_Print(object)
#else
/* Exemplary output
{"queries in database":70,"database filesize":49152,"SQLite version":"3.30.1"}
*/
#define JSON_FORMATTER(object) cJSON_PrintUnformatted(object)
#endif

#define JSON_NEW_OBJ() cJSON_CreateObject();

#define JSON_OBJ_ADD_STR(object, key,string){ \
	if(cJSON_AddStringToObject(json, key, (const char*)string) == NULL) \
	{ \
		cJSON_Delete(object); \
		send_http_error(conn); \
		return 500; \
	} \
}

#define JSON_OBJ_ADD_NUMBER(object, key, number){ \
	if(cJSON_AddNumberToObject(object, key, (double)number) == NULL) \
	{ \
		cJSON_Delete(object); \
		send_http_error(conn); \
		return 500; \
	} \
}

// cJSON_AddItemToObject() does not return anything
// Note that this operation transfers the ownership of the added item to the
// new parent so that when that array or object is deleted, it gets deleted as well.
#define JSON_OBJ_ADD_ITEM(object, key, item) cJSON_AddItemToObject(object, key, item)

#define JSON_SENT_OBJECT(object){ \
	const char* msg = JSON_FORMATTER(object); \
	if(msg == NULL) \
	{ \
		cJSON_Delete(object); \
		send_http_error(conn); \
		return 500; \
	} \
	send_http(conn, "application/json", msg); \
	return 200; \
}
