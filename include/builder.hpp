// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_BUILDER_HPP_
#define INCLUDE_BUILDER_HPP_


#include <async++.h>
#include <boost/process.hpp>
#include <boost/program_options.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/process/pipe.hpp>
#include <boost/iostreams/stream.hpp>
#include <vector>
#include <iostream>
#include <csignal>
#include <string>
#include <thread>
#include <chrono>

using boost::process::child;
using boost::process::execute;
using boost::process::search_path;
using boost::process::initializers::set_args;
using boost::process::initializers::set_on_error;
using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;

bool create_child_process(const std::vector<std::string>& command);
#endif // INCLUDE_BUILDER_HPP_
