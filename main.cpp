#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <iostream>

const int NAGIOS_OK=0;
const int NAGIOS_WARN=1;
const int NAGIOS_CRIT=2;
const int NAGIOS_UNKNOWN=3;
static bool verbose=false;

std::string talk(std::string host, int port, std::string message) {
	boost::asio::io_service ios;
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::ip::tcp::socket socket(ios);

	try {
		endpoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port);
	}
	catch (boost::system::system_error& e) {
		std::cerr << "Error while resolving: " << e.what() << std::endl;
		return std::string("ERROR RESOLVING") + e.what();
	}
	
	try {
		socket.connect(endpoint);
	}
	catch (boost::system::system_error& e) {
		std::cerr << "Error while connecting: " << e.what() << std::endl;
		return std::string("ERROR CONNECTING") + e.what();
	}
	
	boost::array<char, 128> buf;
	boost::array<char, 4096> readbuf;
	std::copy(message.begin(),message.end(),buf.begin());
	boost::system::error_code error;
	socket.write_some(boost::asio::buffer(buf, message.size()), error);
	if (error!=0) return "ERROR WRITING";
	int rsize=socket.read_some(boost::asio::buffer(readbuf, readbuf.size()),error);
	if (error!=0) return "ERROR READING";
	socket.close();
	return std::string(readbuf.begin(),readbuf.begin()+rsize);
}

int parse_status(std::string status) {
	std::string res("UNKNOWN");
	int outcode=NAGIOS_UNKNOWN; // unknown
	
	std::vector<std::string> stat_array;
	trim_if(status, boost::is_any_of("\n\r"));
	split(stat_array, status, boost::is_any_of("\n\r"), boost::token_compress_on);
	boost::regex expr{"(\\w+)\\s+(\\w+)"};

	for (std::vector<std::string>::iterator i=stat_array.begin(); i!=stat_array.end(); i++) {
		boost::smatch m;
		if (boost::regex_match(*i, m, expr)) {
			if (m[1]=="zk_server_state") {
				outcode=NAGIOS_OK;
				res=m[2];
			}
			if (verbose) std::cout<<(*i)<< ":" << m[1] << "," << m[2] << std::endl;
		}
	}
	std::cout << outcode << ":" << res << std::endl;
	return outcode;
}

int main(int argc, char **argv) {
	boost::program_options::options_description desc{"check_zk Options"};
	desc.add_options()
		("help,h", "Help screen")
		("host,H", boost::program_options::value<std::string>()->default_value("127.0.0.1"), "Host address")
// 		("verbose,v", boost::program_options::value<bool>()->default_value(false), "Verbose output")
		("verbose,v", "Verbose output")
		("port,p", boost::program_options::value<int>()->default_value(2181), "Port");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);    

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}
	if (vm.count("verbose"))
		verbose=true;
	
	std::string status=talk(vm["host"].as<std::string>(),vm["port"].as<int>(),"mntr");
	if (boost::starts_with(status,"ERROR")) {
		std::cout << status << std::endl;
		return NAGIOS_CRIT; // critical
	}
	int outcode=parse_status(status);
	return outcode;
}
