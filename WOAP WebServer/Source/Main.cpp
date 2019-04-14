#include "../JuceLibraryCode/JuceHeader.h"
#include "mongoose.h"

static const char *s_http_port = "80";
static struct mg_serve_http_opts s_http_server_opts;

static void ev_handler(struct mg_connection *nc, int ev, void *p) 
{
	if (ev == MG_EV_HTTP_REQUEST) 
	{
		mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
	}
}

int main(void) 
{
	struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) 
	{
		printf("Failed to start.\n");
		return 1;
	}

	printf("WebServer running at port %s\n", s_http_port);

	const String appPath = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getParentDirectory().getFullPathName();

	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.index_files = "index.html";
	s_http_server_opts.document_root = appPath.toRawUTF8();
	s_http_server_opts.per_directory_auth_file = NULL;
	s_http_server_opts.global_auth_file = NULL;
	s_http_server_opts.dav_document_root = NULL;
	s_http_server_opts.dav_auth_file = NULL;
	s_http_server_opts.hidden_file_pattern = "**.exe$";
	s_http_server_opts.cgi_file_pattern = NULL;
	s_http_server_opts.cgi_interpreter = NULL;
	s_http_server_opts.enable_directory_listing = "no";

	for (;;) 
	{
		if (mg_mgr_poll(&mgr, 1000) == 0)
			break;
	}
	mg_mgr_free(&mgr);

	return 0;
}