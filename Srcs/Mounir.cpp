/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mounir.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 20:12:14 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/05 13:36:45 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.hpp"
#include "server.hpp"

#define MIN_HEIGHT 0
#define ESC 27
#define BRACKET '['
#define UP 'A'
#define DOWN 'B'

// enter alternate screen buffer
#define ENTER_SCREEN "\x1b[?1049h"

// close alternate screen buffer
#define EXIT_SCREEN "\x1b[?1049l"

// show cursor
#define SHOW_CURSOR "\x1b[?25h"

// hide cursor
#define HIDE_CURSOR "\x1b[?25l"

// clear screen
#define CLEAR_SCREEN "\x1b[2J"

// move cursor home
#define MOVE_HOME "\x1b[H"

#define C1 "\x1b[31m"
#define C2 "\x1b[32m"
#define C3 "\x1b[33m"
#define C4 "\x1b[34m"
#define C5 "\x1b[35m"
#define C6 "\x1b[36m"
#define RE "\x1b[0m"

#include <stdint.h>

#define CRLF "\r\n"
static const std::string logo2 = \
CRLF \
"  ██╗██████╗  ██████╗    ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗"  CRLF \
"  ██║██╔══██╗██╔════╝    ██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗" CRLF \
"  ██║██████╔╝██║         ███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝" CRLF \
"  ██║██╔══██╗██║         ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗" CRLF \
"  ██║██║  ██║╚██████╗    ███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║" CRLF \
"  ╚═╝╚═╝  ╚═╝ ╚═════╝    ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\x1b[0m" CRLF;
//static std::size_t logo_height2 = 7;

static const std::string logo = \
CRLF \
"   ██▓ ██▀███   ▄████▄       ██████ ▓█████  ██▀███   ██▒   █▓▓█████  ██▀███"   CRLF \
"  ▓██▒▓██ ▒ ██▒▒██▀ ▀█     ▒██    ▒ ▓█   ▀ ▓██ ▒ ██▒▓██░   █▒▓█   ▀ ▓██ ▒ ██▒" CRLF \
"  ▒██▒▓██ ░▄█ ▒▒▓█    ▄    ░ ▓██▄   ▒███   ▓██ ░▄█ ▒ ▓██  █▒░▒███   ▓██ ░▄█ ▒" CRLF \
"  ░██░▒██▀▀█▄  ▒▓▓▄ ▄██▒     ▒   ██▒▒▓█  ▄ ▒██▀▀█▄    ▒██ █░░▒▓█  ▄ ▒██▀▀█▄"   CRLF \
"  ░██░░██▓ ▒██▒▒ ▓███▀ ░   ▒██████▒▒░▒████▒░██▓ ▒██▒   ▒▀█░  ░▒████▒░██▓ ▒██▒" CRLF \
"  ░▓  ░ ▒▓ ░▒▓░░ ░▒ ▒  ░   ▒ ▒▓▒ ▒ ░░░ ▒░ ░░ ▒▓ ░▒▓░   ░ ▐░  ░░ ▒░ ░░ ▒▓ ░▒▓░" CRLF \
"   ▒ ░  ░▒ ░ ▒░  ░  ▒      ░ ░▒  ░ ░ ░ ░  ░  ░▒ ░ ▒░   ░ ░░   ░ ░  ░  ░▒ ░ ▒░" CRLF \
"   ▒ ░  ░░   ░ ░           ░  ░  ░     ░     ░░   ░      ░░     ░     ░░   ░"  CRLF \
"   ░     ░     ░ ░               ░     ░  ░   ░           ░     ░  ░   ░"      CRLF \
"               ░                                         ░\x1b[0m"				CRLF;
static std::size_t logo_height = 11;

std::string color(uint8_t red, uint8_t green, uint8_t blue) {
	// generate true color escape sequence
	char buff[32];
	std::snprintf(buff, sizeof(buff), "\x1b[38;2;%d;%d;%dm", red, green, blue);
	return buff;
}

#include <cmath>

std::string cycle(void) {
	static float t = 0.0f;

    // Facteur de mélange pour adoucir la saturation
    float mixFactor = 0.5; // 0.0: aucune atténuation, 1.0: complètement gris

    // Réduire cette valeur pour des transitions plus longues
    float increment = 0.01;

    uint8_t r_raw = static_cast<uint8_t>((sin(t + 0) + 1) * 127.5);
    uint8_t g_raw = static_cast<uint8_t>((sin(t + 2) + 1) * 127.5);
    uint8_t b_raw = static_cast<uint8_t>((sin(t + 4) + 1) * 127.5);

    // Appliquer le facteur de mélange pour adoucir la saturation
    uint8_t r = static_cast<uint8_t>(r_raw * (1 - mixFactor) + 127.5 * mixFactor);
    uint8_t g = static_cast<uint8_t>(g_raw * (1 - mixFactor) + 127.5 * mixFactor);
    uint8_t b = static_cast<uint8_t>(b_raw * (1 - mixFactor) + 127.5 * mixFactor);

    t += increment;

    return color(r, g, b);
}

