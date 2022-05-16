#include <builder.hpp>

auto cmakePath = search_path("cmake");
std::vector<std::string> command1 = {cmakePath, "-H.", "-B_build",
                                     "-DCMAKE_INSTALL_PREFIX=_install"};
std::vector<std::string> command2 = {cmakePath, "--build", "_builds"};
std::vector<std::string> command3 = {cmakePath, "--build", "_builds",
                                     "--target", "install"};
std::vector<std::string> command4 = {cmakePath, "--build", "_builds",
                                     "--target", "package"};

auto async_start(bool& flag, time_t& time_spent, time_t& timeout, std::vector<std::string>& command, std::vector<std::string>& _command) {
  auto t1 = async::spawn([&flag, &time_spent, &timeout, &command, _command]() mutable {
    auto start = std::chrono::high_resolution_clock::now();
    flag = create_child_process(command);
    auto end = std::chrono::high_resolution_clock::now();
    time_spent += std::chrono::duration_cast<
                      std::chrono::milliseconds>(end - start).count();
    if (flag && time_spent < timeout) {
      flag = create_child_process(_command);
      end = std::chrono::high_resolution_clock::now();
      time_spent = std::chrono::duration_cast<
                       std::chrono::milliseconds>(end - start).count();
    }
    std::cout << "Time for the first command: " <<
        std::chrono::duration_cast<
            std::chrono::milliseconds>(end - start).count()
              << " milliseconds\n";
  });
  return t1;
}

void init_variables(bool& flag, time_t& time_spent, time_t& timeout, std::vector<std::string>& command) {
  if (flag && time_spent < timeout) {
    auto start = std::chrono::high_resolution_clock::now();
    flag = create_child_process(command);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time for the second command: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << " milliseconds\n";
  }
}

int main(int argc, char* argv[]) {
  std::string config;
  time_t timeout;
  options_description desc("Allowed options");
  desc.add_options()
      ("help", "выводим вспомогательное сообщение")
          ("config", value<std::string>(&config)->default_value("Debug"),
           "указываем конфигурацию сборки (по умолчанию Debug)")
              ("install", "добавляем этап установки (в директорию _install)")
                  ("pack", "добавляем этап упаковки (в архив формата tar.gz)")
                      ("timeout", value<time_t>(&timeout)->
                                  default_value(std::chrono::high_resolution_clock::to_time_t(
                                      std::chrono::high_resolution_clock::now())),
                       "указываем время ожидания (в секундах)")
      ;
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);


  if (vm.count("help")) {
    std::cout << desc << "\n";
  } else {
    time_t time_spent = 0;
    bool flag = true;
    if (config == "Debug" || config == "Release") {
      std::string mode = "-DCMAKE_BUILD_TYPE=" + config;
      command1.push_back(mode);
      auto t1 = async_start(flag, time_spent, timeout, command1, command2);
      if (vm.count("install")) {
       auto t2 = t1.then([&flag, &time_spent, &timeout]() {
          init_variables(flag, time_spent, timeout, command3);
        });
        if (vm.count("pack")) {
          auto t3 = t2.then([&flag, &time_spent, &timeout]() {
            init_variables(flag, time_spent, timeout, command4);
          });
          t3.get();
        }
        else{
          t2.get();
        }
      }
      else if (vm.count("pack") && !vm.count("install")) {
        auto t2 = t1.then([&flag, &time_spent, &timeout]() {
          init_variables(flag, time_spent, timeout, command4);
        });
        t2.get();
      }
      else{
        t1.get();
      }
    } else {
      std::cout << "I don't know this config....\n";
    }
  }

  return  0;
}
