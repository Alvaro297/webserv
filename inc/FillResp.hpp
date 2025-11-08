#ifndef FILLRESP_HPP
#define FILLRESP_HPP

#include <iostream>
#include "Response.hpp"
#include "Request.hpp"

class FillResp
{
	public:
		static void	set200(Response &res, Request &req, std::string &answer) {
		res.setStatus(200, "OK");
		res.setHeader("Content-Type", req.getType());
		res.setBody(answer);
		}

		static void	set201(Response &res, Request &req) {
			res.setStatus(201, "Created");
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>201 Created</h1>");
		}

		static void	set400(Response &res, Request &req) {
			res.setStatus(400, "Bad Request");
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>400 Bad Request</h1>");
		}

		static void	set403(Response &res, Request &req) {
			res.setStatus(403, "Forbidden");
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>403 Forbidden</h1>");
		}

		static void	set404(Response &res, Request &req) {
			res.setStatus(404, "Not Found");
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>404 Not Found</h1>");
		}

		static void	set405(Response &res, Request &req) {
			res.setStatus(405, "Method Not Allowed");
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>405 Method Not Allowed</h1>");
		}

		static void	set500(Response &res, Request &req) {
			res.setStatus(500, "Internal Server Error");
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>500 Internal Server Error</h1>");
		}

		static void	set505(Response &res, Request &req) {
			res.setStatus(505, "HTTP Version Not Supported");
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>505 Version Not Supported</h1>");
		}
};

#endif