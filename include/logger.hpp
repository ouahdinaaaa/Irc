#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

# include <signal.h>
# include <sys/ioctl.h>
# include <termios.h>
# include "io.event.hpp"
# include <sys/socket.h>


#define IRC_LOGGER_INFO
#define IRC_LOGGER_WARNING
#define IRC_LOGGER_DEBUG
#define IRC_LOGGER_ERROR
#define IRC_LOGGER_RECV
#define IRC_LOGGER_SEND

#define IRC_LOGGER_IGNORE_PING_PONG


class Server;

class Logger : public IOEvent {

	private:

		struct log_line {
			log_line(const std::string& line, std::size_t length)
			: line(line), length(length) { }
			std::string line;
			std::size_t length;
		};


	public:

		/* start logging */
		static void start(void);

		/* end logging */
		static void end(void);


		static void recv(const std::string&);
		static void send(const std::string&);
		static void error(const std::string&);
		static void info(const std::string&);
		static void debug(const std::string&);
		static void warning(const std::string&);

		/* print all logs */
		static void render(void);

		static void set_server(Server&);



		// -- override methods -------------------------------------------------

		void read(void);

		int fd(void) const;

		void disconnect(void);

		void write(void);

		static Logger& shared(void);


	private:

		/* default constructor */
		Logger(void);

		/* destructor */
		~Logger(void);


		/* log a line */
		void log(const char*, const std::string&);


		// -- private methods --------------------------------------------------

		/* read stdin */
		void read_stdin(void);

		/* render logs */
		void render_impl(void);

		/* generate header */
		const std::string& header(void) const;

		/* get terminal size */
		void query_terminal_size(void);

		/* generate separator */
		void regenerate_separator(void);

		/* setup terminal */
		void setup_terminal(void);


		// -- private accessors ------------------------------------------------

		std::string background(void);

		/* header height */
		std::size_t header_height(void) const;

		/* remaining height */
		std::size_t remaining_height(void) const;


		// -- private methods --------------------------------------------------

		/* compute length of a string */
		static std::size_t compute_length(const std::string&);

		/* clean string */
		static std::string clean_string(const std::string&);

		/* is escape sequence */
		static std::size_t is_escape_sequence(const char*);

		/* extract */
		static std::string extract(const std::string&, const std::size_t);


		static void terminal_resize_handler(int);


		// -- private members --------------------------------------------------

		/* server */
		Server* _server;

		/* log lines */
		std::vector<log_line> _logs;
		// std::vector<std::string> _logs;

		/* is started */
		bool _started;

		/* terminal width */
		std::size_t _width;

		/* terminal height */
		std::size_t _height;

		/* line separator */
		std::string _separator;

		/* original terminal settings */
		struct termios _origin;

		/* is terminal in raw mode */
		bool _is_raw;

		/* offset */
		std::size_t _offset;

		/* has logo */
		bool _has_logo;


		enum { BUFF_SIZE = 1024 };




};

#endif