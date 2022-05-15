// Copyright 2021 Your Name <your_email>

#include <builder.hpp>

bool create_child_process(const std::vector<std::string>& arguments) {
  boost::system::error_code ec;
  child process = execute(set_args(arguments), set_on_error(ec));
  auto exit_code = wait_for_exit(process);
  if (exit_code){
    return false;
  }
  return true;
}
