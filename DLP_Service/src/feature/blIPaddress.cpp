#include <string>
#include <iostream>
#include "../headers/blIPaddress/RuleParser.h"
#include "../headers/blIPaddress/FilterIP.h"
#include "../headers/main.h"

// TODO this should be taken from argv and stored in the parser
const char* rules_file_name = "..\\..\\..\\config\\firewall.cfg";

void print_banner(void) {
    std::cout <<"LANCSDLP_CONTROL_WEBSITES" <<std::endl;
}

int blIPaddress()
{
    std::string userInput;
    std::cout << "Enter 'ON' to start or 'OFF' to exit: ";
    std::cin >> userInput;
    if(userInput == "ON" || userInput == "on"){
    print_banner();

    std::ifstream rules(rules_file_name);
    RuleParser rule_parser(rules);
    if (rule_parser.size() == 0) {
        std::cerr << rules_file_name << ": found no valid rules." << std::endl;
        return 1;
    }
    std::cout << rules_file_name << ": found " << rule_parser.size() << " rule" <<
        (rule_parser.size() > 1 ? "s:" : ":") << std::endl;

    FirewallEngine fw;
    for (auto& r : rule_parser) {
        std::cout << "\t allow " << r.host << " for " << r.value <<
            (r.unit == LimitType::Bytes ? " bytes" : " seconds") << std::endl;
    }

    for (auto& r : rule_parser) {
        switch(r.unit) {
            case LimitType::Bytes:
                fw.addFilterDataLimit(r.host, r.ip, r.mask, r.value);
                break;
            case LimitType::Seconds:
                fw.addFilterTimeLimit(r.host, r.ip, r.mask, r.value, false, false);
                break;
        }
    }

    std::cout << std::endl <<
        "Rules added. Press any key to terminate the program or wait for the rules to expire. " << std::endl <<
        "Rules that have expired are now persistent and will remain after reboot. " << std::endl <<
        "Rules that have not expired will be removed automatically on reboot." << std::endl << std::endl <<
        "Rule expiration log:" << std::endl << std::endl;
    std::cin.get();
    }
    if(userInput == "OFF" || userInput == "off"){
        return 0;
    }
    
    return 0;
}