std::string Logger::background(void) {

	std::string buff(cycle());
	buff.append(logo);

	return buff;


	for (std::size_t i = 0; i < _height; ++i) {
		for (std::size_t j = 0; j < _width; ++j) {
			buff.append(" ");
		}
		buff.append(CRLF);
	}
	buff.append("\x1b[0m");

	return buff;
}





const char* ascii[] = {
	"[NUL]", "[SOH]", "[STX]", "[ETX]",
	"[EOT]", "[ENQ]", "[ACK]", "[BEL]",
	"[BS]",  "[HT]",  "[LF]",  "[VT]",
	"[FF]",  "[CR]",  "[SO]",  "[SI]",
	"[DLE]", "[DC1]", "[DC2]", "[DC3]",
	"[DC4]", "[NAK]", "[SYN]", "[ETB]",
	"[CAN]", "[EM]",  "[SUB]", "[ESC]",
	"[FS]",  "[GS]",  "[RS]",  "[US]",
	" ",   "!",   "\"",  "#",
	"$",   "%",   "&",   "'",
	"(",   ")",   "*",   "+",
	",",   "-",   ".",   "/",
	"0",   "1",   "2",   "3",
	"4",   "5",   "6",   "7",
	"8",   "9",   ":",   ";",
	"<",   "=",   ">",   "?",
	"@",   "A",   "B",   "C",
	"D",   "E",   "F",   "G",
	"H",   "I",   "J",   "K",
	"L",   "M",   "N",   "O",
	"P",   "Q",   "R",   "S",
	"T",   "U",   "V",   "W",
	"X",   "Y",   "Z",   "[",
	"\\",  "]",   "^",   "_",
	"`",   "a",   "b",   "c",
	"d",   "e",   "f",   "g",
	"h",   "i",   "j",   "k",
	"l",   "m",   "n",   "o",
	"p",   "q",   "r",   "s",
	"t",   "u",   "v",   "w",
	"x",   "y",   "z",   "{",
	"|",   "}",   "~",   "[DEL]",
	"\x80", "\x81", "\x82", "\x83",
	"\x84", "\x85", "\x86", "\x87",
	"\x88", "\x89", "\x8A", "\x8B",
	"\x8C", "\x8D", "\x8E", "\x8F",
	"\x90", "\x91", "\x92", "\x93",
	"\x94", "\x95", "\x96", "\x97",
	"\x98", "\x99", "\x9A", "\x9B",
	"\x9C", "\x9D", "\x9E", "\x9F",
	"\xA0", "\xA1", "\xA2", "\xA3",
	"\xA4", "\xA5", "\xA6", "\xA7",
	"\xA8", "\xA9", "\xAA", "\xAB",
	"\xAC", "\xAD", "\xAE", "\xAF",
	"\xB0", "\xB1", "\xB2", "\xB3",
	"\xB4", "\xB5", "\xB6", "\xB7",
	"\xB8", "\xB9", "\xBA", "\xBB",
	"\xBC", "\xBD", "\xBE", "\xBF",
	"\xC0", "\xC1", "\xC2", "\xC3",
	"\xC4", "\xC5", "\xC6", "\xC7",
	"\xC8", "\xC9", "\xCA", "\xCB",
	"\xCC", "\xCD", "\xCE", "\xCF",
	"\xD0", "\xD1", "\xD2", "\xD3",
	"\xD4", "\xD5", "\xD6", "\xD7",
	"\xD8", "\xD9", "\xDA", "\xDB",
	"\xDC", "\xDD", "\xDE", "\xDF",
	"\xE0", "\xE1", "\xE2", "\xE3",
	"\xE4", "\xE5", "\xE6", "\xE7",
	"\xE8", "\xE9", "\xEA", "\xEB",
	"\xEC", "\xED", "\xEE", "\xEF",
	"\xF0", "\xF1", "\xF2", "\xF3",
	"\xF4", "\xF5", "\xF6", "\xF7",
	"\xF8", "\xF9", "\xFA", "\xFB",
	"\xFC", "\xFD", "\xFE", "\xFF"
};


