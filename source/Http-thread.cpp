/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <File.h>
#include <AppFileInfo.h>
#include <Resources.h>
#include <File.h>
#include <Path.h>
#include <syslog.h>
#include <string>
#include <SupportDefs.h>
#include <NetAddress.h>
#include <NetEndpoint.h>
#include <FindDirectory.h>

#include "MP3Query.h"
#include "pluginproto.h"
#include "HTTPRequest.h"
#include "constants.h"

extern int32 handlerfunc(void *data);

int32 http_handler(void *data)
{
	HTTPRequest *net=(HTTPRequest*)data;
	enum {
		NO_ERROR = 0,
		PARSE_ERROR,
		FILE_NOT_FOUND,
		FORK_ERROR
	} parse_error = PARSE_ERROR;

	
	string request;

	// read the client's request, adding it to the result string until 2 carriage returns are seen
	while (1) {
		char chunk[256];
		int32 numread = net->endpoint->Receive(chunk,sizeof(chunk)-1);
		chunk[numread]=0;
		request += chunk;
	
		if (strstr(chunk, "\r\n\r\n")) break;
	}

	// parse the request: dig out method, uri, and query string
	const char *request_str = request.c_str();
	
	char uri[1024];
	char *query_string = NULL; // I'll have to free() this later

	if (!strncmp(request_str, "GET ", 4)) {
		sscanf(request_str+4, "%1023s HTTP", uri);

		char *query_start = strchr(uri, '?');
		if (query_start) {
			query_string = strdup(query_start+1);
			*query_start = 0;
		}
		parse_error = NO_ERROR;
	} else if (!strncmp(request_str, "POST ", 5)) {
		sscanf(request_str+5, "%1023s HTTP", uri);

		// we've already read up to \r\n, so the content-length header is here: read it
		char *content_length_start = strstr(request_str, "\r\nContent-length: ");
		if (content_length_start) {
			int content_length;
			sscanf(content_length_start+18, "%d", &content_length);

			if (content_length>0) {
				query_string = (char*) calloc(1, content_length+1);
				// see where the content starts in what we've read so far.
				char *content_start = strstr(content_length_start, "\r\n\r\n")+4;

				int content_read = 0;
				memcpy (query_string, content_start, strlen(request_str)-(content_start - request_str));
				content_read += strlen(request_str)-(content_start - request_str);
				
				while (content_read != content_length) {
					char chunk[256];
					int32 numread = net->endpoint->Receive(chunk,sizeof(chunk)-1);
					memcpy(query_string + content_read, chunk, numread);
					content_read += numread;
				}	
			}
		}
		parse_error = NO_ERROR;
	}

	// do the request	
	if(parse_error == NO_ERROR) {
		bigtime_t requestStartTime = system_time();
		
		parse_error = FILE_NOT_FOUND;

		if (query_string) {
			char temp[strlen(query_string)+15];
			sprintf(temp, "QUERY_STRING=%s", query_string);
			putenv(temp);
		}
		
		// see if the filename is there under the document root
		char filename[2048];
		sprintf(filename, BIYS_SKIN_SELECTION"%s", uri);

		BEntry f(filename, true);
		
		// if it's a directory, add index.php
		if (f.Exists() && f.IsDirectory()) {
			sprintf(filename, BIYS_SKIN_SELECTION"%s/index.php", uri);
			f.SetTo(filename, true);
		}

		// if it's a file, return gif, jpeg, png as special cases, and send the rest through php		
		if (f.Exists() && f.IsFile()) {
			FILE *fd = NULL;
			char *content_type = NULL;
			char *status_line = "HTTP/1.0 200 OK\r\n";
			
			int length = strlen(filename);
			if (!strcmp(filename+length-4, ".gif")) {
				content_type = "Content-Type: image/gif\r\n\r\n";
			}
			if (!strcmp(filename+length-5, ".jpeg") || !strcmp(filename+length-4, ".jpg")) {
				content_type = "Content-Type: image/jpeg\r\n\r\n";
			}
			if (!strcmp(filename+length-4, ".png")) {
				content_type = "Content-Type: image/png\r\n\r\n";
			}
			if (!strcmp(filename+length-5, ".html")) {
				content_type = "Content-Type: text/html\r\n\r\n";
			}
			if (!strcmp(filename+length-5, ".css")) {
				content_type = "Content-Type: text/css\r\n\r\n";
			}
			if (!strcmp(filename+length-5, ".pls")) {
				content_type = "Content-Type: audio/x-scpls\r\n\r\n";
			}
			if (!strcmp(filename+length-4, ".txt")) {
				content_type = "Content-Type: text/plain\r\n\r\n";
			}

			bool headersSent = false;

			if (content_type) {
				net->endpoint->Send(status_line, strlen(status_line));
				net->endpoint->Send(content_type, strlen(content_type));
				headersSent = true;
				fd = fopen(filename, "r");
			} else {
				char buffer[2048];
				sprintf(buffer, "php %s", filename);
				fd = popen(buffer, "r");
				parse_error = FORK_ERROR;
			}
			
			if (fd) {
				int count;
				char buffer[2048];
				while((count = fread(buffer, 1, 2048, fd))>0) {
					if (!headersSent) {
						if (strstr(buffer, "Location: ")) {
							status_line = "HTTP/1.0 302 Found";
						}
						net->endpoint->Send(status_line, strlen(status_line));
						headersSent = true;
					}

					net->endpoint->Send(buffer, count);
				}

				if (content_type) {
					fclose(fd);
				} else {
					pclose(fd);
				}
				
				parse_error = NO_ERROR;
			}
		}
		
		// clear out any environment variables now
		putenv("QUERY_STRING=");
		
		if (parse_error != NO_ERROR) {
			string error;
			switch (parse_error) {
				case PARSE_ERROR:
					error = "<h1>Parse Error</h1>I wasn't able to parse the URL you gave: '";
					error += uri;
					error += "'.";
					break;
				case FILE_NOT_FOUND:
					error = "<h1>File not found</h1>I couldn't find the file for '";
					error += uri;
					error += "'.";
					error += "  I tried looking in ";
					error += filename;
					break;
				case FORK_ERROR:
					error = "<h1>Fork error</h1>I tried to fork PHP, but couldn't!";
					break;
					
				case NO_ERROR:
					// never comes here
					break;
			}
			
			error = 			"HTTP/1.0 400 ERROR\n"
			"Content-type: text/html\n"
			"Server: BeOS/SoundPlay\n\n"
			"<HTML>\n"
			"<HEAD>"
			"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=UTF-8\">"
			"<TITLE>ERROR</TITLE>"
			"</HEAD><body>"+error+"\n\n<P><a href=\"/\">Try again?</a><hr><a href=\"http://bang.dhs.org/be/biys/\">Be in your Stereo</a></BODY></HTML>\n";
			net->endpoint->Send(error.data(),error.length());
		}

		net->endpoint->Close();

		if (strncmp("/pics",uri,5)) {
			BNetAddress addrC= net->endpoint->RemoteAddr();
			in_addr addr;
			addrC.GetAddr(addr);
			syslog(LOG_INFO, "%d.%d.%d.%d - %Ld - %s", addr.s_addr & 0xff,
											 addr.s_addr>>8 & 0xff,
		                                     addr.s_addr>>16 & 0xff,
		                                     addr.s_addr>>24 & 0xff, system_time()-requestStartTime, uri);
		}
		delete net->endpoint;
		delete net;
		return B_OK;
	}
	return B_OK;
}
