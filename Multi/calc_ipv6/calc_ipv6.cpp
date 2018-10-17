#include <iostream>
#include <vector>
#include <sstream>
#include <bitset>

#include "OS.h"   // Found on "https://github.com/Radicalware"
#include "SYS.h"  // Found on "https://github.com/Radicalware"
#include "re.h"   // Found on "https://github.com/Radicalware"
#include "ord.h"  // Found on "https://github.com/Radicalware"

using std::cout;
using std::endl;
using std::string;
using std::vector;



/*
* Copyright[2018][Joel Leagues aka Scourge]
* Scourge /at\ protonmail /dot\ com
* www.Radicalware.com
* https://www.youtube.com/channel/UCivwmYxoOdDT3GmDnD0CfQA/playlists
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http ://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

extern SYS sys;

int main(int argc, char** argv){

	sys.set_args(argc, argv);

	if (argc == 1 || argc > 5 || !sys("-mac") || !sys("-ipv6")){
		cout << R"(
Calculate the IPv6 Link Local Address from a
1.) -IPv6 known address or -IPv6 network address
2.) -mac address of your target

	2 required arguments

	-ipv6 <ipv6>
	-mac <mac>

example from ippSec's HTB
./calc_ipv6 -ipv6 fe80:: -mac 00:50:56:aa:8a:25

)";
		return 1;
	}

	string ipv6_start = sys["-ipv6"][0];
	string mac_str  = re::sub(":","",sys["-mac"][0]);

	if (re::scan("::", mac_str) || re::count("::",mac_str) > 5){
		cout << "It looks like you may have used the -ipv6 in place of the -mac\n"; return 1;
	}

	if((re::count(":", ipv6_start) != 7 && re::scan("::", ipv6_start) == false) || re::count(":",ipv6_start) > 7){
		cout << "You don't have the right amout of IPv6 segments\n"; return 1;
	}

	vector<string> split_ipv6 = re::split(":",ipv6_start);
	int colons = re::char_count(':',ipv6_start); // 

	vector<string> ipv6_vec;


	// Normalize IPv6 // This needed atm but I may be adding to this code later.
	for(string& i : split_ipv6){
		if(i.size() > 4){ cout << "Invalid IPv6 syntax\n"; return 1; }
		while(i.size() < 4)
			i = string("0") + i;
		ipv6_vec.push_back(i);
	}

	// Modify MAC
	string mac_injection("ff:fe");

	mac_str.insert(mac_str.begin()+6,mac_injection.begin(), mac_injection.end());

	string mac_first_byte_str;
	mac_first_byte_str.insert(mac_first_byte_str.begin(), mac_str.begin(), mac_str.begin()+2);

	std::stringstream ss;
	int mac_first_byte_int;
	ss << std::hex << mac_first_byte_str;
	ss >> mac_first_byte_int;
	std::bitset<8> mac_first_byte_bits(mac_first_byte_int);
	mac_first_byte_bits.flip(1);
	ss.clear();
	ss << std::hex << mac_first_byte_bits.to_ulong() << endl;
	ss >> mac_first_byte_str;
	mac_first_byte_str = (mac_first_byte_str.size() == 1)? (std::string("0")+mac_first_byte_str) : mac_first_byte_str;
	
	mac_str.erase(mac_str.begin(),mac_str.begin()+2);
	mac_str.insert(mac_str.begin(),mac_first_byte_str.begin(), mac_first_byte_str.end());	
	mac_str.insert(mac_str.begin()+4, ':');
	mac_str.insert(mac_str.begin()+14, ':');


	// Inject MAC into IPv6
	vector<string> mac_segs = re::split(":",mac_str);
	string ipv6_out = ipv6_vec[0] + "::" + ord::join(mac_segs,":");
	ipv6_out = re::sub(":0",":",ipv6_out);	

	cout << "mac addr   = " << sys["-mac"][0] << endl;
	cout << "input IPv6 = " << sys["-ipv6"][0] << endl;
	cout << "link local = " << ipv6_out << endl;

	return 0;
}