Logger::Logger(void)
: _server(NULL), _logs(), _started(false), _width(0), _height(0), _separator(),
	_origin(), _is_raw(false), _offset(0), _has_logo(true) {
#if defined(IRC_LOGGER)
	// query terminal size
	query_terminal_size();
	// set resize signal handler
	::signal(SIGWINCH, &Logger::terminal_resize_handler);
#endif
	// setup raw terminal
	setup_terminal();
}


void Logger::setup_terminal(void) {

	// get terminal settings
	if (::tcgetattr(STDIN_FILENO, &_origin) == -1)
		return;

	// set terminal settings
	struct termios raw = _origin;

	// disable canonical mode and echo
	raw.c_lflag &= ~(ICANON | ECHO);

	// set terminal to raw mode
	if (::tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1)
		return;

	_is_raw = true;
}

Logger::~Logger(void) {
	// restore terminal settings
	if (_is_raw)
		::tcsetattr(STDIN_FILENO, TCSANOW, &_origin);
}

void Logger::terminal_resize_handler(int) {
	Logger& logger = shared();
	logger.query_terminal_size();
	logger.render_impl();
}

void Logger::query_terminal_size(void) {
	struct winsize w;
	if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
		return;
	_width = w.ws_col;
	_height = w.ws_row;
	regenerate_separator();
}




void Logger::start(void) {
#if defined(IRC_LOGGER)
	Logger& logger = shared();
	if (logger._started) return;
	static const std::string msg = \
		ENTER_SCREEN \
		HIDE_CURSOR \
		CLEAR_SCREEN \
		MOVE_HOME;
	std::cout.write(msg.c_str(), msg.size());
	logger._started = true;
#endif
}

void Logger::end(void) {
#if defined(IRC_LOGGER)
	Logger& logger = shared();
	if (not logger._started) return;
	const std::string msg = \
		SHOW_CURSOR \
		EXIT_SCREEN;
	std::cout.write(msg.c_str(), msg.size());
	logger._started = false;
#endif
}

Logger& Logger::shared(void) {
	static Logger logger;
	return logger;
}

void Logger::set_server(Server& server) {
	shared()._server = &server;
}


void Logger::recv(const std::string& line) {
#if defined(IRC_LOGGER_IGNORE_PING_PONG)
	if (line.find("PING") != std::string::npos
	or line.find("PONG") != std::string::npos)
		return;
#endif
	static_cast<void>(line);
#if defined(IRC_LOGGER_RECV)
	shared().log(C2 "   [recv] " RE, line);
#endif
}



void Logger::send(const std::string& line) {
#if defined(IRC_LOGGER_IGNORE_PING_PONG)
	if (line.find("PING") != std::string::npos
	or line.find("PONG") != std::string::npos)
		return;
#endif
	static_cast<void>(line);
#if defined(IRC_LOGGER_SEND)
	shared().log(C3 "   [send] " RE, line);
#endif
}

void Logger::warning(const std::string& line) {
	static_cast<void>(line);
#if defined(IRC_LOGGER_WARNING)
	shared().log(C1 "[warning] " RE, line);
#endif
}

void Logger::error(const std::string& line) {
	static_cast<void>(line);
#if defined(IRC_LOGGER_ERROR)
	shared().log(C1 "  [error] " RE, line);
#endif
}

void Logger::info(const std::string& line) {
	static_cast<void>(line);
#if defined(IRC_LOGGER_INFO)
	shared().log(C4 "   [info] " RE, line);
#endif
}

void Logger::debug(const std::string& line) {
	static_cast<void>(line);
#if defined(IRC_LOGGER_DEBUG)
	shared().log(C5 "   [DEBUG] " RE, line);
#endif
}



void Logger::log(const char* head, const std::string& line) {

	std::string log(head);

	log.append(clean_string(line));

#if defined(IRC_LOGGER)
	std::size_t len = compute_length(log);
	_logs.push_back(log_line(log, len));
#else
	std::cout << log << "\n";
#endif
}




void Logger::regenerate_separator(void) {
	_separator.clear();
	for (std::size_t i = 0; i < _width; ++i)
		_separator.push_back('-');
	_separator.append(CRLF);
}


void Logger::render(void) {
#if defined(IRC_LOGGER)
	Logger& logger = shared();
	logger.render_impl();
#endif
}


std::size_t Logger::is_escape_sequence(const char* ptr) {
	// check if escape sequence
	if (*ptr != 27 || *(ptr + 1) != '[') return 0;
	// init length
	std::size_t len = 2;
	// offset ptr
	ptr += 2;
	// loop until 'm' is found + count length
	while (*ptr != 'm') { ++len; ++ptr; }
	// return length
	return len;
}

