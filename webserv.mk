SRC_FILES	= 	webserv.cpp					\
				configuration.cpp			\
				location.cpp				\
				server.cpp					\
				hub.cpp						\
				request.cpp					\
				uri.cpp						\
				html.cpp					\
				configMatch.cpp				\
				response.cpp				\
				responseConstructor.cpp		\
				cgiSocketFromCgi.cpp		\
				cgiSocketToCgi.cpp			\
				clientSocket.cpp			\
				listeningSocket.cpp			\
				socket.cpp					\
				httpStatus.cpp				\
				file.cpp					\
				log.cpp						\
				mime.cpp					\
				str.cpp						\
				cgiConstructor.cpp			\
				cgiExecutor.cpp

INC_FILES	= 	srcs/webserv.hpp									\
				srcs/config/configuration/configuration.hpp			\
				srcs/config/location/location.hpp					\
				srcs/config/server/server.hpp						\
				srcs/config/hub.hpp									\
				srcs/config/request/request.hpp						\
				srcs/config/request/uri.hpp							\
				srcs/html/html.hpp									\
				srcs/response/configMatch.hpp						\
				srcs/response/response.hpp							\
				srcs/response/responseConstructor.hpp				\
				srcs/socket/cgiSocketFromCgi.hpp					\
				srcs/socket/cgiSocketToCgi.hpp						\
				srcs/socket/clientSocket.hpp						\
				srcs/socket/listeningSocket.hpp						\
				srcs/socket/socket.hpp								\
				srcs/httpStatus/httpStatus.hpp						\
				srcs/utils/file/file.hpp							\
				srcs/utils/log/log.hpp								\
				srcs/utils/mime/mime.hpp							\
				srcs/utils/singleton/singleton.hpp					\
				srcs/utils/str/str.hpp								\
				srcs/cgi/cgiConstructor.hpp							\
				srcs/cgi/cgiExecutor.hpp
