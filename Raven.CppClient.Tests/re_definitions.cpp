#include "pch.h"
#include "re_definitions.h"
#include "CreateDatabaseOperation.h"
#include "DeleteDatabasesOperation.h"
#include "ConnectionDetailsHolder.h"
#include "raven_test_definitions.h"
#include "TasksExecutor.h"

// We haven't checked which filesystem to include yet
#ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

// Check for feature test macro for <filesystem>
#   if defined(__cpp_lib_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0

// Check for feature test macro for <experimental/filesystem>
#   elif defined(__cpp_lib_experimental_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// We can't check if headers exist...
// Let's assume experimental to be safe
#   elif !defined(__has_include)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Check if the header "<filesystem>" exists
#   elif __has_include(<filesystem>)

// If we're compiling on Visual Studio and are not compiling with C++17, we need to use experimental
#       ifdef _MSC_VER

// Check and include header that defines "_HAS_CXX17"
#           if __has_include(<yvals_core.h>)
#               include <yvals_core.h>

// Check for enabled C++17 support
#               if defined(_HAS_CXX17) && _HAS_CXX17
// We're using C++17, so let's use the normal version
#                   define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#               endif
#           endif

// If the marco isn't defined yet, that means any of the other VS specific checks failed, so we need to use experimental
#           ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
#               define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#           endif

// Not on Visual Studio. Let's use the normal version
#       else // #ifdef _MSC_VER
#           define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#       endif

// Check if the header "<filesystem>" exists
#   elif __has_include(<experimental/filesystem>)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Fail if neither header is available with a nice error message
#   else
#       error Could not find system header "<filesystem>" or "<experimental/filesystem>"
#   endif

// We priously determined that we need the exprimental version
#   if INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
// Include it
#       include <experimental/filesystem>

// We need the alias from std::experimental::filesystem to std::filesystem
namespace std {
    namespace filesystem = experimental::filesystem;
}

// We have a decent compiler and can use the normal version
#   else
// Include it
#       include <filesystem>
#   endif

#endif // #ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

namespace ravendb::client::tests::definitions
{
	RequestExecutorScope::RequestExecutorScope(std::string db_name, bool is_secured, bool use_fiddler)
		: _db_name(std::move(db_name))
	{
		auto server_wide_req_exec = get_raw_request_executor({}, is_secured, use_fiddler);

		ravendb::client::serverwide::DatabaseRecord rec{};
		rec.database_name = _db_name;
		serverwide::operations::CreateDatabaseOperation op(rec);
		server_wide_req_exec->execute(*op.get_command(server_wide_req_exec->get_conventions()));

		_executor = get_raw_request_executor(_db_name, is_secured, use_fiddler);
	}

	RequestExecutorScope::~RequestExecutorScope()
	{
		try
		{
			ravendb::client::serverwide::operations::DeleteDatabasesOperation op(_db_name, true, {}, std::chrono::seconds(20));
			_executor->execute(*op.get_command(_executor->get_conventions()));
		}
		catch (...)
		{}
	}


	//request executor only - no DB is created
	std::shared_ptr<ravendb::client::http::RequestExecutor> get_raw_request_executor
	(const std::string& db, bool is_secured, bool use_fiddler)
	{
		static const auto sec_conn_details = infrastructure::ConnectionDetailsHolder(infrastructure::SECURED_RE_DETAILS_FILE_NAME, true);
		static const auto unsec_conn_details = infrastructure::ConnectionDetailsHolder(infrastructure::UNSECURED_RE_DETAILS_FILE_NAME, false);

		if (db.empty())
		{
			auto conventions = documents::conventions::DocumentConventions::create();
			conventions->set_disable_topology_updates(true);
			conventions->freeze();

			return is_secured ?
				http::RequestExecutor::create_for_single_node_with_configuration_updates(
					{ sec_conn_details.get_url() }, db, sec_conn_details.get_certificate_details(),
					std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
					conventions,
					infrastructure::set_no_proxy) :
				http::RequestExecutor::create_for_single_node_with_configuration_updates(
					{ unsec_conn_details.get_url() }, db, {},
					std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
					conventions,
					use_fiddler ? infrastructure::set_for_fiddler : infrastructure::set_no_proxy, {});
		}
		return is_secured ?
			http::RequestExecutor::create(
				{ sec_conn_details.get_url() }, db, sec_conn_details.get_certificate_details(),
				std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
				documents::conventions::DocumentConventions::default_conventions(),
				infrastructure::set_no_proxy) :
			http::RequestExecutor::create(
				{ unsec_conn_details.get_url() }, db, {},
				std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
				documents::conventions::DocumentConventions::default_conventions(),
				use_fiddler ? infrastructure::set_for_fiddler : infrastructure::set_no_proxy, {});

	}

	std::shared_ptr<RequestExecutorScope> RequestExecutorScope::get_request_executor_with_db(
		const std::string& file, int line, int counter, bool is_secured, bool use_fiddler)
	{
		std::filesystem::path path(file);
		std::ostringstream name;
		name << path.filename().replace_extension().string() << "_" << line << "_" << counter;
		return is_secured ?
			std::make_shared<RequestExecutorScope>(name.str(), true, use_fiddler) :
			std::make_shared<RequestExecutorScope>(name.str(), false, use_fiddler);
	}

}

