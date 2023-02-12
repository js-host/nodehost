#include <iostream>
#include <vector>
#include <thread>

#include "../nodehost/libnodehost.h"
#include "../nodehost/internal/utils.h"

#include "nodehostsample.h"

int main(int argc, char** argv) {
	int err = 0;

    std::vector<std::string> args {};
    args.reserve(argc + 1);
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
  
    std::vector<char*> cargs {};
    cargs.reserve(args.size());
    for (auto& arg : args) {
        cargs.push_back(&arg.front());
    }

	err = nodehost_process__startup(cargs.size(), cargs.data());
	if (err != 0) {
		std::cerr << nodehost_getlasterror() << std::endl;
		return 1;
	}

	std::vector<std::thread> threads;
    threads.reserve(5);

	for (int i = 0; i < 1; i++)
	{
		threads.emplace_back([&](int i) {

			nodehost::api::NodeInstance* instance;
			if (nodehost_instance__new(&instance)) {
				std::cerr << nodehost_getlasterror() << std::endl;
				return;
			}

		    for (int ci = 0; ci < 5; ci++) {
		        nodehost::api::NodeContext* context;
                if (nodehost_instance__main_context(instance, &context)) {
                    nodehost_instance__dispose(instance);

                    std::cerr << nodehost_getlasterror() << std::endl;
                    return;
                }

                const std::string codeScript = nodehost::internal::string_format("globalThis.ident = %d; console.log('%d: context hi'); setTimeout(() => console.log('%d: timer 5000 ' + globalThis.ident), 5000);", ci, ci, ci);
                if (nodehost_context__execute_string(context, codeScript.c_str())) {
                    nodehost_instance__dispose(instance);

                    std::cerr << nodehost_getlasterror() << std::endl;
                    return;
                }

                // auto t = std::thread([&]() {
                //     const std::string threadCodeScript = nodehost::internal::string_format("console.log('%d: another thread'); setTimeout(() => console.log('%d: timer 100000 ' + globalThis.ident), 100000)", ci, ci);
                //     if (nodehost_context__execute_string(context, threadCodeScript.c_str())) {
                //         std::cerr << nodehost_getlasterror() << std::endl;
                //         return;
                //     }
                // });
                // t.join();

		        const std::string codeScript2 = nodehost::internal::string_format("console.log('%d: original thread');", ci);
                if (nodehost_context__execute_string(context, codeScript2.c_str())) {
                    nodehost_instance__dispose(instance);

                    std::cerr << nodehost_getlasterror() << std::endl;
                    return;
                }
		    }

		    if (nodehost_instance__run(instance)) {
              std::cerr << nodehost_getlasterror() << std::endl;
              return;
		    }

			nodehost_instance__dispose(instance);

		}, i);
	}

	for (auto& thread : threads) {
		thread.join();
	}

	err = nodehost_process__shutdown();
	if (err != 0) {
		std::cout << nodehost_getlasterror() << std::endl;
		return 1;
	}

	return 0;
}