std::size_t Logger::compute_length(const std::string& line) {

	if (line.empty()) return 0;

	const char* ptr = line.c_str();
	const char* end = ptr + line.size();
	std::size_t len = 0;

	while (ptr != end) {

		std::size_t esc = is_escape_sequence(ptr);
		if (esc) { ptr += esc;   }
		else     { ++len; ++ptr; }
	}

	return len;
}



/* clean string */
std::string Logger::clean_string(const std::string& line) {

	if (line.empty()) return line;

	std::string clean;
	clean.reserve(line.size());

	const char* ptr = line.c_str();
	const char* end = ptr + line.size();

	while (ptr != end) {

		std::size_t esc = is_escape_sequence(ptr);
		if (esc) {
			clean.append(ptr, esc);
			ptr += esc;
		}
		else {
			clean.append(ascii[static_cast<unsigned char>(*ptr)]);
			++ptr;
		}
	}
	return clean;
}

std::string Logger::extract(const std::string& line, const std::size_t len) {

	std::size_t count = 0;
	std::size_t i = 0;

	while (i < line.size() && count < len) {

		std::size_t esc = is_escape_sequence(&(line[i]));

		if (esc) { i += esc; continue; }

		++count;
		++i;
	}
	return line.substr(0, i);
}


void Logger::read_stdin(void) {

	char buff[BUFF_SIZE + 1];

	const int readed = ::read(STDIN_FILENO, buff, BUFF_SIZE);

	if (readed < 1) { return; }

	if (readed == 1) {

		switch (buff[0]) {
			case 'q':
				if (_server) _server->stop();
				break;
			default: break;
		}

	}
	else if (readed == 3) {

		if (buff[0] == ESC && buff[1] == BRACKET) {

			switch (buff[2]) {
				case UP:
					_offset += (_logs.size() - _offset > remaining_height()) ? 1 : 0;
					break;
				case DOWN:
					_offset -= (_offset > 0) ? 1 : 0;
					break;
				default: break;
			}
		}
	}
}


// -- public override methods ------------------------------------------------

/* read */
void Logger::read(void) {
	read_stdin();
}

/* get file descriptor */
int Logger::fd(void) const {
	return STDIN_FILENO;
}

/* disconnect */
void Logger::disconnect(void) {}

void Logger::write(void) {}


// -- private accessors ------------------------------------------------------

/* header height */
std::size_t Logger::header_height(void) const {
	static const std::size_t header_height = \
		logo_height + // logo
		1 + // total lines
		1 + // server name
		1 + // server node
		1 + // server service
		1 + // server connections
		1;  // separator
	return header_height;
}

/* remaining height */
std::size_t Logger::remaining_height(void) const {
	return _height - header_height();
}




void Logger::render_impl(void) {

	if (not _started) Logger::start();

	static std::string buff;
	buff.clear();

	/* clear screen + move home */
	buff.append(CLEAR_SCREEN MOVE_HOME);


	if (_height < header_height() + MIN_HEIGHT) {
		buff.append("terminal too small");
		std::cout.write(buff.c_str(), buff.size());
		std::cout.flush();
		return;
	}



	buff.append(background());
	// buff.append(logo);
	buff.append(_separator);
	buff.append(header());
	buff.append(_separator);

	std::size_t lsize;
	std::size_t woffset;

	if (_logs.size() > remaining_height()) {

		lsize = _logs.size();
		woffset = (lsize - remaining_height());

		if (_offset > woffset) {
			woffset = 0;
			lsize = remaining_height();
		}
		else {
			woffset = woffset - _offset;
			lsize = woffset + remaining_height();
		}
	}
	else {
		_offset = 0;
		woffset = 0;
		lsize = _logs.size();
	}

	for (std::size_t i = woffset; i < lsize; ++i) {

		log_line& log = _logs[i];

		if (log.length > _width)
			buff.append(extract(log.line, _width));
		else
			buff.append(_logs[i].line);
		buff.append(CRLF);
	}

	// remove last newline
	std::cout.write(buff.c_str(), buff.size() - 1);
	std::cout.flush();
}



const std::string& Logger::header(void) const {

	static std::string hd;

	hd.clear();

	if (not _server) {
		hd.assign(C1 "logger: " RE "no server set :(" CRLF);
	}
	else {

		// get server name
		hd.append(C1 "       server: " RE);
		hd.append(_server->name());
		hd.append(CRLF);

		// get address
		hd.append(C1 "         node: " RE);
		hd.append(_server->address());
		hd.append(CRLF);

		// get port
		hd.append(C1 "      service: " RE);
		hd.append(_server->port());
		hd.append(CRLF);

		// get number of connections
		hd.append(C1 "  connections: " RE);
		hd.append(utils::to_string(_server->get_nb_conns()));
		hd.append(CRLF);

	}

	return hd;
}
